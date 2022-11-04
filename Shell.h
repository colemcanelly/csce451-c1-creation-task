#ifndef _SHELL_H_
#define _SHELL_H_

#include <iostream>

#include <list>
#include <unistd.h>     // pipe, fork, dup2, execvp, close
#include <sys/wait.h>   // wait, waitpid

#include <cstring>

#include "Tokenizer.h"

// all the basic colours for a shell prompt
#define RED     "\033[1;31m"
#define GREEN	"\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE	"\033[1;34m"
#define WHITE	"\033[1;37m"
#define NC      "\033[0m"

enum FD {READ, WRITE};    // read and write ends of a pipe

struct Job;
class Shell;

extern Shell* const aggieshell;

namespace Custom {
    int direc ( std::vector<std::string> args );
    void jobs ( bool __showrunning = true );
    // void kill ( Command* command, std::list<Job*>* bgjobs );
}

struct Job {
    Job(size_t _n, pid_t _pid, std::string _l) : job_number(_n), pid(_pid), _cmd(_l) {}
    ~Job() {}
    size_t job_number;
    pid_t pid;
    std::string _cmd;
};

class Shell {
private:
    std::string _prompt;

public:
    const int stdin_copy = dup(STDIN_FILENO), stdout_copy = dup(STDOUT_FILENO);
    int sigwinch_received;
    bool running, redirect;
    size_t numJobs;
    std::list<Job*>* bgjobs;
    std::string prev_dir;

    Shell() {
        running = true;
        redirect = false;
        prev_dir = "";
        numJobs = 0;
        sigwinch_received = 0;
        bgjobs = new std::list<Job*>{};
    }

    ~Shell() {
        signal(SIGQUIT, SIG_IGN);       // Immunity from the genocide that is about to unfold
        kill(0, SIGQUIT);               // Genocide all my children
        for (auto it = bgjobs->begin(); it != bgjobs->end();)       // Systematically reaping my children
        { 
            waitpid((*it)->pid, NULL, 0);
            delete *it;
            it = bgjobs->erase(it);
        }
        delete bgjobs;
    }

    const char* prompt() {
        _prompt = "";

        time_t now = time(0);
        std::string d_t = std::string{ctime(&now)};
        d_t.back() = char(']');
        char* env = getenv("USER");
        char* dir = get_current_dir_name();

        _prompt.append(YELLOW + std::string{"["});
        _prompt.append(d_t);
        _prompt.append( GREEN + std::string{env});
        _prompt.append(  NC   + std::string{":"});
        _prompt.append(  BLUE + std::string{dir});
        _prompt.append(  NC   + std::string{"$ "});

        free(dir);

        return (_prompt.c_str());
    }

    std::string* get_expansion_result() {
        const size_t count = 1024;
        ssize_t n_bytes = 0;

        std::string* p_result = new std::string{""};
        char buffer[count + 1];
        do {
            memset(buffer, 0, count);
            n_bytes = read(STDIN_FILENO, buffer, count);
            buffer[count] = '\0';
            p_result->append(buffer);
        } while ((n_bytes != 0) && (n_bytes == count));
        if (n_bytes < 0) {
            perror("pipe_read");
            delete p_result;
            return nullptr;
        }
        if (p_result->back() == char('\n')) p_result->erase(p_result->back());
        return p_result;
    }
};

#endif