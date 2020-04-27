#include "Filesystem.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory.h>

using namespace std;

const int Filesystem::FS_SIZE = 16 * 1024 * 1024; // 16777216
const char* Filesystem::FILE_NAME = "data";

Filesystem::Filesystem() {
    workingDir = "~";
    if (!load()) {
        cerr << "load failed." << endl;
        exit();
    }
}

bool Filesystem::load() {
    memory = (char *) malloc(FS_SIZE);
    if (memory == nullptr) return false;
    ifstream input(FILE_NAME);
    if (input) { // File exists.
        input.seekg(0, ios::end);
        int length = input.tellg();
        input.seekg(0, ios::beg);
        input.read(memory, length);
        input.close();
    } else { // File doesn't exist.
        memset(memory, '0', FS_SIZE);
    }
    return true;
}

bool Filesystem::save() {
    ofstream output(FILE_NAME, ios::binary | ios::trunc);
    output.write(memory, FS_SIZE);
    return true;
}

bool Filesystem::exit() {
    save();
    free(memory);
    memory = nullptr;
    return true;
}

string Filesystem::prompt() {
    return "ubuntu@VM-0-16-ubuntu:" + workingDir + "$ ";
}

void Filesystem::summary() {

}

bool Filesystem::createFile(string filename) {
    return false;
}

bool Filesystem::deleteFile(string filename) {
    return false;
}

bool Filesystem::createDir(string dirname) {
    return false;
}

bool Filesystem::deleteDir(string dirname) {
    return false;
}

bool Filesystem::changeWorkingDir(string dirname) {
    return false;
}

bool Filesystem::copyFile(string sourceFilename, string targetFilename) {
    return false;
}

bool Filesystem::list(string dirname) {
    return false;
}

bool Filesystem::printFile(string filename) {
    return false;
}

string Filesystem::getWorkingDir() {
    return workingDir;
}