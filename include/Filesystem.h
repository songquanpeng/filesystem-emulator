#ifndef FILE_SYSTEM_EMULATOR_FILESYSTEM_H
#define FILE_SYSTEM_EMULATOR_FILESYSTEM_H

#include <string>

using std::string;


struct Inode {
    char type;
    unsigned int size;
    time_t createTime;
    unsigned int address[11];
};

class Filesystem {
public:
    static const int FS_SIZE;
    static const char *FILE_NAME;
    static const int INODE_SIZE;
    Filesystem();
    string prompt();
    void summary();
    bool load();
    bool save();
    bool exit();
    bool createFile(string filename);
    bool deleteFile(string filename);
    bool createDir(string dirname);
    bool deleteDir(string dirname);
    bool changeWorkingDir(string dirname);
    string getWorkingDir();
    bool copyFile(string sourceFilename, string targetFilename);
    bool list(string dirname);
    bool printFile(string filename);
private:
    string workingDir;
    char *memory{};
    void initialize();
    int inodeNumber(string filename);
    int inodeNumber(string filename, int dirInodeNumber); // Search target file in given dir.
    int getInode(bool *success);
    void writeInode(int inodeNumber, Inode* inode);
    Inode* readInode(int inodeNumber);
    Inode createInode(bool isDir = false);
    void writeBlock(int address, char* buffer);
    void readBlock(int address, char* buffer);
};

#endif //FILE_SYSTEM_EMULATOR_FILESYSTEM_H
