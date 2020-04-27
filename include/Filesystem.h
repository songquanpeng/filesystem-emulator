#ifndef FILE_SYSTEM_EMULATOR_FILESYSTEM_H
#define FILE_SYSTEM_EMULATOR_FILESYSTEM_H

#include <string>

using std::string;
using std::fstream;


class Filesystem {
public:
    static const int FS_SIZE;
    static const char *FILE_NAME;
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
};

#endif //FILE_SYSTEM_EMULATOR_FILESYSTEM_H
