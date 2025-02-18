#pragma once
/// c++ Includes
#include <algorithm>
#include <codecvt>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <random>
#include <stack>
#include <string>
#include <vector>
/// c includes
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
/// 3ds Includes
#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
/// RenderD7 Includes
#include <renderd7/Color.hpp>
#include <renderd7/FunctionTrace.hpp>
#include <renderd7/Hardware.hpp>
#include <renderd7/Memory.hpp>
#include <renderd7/Ovl.hpp>
#include <renderd7/ResultDecoder.hpp>
#include <renderd7/Screen.hpp>
#include <renderd7/Security.hpp>
#include <renderd7/Sheet.hpp>
#include <renderd7/Sprite.hpp>
#include <renderd7/SpriteAnimation.hpp>
#include <renderd7/Tasks.hpp>
#include <renderd7/Time.hpp>
#include <renderd7/external/lodepng.h>
#include <renderd7/ini.hpp>
#include <renderd7/lang.hpp>
#include <renderd7/parameter.hpp>
#include <renderd7/stringtool.hpp>
#include <renderd7/thread.hpp>

#define RENDERD7VSTRING "0.9.5"

#define DEFAULT_CENTER 0.5f

/// @param rd7_do_splash Config Value To Enable RenderD7 Splash
extern bool rd7_do_splash;
/// @param rd7_enable_memtrack Config Value to Track Mem Allocations
extern bool rd7_enable_memtrack;
/// @param rd7_max_objects Config Param for C2D Mac objects
extern int rd7_max_objects;
/// @param rd7_enable_scene_system Enable/Disable Scene System (for example for your own implementations)
extern bool rd7_enable_scene_system;

/// RenderD7
namespace RenderD7 {
/// @brief Get Deltatime
/// @return Deltatime
float GetDeltaTime();

/// @brief A Button
struct TObject {
  int x;                 ///< Position X
  int y;                 ///< Position Y
  int w;                 ///< Button Width
  int h;                 ///< Button Height
  std::string text = ""; ///< Text
  float txtsize = 0.7f;  ///< Set Text Size
};
/// @brief Scene Class
class Scene {
public:
  /// @brief Stack of the Scenes
  static std::stack<std::unique_ptr<Scene>> scenes;
  /// @brief Deconstructor
  virtual ~Scene() {}
  virtual void Logic() = 0;
  /// @brief Draw Func to Override
  virtual void Draw() const = 0;
  /// @brief Push a Scene to Stack
  /// @param scene Scene to Push
  /// @param fade FadeEffect (Not Correctly Implementet yet)
  static void Load(std::unique_ptr<Scene> scene, bool fade = false);
  /// @brief Go Back a Scene
  static void Back();
  /// @brief do the Draw (Called in RenderD7::MainLoop())
  static void doDraw();
  static void doLogic();
};

/// @brief Integrated Setting Menu of RenderD7
class RSettings : public RenderD7::Scene {
private:
  /// @brief State (Define for Menus)
  enum RState {
    RSETTINGS,
    RINFO,
    RSERVICES,
    RMCONFIG,
    RFTRACE,
    RUI7,
    RCREDITS
  };

  /// @param shared_request Defines requests from Draw to Logic
  /// As it is not planned to make Draw non const you'll need
  /// A map of data or bool values that are mutable ake
  /// editable by const functions
  mutable std::map<unsigned int, unsigned int> shared_request;
  /// @param m_state Current menu State (Default=MainMenu aka RSETTINGS)
  RenderD7::RSettings::RState m_state = RenderD7::RSettings::RState::RSETTINGS;

  /// @brief Position in FTrace Menu
  int ftrace_index = 0;

  /// @param mtovlstate State of Metricks Overlay
  std::string mtovlstate = "false";
  /// @param mtscreenstate Screen the Overlay is Set to
  std::string mtscreenstate = "Top";
  std::string kbd_test;
  bool statemtold = false;
  bool stateftold = false;

public:
  /// @brief Constructor
  RSettings();
  /// @brief Override for Draw
  /// @param
  void Draw(void) const override;
  /// @brief Deconstructor
  ~RSettings();
  void Logic() override;
};

/// @brief Show Up the RenderD7-Settings Menu
void LoadSettings();
/// @brief Get's The Programs Time running
/// @return Time Running
float GetTime();
/// @brief Get Framerate as Number
/// @return FPS
int GetFps();

/// @brief Get A Rendom Int
/// @param b From
/// @param e To
/// @return Random Int
int GetRandomInt(int b, int e);
/// @brief DrawMetrikOvl (YOUR OWN RISK)
void DrawMetrikOvl();
/// @brief Draw Image from RenderD7 Sheet
/// @param sheet Spritesheet
/// @param index Image index Value
/// @param x Pos X
/// @param y Pos Y
/// @param scaleX Scale on X-Axis
/// @param scaleY Scale on Y-Axis
/// @return success ?
bool DrawImageFromSheet(RenderD7::Sheet *sheet, size_t index, float x, float y,
                        float scaleX = 1.0, float scaleY = 1.0);
/// @brief Fade In
/// @param duration Duration in Frames
void FadeIn();
/// @brief Fade Out
/// @param duration Duration in Frames
void FadeOut();
/// @brief Display Fade Effects
void FadeDisplay();

/// @brief Loads a font
/// @param path path to font (bcfnt)
/// @return link to C2D_Font Object
C2D_Font LoadFont(const std::string& path);

namespace Init {
/// @brief Init Default RenderD7
/// @param app_name Name of Your App
/// @return ResCode
Result Main(std::string app_name = "RD7Game");
/// @brief Init Minimal RenderD7 (For better Hax2.x support)
/// @param app_name Name of Your App
/// @return ResCode
Result Minimal(std::string app_name = "RD7Game");
/// @brief Reload the Graphics Engine
/// @return ResCode
Result Reload();
/// @brief Init Graphics Only (NOT SUPPORTET use Reload)
void Graphics();
/// @brief Init Ndsp for Sounds
void NdspFirm();
} // namespace Init

namespace Convert {
/// @brief Convert a String to Flaot
/// @param inp Input String
/// @return Float
inline float StringtoFloat(std::string inp) { return std::atof(inp.c_str()); }
/// @brief Convert String to Int
/// @param inp Input String
/// @return Int
inline int StringtoInt(std::string inp) { return std::atoi(inp.c_str()); }
/// @brief Convert a Float to Bool
/// @param inp Input Float
/// @return Bool
inline bool FloatToBool(float inp) { return (inp == 1 ? true : false); }
} // namespace Convert

namespace FS {
/// @brief Check if File exists
/// @param path Path to the File
/// @return exists or not
bool FileExist(const std::string &path);
} // namespace FS

/// @brief Check if Ndsp is Init
/// @return is or not
bool IsNdspInit();
/// @brief Get Current Framerate as String
/// @return Framerate String
std::string GetFramerate();
/// @brief MainLoop of RenderD7s
/// @return Is Still Running or not
bool MainLoop();
/// @brief Exit App (brak the MainLoop)
void ExitApp();

/// @brief Clear the Citro2D TextBuffers
/// @param
void ClearTextBufs(void);

/// @brief Draw Overlays And end the Frame. DO NEVER USE C3D_FRAMEEND cause it
/// breaks Overlay crash Security
void FrameEnd();

/// @brief Returns App Working Directory path
/// @return AppDir Path
std::string GetAppDirectory();
} // namespace RenderD7
