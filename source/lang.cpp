#include <3ds.h>
#include <renderd7/lang.hpp>
#include <fstream>
#include <filesystem>

static nlohmann::json appJson;

std::string RenderD7::Lang::getSys() {

  u8 language = 1;
  CFGU_GetSystemLanguage(&language);

  switch (language) {
  case 0:
    return "jp"; // Japanese
    break;

  case 1:
    return "en"; // English
    break;

  case 2:
    return "fr"; // French
    break;

  case 3:
    return "de"; // German
    break;

  case 4:
    return "it"; // Italian
    break;

  case 5:
    return "es"; // Spanish
    break;

  case 6:
    return "zh-CN"; // Chinese (Simplified)
    break;

    // case 7:
    // 	return "ko"; // Korean
    // 	break;

    // case 8:
    // 	return "nl"; // Dutch
    // 	break;

  case 9:
    return "pt"; // Portuguese
    break;

  case 10:
    return "ru"; // Russian
    break;

  case 11:
    return "zh-TW"; // Chinese (Traditional)
    break;

  default:
    return "en"; // Fall back to English if missing
    break;
  }
}
std::string RenderD7::Lang::get(const std::string &key) {
  if (!appJson.contains(key))
    return key;

  return appJson.at(key).get<std::string>();
}

void RenderD7::Lang::load(const std::string &lang) {
  std::fstream values;

  if(std::filesystem::is_character_file("romfs:/lang/" + lang + "/app.json")) {
    values.open("romfs:/lang/" + lang + "/app.json", std::ios::in);
    if(values.is_open()) {
      appJson = nlohmann::json::parse(values);
    }
    values.close();
    if(appJson.is_discarded()) {
      appJson = {};
    }
    return;
  } else {
    values.open("romfs:/lang/en/app.json", std::ios::in);
    if(values.is_open()) {
      appJson = nlohmann::json::parse(values);
    }
    values.close();
    if(appJson.is_discarded()) {
      appJson = {};
    }
    return;
  }
}