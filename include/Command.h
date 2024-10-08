#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <vector>
#include <string>



/*
 * class that stores information about a command
 * 
 * in_file  - string containing the redirected input filename, if it exists
 * out_file - string containing the redirected output filename, if it exists
 * args     - vector of strings containing the arguments of the command
 * 
 * whether or not the command should be run in the background is also stored
 * accessible by calling ->isBackground()
 */
class Command {
private:
    // full command stored for internal convenience
    std::string cmd;
    // list of quoted strings in command
    std::vector<std::string> inner_strings;

    // whether or not the command should be run in the background
    bool bg;
    bool is_expansion;

public:
    // filename of redirected input file, if it exists
    std::string in_file;
    // filename of redirected output file, if it exists
    std::string out_file;
    // command arguments
    std::vector<std::string> args;

    // constructor - takes command and calls internal convenience
    //               functions to parse the arguments
    Command (const std::string _cmd, std::vector<std::string> _inner_strings);
    Command (const std::string _cmd, std::vector<std::string> _inner_strings, bool _is_sign_exp);

    // destructor
    ~Command () {}

    // boolean functions to return if command has I/O redirection
    // or runs in background
    bool hasInput ();
    bool hasOutput ();
    bool isBackground ();

    /* If a command should create a sign expansion result
        by writing its output to a pipe to be read by a sign expansion */
    bool isSignExpansion() { return is_expansion; };

    /* Updates a command with the result of its requested sign expansion */
    void signExpand(std::vector<std::string*>* _se_results);

    // Returns the arguments in an array of c strings
    char** argsToCString();

private:
    // convenience functions to trim whitespace, find input/output filename,
    // and parse arguments
    std::string trim (const std::string in);
    void findInOut ();
    void parseArgs ();
};

#endif
