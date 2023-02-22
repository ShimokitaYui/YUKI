#ifndef __MYOS__MEMORYMAGEMENT_H
#define __MYOS__MEMORYMAGEMENT_H

#include <common/types.h>

namespace myos {
    struct  MemoryChunk {
        MemoryChunk *next;
        MemoryChunk *prev;
        bool allocated;
        myos::common::size_t size;
    };
    
    class MemoryManager {
    protected:
        MemoryChunk* first;
    public:
        
        MemoryManager(myos::common::size_t start, myos::common::size_t size);
        ~MemoryManager();

        void* malloc(myos::common::size_t size);
        void free(void* ptr);
        
        static MemoryManager* activeMemoryManager;
    };
}

void* operator new(unsigned int size);
void* operator new[](unsigned int size);

//plcaement new
void* operator new(unsigned int size, void* ptr);
void* operator new[](unsigned int size, void *ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);
#endif