#pragma once

#include <memory>
#include <3ds.h>

#include <renderd7/Error.hpp>

// Write own LinearAllocator for learning

namespace RenderD7 {
    template<typename T>
    class LinearAllocator : public std::allocator<T> {
        public:
        typedef size_t size_type;
        typedef T* pointer;
        typedef const T* const_pointer;

        template<typename T1>
        struct rebind {typedef LinearAllocator<T1> other;};

        pointer allocate(size_type n, const void* hint=nullptr) {
            if(n > this->max_size()) {
                RenderD7::Error("Linear Allocator: \nBad Alloc -> size is larger than free space!");
                return nullptr;
            }
            return (pointer)linearAlloc(n*sizeof(T));
        }

        void deallocate(pointer p, size_type) {
            linearFree((void*)p);
        }

        size_type max_size() {
            return linearSpaceFree();
        }

        LinearAllocator() throw() {}
        LinearAllocator(const LinearAllocator<T>& a) throw(): std::allocator<T>(a) {}
        ~LinearAllocator() throw() {}
    };
}