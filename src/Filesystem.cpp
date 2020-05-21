#include "Filesystem.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory.h>
#include <ctime>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <utility>

using namespace std;

Filesystem::Filesystem() {
    fsFilename = "data";
    workingDir = "/";
    if (!load()) {
        cerr << "Fatal error: data load failed." << endl;
        exit();
    }
    bitmap = reinterpret_cast<bitset<BITMAP_SIZE> *>(memory + INODE_NUM * INODE_SIZE);
}

void Filesystem::initialize() {
    unsigned int dumpy;
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
         << "Total block number: " << BLOCK_NUM << "  "
         << "Used block: " << bitmap->count() << "  "
         << "Block size: " << BLOCK_SIZE << " B" << endl
         << "Inode number: " << INODE_NUM << "  "
         << "Inode size: " << INODE_SIZE << " B" << endl;
}

bool Filesystem::deleteFile(string path) {
    path = fullPath(path);
    if (path == "/") {
        cerr << "rm: permission denied" << endl;
        return false;
    } else if (path == workingDir) {
        cerr << "rm: cannot delete working dir" << endl;
        return false;
    }
    if (!exist(path)) {
        cerr << "rm: cannot remove '" << path << "': No such file or directory" << endl;
        return false;
    }
    vector<string> names = splitPath(path);
    unsigned int parentDirInodeNumber = inodeNumber(names[0]);
    string name = names[1];
    unsigned int inodeNum = inodeNumber(name, parentDirInodeNumber);
    if (revokeInode(inodeNum)) {
        Inode *parent = readInode(parentDirInodeNumber);
        unsigned int &blockAddress = parent->address[0];
        char *buffer = readBlock(blockAddress);
        auto *dirItem = new DirItem;
        int targetPos;
        for (targetPos = 0; targetPos * DIR_ITEM_SIZE < parent->size; ++targetPos) {
            dirItem = (DirItem *) (buffer + targetPos * DIR_ITEM_SIZE);
            string temp(dirItem->name);
            if (temp == name) {
                break;
            }
        }
        memmove(buffer + parentDirInodeNumber * INODE_SIZE + targetPos * DIR_ITEM_SIZE,
                buffer + parentDirInodeNumber * INODE_SIZE + parent->size - DIR_ITEM_SIZE,
                DIR_ITEM_SIZE);
        parent->size -= DIR_ITEM_SIZE;
        return true;
    }
    return false;
}

bool Filesystem::revokeInode(unsigned int inodeNum) {
    // If this inode is a directory, we also need to delete all files that it include.
    Inode *inode = readInode(inodeNum);
    // TODO: When deleting directory, recursively delete all content in it.
    bool isDir = inode->type == '1';
    auto addresses = blockAddress(inode);
    for (auto address : addresses) {
        revokeBlock(address);
    }
    memset(memory + inodeNum * INODE_SIZE, 0, INODE_SIZE);
    return true;
}

bool Filesystem::createDir(string path) {
    path = fullPath(path);
    if (exist(path)) {
        cerr << "mkdir: cannot create directory '" << path << "': File exists" << endl;
        return false;
    }
    vector<string> names = splitPath(path);
    unsigned int parentDirInodeNum = inodeNumber(names[0]);
    string dirName = names[1];
    unsigned int inodeNum;
    if (createDir(inodeNum)) {
        return addDirItem(parentDirInodeNum, inodeNum, dirName);
    }
    return false;
}

bool Filesystem::createDir(unsigned int &inodeNum) {
    if (assignInode(inodeNum)) {
        Inode inode = createInode(true);
        writeInode(inodeNum, &inode);
        return true;
    }
    return false;
}

