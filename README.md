# File System Emulator
## Requirements
1. Total space size is 16MB with block size 1KB.
2. File attributes that need supported: file size and create time.
3. Each iNode has ten direct addresses and one  indirect address.
4. Print author info on starting.
5. Commands that need supported: 
    + createFile fileName fileSize
    + deleteFile filename
    + createDir dirname
    + deleteDir dirname
    + changeDir dirname
    + dir
    + cp filename1 filename2
    + sum
    + cat filename

## TODO List
- [x] Implement a basic shell for interact.
- [ ] Able to create directory.
- [ ] Able to create file.
- [x] Able to list subdirs and files in target directory.
- [ ] Able to show file content.
- [x] Able to show directory's attributes.
- [ ] Support relative path.
