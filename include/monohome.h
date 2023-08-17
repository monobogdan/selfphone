#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


#include <linux/fb.h>

#include <fcntl.h>
#include <sys/types.h>
#include <EGL/egl.h>

#define LOGF(fmt, ...) printf("[%s]: " fmt, &__FUNCTION__, __VA_ARGS__)
#define LOG(fmt, ...) printf("[%s]: " fmt, &__FUNCTION__)

struct CFrameBuffer
{
	unsigned char* pixels;
	int length;
	int lineLength;
	int width;
	int height;
};

typedef unsigned char byte;

struct CTgaHeader
{
    byte headerLength;
    byte paletteType;
    byte pixelFormat; // Only type 2 supported
    byte palette[5];

    unsigned short x;
    unsigned short y;
    unsigned short width;
    unsigned short height;

    byte bpp;
    byte descriptor;
};

class CImage
{
public:
	int Width;
	int Height;
	bool IsTransparent;
	unsigned char* Pixels;

	static CImage* FromFile(char* fileName);
};

typedef unsigned char colorref;

class CAnimator
{
private:
	int xFrom, yFrom, xTo, yTo;
	float rFrom, rTo;
	float duration;

	long endTime;
public:
	int ThreadId;
	bool IsPlaying;
	float Time;

	int X, Y;
	float Rotation;

	CAnimator();
	~CAnimator();

	void SetTranslation(int xFrom, int yFrom, int xTo, int yTo);
	void SetRotation(float from, float to);
	void SetDuration(float speed);
	bool Update();
	void Run();
};

enum ETouchState
{
	tsIdle,
	tsTouching,
	tsReleased
};

class CGestureDetector
{
public:
	int ScrollX;
	int ScrollY;
	int FPX; // First touch point
	int FPY;

	CGestureDetector();
	~CGestureDetector();

	void Update();
};

class CInput
{
private:
	int evDev;
public:
	ETouchState TouchState;
	int TouchX;
	int TouchY;

	CInput();
	~CInput();

	void Update();
	bool IsTouchedAt(int x, int y, int w, int h);
};

struct CColor
{
	colorref R, G, B;
};

enum EBatteryLevel
{
	blLow,
	blMedium,
	blHigh,
	blFullyCharged
};

class CPowerManager
{
private:
	void ReadSysfs(char* path, char* buf, int len);
	void WriteSysfs(char* path, char* buf);
public:
	CPowerManager();
	~CPowerManager();

	void SetCPUClock(int mhz);
	void SetBacklight(float backLight);

	EBatteryLevel GetBatteryLevel();
	int GetBatteryVoltage();
	bool IsCharging();
};

struct CFontHeader
{
	int GlyphSize;
};

class CFont
{
public:
	CImage* Glyphs[255];
	int GlyphCount;

	CFont();
	~CFont();

	static CFont* FromFile(char* fileName);
};

class CGraphics
{
private:
	int fbDev;
	int glCtx;
	bool flip;
	
	fb_var_screeninfo vInfo;
	CFrameBuffer fbDesc;
	char* backBuffer;

	void InitContext();
	void InitFrameBuffer();
public:
	CGraphics();
	~CGraphics();

	void PutPixel(int x, int y, CColor color);
	void PutPixelAlpha(int x, int y, CColor color, float alpha);
	void LineTo(int x, int y);
	void FillRect(int x, int y, int w, int h, CColor color, float alpha);
	void DrawImage(CImage* img, int x, int y);
	void DrawImageEx(CImage* img, int x, int y, float rotation, float alphaBlending, CColor colorMultiply);

	void DrawGlyph(CImage* img, int x, int y, CColor colorMultiply);
	void DrawString(CFont* font, char* str, int x, int y);
	void DrawStringColored(CFont* font, char* str, int x, int y, CColor colorMultiply);
	void Flip();
};

class CDisplay
{
private:
	int blFd;
public:
	CDisplay();
	~CDisplay();

	void SetBacklight(float bl);
	void TurnScreen(bool onOff);
};

enum EModemDialState
{
	dsIdle,
	dsRinging,
	dsCallInProgress
};

class CModem
{
private:
	int modemFd;
	char imei[64];
	bool readyState;
	EModemDialState dialState;

	void Init();
	void ExitFlightMode();
public:
	CModem();
	~CModem();

	// AT methods
	void SendAT(char* command, int waitTime);
	char* GetATResponse(char* buf, int maxLen);
	bool CheckATStatus(char* response);
	
	// Status
	bool IsReady();
	EModemDialState GetDialState();
	char* GetIMEI();
	char* GetIMSI();
	
	// Misc methods
	void Dial(char* number);
	void Hang();
	void SendSMS(char* number, char* text);
};

class CStatusBar
{
private:
	CImage* imFiller;
	CImage* imBattery[4];
	CImage* imSMS;
	CImage* imDials;
	CImage* imSIgnal[4];

	CAnimator* animator;
public:
	CStatusBar();
	~CStatusBar();

	void Update();
	void Draw();
};

class CNotifyDialog
{
private:
	int time;
public:
	char* Text;
	int Duration;
	bool Visible;

	void Draw();
};

// Simple immediate-mode GUI implementation
// Also, this class features keyboard implementation
class CGUI
{
public:
	static bool Button(CImage* img, int x, int y);
};

class CScreen
{
protected:
	CAnimator* windowAnimator;
public:
	CScreen();
	~CScreen();

	virtual void Show();
	virtual void Update();
	virtual void Draw();
	virtual void Hide();
};

class CDialerScreen : public CScreen
{
private:
	CImage* dialerField;
	CImage* dialerButton;

	char number[32];
public:
	CDialerScreen();
	~CDialerScreen();

	void Update();
	void Draw();
};

class CConversationUI
{
private:
	CImage* directionImage;
	CImage* phoneImage;
public:
	CConversationUI();
	~CConversationUI();

	void Update();
	void Draw();
};

#define APP_FACTORY(clazz) CScreen* __phone_factory_##clazz () { return new clazz (); }

struct CAppDesc
{
	char Name[16];
	char IconPath[32];

	CImage* Icon;
	CScreen* MainScreen;

	CScreen*(*Factory)();
};

class CLauncher
{
private:
	CAnimator* drawerAnimator;
	CScreen* currentScreen;

	void DrawAppDrawer();
public:
	CModem* Modem;
	CGraphics* Graphics;
	CInput* Input;
	CPowerManager* PowerManager;

	CFont* Font;
	CImage* Wallpaper;
	CNotifyDialog* Dialog;
	CStatusBar* Status;

	long Uptime;
	
	CLauncher();
	~CLauncher();

	void StartScreen(CScreen* screen);
	
	void Run();
};

extern CLauncher* gLauncher;