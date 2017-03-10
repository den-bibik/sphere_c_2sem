#ifndef ALLOCATOR_POINTER

#include <cstddef>
#include <iostream>

#define ALLOCATOR_POINTER

// Forward declaration. Do not include real class definition
// to avoid expensive macros calculations and increase compile speed
class Allocator;


struct cptr{
    void* start;
    void* end;
    cptr* next;
    cptr* pred;
};

class Pointer {
    cptr* link;

public:
    Pointer(cptr *link1 = nullptr){
        link = link1;
    }
    std::size_t size(){
        if(link == nullptr) return 0;
        if(link->start == nullptr) return 0;
        return static_cast<char *>(link->end) - static_cast<char *>(link->start);
    }
    void* end(){
        return link->end;
    };
    void* get() const {
        if(link)
            return link->start;
        return link;
    }
    void free() {
        if(link) {
            link->end = link->start;
            link->start = nullptr;
        }
    }
    void print(){
        std::cout << "start " <<link->start << std::endl;
        std::cout << "end   " <<link->end << std::endl;
        if(link->next) std::cout << "next  " <<link->next->start << std::endl;
        if(link->pred) std::cout << "pred  " <<link->pred->end << std::endl;
        if(link->pred) std::cout << "pred size " << static_cast<char*>(link->pred->end) - static_cast<char*>(link->pred->start)<< std::endl;
    }
};

#endif //ALLOCATOR_POINTER
