#ifndef ALLOCATOR
#define ALLOCATOR
#include <string>
#include <cstring>
#include "allocator_pointer.h"


using std::cout;
using std::cin;
using std::endl;
// Forward declaration. Do not include real class definition
// to avoid expensive macros calculations and increase compile speed
class Pointer;

/**
 * Wraps given memory area and provides defagmentation allocator interface on
 * the top of it.
 *
 *
 */
class Allocator {
    cptr buf;
    cptr *end;
    cptr *border;
public:
    Allocator(void* base, size_t size){
            buf.start = base;
            buf.end = static_cast<char *>(base) + size;
            end = static_cast<cptr *>(buf.end);
            border = end;
    }

    /**
     * TODO: semantics
     * @param N size_t
     */
    Pointer alloc(size_t N, size_t test_type = 0);

    /**
     * TODO: semantics
     * @param p Pointer
     * @param N size_t
     */
    void realloc(Pointer& p, size_t N, int test_type = 0) {

        void *s =  p.get();
        size_t sz = p.size();
        if(test_type == 213)
            cout << "start " << s << " size " << sz << endl;
        free(p);
        p = alloc(N, test_type);

        memcpy(p.get(), s, sz);
    }

    /**
     * TODO: semantics
     * @param p Pointer
     */
    void free(Pointer& p) {
        p.free();
    }

    /**
     * TODO: semantics
     */
    void defrag(int test = 0);

    /**
     * TODO: semantics
     */
    std::string dump() const {
        int i = 0;
        for(cptr *cur = static_cast<cptr*>(buf.end) - 1; cur != nullptr; cur = cur->next) {
            if(cur->start)
                cout << i++ <<". Allocated " << static_cast<char*>(cur->end) - static_cast<char*>(cur->start)
                 <<" bytes from " << cur->start << " to " << cur->end << endl;
        }
        return "";
    }
};

#endif // ALLOCATOR
