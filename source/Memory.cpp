#include <cstdlib>
#include <map>
#include <renderd7/Memory.hpp>

static RenderD7::Memory::memory_metrics metrics;

bool rd7i_enable_memtrack;

void *operator new(size_t size) {
  void *ptr = malloc(size);
  if (rd7i_enable_memtrack) metrics.t_TotalAllocated += size;
  return ptr;
}

void operator delete(void *memory, size_t size) {
  if (rd7i_enable_memtrack) metrics.t_TotalFreed += size;
  free(memory);
}

int allocations = 0;
int total_size = 0;
std::map<void *, size_t> sizes;

void *operator new[](size_t size) {
  void *ptr = malloc(size);
  if (rd7i_enable_memtrack) {
    allocations++;
    total_size += size;
    sizes[ptr] = size;
    metrics.t_TotalAllocated += size;
  }

  return ptr;
}

void operator delete[](void *ptr) {
  if (rd7i_enable_memtrack) {
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
