/**
 *   This file is part of RenderD7
 *   Copyright (C) 2021-2024 NPI-D7, tobid7
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <3ds.h>

#include <memory>
#include <renderd7/Error.hpp>

// Write own LinearAllocator for learning

namespace RenderD7 {
template <typename T>
class LinearAllocator : public std::allocator<T> {
 public:
  typedef size_t size_type;
  typedef T* pointer;
  typedef const T* const_pointer;

  template <typename T1>
  struct rebind {
    typedef LinearAllocator<T1> other;
  };

  pointer allocate(size_type n, const void* hint = nullptr) {
    if (n > this->max_size()) {
      RenderD7::Error(
          "Linear Allocator: \nBad Alloc -> size is larger than free space!");
      return nullptr;
    }
    return (pointer)linearAlloc(n * sizeof(T));
  }

  void deallocate(pointer p, size_type) { linearFree((void*)p); }

  size_type max_size() { return linearSpaceFree(); }

  LinearAllocator() throw() {}
  LinearAllocator(const LinearAllocator<T>& a) throw() : std::allocator<T>(a) {}
  ~LinearAllocator() throw() {}
};
}  // namespace RenderD7