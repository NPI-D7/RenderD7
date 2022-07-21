#include "renderd7.hpp"
#include "log.hpp"
#include <regex>

#define RGBA8(r, g, b, a) ((((r) & 0xFF) << 0) | (((g) & 0xFF) << 8) | (((b) & 0xFF) << 16) | (((a) & 0xFF) << 24))
#define D7_NOTHING C2D_Color32(0, 0, 0, 0)
#define CFGVER "2"
Log renderd7log;
float animtime;
bool isndspinit = false;
bool running = true;
std::stack<std::unique_ptr<RenderD7::Scene>> RenderD7::Scene::scenes;
std::stack<std::unique_ptr<RenderD7::Ovl>> overlays;
bool usedbgmsg = false;
std::string dspststus = "Not Initialisized!";

int cobj___;
int maxobj__;

//INI::INIFile cfgfile;
std::unique_ptr<INI::INIFile> cfgfile = nullptr;
INI::INIStructure cfgstruct;
std::string cfgpath;

// RD7 SuperReselution
bool rd7_superreselution;
u8 consoleModel = 0;
u8 sysRegion = CFG_REGION_USA;
bool is_citra = false;
//---------------------------------------
bool rd7settings = false;

std::string D_app_name;

u32 d7_hDown;
u32 d7_hHeld;
u32 d7_hUp;
touchPosition d7_touch;

C2D_TextBuf TextBuf;
C2D_Font Font;

// Fps Calc
static float current_fps = 0.0f;
static unsigned int frames = 0;
static u64 last_time = 0;
float d11framerate = 0;
//-----------------

//Metrik-------------------------------------
u32 mt_color;
u32 mt_txtcolor;

std::string mt_fps;
std::string mt_cpu;
std::string mt_gpu;
std::string mt_cmd;

bool shouldbe_disabled = false;
int cnttttt = 0;
int mt_screen;
//int mt_width = mt_screen ? 320 : 400;
float mt_txtSize;
bool metrikd = false;
//double mt_fpsgraph[320];
std::vector<int> mt_fpsgraph(320);

//Metrik-CSV
std::string csvpc;
bool mt_dumpcsv = false; //Logs the Fps and stuff to csv. It saves every second to not loose performence.
bool mt_csvloop = false; //Saves In Every Frame but slows down performens. mt_dumpcsv must be enabled.
std::ofstream mt_csv;
std::string mt_cname;
//-------------------------------------------
bool currentScreen = false;

C3D_RenderTarget* Top;
C3D_RenderTarget* TopRight;
C3D_RenderTarget* Bottom;

#define DSEVENBLACK C2D_Color32(0, 0 ,0, 255)
#define DSEVENWHITE C2D_Color32(255, 255, 255, 255)

u64 delta_time;

//Screen Fade
bool fadeout = false, fadein = false, fadeout2 = false, fadein2 = false;
int fadealpha = 0;
int fadecolor = 0;


std::string _FMT_(const std::string& fmt_str, ...)
{
	va_list ap;
	char* fp = NULL;
	va_start(ap, fmt_str);
	vasprintf(&fp, fmt_str.c_str(), ap);
	va_end(ap);
	std::unique_ptr<char, decltype(free)*> formatted(fp, free);
	return std::string(formatted.get());
}

std::string Date(void)
{
	time_t unixTime;
	struct tm timeStruct;
	time(&unixTime);
	localtime_r(&unixTime, &timeStruct);
	return _FMT_("%04i-%02i-%02i_%02i-%02i-%02i", timeStruct.tm_year + 1900, timeStruct.tm_mon + 1, timeStruct.tm_mday,
		timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec);
}

void screenoff()
{
    gspLcdInit();\
    GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTH);\
    gspLcdExit();
}

void screenon()
{
    gspLcdInit();\
    GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_BOTH);\
    gspLcdExit();
}

RenderD7::SpriteSheetAnimation::SpriteSheetAnimation()
{
    renderd7log.Write("SpriteSheetAnimation createt!");
}
RenderD7::SpriteSheetAnimation::~SpriteSheetAnimation()
{
    //
}

float RenderD7::GetDeltaTime()
{
	return delta_time;
}

bool RenderD7::DrawImageFromSheet(RenderD7::Sheet* sheet, size_t index, float x, float y, float scaleX, float scaleY)
{
    if (sheet->spritesheet != nullptr)
    {
		if (C2D_SpriteSheetCount(sheet->spritesheet) >= index)
        {
			return C2D_DrawImageAt(C2D_SpriteSheetGetImage(sheet->spritesheet, index), x, y, 0.5f, nullptr, scaleX, scaleY);
		}
	}
    return false;
}
void RenderD7::Init::NdspFirm(bool useit)
{
    if (useit)
    {
        if ( access( "sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) 
        {
            ndspInit();
            isndspinit = true;
			dspststus = "Initialisized success!";
        }
        else
        {
			dspststus = "Not found: dspfirm.cdc";
            renderd7log.Write("RenderD7: SoundEngine Error! ndspfirm not found!");
			RenderD7::AddOvl(std::make_unique<RenderD7::DSP_NF>());
        }
    }
}
void RenderD7::Exit::NdspFirm()
{
    if (isndspinit)
    {
        ndspExit();
    }
}
void RenderD7::Msg::Display(std::string titletxt, std::string subtext, C3D_RenderTarget *target)
{
	shouldbe_disabled = true;
	cnttttt = 0;
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, DSEVENBLACK);
	C2D_TargetClear(Bottom, DSEVENBLACK);
    RenderD7::ClearTextBufs();
	RenderD7::OnScreen(Top);
	RenderD7::DrawRect(0, 0, 400, 240, RenderD7::Color::Hex("#111111"));
	RenderD7::OnScreen(Bottom);
	RenderD7::DrawRect(0, 0, 320, 240, RenderD7::Color::Hex("#111111"));
	RenderD7::OnScreen(target);
	RenderD7::DrawRect(0, 0, 400, 26, RenderD7::Color::Hex("#333333", 200));
    RenderD7::DrawText(5, 2, 0.7f, DSEVENWHITE, titletxt);
	RenderD7::DrawText(5, 30, 0.6f, DSEVENWHITE, subtext);
	C3D_FrameEnd(0);
}
void RenderD7::Msg::DisplayWithProgress(std::string titletext, std::string subtext, float current, float total, u32 prgbarcolor)
{
	shouldbe_disabled = true;
	cnttttt = 0;
	RenderD7::ClearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, DSEVENBLACK);
	C2D_TargetClear(Bottom, DSEVENBLACK);
	char str[256];
	snprintf(str, sizeof(str), "(%.2f%%)", ((float)current/(float)total) * 100.0f);
	RenderD7::OnScreen(Top);
	RenderD7::DrawRect(0, 0, 400, 240, RenderD7::Color::Hex("#111111"));
	RenderD7::DrawRect(0, 0, 400, 26, RenderD7::Color::Hex("#333333", 200));
	RenderD7::DrawText(5, 2, 0.7f, DSEVENWHITE, titletext);
	RenderD7::DrawText(5, 30, 0.6f, DSEVENWHITE, subtext);
	RenderD7::DrawRect(30, 120, 342, 30, RenderD7::Color::Hex("#333333"));
	RenderD7::DrawRect(31, 121, (int)(((float)current / (float)total) * 338.0f), 28, prgbarcolor);
	RenderD7::DrawTextCentered(5, 124, 0.7f, RenderD7::Color::Hex("#111111"), str, 390); 
	RenderD7::OnScreen(Bottom);
	RenderD7::DrawRect(0, 0, 320, 240, RenderD7::Color::Hex("#111111"));
	C3D_FrameEnd(0);
}
void RenderD7::SetupLog()
{
    renderd7log.Init("sdmc:/RenderD7.log");
}
void RenderD7::SpriteSheetAnimation::Setup(RenderD7::Sheet *sheet, size_t imagecount, size_t startimage, float frame_begin, float frame_finish)
{
    D_totaltime = frame_begin;
    renderd7log.Write("frame_begin success");
    this->images = imagecount;
    renderd7log.Write("imagecount success");
    this->sheet = sheet;
    renderd7log.Write("sheet success");
    this->time = frame_finish;
    renderd7log.Write("frame_finish success");
    RenderD7::SpriteSheetAnimation::FromSheet(this->sheet, startimage);
}
void RenderD7::SpriteSheetAnimation::Play(float timespeed)
{
    D_totaltime += timespeed;
    if (D_totaltime >= time)
    {
        D_totaltime -= time;
        imgs++;
        if (imgs == images)
        {
            imgs = 0;
        }
    }
    RenderD7::SpriteSheetAnimation::FromSheet(sheet, imgs);
    //RenderD7::SpriteSheetAnimation::Draw();
}

