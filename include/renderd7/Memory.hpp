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

#include <cstddef>

namespace RenderD7 {
namespace Memory {
/// @brief Metriks struct For the Internal Tracker
struct memory_metrics {
  uint32_t t_TotalAllocated = 0;  ///< Total Allocated Memory
  uint32_t t_TotalFreed = 0;      ///< Total Deleted Memory
  /// @brief Gets the Currently Allocated Memory
  uint32_t t_CurrentlyAllocated() { return t_TotalAllocated - t_TotalFreed; }
};
/// @brief Get Total Allocated Memory
/// @return Total Allocated Memory
size_t GetTotalAllocated();
/// @brief Get Total Deleted Memory
/// @return Total Deleted Memory
size_t GetTotalFreed();
/// @brief Get Current Allocated Memory
/// @return Current Allocated Memory
size_t GetCurrent();
}  // namespace Memory
}  // namespace RenderD7
