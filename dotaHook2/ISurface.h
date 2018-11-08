#ifndef ISURFACE_H
#define ISURFACE_H

#include "Hook.h"

class ISurface
{
public:
	unsigned long CreateFont(void)
	{
		typedef unsigned long(__thiscall* CreateFontFn)(void*);
		auto out = GetMethod<CreateFontFn>(this, 51)(this);

		return out;
	}

	bool SetFontGlyphSet(unsigned long font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
	{
		typedef bool(__thiscall* SetFontGlyphSetFn)(void*, unsigned long, const char*, int, int, int, int, int, int, int);
		
		auto out = GetMethod<SetFontGlyphSetFn>(this, 58)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);

		return out;
	}

	void GetTextSize(unsigned long font, const wchar_t* text, int& wide, int& tall)
	{
		typedef void(__thiscall* GetTextSizeFn)(void*, unsigned long, const wchar_t*, int&, int&);
		GetMethod<GetTextSizeFn>(this, 66)(this, font, text, wide, tall);
	}
};

class IIPaintSurface
{
public:
	void DrawSetColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* DrawSetColorFn)(void*, int, int, int, int);
		GetMethod<DrawSetColorFn>(this, 17)(this, r, g, b, a);
	}

	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* DrawFilledRectFn)(void*, int, int, int, int);
		GetMethod<DrawFilledRectFn>(this, 19)(this, x0, y0, x1, y1);
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* DrawLineFn)(void*, int, int, int, int);
		GetMethod<DrawLineFn>(this, 22)(this, x0, y0, x1, y1);
	}

	void DrawSetTextFont(unsigned long font)
	{
		typedef void(__thiscall* DrawSetTextFontFn)(void*, unsigned long);
		GetMethod<DrawSetTextFontFn>(this, 13)(this, font);
	}

	void DrawSetTextColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* DrawSetTextColorFn)(void*, int, int, int, int);
		GetMethod<DrawSetTextColorFn>(this, 25)(this, r, g, b, a);
	}

	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall* DrawSetTextPosFn)(void*, int, int);
		GetMethod<DrawSetTextPosFn>(this, 26)(this, x, y);
	}

	void DrawPrintText(const wchar_t* text, int textLen) // another file
	{
		typedef void(__thiscall* DrawPrintTextFn)(void*, const wchar_t*, int, int);
		GetMethod<DrawPrintTextFn>(this, 27)(this, text, textLen, 0);
	}
};

extern ISurface *surface;
extern IIPaintSurface *paintSurface;

#endif