bool Filesystem::createFile(string path, int size, bool edit) {
    if (size < 0) size = 0;
    unsigned restSpace = (BLOCK_NUM - bitmap->count()) * BLOCK_SIZE;
    if (size > restSpace) {
        cerr << "Warning: no enough space" << endl;
        size = 0;
    }

    if (size > MAX_FILE_SIZE) {
        cerr << "Warning: max file size is " << MAX_FILE_SIZE << " Bytes" << endl;
        size = MAX_FILE_SIZE;
    }
    path = fullPath(path);
    if (exist(path)) {
        cerr << "touch: cannot create file '" << path << "': File exists" << endl;
        return false;
    }
    vector<string> names = splitPath(path);
    unsigned int parentDirInodeNum = inodeNumber(names[0]);
    string fileName = names[1];
    unsigned int inodeNum;
    if (createFile(inodeNum, size, edit)) {
        return addDirItem(parentDirInodeNum, inodeNum, fileName);
    }
    return false;
}

bool Filesystem::createFile(unsigned int &inodeNum, unsigned int size, bool edit) {
    if (assignInode(inodeNum)) {
        Inode inode = createInode(false);
        inode.size = size;
        int blockNum = size / BLOCK_SIZE;
        if (blockNum * BLOCK_SIZE < size) blockNum++;
        for (int i = 0; i < min(blockNum, DIRECT_ADDRESS_NUM); ++i) {
            assignBlock(inode.address[i]);
            fillBlock(inode.address[i], edit);
            if (i == 0 && edit) {
                editFile(inode.address[i]);
            }
        }
        if (blockNum > DIRECT_ADDRESS_NUM) {
            assignBlock(inode.address[DIRECT_ADDRESS_NUM]);
            auto *blockAddresses = new unsigned[blockNum - DIRECT_ADDRESS_NUM];
            for (int i = 0; i < blockNum - DIRECT_ADDRESS_NUM; ++i) {
                assignBlock(blockAddresses[i]);
                fillBlock(blockAddresses[i], edit);
            }
            // Write the extra block address into the last direct block
            writeBlock(inode.address[DIRECT_ADDRESS_NUM], reinterpret_cast<char *>(blockAddresses));
            delete[] blockAddresses;
        }
        writeInode(inodeNum, &inode);
        return true;
    }
    return false;
}

void Filesystem::editFile(unsigned int blockAddress) {
    string file;
    string row;
    while (getline(cin, row)) {
        if (row == ":wq") break;
        file += row + "\n";
    }
    file += '\0';
    char *dis = memory + BLOCK_START_POS + blockAddress * BLOCK_SIZE;
    int size = min(BLOCK_SIZE, (int)file.size());
    memcpy(dis, file.c_str(), size);
}

bool Filesystem::changeWorkingDir(string path) {
    path = fullPath(path);
    if (!existPath(path)) {
        cerr << "cd: No such directory " << path << endl;
        return false;
    }
    workingDir = path;
    if (workingDir[0] != '/') {
        workingDir = '/' + workingDir;
    }
    return true;
}

string Filesystem::getWorkingDir() {
    return workingDir;
}

bool Filesystem::list(string path) {
    if (path.empty()) {
        path = workingDir;
    } else {
        path = fullPath(path);
    }
    unsigned dirInodeNumber = inodeNumber(path);
    if (dirInodeNumber == 0 && path != "/") {
        cerr << "ls: cannot access '" << path << "': No such file or directory" << endl;
        return false;
    }
    Inode *parent = readInode(dirInodeNumber);
    unsigned int &blockAddress = parent->address[0];
    char *buffer = readBlock(blockAddress);
    cout << std::left << setw(5) << "INode" << " Name " << endl;
    DirItem *dirItem;
    for (int i = 0; i * DIR_ITEM_SIZE < parent->size; ++i) {
        dirItem = (DirItem *) (buffer + i * DIR_ITEM_SIZE);
        cout << std::left << setw(5) << dirItem->inodeNum << " " << dirItem->name << endl;
    }
    return true;
}

bool Filesystem::copyFile(string sourceFilePath, string targetFilePath) {
    unsigned srcInodeNum = inodeNumber(std::move(sourceFilePath));
    Inode *srcInode = readInode(srcInodeNum);
    createFile(targetFilePath, srcInode->size);
    unsigned dstInodeNum = inodeNumber(std::move(targetFilePath));
    Inode *dstInode = readInode(dstInodeNum);
    auto srcAddresses = blockAddress(srcInode);
    auto dstAddresses = blockAddress(dstInode);
    for (int i = 0; i < srcAddresses.size(); ++i) {
        char *realSrcAddress = readBlock(srcAddresses[i]);
        char *realDstAddress = readBlock(dstAddresses[i]);
        memcpy(realDstAddress, realSrcAddress, BLOCK_SIZE);
    }
    return true;
}

