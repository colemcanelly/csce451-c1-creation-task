#include "Command.h"

using namespace std;

Command::Command (const string _cmd, vector<string> _inner_strings)
    : is_expansion(false)
{
    cmd = trim(_cmd);
    bg = (cmd.substr(cmd.size()-1) == "&");
    in_file = "";
    out_file = "";
    findInOut();
    inner_strings = _inner_strings;
    parseArgs();
}

Command::Command (const std::string _cmd, std::vector<std::string> _inner_strings, bool __is_exp)
    : is_expansion(__is_exp) 
{
    cmd = trim(_cmd);
    bg = (cmd.substr(cmd.size()-1) == "&");
    in_file = "";
    out_file = "";
    findInOut();
    inner_strings = _inner_strings;
    parseArgs();    
}

bool Command::hasInput () {
    return in_file != "";
}

bool Command::hasOutput () {
    return out_file != "";
}

bool Command::isBackground () {
    return bg;
}

char** Command::argsToCString() {
    char** __arg_c_str = new char*[args.size() + 1];
    for (size_t i = 0; i < args.size(); ++i) {
        __arg_c_str[i] = (char*)args.at(i).c_str();
    }
    __arg_c_str[args.size()] = nullptr;
    return __arg_c_str;
}

string Command::trim (const string in) {
    int i = in.find_first_not_of(" \n\r\t");
    int j = in.find_last_not_of(" \n\r\t");

    if (i >= 0 && j >= i) {
        return in.substr(i, j-i+1);
    }
    return in;
}

void Command::findInOut () {
    if (cmd.find("<") != string::npos) {  // input redirection
        int in_start = cmd.find("<");
        int in_end = cmd.find_first_of(" \n\r\t>", cmd.find_first_not_of(" \n\r\t", in_start+1));
        if ((size_t) in_end == string::npos) {
            in_end = cmd.size();
        }

        in_file = trim(cmd.substr(in_start+1, in_end-in_start-1));
        cmd = trim(cmd.substr(0, in_start) + cmd.substr(in_end));
    }

    if (cmd.find(">") != string::npos) {  // output redirection
        int out_start = cmd.find(">");
        int out_end = cmd.find_first_of(" \n\r\t<", cmd.find_first_not_of(" \n\r\t", out_start+1));
        if ((size_t) out_end == string::npos) {
            out_end = cmd.size();
        }
        
        out_file = trim(cmd.substr(out_start+1, out_end-out_start-1));
        cmd = trim(cmd.substr(0, out_start) + cmd.substr(out_end));
    }
}

void Command::signExpand(std::vector<std::string*>* _se_results) {
    for (string& arg : args) {
        while (arg.find("--tkn") != string::npos) {
            size_t start = arg.find("--tkn");
            size_t end = arg.find("nkt--", start);
            size_t index = stoi(arg.substr(start + 5, end - start - 5));
            
            string str_beg = arg.substr(0, start);
            string str_mid = *(_se_results->at(index));
            while (str_mid.find('\n') != string::npos) {
                str_mid.erase(str_mid.find('\n'));
            }
            string str_end = arg.substr(end + 5);
            arg = str_beg + str_mid + str_end;
        }
    }
}

void Command::parseArgs () {
    string temp = cmd;
    string delim = " ";
	
	size_t i = 0;
	while ((i = temp.find(delim)) != string::npos) {
		args.push_back(trim(temp.substr(0, i)));
		temp = trim(temp.substr(i+1));
	}
	args.push_back(trim(temp));

    if (bg) {  // remove "&" if background process
        args.pop_back();
    }

    int offset = 1;
    if (args.at(0) == "ls" || args.at(0) == "grep") {  // color text (if applicable)
        offset = 2;
    }

    i = 0;
    while (i < args.size()) {  // generate arguments
        if (args.at(i) == "--str") {
            args.at(i) = (char*) inner_strings.at(stoi(args.at(i+1))).c_str();
            args.erase(args.begin()+i+1);
        }
        i++;
    }
    if (offset > 1) {
        args.insert(args.begin()+1, "--color=auto");
    }
}
