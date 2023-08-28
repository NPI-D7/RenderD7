# <img alt="LOGO" src="https://github.com/NPI-D7/RenderD7/blob/main/logo.png" height="90">
RenderD7 is now LibRenderD7.
### Installation
Download a Package From Releses Page
`https://github.com/NPI-D7/RenderD7/releases/download/v0.9.4/renderd7.tar.bz2 -o renderd7.tar.bz2`
Then Extract it to your Libraries Path
`bzip2 -cd renderd7.tar.bz2 | tar -xf - -C path_to_your_libs`
Finally put `-lrenderd7` to the First Place and add the path_to_your_libs
```
LIBS	:= -lrenderd7 -lcurl -lstdc++ -lm -lcitro2d -lcitro3d -lctru
#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(CTRULIB) ../path_to_your_libs
```
Make sure that `-lrenderd7` is before `-lcitro2d`, `-lcitro3d`, `-lctru`.
Here an example tree
```
Example-App
├── gfx
├── libs
│   ├── include
│   │   ├── rd7.hpp
│   │   └── renderd7
│   └── lib
│       ├── librenderd7.a
│       └── librenderd7d.a
├── Makefile
├── romfs
│   └── gfx
└── src
    └── main.cpp
```
# Back to Old Usage aka Submodule (0.9.5+)
If you want to use RenderD7 as Submodule run
`git submodule add https://github.com/NPI-D7/RenderD7`

And to Use Add this to make file
```
# Make Sure to Change this paths if your Submodule
# is located somewhere else
RENDERD7_SRC := RenderD7/source RenderD7/external
RENDERD7_INC := RenderD7/include
```
Now you need to add it to your sources and includes
```
SOURCES		:=	source $(RENDERD7_SRC)
INCLUDES	:=	source $(RENDERD7_INC)
```
Example from rd7tf
# Credits
- NPI-D7
  - Tobi-D7 Main Dev

Some Icons are From
https://icons8.de/
See Subfolder Readmes