void RenderD7::Error::DisplayError(std::string toptext, std::string errortext, int timesec)
{
	shouldbe_disabled = true;
	cnttttt = 0;
    RenderD7::ClearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, DSEVENBLACK);
	C2D_TargetClear(Bottom, DSEVENBLACK);
	RenderD7::OnScreen(Top);
    RenderD7::DrawText(0, 0, 0.7f, DSEVENWHITE, toptext);
	RenderD7::DrawText(0, 30, 0.6f, DSEVENWHITE, errortext);
	C3D_FrameEnd(0);
    for (int i = 0; i < 60*timesec; i++) {
		RenderD7::DrawRect(0, 236, (int)(((float)i / (float)60*timesec) * 400.0f), 4, RenderD7::Color::Hex("#00ff00"));
		gspWaitForVBlank();
		
		
	}
}
void RenderD7::Error::DisplayFatalError(std::string toptext, std::string errortext)
{
	shouldbe_disabled = true;
	cnttttt = 0;
	bool error___ = true;
	RenderD7::ClearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, DSEVENBLACK);
	C2D_TargetClear(Bottom, DSEVENBLACK);
	RenderD7::OnScreen(Top);
    RenderD7::DrawTextCentered(0, 0, 0.7f, DSEVENWHITE, toptext, 400);
	RenderD7::DrawTextCentered(0, 100, 0.6f, DSEVENWHITE, errortext, 400);
	RenderD7::DrawTextCentered(0, 200, 0.6f, DSEVENWHITE, "Press Start to Exit!", 400);
	C3D_FrameEnd(0);
    while (error___)
	{
		if(d7_hDown & KEY_START)
		{
			RenderD7::ExitApp();
		}
    }
}
u32 RenderD7::Color::Hex(const std::string color, u8 a)
{
    if (color.length() < 7 || std::regex_search(color.substr(1), std::regex("[^0-9A-Fa-f]"))) { // invalid color.
		return D7_NOTHING;
	}
    int r = std::stoi(color.substr(1, 2), nullptr, 16);
	int g = std::stoi(color.substr(3, 2), nullptr, 16);
	int b = std::stoi(color.substr(5, 2), nullptr, 16);
	return RGBA8(r, g, b, a);
}

std::string RenderD7::Color::RGB2Hex(int r, int g, int b)
{
  	std::stringstream ss;
  	ss << "#";
  	ss << std::hex << (r << 16 | g << 8 | b );
  	return ss.str();
}

void RenderD7::Scene::doDraw() {
	if(!RenderD7::Scene::scenes.empty())
		RenderD7::Scene::scenes.top()->Draw();
}

void RenderD7::Scene::doLogic(u32 hDown, u32 hHeld, u32 hUp, touchPosition touch) {
	if(!RenderD7::Scene::scenes.empty())
		RenderD7::Scene::scenes.top()->Logic(hDown, hHeld, hUp, touch);
}

void RenderD7::Scene::Load(std::unique_ptr<Scene> scene, bool fade)
{ 
	Scene::scenes.push(std::move(scene));
}

void RenderD7::Scene::Back() {
	if(RenderD7::Scene::scenes.size() > 0)
		RenderD7::Scene::scenes.pop();
}

void RenderD7::OnScreen(C3D_RenderTarget *target)
{
    C2D_SceneBegin(target);
	currentScreen = (target == Top || target == TopRight) ? 1 : 0;
}

void frameloop()
{
	frames++;
	delta_time = osGetTime() - last_time;
	if (delta_time >= 1000) {
		current_fps = frames/(delta_time/1000.0f)+1;
		frames = 0;
		last_time = osGetTime();
	}
	d11framerate = current_fps;
	//for (int i = 0; i < 320; i++) mt_fpsgraph[i] = current_fps;

}
float getframerate()
{
	return d11framerate;
}

std::string RenderD7::GetFramerate()
{
	return (std::to_string((int)d11framerate).substr(0, 2));
}

bool RenderD7::MainLoop()
{
    if (!aptMainLoop()) return false;
    hidScanInput();
	d7_hDown = hidKeysDown();
	d7_hUp = hidKeysUp();
	d7_hHeld = hidKeysHeld();
	hidTouchRead(&d7_touch);

    RenderD7::ClearTextBufs();
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
    frameloop();
    RenderD7::Scene::doDraw();
    RenderD7::Scene::doLogic(d7_hDown, d7_hHeld, d7_hUp, d7_touch);
    cnttttt++;
	if (cnttttt > 90)
	{
		shouldbe_disabled = false;
		cnttttt = 0;
	}
	
    return running;
}


RenderD7::Sheet::Sheet()
{
    //
}
RenderD7::Sheet::~Sheet()
{
    //
}

Result RenderD7::Sheet::Load(const char *path)
{
	this->spritesheet = C2D_SpriteSheetLoad(path);
	return 0;
}

void RenderD7::Sheet::Free()
{
	C2D_SpriteSheetFree(this->spritesheet);
}

RenderD7::Sprite::Sprite()
{
//
}
RenderD7::Sprite::~Sprite()
{
	//
}
void RenderD7::Sprite::FromSheet(RenderD7::Sheet *sheet, size_t index)
{
	C2D_SpriteFromSheet(&this->sprite, sheet->spritesheet, index);
}
bool RenderD7::Sprite::Draw()
{
	return C2D_DrawSprite(&this->sprite);
}
void RenderD7::Sprite::SetCenter(float x, float y)
{
	C2D_SpriteSetCenter(&this->sprite, x, y);
}
void RenderD7::Sprite::SetPos(float x, float y)
{
	C2D_SpriteSetPos(&this->sprite, x, y);
}
void RenderD7::Sprite::SetRotation(float rotation)
{
	C2D_SpriteSetRotation(&this->sprite, rotation);
}
void RenderD7::Sprite::Rotate(float speed)
{
	C2D_SpriteRotateDegrees(&this->sprite, speed);
}
float RenderD7::Sprite::getHeigh()
{
	return this->sprite.params.pos.h;
}
float RenderD7::Sprite::getWidth()
{
	return this->sprite.params.pos.w;
}
float RenderD7::Sprite::getPosX()
{
	return this->sprite.params.pos.x;
}
float RenderD7::Sprite::getPosY()
{
	return this->sprite.params.pos.y;
}

