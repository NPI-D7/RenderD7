#include <3ds.h>
#include <stdio.h>
#include <string.h>

#include <renderd7/Tasks.hpp>

static std::vector<Thread> threads;

void RenderD7::Tasks::create(ThreadFunc entrypoint) {
  s32 prio = 0;
  svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
  Thread thread = threadCreate((ThreadFunc)entrypoint, NULL, 64 * 1024,
                               prio - 1, -2, false);
  threads.push_back(thread);
}

void RenderD7::Tasks::destroy(void) {
  for (u32 i = 0; i < threads.size(); i++) {
    threadJoin(threads.at(i), U64_MAX);
    threadFree(threads.at(i));
  }
}