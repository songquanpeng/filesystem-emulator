#ifndef FILE_SYSTEM_EMULATOR_FILESYSTEM_H
#define FILE_SYSTEM_EMULATOR_FILESYSTEM_H

#include <string>
using std::string;


class Filesystem {
public:
    Filesystem();
    string prompt();
    void summary();
    bool load();
    bool save();
    bool createFile(string filename);
    bool deleteFile(string filename);
    bool createDir(string dirname);
    bool deleteDir(string dirname);
    bool changeWorkingDir(string dirname);
    bool copyFile(string sourceFilename, string targetFilename);
    bool list(string dirname);
    bool printFile(string filename);
private:
    string workingDir;
};

#endif //FILE_SYSTEM_EMULATOR_FILESYSTEM_H
