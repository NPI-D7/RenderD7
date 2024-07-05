#include <memory>
#include <renderd7/Tasks.hpp>
#include <thread>
#include <vector>

static std::vector<std::shared_ptr<std::thread>> threads;

int RenderD7::Tasks::Create(std::function<void()> fun) {
  auto thrd = std::make_shared<std::thread>(fun);
  threads.push_back(thrd);
  return threads.size();
}

void RenderD7::Tasks::DestroyAll() {
  for (auto& it : threads) {
    if (it && it->joinable()) {
      it->join();
    }
  }
  // Delete Pointers
  threads.clear();
}
