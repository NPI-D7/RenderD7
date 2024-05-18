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

#include <rd7.hpp>

class Sample : public RD7::Scene {
 public:
  enum State {
    State_Menu,
  };
  Sample();
  ~Sample();

  void Draw() const override;
  void Logic() override;

 private:
  // Just as the RD7 Settings Shows here as example
  // by using a mutale map you're able to process
  // requests from the const Draw function in the
  // Logic Function! you could also create lots of
  // mutable bool or make all Variables mutable but
  // i think seperating draw and logic is better
  // editable map  request_id    data/operation
  mutable std::map<unsigned int, unsigned int> shared_requests;
  // For Checkbox its best to use mutable
  mutable bool debug_background = false;
  mutable std::string search__ = "";
  mutable std::vector<std::string> names;
  mutable std::vector<std::string> files;
  mutable int sel;
  mutable float txt_size = 0.5f;
  State state = State_Menu;
};