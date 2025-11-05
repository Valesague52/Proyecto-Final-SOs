
//file_system.cpp
#include "file_system.h"
#include <iostream>

void FileSystem::createFile(const std::string &name, const std::string &content) {
    for (const auto &f : files) {
        if (f.name == name) {
            std::cout << "Error: ya existe un archivo con ese nombre.\n";
            return;
        }
    }
    File newFile{name, content, (int)content.size()};
    files.push_back(newFile);
    std::cout << "Archivo '" << name << "' creado correctamente (" << newFile.size << " bytes).\n";
}

void FileSystem::listFiles() const {
    std::cout << "--- Archivos en el sistema ---\n";
    if (files.empty()) {
        std::cout << "No hay archivos en el sistema.\n";
        return;
    }
    for (const auto &f : files) {
        std::cout << "Nombre: " << f.name << " | Tamaño: " << f.size << " bytes\n";
    }
}

File* FileSystem::findFile(const std::string &name) {
    for (auto &f : files) {
        if (f.name == name)
            return &f;
    }
    return nullptr;
}

void FileSystem::writeFile(const std::string &name, const std::string &data) {
    File* f = findFile(name);
    if (!f) {
        std::cout << "Error: el archivo '" << name << "' no existe. No se puede escribir.\n";
        return;
    }
    f->content += data;
    f->size = (int)f->content.size();
    std::cout << "Datos agregados al archivo '" << name << "'. Nuevo tamaño: " << f->size << " bytes.\n";
}

File* FileSystem::selectFileForWrite() {
    if (files.empty()) {
        std::cout << "No hay archivos en el sistema.\n";
        return nullptr;
    }

    std::cout << "Seleccione un archivo para escribir:\n";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << i + 1 << ". " << files[i].name << " (" << files[i].size << " bytes)\n";
    }

    int opcion = 0;
    std::cout << "Opción: ";
    if (!(std::cin >> opcion)) {
        std::cin.clear();
        std::string dump;
        std::getline(std::cin, dump);
        std::cout << "Opción inválida.\n";
        return nullptr;
    }

    if (opcion < 1 || opcion > (int)files.size()) {
        std::cout << "Opción inválida.\n";
        return nullptr;
    }

    return &files[opcion - 1];
}