void RenderD7::Sprite::FromImage(RenderD7::Image *img)
{
	C2D_SpriteFromImage(&this->sprite, img->img);
}

void RenderD7::Sprite::SetScale(float x, float y)
{
	C2D_SpriteScale(&this->sprite, x, y);
}

void RenderD7::ClearTextBufs(void)
{
	C2D_TextBufClear(TextBuf);
}

bool RenderD7::DrawRect(float x, float y, float w, float h, u32 color)
{
	return C2D_DrawRectSolid(x, y, 0.5f, w, h, color);
}

bool RenderD7::DrawPx(float x, float y, u32 color)
{
	return C2D_DrawRectSolid(x, y, 0.5f, 1, 1, color);
}

void RenderD7::DrawTextCentered(float x, float y, float size, u32 color, std::string Text, int maxWidth, int maxHeight, C2D_Font fnt) {
	float lineHeight, widthScale;

	// Check for the lineHeight.
	if (fnt != nullptr) {
		lineHeight = RenderD7::GetTextHeight(size, " ", fnt);
	} else {
		lineHeight = RenderD7::GetTextHeight(size, " ");
	}

	int line = 0;
	while(Text.find('\n') != Text.npos) {
		if (maxWidth == 0) {
			// Do the widthScale.
			if (fnt != nullptr) {
				widthScale = RenderD7::GetTextWidth(size, Text.substr(0, Text.find('\n')), fnt);
			} else {
				widthScale = RenderD7::GetTextWidth(size, Text.substr(0, Text.find('\n')));
			}
		} else {
			// Do the widthScale 2.
			if (fnt != nullptr) {
				widthScale = std::min((float)maxWidth, RenderD7::GetTextWidth(size, Text.substr(0, Text.find('\n')), fnt));
			} else {
				widthScale = std::min((float)maxWidth, RenderD7::GetTextWidth(size, Text.substr(0, Text.find('\n'))));
			}
		}
		if (fnt != nullptr) {
			RenderD7::DrawText((currentScreen ? 200 : 160)+x-(widthScale/2), y+(lineHeight*line), size, color, Text.substr(0, Text.find('\n')), maxWidth, maxHeight, fnt);
		} else {
			RenderD7::DrawText((currentScreen ? 200 : 160)+x-(widthScale/2), y+(lineHeight*line), size, color, Text.substr(0, Text.find('\n')), maxWidth, maxHeight);
		}

		Text = Text.substr(Text.find('\n')+1);
		line++;
	}

	if (maxWidth == 0) {
		// Do the next WidthScale.
		if (fnt != nullptr) {
			widthScale = RenderD7::GetTextWidth(size, Text.substr(0, Text.find('\n')), fnt);
		} else {
			widthScale = RenderD7::GetTextWidth(size, Text.substr(0, Text.find('\n')));
		}
	} else {
		// And again.
		if (fnt != nullptr) {
			widthScale = std::min((float)maxWidth, RenderD7::GetTextWidth(size, Text.substr(0, Text.find('\n')), fnt));
		} else {
			widthScale = std::min((float)maxWidth, RenderD7::GetTextWidth(size, Text.substr(0, Text.find('\n'))));
		}
	}
	if (fnt != nullptr) {
		RenderD7::DrawText((currentScreen ? 200 : 160)+x-(widthScale/2), y+(lineHeight*line), size, color, Text.substr(0, Text.find('\n')), maxWidth, maxHeight, fnt);
	} else {
		RenderD7::DrawText((currentScreen ? 200 : 160)+x-(widthScale/2), y+(lineHeight*line), size, color, Text.substr(0, Text.find('\n')), maxWidth, maxHeight);
	}
}

// Draw String or Text.
void RenderD7::DrawText(float x, float y, float size, u32 color, std::string Text, int maxWidth, int maxHeight, C2D_Font fnt) {
	C2D_Text c2d_text;

	if (fnt != nullptr) {
		C2D_TextFontParse(&c2d_text, fnt, TextBuf, Text.c_str());
	} else {
		C2D_TextFontParse(&c2d_text, Font, TextBuf, Text.c_str());
	}

	C2D_TextOptimize(&c2d_text);

	float heightScale;
	if (maxHeight == 0) {
		heightScale = size;
	} else {
		if (fnt != nullptr) {
			heightScale = std::min(size, size*(maxHeight/RenderD7::GetTextHeight(size, Text, fnt)));
		} else {
			heightScale = std::min(size, size*(maxHeight/RenderD7::GetTextHeight(size, Text)));
		}
	}

	if (maxWidth == 0) {
		C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, size, heightScale, color);
	} else {
		if (fnt != nullptr) {
			C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, std::min(size, size*(maxWidth/RenderD7::GetTextWidth(size, Text, fnt))), heightScale, color);
		} else {
			C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, std::min(size, size*(maxWidth/RenderD7::GetTextWidth(size, Text))), heightScale, color);
		}
	}
}
void RenderD7::DrawTextLeft(float x, float y, float size, u32 color, std::string Text, int maxWidth, int maxHeight, C2D_Font fnt)
{
	RenderD7::DrawText(x - RenderD7::GetTextWidth(size, Text, fnt), y, size, color, Text, maxWidth, maxHeight, fnt);
}
// Get String or Text Width.
float RenderD7::GetTextWidth(float size, std::string Text, C2D_Font fnt) {
	float width = 0;
	if (fnt != nullptr) {
		GetTextSize(size, &width, NULL, Text, fnt);
	} else {
		GetTextSize(size, &width, NULL, Text);
	}
	return width;
}

// Get String or Text Size.
void RenderD7::GetTextSize(float size, float *width, float *height, std::string Text, C2D_Font fnt) {
	C2D_Text c2d_text;
	if (fnt != nullptr) {
		C2D_TextFontParse(&c2d_text, fnt, TextBuf, Text.c_str());
	} else {
		C2D_TextFontParse(&c2d_text, Font, TextBuf, Text.c_str());
	}
	C2D_TextGetDimensions(&c2d_text, size, size, width, height);
}


// Get String or Text Height.
float RenderD7::GetTextHeight(float size, std::string Text, C2D_Font fnt) {
	float height = 0;
	if (fnt != nullptr) {
		GetTextSize(size, NULL, &height, Text.c_str(), fnt);
	} else {
		GetTextSize(size, NULL, &height, Text.c_str());
	}
	return height;
}

Result RenderD7::loadFont(C2D_Font &fnt, const char* Path) {
	fnt = C2D_FontLoad(Path); // Only load if found.
	return 0;
}

// Unload a Font.
Result RenderD7::unloadFont(C2D_Font &fnt) {
	if (fnt != nullptr) {
		C2D_FontFree(fnt); // Make sure to only unload if not nullptr.
	}
	return 0;
}

bool RenderD7::DrawCircle(float x, float y, float radius, u32 color)
{
	return C2D_DrawCircleSolid(x, y, 0.5f, radius, color);
}