bool Filesystem::moveFile(const string &sourceFilePath, string targetFilePath) {
    copyFile(sourceFilePath, std::move(targetFilePath));
    deleteFile(sourceFilePath);
}

bool Filesystem::printFile(string path) {
    unsigned fileInodeNum = inodeNumber(std::move(path));
    Inode *inode = readInode(fileInodeNum);
    char *buffer = new char[inode->size];
    auto addresses = blockAddress(inode);
    char *dstAddress = buffer;
    for (auto address : addresses) {
        char *srcAddress = readBlock(address);
        memcpy(dstAddress, srcAddress, BLOCK_SIZE);
        dstAddress += BLOCK_SIZE;
    }
    for (int i = 0, counter = 0; i < inode->size; ++i, ++counter) {
        cout << buffer[i];
        if (counter == 63) {
            counter = 0;
            cout << endl;
        }
    }
    cout << endl;
    delete[] buffer;
    return true;
}

unsigned int Filesystem::inodeNumber(string path) {
    path = fullPath(path);
    auto names = parsePath(path);
    unsigned int targetInodeNumber = 0;
    for (const string &name : names) {
        targetInodeNumber = inodeNumber(name, targetInodeNumber);
    }
    return targetInodeNumber;
}

unsigned int Filesystem::inodeNumber(const string &name, unsigned int dirInodeNumber) {
    Inode *parent = readInode(dirInodeNumber);
    unsigned int &blockAddress = parent->address[0];
    char *buffer = readBlock(blockAddress);
    DirItem *dirItem;
    for (int i = 0; i * DIR_ITEM_SIZE < parent->size; ++i) {
        dirItem = (DirItem *) (buffer + i * DIR_ITEM_SIZE);
        string temp(dirItem->name);
        if (temp == name) {
            return dirItem->inodeNum;
        }
    }
    return 0;
}

bool Filesystem::assignInode(unsigned int &inodeNum) {
    Inode *temp;
    for (int i = 0; i < INODE_NUM; ++i) {
        temp = readInode(i);
        if (temp->type == 0) {
            inodeNum = i;
            return true;
        }
    }
    cerr << "No more iNode available." << endl;
    return false;
}

Inode Filesystem::createInode(bool isDir) {
    Inode inode{};
    inode.createTime = time(nullptr);
    inode.type = isDir ? '1' : '2';
    return inode;
}

void Filesystem::writeInode(unsigned int inodeNumber, Inode *inode) {
    char *src = (char *) inode;
    char *dis = memory + inodeNumber * INODE_SIZE;
    memcpy(dis, src, INODE_SIZE);
}

Inode *Filesystem::readInode(unsigned int inodeNumber) {
    return (Inode *) (memory + inodeNumber * INODE_SIZE);
}

void Filesystem::writeBlock(unsigned int address, char *buffer) {
    char *dis = memory + BLOCK_START_POS + address * BLOCK_SIZE;
    memcpy(dis, buffer, BLOCK_SIZE);
}

void Filesystem::fillBlock(unsigned int address, bool empty) {
    const int num = BLOCK_SIZE / sizeof(char);
    char buffer[num];
    if(empty) {
        memset(buffer, 0, num);
    } else{
        srand(time(nullptr));
        for (int i = 0; i < num; i++) {
            buffer[i] = 'a' + rand() % 26;
        }
    }
    writeBlock(address, buffer);
}


char *Filesystem::readBlock(unsigned int address) {
    return memory + BLOCK_START_POS + address * BLOCK_SIZE;
}

bool Filesystem::assignBlock(unsigned int &blockNum) {
    // We need 0 to tell us whether the address is used.
    for (int i = 1; i < BITMAP_SIZE; ++i) {
        if (!(*bitmap)[i]) {
            blockNum = i;
            (*bitmap)[blockNum] = true;
            return true;
        }
    }
    cerr << "fail to assign block" << endl;
    return false;
}

