#ifndef FILE_SYSTEM_EMULATOR_FILESYSTEM_H
#define FILE_SYSTEM_EMULATOR_FILESYSTEM_H

#include <string>
#include <vector>
#include <bitset>

using std::string;
using std::vector;
using std::bitset;

const int DIRECT_ADDRESS_NUM = 10;

struct Inode {
    char type; // '1': dir, '2': regular file
    unsigned int size;
    time_t createTime;
    unsigned int address[DIRECT_ADDRESS_NUM + 1];
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
const int MAX_FILE_SIZE = (DIRECT_ADDRESS_NUM + BLOCK_SIZE / sizeof(unsigned)) * BLOCK_SIZE;

class Filesystem {
public:
    Filesystem();

    void summary();

    bool load();

    bool save();

    bool exit();

    // The unit of size is byte here.
    bool createFile(string path, int size = 0);

    // Including directory.
    bool deleteFile(const string &path);

    bool createDir(string path);

    bool changeWorkingDir(string path);

    string getWorkingDir();

    bool copyFile(const string &sourceFilePath, const string &targetFilePath);

    bool list(string &path);

    bool showFileStatus(const string &path);

    bool printFile(const string &path);

private:
    string workingDir;
    bitset<BITMAP_SIZE> *bitmap;
    string fsFilename;
    char *memory{};

    void initialize();

    unsigned int inodeNumber(const string &path);

    // Search target file or folder in given dir.
    unsigned int inodeNumber(const string &name, unsigned int dirInodeNumber);

    bool assignInode(unsigned int &inodeNum);

    void writeInode(unsigned int inodeNumber, Inode *inode);

    Inode *readInode(unsigned int inodeNumber);

    static Inode createInode(bool isDir = false);

    void writeBlock(unsigned int address, char *buffer);

    void fillBlock(unsigned int address);

    char *readBlock(unsigned int address);

    bool assignBlock(unsigned int &blockNum);

    bool revokeBlock(unsigned int &blockNum);

    bool createDir(unsigned int &inodeNum);

    bool createFile(unsigned int &inodeNum, unsigned int size = 0);

    // Revoke inode and free its blocks.
    bool revokeInode(unsigned int inodeNum);

    bool exist(const string &path);

    bool existPath(const string &path);

    static vector<string> parsePath(const string &path);

    static vector<string> splitPath(const string &path);

    string fullPath(const string &path);

    vector<unsigned> blockAddress(Inode *inode);

    bool addDirItem(unsigned parentDirInodeNum, unsigned fileInodeNum, const string &name);
};

#endif //FILE_SYSTEM_EMULATOR_FILESYSTEM_H
