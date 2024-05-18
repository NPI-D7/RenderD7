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
// clang-format off
#include <string>
#include <renderd7/external/json.hpp>
// clang-format on

namespace RenderD7 {
namespace Lang {
/// @brief Get 3ds System lang! [en] by default
/// @return Sytemlang as string
std::string GetSys();
/// @brief Get The Translation String
/// @param key Key of Translation
/// @return The Translated String
std::string Get(const std::string &key);
/// @brief Load A Language json
/// @param lang The Language Key [en], [de], etc, or getSys()
void Load(const std::string &lang);
// New funcs
std::string GetName();
std::string GetAuthor();
std::string GetShortcut();
}  // namespace Lang
}  // namespace RenderD7
