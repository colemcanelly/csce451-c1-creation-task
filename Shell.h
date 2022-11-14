#ifndef _SHELL_H_
#define _SHELL_H_

#include <iostream>

#include <list>
#include <unistd.h>     // pipe, fork, dup2, execvp, close, chdir
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
    std::string prev_dir, home_dir;

    Shell() {
        running = true;
        redirect = false;
        prev_dir = "";
        home_dir = "";
        char* home = getenv("HOME");
        if (home) home_dir.assign(home);
        numJobs = 0;
        sigwinch_received = 0;
        bgjobs = new std::list<Job*>{};
    }

    ~Shell() {
        // signal(SIGQUIT, SIG_IGN);       // Immunity from the genocide that is about to unfold
        // kill(0, SIGQUIT);               // Genocide all my children
        for (auto it = bgjobs->begin(); it != bgjobs->end();)       // Systematically reaping my children
        {
            kill((*it)->pid, SIGQUIT); 
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
            memset(buffer, '\0', count + 1);
            n_bytes = read(STDIN_FILENO, buffer, count);
            p_result->append(buffer, n_bytes);
            while (p_result->find('\n') != std::string::npos) {
                p_result->erase(p_result->find('\n'));
            }
        } while ((n_bytes > 0) && (n_bytes == count));
        if (n_bytes < 0) {
            perror("pipe_read");
            delete p_result;
            return nullptr;
        }
        if (p_result->back() == char('\n')) p_result->erase(p_result->back());
        return p_result;
    }
};

namespace Custom {
    // Had to include these files from Custom.cpp because the remote tester wouldn't compile it.
    int direc ( std::vector<std::string> args )
    {
        char* tmp = get_current_dir_name();
        int status = -1;
        if (args.back() == "-") {
            if (aggieshell->prev_dir != "") status = chdir((char*)aggieshell->prev_dir.c_str());
        }
        else {
            if (args.back().front() == char('~') && aggieshell->home_dir != "") args.back().replace(0, 1, aggieshell->home_dir);
            status = chdir((char*)args.back().c_str());
        }
        aggieshell->prev_dir.assign(tmp);
        free(tmp);
        if (status < 0) {
            return status;
        }
        return 0;
    }

    void jobs ( bool __showrunning = true )
    {
        for (auto it = aggieshell->bgjobs->begin(); it != aggieshell->bgjobs->end();)
        {
            if (waitpid((*it)->pid, NULL, WNOHANG) > 0) {
                (*it)->_cmd.erase((*it)->_cmd.rfind("&"));
                std::cout << "\t\t\t\t" << (*it)->_cmd << "\r[" << (*it)->job_number << "]   Done" << std::endl;
                delete *it;
                it = aggieshell->bgjobs->erase(it);      // Incr it and erase prev element (calls the destructor)
            }
            else {
                if (__showrunning) std::cout << "\t\t\t\t" << (*it)->_cmd << "\r[" << (*it)->job_number << "]   Running" << std::endl;
                ++it;
            }
        }
    }
    // void kill ( Command* command, std::list<Job*>* bgjobs );
}


#endif