void MetrikThread(RenderD7::Parameter param) {
    while (true) {
        RenderD7::DrawMetrikOvl();
        RenderD7::Thread::sleep(1000 * 1); // wait; also, this is needed to allow for concurrency (refer to the documentation for m3d::Thread::sleep())
    }
}

void RenderD7::Init::Graphics()
{
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(size_t(maxobj__));
	C2D_Prepare();
	Top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	TopRight = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
	Bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	TextBuf = C2D_TextBufNew(4096);
	Font = C2D_FontLoadSystem(CFG_REGION_USA);
}

void RenderD7::Exit::Graphics()
{
	C2D_TextBufDelete(TextBuf);
	C2D_Fini();
	C3D_Fini();
}

Result RenderD7::Init::Main(std::string app_name)
{
    gfxInitDefault();
	Result res = cfguInit();
	if (R_SUCCEEDED(res)) {
		CFGU_SecureInfoGetRegion(&sysRegion);
		CFGU_GetSystemModel(&consoleModel);
		cfguExit();
	}
	if (rd7_superreselution)
	{
		gfxSetWide(consoleModel != 3);
	}
	
    aptInit();
    romfsInit();
    cfguInit();
    if (cobj___){maxobj__ = cobj___;}
    if (!cobj___){maxobj__ = C2D_DEFAULT_MAX_OBJECTS;}
    D_app_name = app_name;
    cfgpath = "sdmc:/RenderD7/Apps/";
    cfgpath += D_app_name;
	csvpc = "sdmc:/RenderD7/Apps/";
	csvpc += D_app_name;
	csvpc += "/mt";
	mkdir("sdmc:/RenderD7/", 0777);
    mkdir("sdmc:/RenderD7/Apps", 0777);
    mkdir(cfgpath.c_str(), 0777);
	mkdir(csvpc.c_str(), 0777);
	bool renew = false;
	if (FS::FileExist(cfgpath + "/config.ini"))
	{
		cfgfile = std::make_unique<INI::INIFile>(cfgpath + "/config.ini");
		cfgfile->read(cfgstruct);
		std::string version = cfgstruct["info"]["version"];
		if (version != CFGVER) renew = true;
	}
	if (!FS::FileExist(cfgpath + "/config.ini") || renew)
	{
		cfgfile = std::make_unique<INI::INIFile>(cfgpath+ "/config.ini");
		cfgfile->read(cfgstruct);
		cfgstruct["info"]["version"] = CFGVER;
		cfgstruct["info"]["renderd7ver"] = RENDERD7VSTRING;
		cfgstruct["settings"]["doscreentimeout"] = "0";
		cfgstruct["settings"]["forcetimeoutLB"] = "1";
		cfgstruct["settings"]["forceFrameRate"] = "60";
		cfgstruct["settings"]["super-reselution"] = "0";
		cfgstruct["metrik-settings"]["enableoverlay"] = "0";
		cfgstruct["metrik-settings"]["Screen"] = "0";
		cfgstruct["metrik-settings"]["txtColor"] = "#ffffff";
		cfgstruct["metrik-settings"]["txtColorA"] = "255";
        cfgstruct["metrik-settings"]["ColorA"] = "255";
        cfgstruct["metrik-settings"]["Color"] = "#000000";
        cfgstruct["metrik-settings"]["txtSize"] = "0.7f";
		cfgstruct["metrik-settings"]["dumpcsv"] = "0";
		cfgstruct["metrik-settings"]["dumpcsvloop"] = "0";
		cfgfile->write(cfgstruct);
	}
	cfgfile = std::make_unique<INI::INIFile>(cfgpath+ "/config.ini");
	cfgfile->read(cfgstruct);
	std::string Fps = cfgstruct["settings"]["forceFrameRate"];
	C3D_FrameRate(RenderD7::Convert::StringtoFloat(Fps));
	metrikd = RenderD7::Convert::FloatToBool(RenderD7::Convert::StringtoFloat(cfgstruct["metrik-settings"]["enableoverlay"]));
	mt_txtcolor = RenderD7::Color::Hex(cfgstruct["metrik-settings"]["txtColor"], (u8)RenderD7::Convert::StringtoFloat(cfgstruct["metrik-settings"]["txtColorA"]));
    mt_color = RenderD7::Color::Hex(cfgstruct["metrik-settings"]["Color"], (u8)RenderD7::Convert::StringtoFloat(cfgstruct["metrik-settings"]["ColorA"]));
    mt_txtSize = RenderD7::Convert::StringtoFloat(cfgstruct["metrik-settings"]["txtSize"]);
    mt_screen = RenderD7::Convert::StringtoInt(cfgstruct["metrik-settings"]["Screen"]);
	rd7_superreselution = RenderD7::Convert::FloatToBool(RenderD7::Convert::StringtoFloat(cfgstruct["settings"]["super-reselution"]));
	mt_dumpcsv = RenderD7::Convert::FloatToBool(RenderD7::Convert::StringtoFloat(cfgstruct["metrik-settings"]["dumpcsv"]));
	mt_csvloop = RenderD7::Convert::FloatToBool(RenderD7::Convert::StringtoFloat(cfgstruct["metrik-settings"]["dumpcsvloop"]));
	//Check if citra
	s64 citracheck = 0;
	svcGetSystemInfo(&citracheck, 0x20000, 0);
	is_citra = citracheck ? true : false;
	//Speedup
    osSetSpeedupEnable(true);
	if (!is_citra && rd7_superreselution)
	{
		if (consoleModel != 3) gfxSetWide(true);
	}
	//consoleInit(GFX_BOTTOM, NULL);
	if (mt_dumpcsv)
	{
		mt_cname = csvpc;
		mt_cname += "/";
		mt_cname += Date();
		mt_cname += ".csv";
		
		FILE* logfile = fopen((mt_cname.c_str()), "w");
		fclose(logfile);
		
		mt_csv.open((mt_cname), std::ofstream::app);
		mt_csv << "FPS,CPU,GPU,CMD\n";
		mt_csv.close();
	}
	
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(size_t(maxobj__));
	C2D_Prepare();
	Top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	TopRight = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
	Bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	TextBuf = C2D_TextBufNew(4096);
	Font = C2D_FontLoadSystem(CFG_REGION_USA);
	
    //RenderD7::Msg::Display("RenderD7", "RenderD7 init success!\nWaiting for MainLoop!", Top);
	return 0;
}

Result RenderD7::Init::Reload()
{
	C2D_TextBufDelete(TextBuf);
	C2D_Fini();
	C3D_Fini();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(size_t(maxobj__));
	C2D_Prepare();
	Top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	TopRight = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
	Bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	TextBuf = C2D_TextBufNew(4096);
	Font = C2D_FontLoadSystem(CFG_REGION_USA);

	return 0;
}

void RenderD7::ToggleRD7SR()
{
	shouldbe_disabled = true;
	cnttttt = 0;
	// Display black screen
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, RenderD7::Color::Hex("#000000"));
	RenderD7::OnScreen(Top);
	C3D_FrameEnd(0);
	// Toggle 400px/800px mode
	gfxSetWide(gfxIsWide() ? false : true);
	rd7_superreselution = gfxIsWide();
	RenderD7::Init::Reload();
}

bool RenderD7::IsRD7SR()
{
	return gfxIsWide();
}

