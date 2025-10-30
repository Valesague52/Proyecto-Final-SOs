//disk_manager.h
#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <string>
#include "file_system.h"

class DiskManager {
public:
    // lee de "disco" (usa FileSystem como almacenamiento simulado)
    void readFromDisk(FileSystem &fs, const std::string &filename);

    // escribe en "disco" (append) - estrictamente solo sobre un archivo existente
    void writeToDisk(FileSystem &fs, const std::string &filename, const std::string &data);
};

#endif