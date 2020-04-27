#include "Filesystem.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory.h>
#include <ctime>

using namespace std;

Filesystem::Filesystem() {
    fsFilename = "data";
    workingDir = "/";
    if (!load()) {
        cerr << "Fatal error: data load failed." << endl;
        exit();
    }
    bitmap = reinterpret_cast<bitset<BITMAP_SIZE> *>(memory + INODE_NUM * INODE_SIZE);
    // Test area, delete later.

}

void Filesystem::initialize() {
    int dumpy;
    createDir(dumpy);
}

bool Filesystem::load() {
    memory = (char *) malloc(FS_SIZE);
    if (memory == nullptr) return false;
    ifstream input(fsFilename);
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
    ofstream output(fsFilename, ios::binary | ios::trunc);
    output.write(memory, FS_SIZE);
    return true;
}

bool Filesystem::exit() {
    save();
    free(memory);
    memory = nullptr;
    return true;
}

void Filesystem::summary() {
    cout << "Available space: " << (BLOCK_NUM - bitmap->count()) * BLOCK_SIZE / 1024 << " KB" << endl
         << "Total block number: " << BLOCK_NUM << endl
         << "Used block: " << bitmap->count() << endl
         << "Block size: " << BLOCK_SIZE << " B" << endl
         << "Inode number: " << INODE_NUM << endl
         << "Inode size: " << INODE_SIZE << " B" << endl;
}

bool Filesystem::createFile(const string &path) {

    return true;
}

bool Filesystem::deleteFile(const string &path) {
    return false;
}

bool Filesystem::createDir(const string &path) {
    vector<string> names = splitPath(path);
    int parentDirInodeNumber = inodeNumber(names[0]);
    string dirName = names[1];
    // TODO
    return false;
}

bool Filesystem::createDir(int &inodeNum) {
    if (assignInode(inodeNum)) {
        Inode inode = createInode(true);
        writeInode(inodeNum, &inode);
        return true;
    } else {
        cerr << "No more iNode available." << endl;
        return false;
    }
}

bool Filesystem::createFile(int &inodeNum) {
    if (assignInode(inodeNum)) {
        Inode inode = createInode(false);
        // TODO: init file
        writeInode(inodeNum, &inode);
        return true;
    } else {
        cerr << "No more iNode available." << endl;
        return false;
    }
}

bool Filesystem::deleteDir(const string &path) {
    return false;
}

bool Filesystem::changeWorkingDir(const string &path) {
    return false;
}

bool Filesystem::copyFile(const string &sourceFilePath, const string &targetFilePath) {
    return false;
}

bool Filesystem::list(const string &path) {
    return false;
}

bool Filesystem::printFile(const string &path) {
    return false;
}

string Filesystem::getWorkingDir() {
    return workingDir;
}

int Filesystem::inodeNumber(const string &path) {
    auto names = parsePath(path);
    int targetInodeNumber = 0;
    for (const string &name : names) {
        targetInodeNumber = inodeNumber(name, targetInodeNumber);
    }
    return targetInodeNumber;
}

int Filesystem::inodeNumber(const string &name, int dirInodeNumber) {
    Inode *parent = readInode(dirInodeNumber);
    // TODO
    return 0;
}

void Filesystem::writeInode(int inodeNumber, Inode *inode) {
    char *src = (char *) inode;
    char *dis = memory + inodeNumber * INODE_SIZE;
    memcpy(dis, src, INODE_SIZE);
}

bool Filesystem::assignInode(int &inodeNum) {
    Inode *temp;
    for (int i = 0; i < INODE_NUM; ++i) {
        temp = readInode(i);
        if (temp->type == 0) {
            inodeNum = i;
            return true;
        }
    }
    return false;
}

Inode Filesystem::createInode(bool isDir) {
    Inode inode{};
    inode.createTime = time(nullptr);
    inode.type = isDir ? '1' : '2';
    return inode;
}

Inode *Filesystem::readInode(int inodeNumber) {
    return (Inode *) (memory + inodeNumber * INODE_SIZE);
}

void Filesystem::writeBlock(int address, char *buffer) {

}

void Filesystem::readBlock(int address, char *buffer) {

}

bool Filesystem::assignBlock(int &blockNum) {
    for (int i = 0; i < BITMAP_SIZE; ++i) {
        if (!(*bitmap)[i]) {
            blockNum = i;
            return true;
        }
    }
    return false;
}

vector<string> Filesystem::parsePath(const string &path) {
    vector<string> names;
    string temp;
    for (char c : path) {
        if (c != '/') {
            temp += c;
        } else {
            if (!temp.empty()) {
                names.emplace_back(temp);
                temp = "";
            }
        }
    }
    if (!temp.empty()) names.emplace_back(temp);
    return names;
}

vector<string> Filesystem::splitPath(const string &path) {
    vector<string> names;
    int splitIndex = 0;
    string temp;
    for (int i = path.size() - 1; i >= 0; --i) {
        if (path[i] != '/') {
            temp += path[i];
        } else {
            if (!temp.empty()) {
                splitIndex = i;
            }
        }
    }
    names.push_back(path.substr(0, splitIndex));
    names.push_back(path.substr(splitIndex));
    return names;
}

bool Filesystem::showFileStatus(const string &path) {
    int inodeNum = inodeNumber(path);
    Inode *inode = readInode(inodeNum);
    cout << "File: " << path << endl
         << "Inode: " << inodeNum << endl
         << "Size: " << inode->size << endl
         << "Type: " << (inode->type == '1' ? "directory" : "regular file") << endl
         << "Create time: " << inode->createTime << endl;
    return true;
}