void RenderD7::Exit::Main()
{
	cfgfile->write(cfgstruct);
    if (RenderD7::Threads::threadrunning) RenderD7::Threads::Exit();
    C2D_TextBufDelete(TextBuf);
	C2D_Fini();
	C3D_Fini();
    aptExit();
	gfxExit();
    romfsExit();
    cfguExit();
	romfsExit();
}

void RenderD7::DrawTObjects(std::vector<RenderD7::TObject> tobjects, u32 color, u32 txtcolor, int selection, u32 selbgcolor, u32 selcolor)
{
	for(int i = 0; i < (int)tobjects.size(); i++)
	{
		if (selection == i)
		{
			RenderD7::DrawRect(tobjects[i].x - 2, tobjects[i].y - 2, tobjects[i].w + 4, tobjects[i].h + 4, selbgcolor);
			RenderD7::DrawRect(tobjects[i].x, tobjects[i].y, tobjects[i].w, tobjects[i].h, color);
			RenderD7::DrawRect(tobjects[i].x, tobjects[i].y, tobjects[i].w, tobjects[i].h, selcolor);
			RenderD7::DrawText(tobjects[i].x + (tobjects[i].w/2) - RenderD7::GetTextHeight(tobjects[i].txtsize , tobjects[i].text) + tobjects[i].correctx, tobjects[i].y + (tobjects[i].h/2) - RenderD7::GetTextHeight(tobjects[i].txtsize, tobjects[i].text) + tobjects[i].correcty, tobjects[i].txtsize, txtcolor, tobjects[i].text);
		}
		else
		{
			RenderD7::DrawRect(tobjects[i].x, tobjects[i].y - 1, tobjects[i].w, tobjects[i].h, color);
			RenderD7::DrawText(tobjects[i].x + (tobjects[i].w/2) - RenderD7::GetTextHeight(tobjects[i].txtsize , tobjects[i].text) + tobjects[i].correctx, tobjects[i].y + (tobjects[i].h/2) - RenderD7::GetTextHeight(tobjects[i].txtsize, tobjects[i].text) + tobjects[i].correcty, tobjects[i].txtsize, txtcolor, tobjects[i].text);
		}
	}
}

void RenderD7::DrawTLBtns(std::vector<RenderD7::TLBtn> btns, u32 color, int selection, u32 selbgcolor, u32 selcolor)
{
	for(int i = 0; i < (int)btns.size(); i++)
	{
		if (selection == i)
		{
			RenderD7::DrawRect(btns[i].x - 2, btns[i].y - 2, btns[i].w + 4, btns[i].h + 4, selbgcolor);
			RenderD7::DrawRect(btns[i].x, btns[i].y, btns[i].w, btns[i].h, color);
			RenderD7::DrawRect(btns[i].x, btns[i].y, btns[i].w,  btns[i].h, selcolor);
	        }
		else
		{
			RenderD7::DrawRect(btns[i].x, btns[i].y - 1, btns[i].w, btns[i].h, color);
		}
	}
}

void RenderD7::ExitApp()
{
	if (!rd7settings) running = false;
}

bool RenderD7::touchTObj(touchPosition touch, RenderD7::TObject button)
{
   if (touch.px >= button.x && touch.px <= (button.x + button.w) && touch.py >= button.y && touch.py <= (button.y + button.h))	return true;
    else	return false;
}

bool RenderD7::touchTLBtn(touchPosition touch, RenderD7::TLBtn button)
{
   if (touch.px >= button.x && touch.px <= (button.x + button.w) && touch.py >= button.y && touch.py <= (button.y + button.h))	return true;
    else	return false;
}

int RenderD7::GetRandomInt(int b, int e)
{
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(b, e);
	int r = distribution(generator); 
	return r;
}

void RenderD7::DrawSTObject(std::vector<RenderD7::TObject> tobject, int tobjectindex, u32 color, u32 txtcolor)
{
	RenderD7::DrawRect(tobject[tobjectindex].x, tobject[tobjectindex].y, tobject[tobjectindex].w, tobject[tobjectindex].h, color);
	RenderD7::DrawText(tobject[tobjectindex].x + (tobject[tobjectindex].w/2) - RenderD7::GetTextHeight(tobject[tobjectindex].txtsize , tobject[tobjectindex].text) + tobject[tobjectindex].correctx, tobject[tobjectindex].y + (tobject[tobjectindex].h/2) - RenderD7::GetTextHeight(tobject[tobjectindex].txtsize, tobject[tobjectindex].text) + tobject[tobjectindex].correcty, tobject[tobjectindex].txtsize, txtcolor, tobject[tobjectindex].text);
}

bool RenderD7::NameIsEndingWith(const std::string &name, const std::vector<std::string> &extensions) {
	if (name.substr(0, 2) == "._") return false;

	if (name.size() == 0) return false;

	if (extensions.size() == 0) return true;

	for(int i = 0; i < (int)extensions.size(); i++) {
		const std::string ext = extensions.at(i);
		if (strcasecmp(name.c_str() + name.size() - ext.size(), ext.c_str()) == 0) return true;
	}

	return false;
}

bool dirEntryPredicate(const RenderD7::DirContent &lhs, const RenderD7::DirContent &rhs) {
	if (!lhs.isDir && rhs.isDir) return false;
	if (lhs.isDir && !rhs.isDir) return true;

	return strcasecmp(lhs.name.c_str(), rhs.name.c_str()) < 0;
}

void RenderD7::GetDirContentsExt(std::vector<RenderD7::DirContent> &dircontent, const std::vector<std::string> &extensions) {
	struct stat st;

	dircontent.clear();

	DIR *pdir = opendir(".");

	if (pdir != nullptr) {
		while(true) {
			RenderD7::DirContent dirEntry;

			struct dirent *pent = readdir(pdir);
			if (pent == NULL) break;

			stat(pent->d_name, &st);
			dirEntry.name = pent->d_name;
			dirEntry.isDir = (st.st_mode & S_IFDIR) ? true : false;

			if (dirEntry.name.compare(".") != 0 && (dirEntry.isDir || RenderD7::NameIsEndingWith(dirEntry.name, extensions))) {
				dircontent.push_back(dirEntry);
			}
		}

		closedir(pdir);
	}
	sort(dircontent.begin(), dircontent.end(), dirEntryPredicate);
}

void RenderD7::GetDirContents(std::vector<RenderD7::DirContent> &dircontent) {
	RenderD7::GetDirContentsExt(dircontent, {});
}

