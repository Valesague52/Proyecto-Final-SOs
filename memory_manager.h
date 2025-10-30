//memory_manager.h
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>

class MemoryManager {
private:
    int totalPages;
    int usedPages;

public:
    MemoryManager(int total = 16);
    bool allocate(int pages);
    void free(int pages);
    int getUsedPages() const;
    int getTotalPages() const;
    void showMemoryStatus() const;
};

#endif