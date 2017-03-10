#include <iostream>
#include "allocator.h"
#include "allocator_pointer.h"
#include "allocator_error.h"


Pointer Allocator::alloc(size_t N, size_t test_type) {
    if(end->pred == nullptr && end->next == nullptr && end->start == nullptr && end->end == nullptr &&
            N + sizeof(cptr) < static_cast<char*>(buf.end) - static_cast<char*>(buf.start))
    {
        if(test_type == 213)
            cout << "first" << endl;
        end->start = buf.start;
        end->end = static_cast<void*>(static_cast<char*>(buf.start) + N);
        return Pointer(end);
    }
    if(border > static_cast<void*>(static_cast<char*>(end->end) + N + sizeof(cptr))){
        if(test_type == 213)
            cout << "to end" << endl;
        end->next = border - 1;
        border = end->next;

        border->pred = end;
        end = border;
        end->start = end->pred->end;
        end->end = static_cast<void*>(static_cast<char*>(end->pred->end) + N);
        end->next = nullptr;
        return Pointer(end);
    }
    else{
        for(cptr *cur = static_cast<cptr*>(buf.end) - 1; cur->next != nullptr; cur = cur->next){
            void *s = cur->next->start;
            if(s == nullptr) s = cur->next->end;
            size_t sz = static_cast<char*>(s) - static_cast<char*>(cur->end);
            if(sz >= N){
                if(test_type == 213) {
                    cout << "to cur " << cur << endl;
                    cout << "size" <<sz << endl;
                    cout <<"end " << cur->end <<" next start " << cur->next->start << endl;
                }
                border--;
                cur->next = border;
                border->pred = cur;
                border->start = cur->end;
                border->end = static_cast<void*>(static_cast<char*>(cur->end) + N);
                border->next = cur->next;

                return Pointer(border);
            }
        }
    }

    throw AllocError(AllocErrorType::NoMemory, "Try defraq");
}


void Allocator::defrag(int test) {
    int i = 0;
    for(cptr *cur = static_cast<cptr*>(buf.end) -> next; cur != nullptr; cur = cur->next){
        if(test == 213) cout << cur << endl;
        if(cur->start != nullptr) {
            if (cur->start > cur->pred->end) {
                size_t sz = static_cast<char*>(cur->end) - static_cast<char*>(cur->start);
                memcpy(cur->pred->end, cur->start, sz);
                cur->start = cur->pred->end;
                cur->end = static_cast<char*>(cur->start) + sz;
            }
        }
        else{
            cur->end = cur->pred->end;
        }
    }
}
