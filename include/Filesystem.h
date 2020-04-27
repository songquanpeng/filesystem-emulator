#ifndef FILE_SYSTEM_EMULATOR_FILESYSTEM_H
#define FILE_SYSTEM_EMULATOR_FILESYSTEM_H

#include <string>
#include <vector>
#include <bitset>

using std::string;
using std::vector;
using std::bitset;

struct Inode {
    char type; // '1': dir, '2': regular file
    unsigned int size;
    time_t createTime;
    unsigned int address[11];
};

const int MAX_FILENAME_LENGTH = 60;

struct DirItem {
    unsigned int inodeNum; // 4 bytes
    char name[MAX_FILENAME_LENGTH]; // 60 bytes
};

const int FS_SIZE = 16 * 1024 * 1024; // 16777216
const int INODE_NUM = 4 * 1024; // 12 bit
const int INODE_SIZE = sizeof(Inode);
const int BITMAP_SIZE = FS_SIZE / 1024;
const int BLOCK_START_POS = INODE_NUM * INODE_SIZE + BITMAP_SIZE;
const int BLOCK_SIZE = 1024;
const int BLOCK_NUM = (FS_SIZE - INODE_NUM * INODE_SIZE - BITMAP_SIZE / 8) / BLOCK_SIZE;
const int DIR_ITEM_SIZE = sizeof(DirItem);

class Filesystem {
public:
    Filesystem();
    void summary();
    bool load();
    bool save();
    bool exit();
    bool createFile(const string& path, int size=1);
    bool deleteFile(const string& path);
    bool createDir(const string& path);
    bool deleteDir(const string& path);
    bool changeWorkingDir(const string& path);
    string getWorkingDir();
    bool copyFile(const string& sourceFilePath, const string& targetFilePath);
    bool list(string& path);
    bool showFileStatus(const string& path);
    bool printFile(const string& path);

private:
    string workingDir;
    bitset<BITMAP_SIZE> *bitmap;
    string fsFilename;
    char *memory{};
    void initialize();
    unsigned int inodeNumber(const string& path);
    // Search target file or folder in given dir.
    unsigned int inodeNumber(const string& name, unsigned int dirInodeNumber);
    bool assignInode(unsigned int &inodeNum);
    void writeInode(unsigned int inodeNumber, Inode* inode);
    Inode* readInode(unsigned int inodeNumber);
    static Inode createInode(bool isDir = false);
    void writeBlock(unsigned int address, char* buffer);
    char* readBlock(unsigned int address);
    bool assignBlock(unsigned int &blockNum);
    bool createDir(unsigned int &inodeNum);
    bool createFile(unsigned int &inodeNum);
    static vector<string> parsePath(const string& path);
    static vector<string> splitPath(const string& path);
};

#endif //FILE_SYSTEM_EMULATOR_FILESYSTEM_H
