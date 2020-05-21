#ifndef FILE_SYSTEM_EMULATOR_SHELL_H
#define FILE_SYSTEM_EMULATOR_SHELL_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "Filesystem.h"

using namespace std;

string introduction() {
    return "Welcome to Ubuntu 16.04.6 LTS (GNU/Linux 4.4.0-130-generic x86_64)\n"
           "\n"
           " * Documentation:  https://help.ubuntu.com\n"
           " * Management:     https://landscape.canonical.com\n"
           " * Support:        https://ubuntu.com/advantage";
}

string help() {
    return "rm\tdelete file or directory\n"
           "mkdir\tcreate directory\n"
           "touch\tcreate file\n"
           "vim\tcreate and edit file\n"
           "ls\tlist files and directories of the given directory\n"
           "cd\tchange working directory\n"
           "mv\tmove file\n"
           "df\tprint summary\n"
           "cat\tprint file content\n"
           "stat\tprint file info\n"
           "help\tprint this help message\n"
           "exit\texit the shell";
}

string prompt(const string &workingDir) {
    return "ubuntu@VM-0-16-ubuntu:" + workingDir + "$ ";
}

bool execute(Filesystem &fs, const string &command) {
    istringstream iss(command);
    vector<string> tokens;
    copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));
    tokens.emplace_back("");
    tokens.emplace_back("");
    if (tokens[0] == "cp") {
        fs.copyFile(tokens[1], tokens[2]);
    } else if (tokens[0] == "mv") {
        fs.moveFile(tokens[1], tokens[2]);
    } else if (tokens[0] == "rm" || tokens[0] == "deleteFile" ||
               tokens[0] == "rmdir" || tokens[0] == "deleteDir") {
        if (!tokens[1].empty()) {
            fs.deleteFile(tokens[1]);
        } else {
            cerr << "rm: missing file operand" << endl;
        }
    } else if (tokens[0] == "mkdir" || tokens[0] == "createDir") {
        if (!tokens[1].empty()) {
            fs.createDir(tokens[1]);
        } else {
            cerr << "mkdir: missing file operand" << endl;
        }
    } else if (tokens[0] == "touch" || tokens[0] == "createFile") {
        if (!tokens[1].empty()) {
            int size = 0;
            try {
                size = stoi(tokens[2]);
            } catch (const exception &e) {}
            fs.createFile(tokens[1], size * 1024);
        } else {
            cerr << "touch: missing file operand" << endl;
        }
    } else if (tokens[0] == "vim" || tokens[0] == "vi") {
        if (!tokens[1].empty()) {
            int size = 0;
            try {
                size = stoi(tokens[2]);
            } catch (const exception &e) {}
            fs.createFile(tokens[1], size * 1024, true);
        } else {
            cerr << "vim: missing file operand" << endl;
        }
    } else if (tokens[0] == "ls" || tokens[0] == "dir") {
        fs.list(tokens[1]);
    } else if (tokens[0] == "cd" || tokens[0] == "changeDir") {
        fs.changeWorkingDir(tokens[1]);
    } else if (tokens[0] == "mv") {
        cout << "TODO" << endl;
    } else if (tokens[0] == "pwd") {
        cout << fs.getWorkingDir() << endl;
    } else if (tokens[0] == "df" || tokens[0] == "sum") {
        fs.summary();
    } else if (tokens[0] == "cat") {
        fs.printFile(tokens[1]);
    } else if (tokens[0] == "stat") {
        if (!tokens[1].empty()) {
            fs.showFileStatus(tokens[1]);
        } else {
            cerr << "stat: missing file operand" << endl;
        }
    } else if (tokens[0] == "help") {
        cout << help() << endl;
    } else if (tokens[0] == "exit") {
        fs.exit();
        return false;
    } else {
        cout << command << ": command not found" << endl;
    }
    return true;
}


#endif //FILE_SYSTEM_EMULATOR_SHELL_H
