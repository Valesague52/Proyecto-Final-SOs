//disk_manager.h
#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <string>
#include "file_system.h"

class DiskManager {
public:
    void readFromDisk(FileSystem &fs, const std::string &filename);
    void writeToDisk(FileSystem &fs, const std::string &filename, const std::string &data);
};

#endif