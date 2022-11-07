#include "Shell.h"

int Custom::direc ( std::vector<std::string> args )
{
    char* tmp = get_current_dir_name();
    int status = -1;
    if (args.back() == "-") {
        if (aggieshell->prev_dir != "") status = chdir((char*)aggieshell->prev_dir.c_str());
    }
    else {
        if (args.back().front() == char('~')) args.back().replace(0, 1, aggieshell->home_dir);
        status = chdir((char*)args.back().c_str());
    }
    aggieshell->prev_dir.assign(tmp);
    free(tmp);
    if (status < 0) {
        return status;
    }
    return 0;
}

void Custom::jobs (bool __showrunning)
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

// void kill ( Command* command ) {
//     if (&command->args.at(1).front() == "%") {

//     }
// }