bool Filesystem::revokeBlock(unsigned int &blockNum) {
    if (blockNum >= BITMAP_SIZE) return false;
    (*bitmap)[blockNum] = false;
    return true;
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
                break;
            }
        }
    }
    names.push_back(path.substr(0, splitIndex));
    if (path.substr(splitIndex)[0] == '/') {
        names.push_back(path.substr(splitIndex + 1));
    } else {
        names.push_back(path.substr(splitIndex));
    }
    if (names[0].empty()) names[0] = "/";
    return names;
}

bool Filesystem::showFileStatus(string path) {
    path = fullPath(path);
    unsigned int inodeNum = inodeNumber(path);
    if (inodeNum == 0 && path != "/") {
        cout << "stat: cannot stat '" << path << "': No such file or directory" << endl;
        return false;
    }
    Inode *inode = readInode(inodeNum);
    cout << "File: " << path << endl
         << "Inode: " << inodeNum << endl
         << "Size: " << inode->size << " B" << endl
         << "Type: " << (inode->type == '1' ? "directory" : "regular file") << endl
         << "Create time: " << inode->createTime << endl
         << "Block address: ";
    auto addresses = blockAddress(inode);
    for (auto address : addresses) {
        cout << address << " ";
    }
    cout << endl;
    return true;
}

bool Filesystem::exist(const string &path) {
    unsigned int inodeNum = inodeNumber(path);
    return !(inodeNum == 0 && path != "/");
}

bool Filesystem::existPath(const string &path) {
    unsigned int inodeNum = inodeNumber(path);
    Inode *inode = readInode(inodeNum);
    return (inodeNum != 0 || path == "/") && (inode->type == '1');
}

string Filesystem::fullPath(const string &path) {
    if (path.empty()) {
        cerr << "Warning: empty path detected." << endl;
        return "";
    }
    if (path[0] == '/') return path;
    if (workingDir.front() != '/') workingDir = '/' + workingDir;
    if (workingDir.back() != '/') workingDir += '/';
    return workingDir + path;
}

vector<unsigned> Filesystem::blockAddress(Inode *inode) {
    vector<unsigned> addresses;
    int addressNum = inode->size / BLOCK_SIZE;
    if (addressNum * BLOCK_SIZE < inode->size) addressNum++;
    for (int i = 0; i < min(addressNum, DIRECT_ADDRESS_NUM); ++i) {
        addresses.push_back(inode->address[i]);
    }
    if (addressNum > DIRECT_ADDRESS_NUM) {
        //auto *indirectAddresses = reinterpret_cast<unsigned int *>(readBlock(addresses[DIRECT_ADDRESS_NUM]));
        auto *buffer = new unsigned[addressNum - DIRECT_ADDRESS_NUM];
        memcpy(buffer, readBlock(inode->address[DIRECT_ADDRESS_NUM]),
               (addressNum - DIRECT_ADDRESS_NUM) * sizeof(unsigned));
        for (int i = 0; i + DIRECT_ADDRESS_NUM < addressNum; ++i) {
            addresses.push_back(buffer[i]);
        }
        delete[] buffer;
    }
    return addresses;
}

bool Filesystem::addDirItem(unsigned parentDirInodeNum, unsigned fileInodeNum, const string &name) {
    Inode *inode = readInode(parentDirInodeNum);
    unsigned int &blockAddress = inode->address[0];
    if (blockAddress == 0 && !assignBlock(blockAddress)) {
        cerr << "fail to assign block" << endl;
        return false;
    }
    char *buffer = readBlock(blockAddress);
    auto *dirItem = new DirItem;
    dirItem->inodeNum = fileInodeNum;
    strncpy(dirItem->name, name.c_str(), MAX_FILENAME_LENGTH);
    memcpy(buffer + inode->size, dirItem, DIR_ITEM_SIZE);
    inode->size += DIR_ITEM_SIZE;
    delete dirItem;
    return true;
}