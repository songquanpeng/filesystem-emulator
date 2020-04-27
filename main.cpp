#include <iostream>
#include <string>
#include "Filesystem.h"
#include "Shell.h"

using namespace std;

int main() {
    cout << introduction() << endl << endl;
    Filesystem fs;
    while (true) {
        cout << prompt(fs.getWorkingDir());
        string input;
        getline(cin, input);
        if (input.empty()) continue;
        if (!execute(fs, input)) break;
    }
    return 0;
}
