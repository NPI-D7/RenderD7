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

#include <3ds.h>
#include <stdio.h>
#include <string.h>

#include <renderd7/Tasks.hpp>
#include <vector>

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
