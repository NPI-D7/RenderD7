#pragma once
#include <3ds.h>

namespace RenderD7 {
namespace Init {
void Security();
}

class Security {
 public:
  /// @brief Security Levels
  enum Level {
    NONE,      ///< Do Completly Nothing (excludes FrameEnd Security)
    FULL,      ///< Display Every Reports even Success
    ERRORS,    ///< Display Only Errors
    WARNINGS,  ///< Display Errors and Warnings
    LOG,       ///< Log Every Error with Detailed Information
  };

  Security();
  ~Security();
  /// @brief Set the Security Level
  /// @param level Level to use
  void SetLevel(Level level);
  /// @brief Get Current Security Level
  /// @return Security Level
  Level GetLevel();
  /// @brief Call a Function at Program Crash/Exit
  /// @param exit_func Function to Call
  void SafeExit(void (*exit_func)());
  /// @brief SaveInit a Function and define a Exit Func
  /// @param init_func Init Function
  /// @param exit_func Exit Function
  void SafeInit(void (*init_func)(), void (*exit_func)());
  /// @brief SaveInit a Function and define a Exit Func
  /// @param init_func Init Function
  /// @param exit_func Exit Function
  void SafeInit(Result (*init_func)(), void (*exit_func)());
  /// @brief SaveInit a Function and define a Exit Func
  /// @param init_func Init Function
  /// @param exit_func Exit Function
  void SafeInit(void (*init_func)(), Result (*exit_func)());
  /// @brief SaveInit a Function and define a Exit Func
  /// @param init_func Init Function
  /// @param exit_func Exit Function
  void SafeInit(Result (*init_func)(), Result (*exit_func)());
};
}  // namespace RenderD7

/// @brief RenderD7 Security Object
extern RenderD7::Security *rd7_security;
