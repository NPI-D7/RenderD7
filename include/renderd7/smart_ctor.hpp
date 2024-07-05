#pragma once

#include <memory>

#define RD7_SMART_CTOR(type)                                     \
  using Ref = std::shared_ptr<type>;                             \
  template <typename... args>                                    \
  static Ref New(args&&... cargs) {                              \
    return std::make_shared<type>(std::forward<args>(cargs)...); \
  }
