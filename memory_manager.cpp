//memory_manager.cpp
#include "memory_manager.h"

MemoryManager::MemoryManager(int total) : totalPages(total), usedPages(0) {}

bool MemoryManager::allocate(int pages) {
    if (usedPages + pages <= totalPages) {
        usedPages += pages;
        return true;
    }
    std::cout << "Memoria insuficiente. Disponible: " 
              << (totalPages - usedPages) << " páginas.\n";
    return false;
}

void MemoryManager::free(int pages) {
    usedPages -= pages;
    if (usedPages < 0) usedPages = 0;
}

int MemoryManager::getUsedPages() const {
    return usedPages;
}

int MemoryManager::getTotalPages() const {
    return totalPages;
}

void MemoryManager::showMemoryStatus() const {
    std::cout << "Memoria usada actualmente: " << usedPages 
              << "/" << totalPages << " páginas\n";
}