//file_system.h
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <vector>

struct File {
    std::string name;
    std::string content;
    int size;
};

class FileSystem {
private:
    std::vector<File> files;

public:
    void createFile(const std::string &name, const std::string &content);
    void listFiles() const;
    File* findFile(const std::string &name);
    void writeFile(const std::string &name, const std::string &data);

    // mostrar archivos y permitir seleccionar uno existente
    File* selectFileForWrite();
};

#endif