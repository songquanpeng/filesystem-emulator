# File System Emulator
## Build
```bash
git clone https://github.com/songquanpeng/filesystem-emulator.git
cd filesystem-emulator
cmake .
make
```

## Demo
```bash
ubuntu@VM-0-16-ubuntu:/$ vim test.py 1
print("Hello World!")

:wq
ubuntu@VM-0-16-ubuntu:/$ ls
INode Name
1     test.py
ubuntu@VM-0-16-ubuntu:/$ cat test.py
print("Hello World!")

ubuntu@VM-0-16-ubuntu:/$ stat test.py
File: /test.py
Inode: 1
Size: 1024 B
Type: regular file
Create time: 1637464120
Block address: 1
ubuntu@VM-0-16-ubuntu:/$ df
Available space: 16124 KB
Total block number: 16126  Used block: 2  Block size: 1024 B
Inode number: 4096  Inode size: 64 B
```

## TODO List
- [x] Implement a basic shell for interact.
- [x] Able to create directory.
- [x] Able to create file.
- [x] Able to delete directory.
- [x] Able to delete file.
- [x] Able to list subdirs and files in target directory.
- [x] Able to show file content.
- [x] Able to edit file on creating.
- [x] Able to show directory's attributes.
- [ ] Support relative path.
- [x] Able to change working directory.
- [x] Able to use all direct block addresses and indirect block address.
- [ ] Support "." and "..".
- [x] When creating file, fill it with random string.
- [ ] When deleting directory, recursively delete all content in it.
- [x] Able to copy file.
- [x] Able to move file.
