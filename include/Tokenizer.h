#ifndef _LEXER_H_
#define _LEXER_H_

#include <vector>
#include <string>

#include "Command.h"

/*
 * class that tokenizes the input into vector of commands
 * vector is then accesible outside of class
 * 
 * if vector length > 1, then commands are piped together:
 *  - vector.front() is first command in piped chain
 *  - vector.back() is last command in piped chain
 */
class Tokenizer {
private:
    // full user input stored for internal convenience
    std::string input;
    // flag for if an error occurs - error will be printed by Tokenizer
    bool error;

    /* Flag for background process */
    bool bg_proc;

    // Flags for sign expansion
    bool is_expansion, has_expansion;

public:
    // vector of commands
    std::vector<Command*> commands;

    // Vector of internal strings that were taken out of the line before parsing
    std::vector<std::string> inner_strings;

    // Vector of internal sign expansions lines `$(...)` that were removed before parsing
    std::vector<std::string> inner_sign_expansions;

    /* Vector of indeces for the `commands` vector
        where each command requires a sign expansion result */
    // std::vector<size_t> se_locations;
    
    // constructor - takes CLI input and calls internal convenience
    //               functions to tokenize into commands
    Tokenizer (const std::string _input);

    Tokenizer (const std::string _input, Tokenizer* parentTokn, std::vector<std::string*>* se_results);

    // destructor - deletes pointers in vector and erases elements
    ~Tokenizer ();

    std::string getInput() { return input; };

    void signExpand (std::vector<std::string*>* se_results);

    // boolean function to return if error ocurred during parsing
    bool hasError ();

    bool isBackground() { return bg_proc; };

    // If a line should create a sign expansion result
    bool hasExpansion() { return has_expansion; };

private:
    // convenience functions to trim whitespace and split input on "|"
    std::string trim (const std::string in);
    void split (const std::string delim, std::vector<std::string*>* se_results = nullptr);
    void remove_se ( std::string& line );
};

#endif
