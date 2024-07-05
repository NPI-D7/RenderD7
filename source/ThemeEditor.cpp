#include <filesystem>
#include <renderd7/Hid.hpp>
#include <renderd7/Message.hpp>
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
    {RD7Color_Text2, "Text Light"},
    {RD7Color_Text, "Text Dark"},
    {RD7Color_TextDisabled, "Text Disabled"},
};

RenderD7::ThemeEditor::ThemeEditor() {
  // Backup active Theme and create New one to edit
  temp_theme = RenderD7::ThemeActive();
  edit_theme = RenderD7::Theme::New();
  edit_theme->CopyOther(temp_theme);
  RenderD7::ThemeSet(edit_theme);
}

RenderD7::ThemeEditor::~ThemeEditor() {
  // Set Back to Acrive Theme
  RenderD7::ThemeSet(temp_theme);
}

void RenderD7::ThemeEditor::Draw() const {
  RenderD7::R2()->OnScreen(R2Screen_Top);
  if (UI7::BeginMenu("RenderD7 -> Theme Editor")) {
    UI7::Label("Sample Text");
    UI7::Checkbox("Checkbox", cm);
    UI7::InputText("Input Text", inpt, "Input Text");
    UI7::Button("Button");
    UI7::Progressbar(0.5f);
    UI7::ColorSelector("Color Selector",
                       edit_theme->GetTableRef()[RD7Color_Progressbar]);
    UI7::EndMenu();
  }
  RenderD7::R2()->OnScreen(R2Screen_Bottom);
  if (UI7::BeginMenu("Theme", R7Vec2(), UI7MenuFlags_Scrolling)) {
    if (menu == 0) {
      if (UI7::Button("Create New")) {
        menu = 1;
        edit_theme->Default();
      } else if (UI7::Button("Edit Current")) {
        menu = 1;
      } else if (UI7::Button("Select Theme")) {
        menu = 2;
        theme_list.clear();
        for (const auto& it : std::filesystem::directory_iterator(
                 RenderD7::GetAppDirectory() + "/themes")) {
          theme_list.push_back(it.path().filename().string());
        }
      }
    } else if (menu == 1) {
      if (UI7::Button("Go back")) {
        edit_theme->CopyOther(temp_theme);
        menu = 0;
      } else if (UI7::Button("Save")) {
        RenderD7::AddOvl(std::make_unique<Ovl_Keyboard>(kbd_text, kbd_state,
                                                        "<name>.theme"));
      }
      for (auto& it : color_names) {
        UI7::ColorSelector(it.second, edit_theme->GetTableRef()[it.first]);
      }
    } else if (menu == 2) {
      if (UI7::Button("Go back")) {
        menu = 0;
      }
      for (auto& it : theme_list) {
        if (UI7::Button(it)) {
          edit_theme->Load(RenderD7::GetAppDirectory() + "/themes/" + it);
          menu = 1;
        }
        UI7::SameLine();
        if (UI7::Button("Make Current")) {
          edit_theme->Load(RenderD7::GetAppDirectory() + "/themes/" + it);
          temp_theme->CopyOther(edit_theme);
          menu = 0;
        }
        UI7::SameLine();
        if (UI7::Button("Delete")) {
          if (std::string(it) != "renderd7.theme") {
            std::filesystem::remove(RenderD7::GetAppDirectory() + "/themes/" +
                                    it);
            theme_list.clear();
            for (const auto& it : std::filesystem::directory_iterator(
                     RenderD7::GetAppDirectory() + "/themes")) {
              theme_list.push_back(it.path().filename().string());
            }
          } else {
            RenderD7::PushMessage("ThemeEditor",
                                  "Cannot Delete\nrenderd7.theme!");
          }
        }
      }
    }
    UI7::EndMenu();
  }
}

void RenderD7::ThemeEditor::Logic() {
  if (kbd_state) {
    if (kbd_state == RD7KeyboardState_Confirm) {
      auto path =
          RenderD7::GetAppDirectory() + "/themes/" + kbd_text + ".theme";
      kbd_text = "";
      if (std::filesystem::exists(path)) {
        // Prompt Override
        return;
      }
      edit_theme->Save(path);
    }
    kbd_state = RD7KeyboardState_None;
  }
  if (Hid::IsEvent("cancel", Hid::Down)) {
    if (menu == 0) {
      RenderD7::Scene::Back();
    } else {
      if (menu == 1) edit_theme->CopyOther(temp_theme);
      menu = 0;
    }
  }
}