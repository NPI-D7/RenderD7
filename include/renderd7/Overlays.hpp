#pragma once

#include <renderd7/Ovl.hpp>
#include <string>

namespace RenderD7 {
class Ovl_Ftrace : public RenderD7::Ovl {
 public:
  /// @brief Constructor
  Ovl_Ftrace(bool* is_enabled);
  /// @brief Override for Draw
  void Draw(void) const override;
  /// @brief Override for Logic
  void Logic() override;

 private:
  bool* i_is_enabled;
};

class Ovl_Metrik : public RenderD7::Ovl {
 public:
  /// @brief Constructor
  Ovl_Metrik(bool* is_enabled, bool* screen, uint32_t* mt_color,
             uint32_t* txt_color, float* txt_size);
  /// @brief Override for Draw
  void Draw(void) const override;
  /// @brief Override for Logic
  void Logic() override;

 private:
  // Mutable internal values
  mutable std::string mt_fps;
  mutable std::string mt_cpu;
  mutable std::string mt_gpu;
  mutable std::string mt_cmd;
  mutable std::string mt_lfr;
  mutable std::string mt_tbs;

  // Importand Adresses
  bool* i_is_enabled;
  bool* i_screen;
  uint32_t* i_mt_color;
  uint32_t* i_txt_color;
  float* i_txt_size;
};

typedef int RD7Keyboard;

enum RD7Keyboard_ {
  RD7Keyboard_Default,
  RD7Keyboard_Numpad,
  RD7Keyboard_Password,
};

class Ovl_Keyboard : public RenderD7::Ovl {
 public:
  /// @brief Constructor
  /// Keyboard Type not Supported for now
  Ovl_Keyboard(std::string& ref, const std::string& hint = "",
               RD7Keyboard type = 0);
  /// @brief Deconstructor
  ~Ovl_Keyboard();
  /// @brief Override for Draw
  void Draw(void) const override;
  /// @brief Override for Logic
  void Logic() override;

 private:
  mutable std::map<unsigned char, char> shared_data;
  // Pointer to useres String
  std::string* typed_text = nullptr;
  std::string str_bak;
  int mode = 0;
  int ft3 = 0;
};
}  // namespace RenderD7