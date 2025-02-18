#include <renderd7/Security.hpp>

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <renderd7/internal_db.hpp>

RenderD7::Security *rd7_security;

bool *running_addr = NULL;

struct SecurityReport {
  uint32_t addr;
  std::string initialisized_at;
  bool success;
};

std::vector<SecurityReport> rd7_security_reports;

RenderD7::Security::Level RenderD7::Security::GetLevel() {
  return RenderD7::Security::Level::FULL;
}

void RenderD7::Security::SetLevel(RenderD7::Security::Level level) {}

void RenderD7::Security::Report(uint32_t addr, void *result_ptr) {
  // Create Report
}
namespace RenderD7 {
Security::Security() { running_addr = &rd7i_running; }

Security::~Security() { *running_addr = false; }
void Security::SafeExit(void (*exit_func)()) { atexit(exit_func); }

void Security::SafeInit(void (*init_func)(), void (*exit_func)()) {
  init_func();
  atexit(exit_func);
}

void Security::SafeInit(Result (*init_func)(), void (*exit_func)()) {
  init_func();
  atexit(exit_func);
}

void Security::SafeInit(void (*init_func)(), Result (*exit_func)()) {
  init_func();
  atexit(reinterpret_cast<void (*)()>(exit_func));
}

void Security::SafeInit(Result (*init_func)(), Result (*exit_func)()) {
  init_func();
  atexit(reinterpret_cast<void (*)()>(exit_func));
}
} // namespace RenderD7
namespace RenderD7 {
namespace Init {
void Security() {
  rd7_security = new RenderD7::Security(); // Load and Init the Security System
}
} // namespace Init
} // namespace RenderD7