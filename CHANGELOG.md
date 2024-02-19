# RenderD7 Changelog
## 0.9.5
- Remove Npi Intro and NVID Api
- Replace Toasts System with Message 
- Lots of Optimisations
- Added GetTime
- Implement Draw2 Api
- Implement RD7Color and R7Vec2
- Add new Features to Color::RGBA
- Cleanup Code
- Added RenderD7 Keyboard (Overlay)
- Added Ftrace Overlay
- Moved MetrikOVL into an Overlay
- Removed Old Font/Text Handlers
- Added UI7 (New UI Api)
- Remove Old UI Api
- Added Music Player (MP3, Vorbis)
- Rewrite of RenderD7::Image
- Internal Debugger/Database (IDB)
- Removed BitmapPrinter
- Added nimg and nimg_engine
- Removed Old Error/Message Handler
- GetTextSize (extra buffer) + New TextShorter
- Require specific FLAG for Music/MemTrack
- Replace all lodepng usage by stb_image
- Move Asset Generator into single python script
- Remove INI reader
- Python based clangformat script
- Move some Init code into functions to not use twice
## 0.9.4
- Implement new Security System To prevent from crashes
- Implement Functiontrace for better Timing Tests
- Implement MemAlloc Tracker (only size)
- Add some new Overlays (not functional yet)
- Complete Rewrite of Overlay System
- Fixed the FrameEnd Crash
- New System to get Hardware Info
- Removed RenderD7 Super Reselution (800px mode)
## 0.9.3
- Completly Documented Everything
- Fix typo in Sprite::getHeight
- Remove Deprecated/Useless Stuff
## 0.9.2
- Add Nvid Support(v0.0.1)
- Add Basic RenderD7 Splash
- Faster Graphics Init
- Fade Effects
- Fix Changelog Screen 
## 0.9.1
- Fix Critical bug in Spritesheet animations
- Fix RenderD7::Color::Hex (Major performance tweak)
## 0.9.0
- Remove Stupid try of Console
- Add Services list 
- Clean up Code
- Added Minimal Init for hax2.x
## 0.8.5
- Fix Deltatime
## 0.8.4
- A lot of Fixes 
- New Features for BitmapPrinter
## 0.8.3
- Added Overlaycount to Info
- Addet ResultDecoder for errors
## 0.8.2
- Fix a lot of Stuff
- Use c++17 std::filesystem for RenderD7::Filesystem
## 0.8.1
- Add abillity to Get Stdout as string to render it to the screen.
## 0.8.0
- Implement BitmapPrinter
## 0.7.3
- Implement Over Render Overlay Framework
## 0.7.2
- Implement MT to csv file saving
- Add RGB2HEX
## 0.7.1
- Add the New Overlay Handler. Its Just in code and does nothing yet
## 0.7.0
- Made Big Progress In the MT Ovl but it still crashes On a 2nd C3D_FrameEnd                                                            \
- Implement 800px but doesn't work that good
## 0.6.2
- Fix Crash when exiting through Home Menu.
## 0.6.10
- Rewrite Threadsystem
- Improve framerate
## 0.6.02
- Fix Code in lang.hpp
- Add Draw Text Left Function (Right since 0.7.0)
- Add changelog
## 0.6.01
- Add Threading system
## 0.6.0
- Better Scene Management
## 0.5.0
- Fixed some Bugs!
## 0.4.0 
- Trying to fix Filesystem and Bugs
## 0.3.0
- Recreate D7-Core into RenderD7
## 0.2.0
- Trying to create Animations of Images instead of Sheets
## 0.1.0
- Inital Release of D7-Core sprite animation plugin