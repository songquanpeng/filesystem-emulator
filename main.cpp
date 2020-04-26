#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "Filesystem.h"

using namespace std;
Filesystem fs;

string introduction() {
    return "Welcome to Ubuntu 16.04.6 LTS (GNU/Linux 4.4.0-130-generic x86_64)\n"
           "\n"
           " * Documentation:  https://help.ubuntu.com\n"
           " * Management:     https://landscape.canonical.com\n"
           " * Support:        https://ubuntu.com/advantage";
}

string help() {

}

bool execute(const string &command) {
    istringstream iss(command);
    vector<string> tokens;
    copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));
    tokens.emplace_back("");
    tokens.emplace_back("");
    if (tokens[0] == "cp") {
        fs.copyFile(tokens[1], tokens[2]);
    } else if (tokens[0] == "rm" || tokens[0] == "deleteFile") {
        fs.deleteFile(tokens[1]);
    } else if (tokens[0] == "rmdir" || tokens[0] == "deleteDir") {
        fs.deleteDir(tokens[1]);
    } else if (tokens[0] == "mkdir" || tokens[0] == "createDir") {
        fs.createDir(tokens[1]);
    } else if (tokens[0] == "touch" || tokens[0] == "createFile") {
        fs.createFile(tokens[1]);
    } else if (tokens[0] == "ls" || tokens[0] == "dir") {
        fs.list(tokens[1]);
    } else if (tokens[0] == "cd" || tokens[0] == "changeDir") {
        fs.changeWorkingDir(tokens[1]);
    } else if (tokens[0] == "mv") {
        cout << "TODO" << endl;
    } else if (tokens[0] == "df" || tokens[0] == "sum") {
        fs.summary();
    } else if (tokens[0] == "cat") {
        fs.printFile(tokens[0]);
    } else if (tokens[0] == "help") {
        cout << help() <<endl;
    } else if (tokens[0] == "exit") {
        fs.save();
        return false;
    } else {
        cout << command << ": command not found" << endl;
    }
    return true;
}

int main() {
    cout << introduction() << endl << endl;
    while (true) {
        cout << fs.prompt();
        string input;
        getline(cin, input);
        if (input.empty()) continue;
        if (!execute(input)) break;
    }
    return 0;
}
