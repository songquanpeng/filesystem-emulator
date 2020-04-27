#include "Filesystem.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory.h>
#include <ctime>

using namespace std;

const int Filesystem::FS_SIZE = 16 * 1024 * 1024; // 16777216
const int Filesystem::INODE_SIZE = sizeof(Inode);
const char *Filesystem::FILE_NAME = "data";

Filesystem::Filesystem() {
    workingDir = "~";
    if (!load()) {
        cerr << "Fatal error: data load failed." << endl;
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
        if (length != FS_SIZE) cerr << "Warning: the file size doesn't match." << endl;
        input.seekg(0, ios::beg);
        input.read(memory, length);
        input.close();
    } else { // File doesn't exist.
        memset(memory, 0, FS_SIZE);
        initialize();
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
    Inode inode = createInode();
    inode.size = 666;
    writeInode(0, &inode);
    Inode *p = readInode(768);
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

void Filesystem::initialize() {
    createDir("/");
}

int Filesystem::inodeNumber(string filename) {
    return 0;
}

int Filesystem::inodeNumber(string filename, int dirInodeNumber) {
    return 0;
}

void Filesystem::writeInode(int inodeNumber, Inode *inode) {
    char *src = (char *) inode;
    char *dis = memory + inodeNumber * INODE_SIZE;
    memcpy(dis, src, INODE_SIZE);
}

Inode *Filesystem::readInode(int inodeNumber) {
    return (Inode *) (memory + inodeNumber * INODE_SIZE);
}

void Filesystem::writeBlock(int address, char *buffer) {

}

void Filesystem::readBlock(int address, char *buffer) {

}

int Filesystem::getInode(bool *success) {

    return 0;
}

Inode Filesystem::createInode(bool isDir) {
    Inode inode{};
    inode.createTime = time(nullptr);
    inode.type = isDir ? '1' : '2';
    return inode;
}
