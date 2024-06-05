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

#include <renderd7/Hid.hpp>
#include <renderd7/ThemeEditor.hpp>
#include <renderd7/UI7.hpp>

std::map<RD7Color, std::string> color_names = {
    {RD7Color_Background, "Background"},
    {RD7Color_Button, "Button"},
    {RD7Color_ButtonActive, "ButtonActive"},
    {RD7Color_ButtonDisabled, "ButtonDisabled"},
    {RD7Color_ButtonHovered, "ButtonHovered"},
    {RD7Color_Checkmark, "Checkmark"},
    {RD7Color_FrameBg, "FrameBg"},
    {RD7Color_FrameBgHovered, "FrameBgHovered"},
    {RD7Color_Header, "Header"},
    {RD7Color_List0, "List0"},
    {RD7Color_List1, "List1"},
    {RD7Color_MessageBackground, "Message Background"},
    {RD7Color_Progressbar, "Progressbar"},
    {RD7Color_Selector, "Selector"},
    {RD7Color_SelectorFade, "SelectorFade"},
    {RD7Color_Text2, "Text2"},
    {RD7Color_Text, "Text"},
    {RD7Color_TextDisabled, "Text Disabled"},
};

RenderD7::ThemeEditor::ThemeEditor() {
  // Backup active Theme and create New one to edit
  temp_theme = RenderD7::ThemeActive();
  edit_theme = RenderD7::Theme::New();
  edit_theme->Default();
  RenderD7::ThemeSet(edit_theme);
}

RenderD7::ThemeEditor::~ThemeEditor() {
  // Set Back to Acrive Theme
  RenderD7::ThemeSet(temp_theme);
}

void RenderD7::ThemeEditor::Draw() const {
  RenderD7::OnScreen(Top);
  if (UI7::BeginMenu("Theme Editor")) {
    UI7::Label("Sample Text");
    UI7::Checkbox("Checkbox", cm);
    UI7::InputText("Input Text", inpt, "Input Text");
    UI7::Button("Button");
    UI7::Progressbar(0.5f);
    UI7::ColorSelector("Color Selector",
                       edit_theme->GetTableRef()[RD7Color_Progressbar]);
    UI7::EndMenu();
  }
  RenderD7::OnScreen(Bottom);
  if (UI7::BeginMenu("Theme", R7Vec2(), UI7MenuFlags_Scrolling)) {
    if (menu == 0) {
      for (auto& it : color_names) {
        UI7::ColorSelector(it.second, edit_theme->GetTableRef()[it.first]);
      }
    } else if (menu == 1) {
    }
    UI7::EndMenu();
  }
}

void RenderD7::ThemeEditor::Logic() {
  if (Hid::IsEvent("cancel", Hid::Down)) {
    RenderD7::Scene::Back();
  }
}