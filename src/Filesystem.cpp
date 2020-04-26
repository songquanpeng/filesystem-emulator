#include "Filesystem.h"

Filesystem::Filesystem() {
    workingDir = "~";
    load();
}

string Filesystem::prompt() {
    return "ubuntu@VM-0-16-ubuntu:"+workingDir+"$ ";
}

void Filesystem::summary() {

}

bool Filesystem::load() {
    return false;
}

bool Filesystem::save() {
    return false;
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




