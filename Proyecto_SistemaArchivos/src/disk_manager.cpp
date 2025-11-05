//disk_manager.cpp
#include "disk_manager.h"
#include <iostream>

void DiskManager::readFromDisk(FileSystem &fs, const std::string &filename) {
    File* f = fs.findFile(filename);
    if (!f) {
        std::cout << "Error: el archivo '" << filename << "' no existe.\n";
        return;
    }
    std::cout << "Contenido del archivo '" << filename << "':\n";
    std::cout << f->content << "\n";
}

void DiskManager::writeToDisk(FileSystem &fs, const std::string &filename, const std::string &data) {
    fs.writeFile(filename, data);
}