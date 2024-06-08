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

#include <cstdlib>
#include <map>
#include <renderd7/Memory.hpp>

static RenderD7::Memory::memory_metrics metrics;

bool rd7_enable_memtrack;

void *operator new(size_t size) {
  void *ptr = malloc(size);
  if (rd7_enable_memtrack) metrics.t_TotalAllocated += size;
  return ptr;
}

void operator delete(void *memory, size_t size) {
  if (rd7_enable_memtrack) metrics.t_TotalFreed += size;
  free(memory);
}

int allocations = 0;
int total_size = 0;
std::map<void *, size_t> sizes;

void *operator new[](size_t size) {
  void *ptr = malloc(size);
  if (rd7_enable_memtrack) {
    allocations++;
    total_size += size;
    sizes[ptr] = size;
    metrics.t_TotalAllocated += size;
  }

  return ptr;
}

void operator delete[](void *ptr) {
  if (rd7_enable_memtrack) {
    allocations--;
    total_size -= sizes[ptr];
    metrics.t_TotalFreed += sizes[ptr];
    sizes.erase(ptr);
  }
  free(ptr);
}

namespace RenderD7 {

namespace Memory {

size_t GetTotalAllocated() { return metrics.t_TotalAllocated; }

size_t GetTotalFreed() { return metrics.t_TotalFreed; }

size_t GetCurrent() { return metrics.t_CurrentlyAllocated(); }
}  // namespace Memory
}  // namespace RenderD7
