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

#include <renderd7/thread.hpp>
namespace RenderD7 {
Thread::Thread() : m_started(false), m_running(false) { /* do nothing */
}

Thread::Thread(std::function<void(RenderD7::Parameter)> t_function,
               RenderD7::Parameter t_parameter, bool t_autostart,
               bool t_detached, unsigned long long int t_stackSize)
    : m_started(false), m_running(false) {
  initialize(t_function, t_parameter, t_autostart, t_detached, t_stackSize);
}

Thread::~Thread() {
  join();

  if (m_started) threadFree(m_thread);
}

void Thread::initialize(std::function<void(RenderD7::Parameter)> t_function,
                        RenderD7::Parameter t_parameter, bool t_autostart,
                        bool t_detached, unsigned long long int t_stackSize) {
  m_stackSize = t_stackSize;
  m_data.m_parameter = t_parameter;
  m_data.m_function = t_function;
  m_data.m_running = &m_running;

  if (t_autostart) {
    start(t_detached);
  }
}

void Thread::setStackSize(unsigned long long int t_stackSize) {
  m_stackSize = t_stackSize;
}

void Thread::start(bool t_detached) {
  if (!m_running) {
    m_started = true;
    m_running = true;
    s32 prio;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    m_thread = threadCreate(threadFunction, &m_data, m_stackSize, prio + 1, -2,
                            t_detached);
  }
}

void Thread::kill() {
  threadDetach(m_thread);
  m_running = false;
  m_started = false;
}

void Thread::join(long long unsigned int t_timeout) {
  if (m_running) {
    threadJoin(m_thread, t_timeout);
    threadFree(m_thread);
    m_running = false;
    m_started = false;
  }
}

bool Thread::isRunning() { return m_running; }

void Thread::sleep() { svcSleepThread(0); }

void Thread::sleep(int t_milliseconds) {
  svcSleepThread(1000000 * t_milliseconds);
}

// private methods
void Thread::threadFunction(void *arg) {
  RenderD7::Thread::ThreadData data =
      *static_cast<RenderD7::Thread::ThreadData *>(arg);
  data.m_function(data.m_parameter);
  *data.m_running = false;
}
}  // namespace RenderD7