void RenderD7::Image::LoadPng(const std::string path)
{
	if (usedbgmsg)
	{
		RenderD7::Msg::Display("RenderD7", "Loading Png:" + path, Top);
	}
	std::vector<u8> ImageBuffer;
	unsigned width, height;
	if (loadet)
	{
		C3D_TexDelete(this->img.tex);
	}
	lodepng::decode(ImageBuffer, width, height, path);

	this->img.tex = new C3D_Tex;
	this->img.subtex = new Tex3DS_SubTexture({(u16)width, (u16)height, 0.0f, 1.0f, width / 1024.0f, 1.0f - (height / 1024.0f)});

	C3D_TexInit(this->img.tex, 1024, 1024, GPU_RGBA8);
	C3D_TexSetFilter(this->img.tex, GPU_LINEAR, GPU_LINEAR);
	this->img.tex->border = 0xFFFFFFFF;
	C3D_TexSetWrap(this->img.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);

	for (u32 x = 0; x < width && x < 1024; x++) {
		for (u32 y = 0; y < height && y < 1024; y++) {
			const u32 dstPos = ((((y >> 3) * (1024 >> 3) + (x >> 3)) << 6) +
								((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) |
								((x & 4) << 2) | ((y & 4) << 3))) * 4;

			const u32 srcPos = (y * width + x) * 4;
			((uint8_t *)this->img.tex->data)[dstPos + 0] = ImageBuffer.data()[srcPos + 3];
			((uint8_t *)this->img.tex->data)[dstPos + 1] = ImageBuffer.data()[srcPos + 2];
			((uint8_t *)this->img.tex->data)[dstPos + 2] = ImageBuffer.data()[srcPos + 1];
			((uint8_t *)this->img.tex->data)[dstPos + 3] = ImageBuffer.data()[srcPos + 0];
		}
	}
	loadet = true;
}

RenderD7::Image::~Image()
{
	C3D_TexDelete(img.tex);
}

void RenderD7::Image::Unload()
{
	C3D_TexDelete(img.tex);
}

void RenderD7::Image::LoadPFromBuffer(const std::vector<u8> &buffer)
{
	std::vector<u8> ImageBuffer;
	if (loadet)
	{
		C3D_TexDelete(this->img.tex);
	}
	unsigned width, height;
	lodepng::decode(ImageBuffer, width, height, buffer);

	img.tex = new C3D_Tex;
	img.subtex = new Tex3DS_SubTexture({(u16)width, (u16)height, 0.0f, 1.0f, width / 512.0f, 1.0f - (height / 512.0f)});

	C3D_TexInit(img.tex, 512, 512, GPU_RGBA8);
	C3D_TexSetFilter(img.tex, GPU_LINEAR, GPU_LINEAR);
	img.tex->border = 0xFFFFFFFF;
	C3D_TexSetWrap(img.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);

	for (u32 x = 0; x < width && x < 512; x++) {
		for (u32 y = 0; y < height && y < 512; y++) {
			const u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) +
								((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) |
								((x & 4) << 2) | ((y & 4) << 3))) * 4;

			const u32 srcPos = (y * width + x) * 4;
			((uint8_t *)img.tex->data)[dstPos + 0] = ImageBuffer.data()[srcPos + 3];
			((uint8_t *)img.tex->data)[dstPos + 1] = ImageBuffer.data()[srcPos + 2];
			((uint8_t *)img.tex->data)[dstPos + 2] = ImageBuffer.data()[srcPos + 1];
			((uint8_t *)img.tex->data)[dstPos + 3] = ImageBuffer.data()[srcPos + 0];
		}
	}
}

static u32 GetNextPowerOf2(u32 v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return (v >= 64 ? v : 64);
}

