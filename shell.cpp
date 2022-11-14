#include <iostream>

#include <sys/types.h>  // pid_t, etc
#include <sys/wait.h>   // wait, waitpid
#include <sys/ioctl.h>  // Get terminal size
#include <unistd.h>     // pipe, fork, dup2, execvp, close
#include <fcntl.h>

// #include <readline/readline.h>  // rl_*()
// #include <readline/history.h>   // add_history()

/* This was for the tab completion and history extra credit
    It does not compile on the remote GitHub tester >:/
#include "ncurses/ncurses.h"
#define READLINE_LIBRARY true
#include "readline/readline.h"
#include "readline/history.h"   // add_history()
*/

#include "Shell.h"     // struct `Job`, namespace `Custom`, class `Shell`

using namespace std;

Shell* const aggieshell = new Shell{};

/* Handle SIGWINCH and window size changes when readline is not active and
    reading a character. */
/*
static void sighandler (int) {
    aggieshell->sigwinch_received = true;
}
*/

void exec_cmd ( Command* command, const int* fds, const bool isLast )
{
    if (!isLast || command->isSignExpansion()) {
        dup2(fds[FD::WRITE], STDOUT_FILENO);
    } else {
        if (command->hasOutput()) {
            int filefd = open((char*)command->out_file.c_str(), (O_CREAT | O_WRONLY | O_TRUNC), (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
            if (filefd < 0) {
                perror("output_file");
                exit(errno);
            }
            dup2(filefd, STDOUT_FILENO);    // Point the Shell's input to the input file
            close(filefd);                  // Remove the fd entry from the fd table
        }
    }
    close(fds[FD::WRITE]);                  // Remove the fd entry from the fd table
    close(fds[FD::READ]);                   // Close the read end of the pipe on the child side.

    int status = 0;
    if (command->args.front() == "jobs") {
        Custom::jobs();
    } else {
        char** args = command->argsToCString();
        status = execvp(args[0], args);
        delete[] args;
    }
    if (status < 0) {
        perror(NULL);       // "No such file or directory"
        exit(errno);
    }
    exit(EXIT_SUCCESS);

}

void exec_line ( Tokenizer* line )
{
    if (line->commands.front()->hasInput()) {
        int filefd = open((char*)line->commands.front()->in_file.c_str(), O_RDONLY);
        if (filefd < 0) {
            perror("input_file");
            // exit(2);
        }
        dup2(filefd, STDIN_FILENO);     // Point the Shell's input to the input file
        close(filefd);                  // Remove the fd entry from the fd table

    }
    pid_t childpid = -1;
    int fds[2];
    bool isLast;
    for (Command*& command : line->commands)
    {
        if (command->args.front() == "cd") {
            if (Custom::direc(command->args) < 0) perror("cd");
            continue;
        }

        pipe(fds); // Create pipe
        switch (childpid = fork())
        {
        case -1:                // ERROR
            perror("fork");
            return; // exit(2);
        case 0:                 // CHILD
            isLast = (&command == &line->commands.back());
            exec_cmd(command, fds, isLast);
            exit(errno);
            break;
        default:                // PARENT
            close(fds[FD::WRITE]);                // Close the write end of the pipe
            dup2(fds[FD::READ], STDIN_FILENO);    // Point the Shell's input to the read end of the pipe.
            close(fds[FD::READ]);                 // Remove the fd entry from the fd table
        }
    }
    if (line->isBackground()) {
        aggieshell->bgjobs->push_back(new Job{++aggieshell->numJobs, childpid, line->getInput() });
        cout << "[" << aggieshell->bgjobs->back()->job_number << "] " << childpid << endl;
        return;
    } else {
        int status = 0;
        waitpid(childpid, &status, 0);         // Reap child process
        // if (status > 1) exit(status);       // exit if child didn't exec properly
        return;
    }
}

void parse_line ( char* input )
{
    Tokenizer line{string{input}};  // get tokenized commands from user input
    if (line.hasError()) return;    // continue to next prompt if input had an error

    if (line.hasExpansion()) {
        vector<string*> se_results;
        for (auto it = line.inner_sign_expansions.begin(); it != line.inner_sign_expansions.end(); ++it) {
            Tokenizer curr{*it, &line, &se_results};
            if (curr.hasError()) return;
            exec_line(&curr);
            se_results.push_back(aggieshell->get_expansion_result());
        }
        line.signExpand(&se_results);
        for (string*& se : se_results) {
            delete se;
        }
    }

    exec_line(&line);
}

/* Callback function called for each line when accept-line executed, EOF
    seen, or EOF character read.   This sets a flag and returns; it could
    also call exit(3). */
static void handle_input (char *line)
{
    /* Can use ^D (stty eof) or "exit" to exit. */
    if (line == NULL || strcmp(line, "exit") == 0) {
        if (line == 0) cout << "\n";
        /* This was for the tab completion and history extra credit
        if (!aggieshell->redirect) {
            if (line == 0) cout << "\n";
            if(line) free(line);
            rl_callback_handler_remove();           // Reset the terminal settings from `shell_default`
        }
        */

        cout << RED << "Now exiting shell..." << endl << "Goodbye" << NC << endl;
        aggieshell->running = false;
    }
    else {
        if (*line) parse_line(line);
        
        dup2(aggieshell->stdin_copy, STDIN_FILENO);
        dup2(aggieshell->stdout_copy, STDOUT_FILENO);
        /* This was for the tab completion and history extra credit
        if (!aggieshell->redirect) {
            if (*line) add_history(line);
            free(line);
            rl_callback_handler_install(aggieshell->prompt(), handle_input);
        }
        */
        if (aggieshell->bgjobs->empty()) aggieshell->numJobs = 0;
        Custom::jobs(false);
    }
}

/* Skeleton version of the shell -- reads a line and executes it until EOF */
void shell_redirected ()
{
    string str;
    bool status = true;
    do {
        cout << aggieshell->prompt();
        if (!getline(cin, str)) status = false;
        // if (aggieshell->redirect) cout << str << endl;
        handle_input(&str[0]);

    } while (aggieshell->running && status);
}

/* This was for the tab completion and history extra credit
// Fancy features like `tab` autocompletion and arrow-key history for a better user experience
void shell_default () 
{
    fd_set fds;
    int r;

    setlocale(LC_ALL, "");              // Set default locale values based on user preference ("")
    signal(SIGWINCH, sighandler);       // Install handler for SIGWINCH
    rl_callback_handler_install(aggieshell->prompt(), handle_input);  // Install the line handler.

    // Enter a simple event loop.   This waits until something is available
    //  to read on readline's input stream (defaults to standard input) and
    //  calls the builtin character read callback to read it.    It does not
    //  have to modify the user's terminal settings. 
    while (aggieshell->running)
    {
        FD_ZERO(&fds);
        FD_SET(fileno(rl_instream), &fds);              

        r = select(FD_SETSIZE, &fds, NULL, NULL, NULL);
        if (r < 0 && errno != EINTR) {
            perror("rltest: select");
            rl_callback_handler_remove();
            break;
        }
        if (aggieshell->sigwinch_received) {
            rl_resize_terminal();
            aggieshell->sigwinch_received = 0;
        }
        if (r < 0) continue;            

        if (FD_ISSET(fileno(rl_instream), &fds)) rl_callback_read_char();
    }
}
*/

int main ()
{
    struct winsize in_w;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &in_w);         // Get the dimensions of stdin
    aggieshell->redirect = (in_w.ws_col == 0);      // Test if input (stdin) has been redirected
    shell_redirected();
    /*
    switch (aggieshell->redirect)                   // We don't need fancy features if input is redirected
    {
        case false:
            shell_default();
            break;
        case true:
            shell_redirected();
            break;
    }
    */
    delete aggieshell;
    return 0;
}
