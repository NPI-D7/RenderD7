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

#pragma once
#include <map>
#include <memory>

namespace RenderD7 {
/// @brief The Overlay Class (Used for Toasts for example)
class Ovl {
 public:
  /// @brief Deconstructor
  virtual ~Ovl() {}
  /// @brief Function Called to Draw this
  virtual void Draw() const = 0;
  /// @brief Logic of the Overlay
  virtual void Logic() = 0;
  /// @brief Should the overlay be killed
  /// @return Killed or Not
  inline bool IsKilled() { return this->iskilled; }
  /// @brief Kill The Overlay
  inline void Kill() { iskilled = true; }

 private:
  /// @param iskilled For IsKilled();
  bool iskilled = false;
};
/// @brief Add an Overlay to the Screen
/// @param scene Overlay to push to Screen
void AddOvl(std::unique_ptr<RenderD7::Ovl> scene);
}  // namespace RenderD7