static bool C3DTexToC2DImage(C2D_Image *texture, u32 width, u32 height, u8 *buf) {
    if (width >= 1024 || height >= 1024)
        return false;
    
    C3D_Tex *tex = new C3D_Tex[sizeof(C3D_Tex)];
    Tex3DS_SubTexture *subtex = new Tex3DS_SubTexture[sizeof(Tex3DS_SubTexture)];
    subtex->width = static_cast<u16>(width);
    subtex->height = static_cast<u16>(height);
    // RGBA -> ABGR
    for (u32 row = 0; row < subtex->width; row++) {
        for (u32 col = 0; col < subtex->height; col++) {
            u32 z = (row + col * subtex->width) * 4;
            
            u8 r = *(u8 *)(buf + z);
            u8 g = *(u8 *)(buf + z + 1);
            u8 b = *(u8 *)(buf + z + 2);
            u8 a = *(u8 *)(buf + z + 3);
            
            *(buf + z) = a;
            *(buf + z + 1) = b;
            *(buf + z + 2) = g;
            *(buf + z + 3) = r;
        }
    }
    
    u32 w_pow2 = GetNextPowerOf2(subtex->width);
    u32 h_pow2 = GetNextPowerOf2(subtex->height);
    subtex->left = 0.f;
    subtex->top = 1.f;
    subtex->right = (subtex->width /static_cast<float>(w_pow2));
    subtex->bottom = (1.0 - (subtex->height / static_cast<float>(h_pow2)));
    C3D_TexInit(tex, static_cast<u16>(w_pow2), static_cast<u16>(h_pow2), GPU_RGBA8);
    C3D_TexSetFilter(tex, GPU_NEAREST, GPU_NEAREST);
    
    std::memset(tex->data, 0, tex->size);
    
    for (u32 x = 0; x < subtex->width; x++) {
        for (u32 y = 0; y < subtex->height; y++) {
            u32 dst_pos = ((((y >> 3) * (w_pow2 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) * 4;
            u32 src_pos = (y * subtex->width + x) * 4;
            std::memcpy(&(static_cast<u8 *>(tex->data))[dst_pos], &(static_cast<u8 *>(buf))[src_pos], 4);
        }
    }
    
    C3D_TexFlush(tex);
    tex->border = RenderD7::Color::Hex("#000000", 0);
    C3D_TexSetWrap(tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
    if (tex && subtex) {
        texture->tex = tex;
        texture->subtex = subtex;
        return true;
    }
    return false;
}

void RenderD7::Image::FromSheet(RenderD7::Sheet sheet, size_t index)
{
	
}

bool RenderD7::DrawImage(C2D_Image img, float x, float y, float scaleX, float scaleY)
{
	return C2D_DrawImageAt(img, x, y, 0.5f, nullptr, scaleX, scaleY);
}

bool RenderD7::Image::Draw(float x, float y, float scaleX, float scaleY)
{
	return C2D_DrawImageAt(this->img, x, y, 0.5f, nullptr, scaleX, scaleY);
}
bool RenderD7::FS::FileExist(const std::string& path)
{
	FILE *test = fopen(path.c_str(), "r");
    if(test != NULL)
    {
        fclose(test);
	
        return true;
    }

    return false;
}

bool RenderD7::IsNdspInit()
{
	if (isndspinit)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void RenderD7::DrawList1(RenderD7::ScrollList1 &l, float txtsize, C3D_RenderTarget *t)
{
	RenderD7::OnScreen(t);
	RenderD7::DrawRect(0, 0, 400, 240, RenderD7::Color::Hex("#dddddd"));
	RenderD7::DrawText(0, 0, 0.8f, RenderD7::Color::Hex("#ffffff"), l.Text);
}


void RenderD7::DrawMetrikOvl()
{
    switch (mt_screen)
    {
        case 0:
        RenderD7::OnScreen(Top);
		break;
        case 1:
        RenderD7::OnScreen(Bottom);
		break;
        default:
        RenderD7::OnScreen(Bottom);
		break;
    }
	std::string __C = RENDERD7VSTRING;
	std::string info = "RenderD7 " + __C + " Debug Overlay";
	float infoy = 240 - RenderD7::GetTextHeight(mt_txtSize, info);
	mt_fps = "FPS: " + RenderD7::GetFramerate();
	mt_cpu = "CPU: " + std::to_string(C3D_GetProcessingTime()*6.0f).substr(0, 4) + "%/" + std::to_string(C3D_GetProcessingTime()).substr(0, 4) + "ms";
	mt_gpu = "GPU: " + std::to_string(C3D_GetDrawingTime()*6.0f).substr(0, 4) + "%/" + std::to_string(C3D_GetDrawingTime()).substr(0, 4) + "ms";
	mt_cmd = "CMD: " + std::to_string(C3D_GetCmdBufUsage()*100.0f).substr(0, 4) + "%/" + std::to_string(C3D_GetCmdBufUsage()).substr(0, 4) + "ms";
	RenderD7::DrawRect(0, 0, RenderD7::GetTextWidth(mt_txtSize, mt_fps), RenderD7::GetTextHeight(mt_txtSize, mt_fps), mt_color);
	RenderD7::DrawRect(0, 50, RenderD7::GetTextWidth(mt_txtSize, mt_cpu), RenderD7::GetTextHeight(mt_txtSize, mt_cpu), mt_color);
	RenderD7::DrawRect(0, 70, RenderD7::GetTextWidth(mt_txtSize, mt_gpu), RenderD7::GetTextHeight(mt_txtSize, mt_gpu), mt_color);
	RenderD7::DrawRect(0, 90, RenderD7::GetTextWidth(mt_txtSize, mt_cmd), RenderD7::GetTextHeight(mt_txtSize, mt_cmd), mt_color);
	RenderD7::DrawRect(0, infoy, RenderD7::GetTextWidth(mt_txtSize, info), RenderD7::GetTextHeight(mt_txtSize, info), mt_color);
	RenderD7::DrawText(0, 0, mt_txtSize, mt_txtcolor, mt_fps);
    RenderD7::DrawText(0, 50, mt_txtSize, mt_txtcolor, mt_cpu);
    RenderD7::DrawText(0, 70, mt_txtSize, mt_txtcolor, mt_gpu);
	RenderD7::DrawText(0, 90, mt_txtSize, mt_txtcolor, mt_cmd);
	RenderD7::DrawText(0, infoy, mt_txtSize, mt_txtcolor, info);
	
    /*for (int z = 0; z < (int)mt_fpsgraph.size(); z++)
    {
		//mt_fpsgraph[z] = (int)d11framerate;
        C2D_DrawLine(z, 239 - mt_fpsgraph[z], mt_txtcolor, z + 1, 239 - mt_fpsgraph[z + 1], mt_txtcolor, 1, 1);
    }*/
}

bool RenderD7::DrawNFRect(float p1x, float p1y, float w, float h, u32 color, float scale)
{  
    C2D_DrawLine(p1x, p1y, color,w, p1y, color, scale, 1);
    C2D_DrawLine(w, p1y, color,w, h, color, scale, 1);
    C2D_DrawLine(w, h, color,p1x, h, color, scale, 1);
    C2D_DrawLine(p1x, h, color,p1x, p1y, color, scale, 1);
    return true;
}

RenderD7::DSP_NF::DSP_NF()
{

}

void RenderD7::DSP_NF::Draw(void) const
{
	RenderD7::OnScreen(Top);
	RenderD7::DrawRect(0, msgposy, 400, 70, RenderD7::Color::Hex("#111111"));
	RenderD7::DrawRect(0, msgposy, 400, 25, RenderD7::Color::Hex("#222222"));
	RenderD7::DrawText(2, msgposy+3, 0.7f, RenderD7::Color::Hex("#ffffff"), "Warning! Code: 00027");
	RenderD7::DrawText(2, msgposy+30, 0.6f, RenderD7::Color::Hex("#ffffff"), "You can't use Sound effects because the file\n<<sdmc:/3ds/dspfirm.cdc>> was not found!");
}

void RenderD7::DSP_NF::Logic()
{
	this->delay++/*=(int)RenderD7::GetDeltaTime()*/;
	if (msgposy > 170 && delay < 5*60) msgposy--/*=(int)RenderD7::GetDeltaTime()*/;
	
	if (delay >= 5*60)
	{
		msgposy++/*=(int)RenderD7::GetDeltaTime*/;
		if(msgposy > 400) this->Kill();
	}
}

void OvlHandler()
{
	if (!overlays.empty())
	{
		overlays.top()->Draw();
	}
	if (!overlays.empty())
	{
		overlays.top()->Logic();
	}
	if (!overlays.empty())
	{
		if (overlays.top()->IsKilled()) overlays.pop(); 
	}
	
}
int lp = 0;
void RenderD7::FrameEnd()
{
	if (metrikd && !shouldbe_disabled)RenderD7::DrawMetrikOvl();
	if (!shouldbe_disabled) OvlHandler();
	/*if (d7_hHeld & KEY_R && d7_hDown & KEY_SELECT)
	{
		RenderD7::LoadSettings();
	}*/
	if (mt_dumpcsv && lp == 60)
	{
		std::string _mt_fps = RenderD7::GetFramerate();
		std::string _mt_cpu = std::to_string(C3D_GetProcessingTime()).substr(0, 4);
		std::string _mt_gpu = std::to_string(C3D_GetDrawingTime()).substr(0, 4);
		std::string _mt_cmd = std::to_string(C3D_GetCmdBufUsage()).substr(0, 4);
		mt_csv.open((mt_cname), std::ofstream::app);
		std::string fmt_ = _mt_fps + "," + _mt_cpu + "," + _mt_gpu + "," + _mt_cmd + "\n";
		mt_csv << fmt_;
		mt_csv.close();
		lp = 0;
	}
	lp++;
	
	C3D_FrameEnd(0);
}

RenderD7::RSettings::RSettings()
{
	cfgfile = std::make_unique<INI::INIFile>(cfgpath+ "/config.ini");
	cfgfile->read(cfgstruct);
	rd7settings = true;
}

RenderD7::RSettings::~RSettings()
{
	cfgfile->write(cfgstruct);
}

void RenderD7::RSettings::Draw(void) const
{
	RenderD7::OnScreen(Top);
	RenderD7::DrawRect(0, 0, 400, 21, RenderD7::Color::Hex("#111111"));
	RenderD7::DrawRect(0, 21, 400, 220, RenderD7::Color::Hex("#eeeeee"));
	RenderD7::DrawText(0, 0, 0.7f, DSEVENWHITE, "RenderD7->Settings");
	RenderD7::DrawTextLeft(400, 0, 0.7f, RenderD7::Color::Hex("#ffffff"), RENDERD7VSTRING);
	RenderD7::DrawText(0, 30, 0.7f, DSEVENBLACK, "RD7SR: " + rd7srstate);
	RenderD7::DrawText(0, 50, 0.7f, DSEVENBLACK, "Metrik to Csv: " + csvstate);
	RenderD7::DrawText(0, 70, 0.7f, DSEVENBLACK, "Metrik Overlay: " + mtovlstate);
	RenderD7::DrawText(0, 90, 0.7f, DSEVENBLACK, "Force FPS: " + fpsstate);
	RenderD7::DrawText(0, 110, 0.7f, DSEVENBLACK, "Metrik Screen: " + mtscreenstate);
	/*RenderD7::DrawText(0, 130, 0.7f, DSEVENBLACK, "Metrik Text RGB: " + mttxtcolstate);
	RenderD7::DrawText(0, 150, 0.7f, DSEVENBLACK, "Metrik Alpha: " + mtcola);
	RenderD7::DrawText(0, 170, 0.7f, DSEVENBLACK, "Metrik Text Alpha: " + mttxtcola);*/
	RenderD7::OnScreen(Bottom);
	std::string verc = "Config Version: ";
	verc += CFGVER;
	RenderD7::DrawRect(0, 0, 320, 240, RenderD7::Color::Hex("#eeeeee"));
	RenderD7::DrawText(0, 0, 0.7f, RenderD7::Color::Hex("#111111"), verc);
	RenderD7::DrawTObjects(buttons, RenderD7::Color::Hex("#111111"), RenderD7::Color::Hex("#eeeeee"));
}
std::string Kbd(int lenght, SwkbdType tp)
{
	RenderD7::FrameEnd();
	SwkbdState state;
	char temp[lenght + 1] = { 0 };
	
	swkbdInit(&state, SwkbdType::SWKBD_TYPE_NUMPAD, 2, lenght);
	swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, SWKBD_FILTER_PROFANITY, 0);
	SwkbdButton ret = swkbdInputText(&state, temp, sizeof(temp));
	temp[lenght] = '\0';
	return (ret == SWKBD_BUTTON_CONFIRM ? temp : "60");
}
void RenderD7::RSettings::Logic(u32 hDown, u32 hHeld, u32 hUp, touchPosition touch)
{
	rd7srstate = rd7_superreselution ? "true" : "false";
	csvstate = mt_dumpcsv ? "true" : "false";
	mtovlstate = metrikd ? "true" : "false";
	fpsstate = cfgstruct["settings"]["forceFrameRate"];
	mtscreenstate = mt_screen ? "Bottom" : "Top";
	if (d7_hDown & KEY_TOUCH && RenderD7::touchTObj(d7_touch, buttons[0]) && !metrikd)
	{
		RenderD7::ToggleRD7SR();
		cfgstruct["settings"]["super-reselution"] = rd7_superreselution ? "1" : "0";
	}
	if (d7_hDown & KEY_TOUCH && RenderD7::touchTObj(d7_touch, buttons[1]))
	{
		mt_dumpcsv = mt_dumpcsv ? false : true;
		cfgstruct["metrik-settings"]["dumpcsv"] = mt_dumpcsv ? "1" : "0";
		if (mt_dumpcsv)
		{
			mt_cname = csvpc;
			mt_cname += "/";
			mt_cname += Date();
			mt_cname += ".csv";
			FILE* logfile = fopen((mt_cname.c_str()), "w");
			fclose(logfile);
			mt_csv.open((mt_cname), std::ofstream::app);
			mt_csv << "FPS,CPU,GPU,CMD\n";
			mt_csv.close();
		}
	}
	if (d7_hDown & KEY_TOUCH && RenderD7::touchTObj(d7_touch, buttons[2]))
	{
		metrikd = metrikd ? false : true;
		cfgstruct["metrik-settings"]["enableoverlay"] = metrikd ? "1" : "0";
	}
	if (d7_hDown & KEY_TOUCH && RenderD7::touchTObj(d7_touch, buttons[3]) && !metrikd)
	{
		cfgstruct["settings"]["forceFrameRate"] = Kbd(2, SWKBD_TYPE_NUMPAD);
	}
	if (d7_hDown & KEY_TOUCH && RenderD7::touchTObj(d7_touch, buttons[4]))
	{
		mt_screen = mt_screen ? 0 : 1;
		cfgstruct["metrik-settings"]["screen"] = mt_screen ? "1" : "0";
	}
	if (d7_hDown & KEY_TOUCH && RenderD7::touchTObj(d7_touch, buttons[5]))
	{
		RenderD7::AddOvl(std::make_unique<RenderD7::DSP_NF>());
	}
	if (d7_hDown & KEY_B)
	{
		cfgfile->write(cfgstruct);
		rd7settings = false;
		RenderD7::Scene::Back();
	}
	
}

void RenderD7::LoadSettings(){
	RenderD7::Scene::Load(std::make_unique<RenderD7::RSettings>());
}

void RenderD7::AddOvl(std::unique_ptr<RenderD7::Ovl> overlay)
{
	overlays.push(std::move(overlay));
}

RenderD7::Console::Console()
{
    this->x = 0;
    this->y = 0;
    this->w = 320;
    this->h = 240;
    this->color = {0, 0, 0, 255};
}
RenderD7::Console::Console(int x, int y, int w, int h, u8 a)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->color = {0, 0, 0, a};
    
}
RenderD7::Console::Console(int x, int y, int w, int h, RenderD7::Color::rgba col)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->color = col;
}
RenderD7::Console::Console(int x, int y, int w, int h, std::string name, RenderD7::Color::rgba col, RenderD7::Color::rgba barcol, RenderD7::Color::rgba outlinecol)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->color = col;
    this->outlinecol = outlinecol;
    this->barcolor = barcol;
    this->m_name = name;
}
RenderD7::Console::~Console()
{

}
void RenderD7::Console::On(C3D_RenderTarget *t_cscreen)
{
     this->cscreen = t_cscreen;
}
bool RenderD7::Console::Update()
{
     bool dr_sc = true;
     return dr_sc;
}


std::string RenderD7::FormatString(std::string fmt_str, ...)
{
	va_list ap;
	char* fp = NULL;
	va_start(ap, fmt_str);
	vasprintf(&fp, fmt_str.c_str(), ap);
	va_end(ap);
	std::unique_ptr<char, decltype(free)*> formatted(fp, free);
	return std::string(formatted.get());
}

std::string RenderD7::GetTimeStr(void)
{
	time_t unixTime       = time(NULL);
	struct tm* timeStruct = gmtime((const time_t*)&unixTime);
	return RenderD7::FormatString("%02i:%02i:%02i", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);
}
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned Image_to_C3D(C2D_Image img, const std::vector<unsigned char>& bmp) {
	C2D_Image* images;
  	C3DTexToC2DImage(images, 1024, 1024, (u8*)bmp.data());
	img.tex = images->tex;
	img.subtex = images->subtex;
  	return false;
}

void RenderD7::Image::LoadFromBitmap(BMP bitmap)
{
	unsigned error = Image_to_C3D(this->img, bitmap.DATA());

  if(error) {
    std::cout << "BMP decoding error " << error << std::endl;
    RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("Bmp - Error", "Code: " + std::to_string(error)));
  }
}

RenderD7::Toast::Toast(std::string head, std::string msg)
{
	this->head = head;
	this->msg = msg;
}

void RenderD7::Toast::Draw(void) const
{
	RenderD7::OnScreen(Top);
	RenderD7::DrawRect(0, msgposy, 400, 70, RenderD7::Color::Hex("#111111"));
	RenderD7::DrawRect(0, msgposy, 400, 25, RenderD7::Color::Hex("#222222"));
	RenderD7::DrawText(2, msgposy+3, 0.7f, RenderD7::Color::Hex("#ffffff"), head);
	RenderD7::DrawText(2, msgposy+30, 0.6f, RenderD7::Color::Hex("#ffffff"), msg);
}

void RenderD7::Toast::Logic()
{
	this->delay++/*=(int)RenderD7::GetDeltaTime()*/;
	if (msgposy > 170 && delay < 5*60) msgposy--/*=(int)RenderD7::GetDeltaTime()*/;
	
	if (delay >= 5*60)
	{
		msgposy++/*=(int)RenderD7::GetDeltaTime*/;
		if(msgposy > 400) this->Kill();
	}
}