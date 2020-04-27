#ifndef FILE_SYSTEM_EMULATOR_FILESYSTEM_H
#define FILE_SYSTEM_EMULATOR_FILESYSTEM_H

#include <string>
#include <vector>
#include <bitset>

using std::string;
using std::vector;
using std::bitset;

struct Inode {
    char type;
    unsigned int size;
    time_t createTime;
    unsigned int address[11];
};

const int FS_SIZE = 16 * 1024 * 1024; // 16777216
const int INODE_NUM = 4 * 1024;
const int INODE_SIZE = sizeof(Inode);
const int BITMAP_SIZE = FS_SIZE / 1024;
const int BLOCK_SIZE = 1024;
const int BLOCK_NUM = (FS_SIZE - INODE_NUM * INODE_SIZE - BITMAP_SIZE / 8) / BLOCK_SIZE;


class Filesystem {
public:
    Filesystem();
    void summary();
    bool load();
    bool save();
    bool exit();
    bool createFile(const string& path);
    bool deleteFile(const string& path);
    bool createDir(const string& path);
    bool deleteDir(const string& path);
    bool changeWorkingDir(const string& path);
    string getWorkingDir();
    bool copyFile(const string& sourceFilePath, const string& targetFilePath);
    bool list(const string& path);
    bool showFileStatus(const string& path);
    bool printFile(const string& path);

private:
    string workingDir;
    bitset<BITMAP_SIZE> *bitmap;
    string fsFilename;
    char *memory{};
    void initialize();
    int inodeNumber(const string& path);
    // Search target file or folder in given dir.
    int inodeNumber(const string& name, int dirInodeNumber);
    bool assignInode(int &inodeNum);
    void writeInode(int inodeNumber, Inode* inode);
    Inode* readInode(int inodeNumber);
    static Inode createInode(bool isDir = false);
    void writeBlock(int address, char* buffer);
    void readBlock(int address, char* buffer);
    bool assignBlock(int &blockNum);
    bool createDir(int &inodeNum);
    bool createFile(int &inodeNum);
    static vector<string> parsePath(const string& path);
    static vector<string> splitPath(const string& path);
};

#endif //FILE_SYSTEM_EMULATOR_FILESYSTEM_H
