#ifndef FONT_H
#define FONT_H

enum FontFlags_t
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,		// custom generated font - never fall back to asian compatibility mode
	FONTFLAG_BITMAP = 0x800,		// compiled bitmap font - no fallbacks
};

enum FontAlign_t
{
	CENTER_V = 0x1,
	CENTER_H = 0x2,
	LEFT = 0x4,
	RIGHT = 0x8,
};



class Color
{
public:
	Color(int r, int g, int b, int a)
		: R(r),
		G(g),
		B(b),
		A(a)
	{

	}

	Color(int r, int g, int b)
		: R(r),
		G(g),
		B(b),
		A(255)
	{

	}

	static Color White(void) { return Color(255, 255, 255, 255); }
	static Color Black(void) { return Color(0, 0, 0, 255); }
	static Color Red(void) { return Color(255, 0, 0, 255); }
	static Color Green(void) { return Color(0, 255, 0, 255); }
	static Color Blue(void) { return Color(0, 0, 255, 255); }
	static Color Cyan(void) { return Color(0, 127, 255, 255); }
	static Color Yellow(void) { return Color(255, 255, 0, 255); }

	int r(void) { return R; }
	int g(void) { return G; }
	int b(void) { return B; }
	int a(void) { return A; }

private:
	int R, G, B, A;
};

class CFont
{
	friend class CRenderer;

public:
	// inherited virtual destroyer
	virtual void Destroy();

	void String(int x, int y, Color color, long long align, const char* text, ...);

	int GetFontSize(void) { return m_iSize; }
private:
	unsigned long	m_uiFont;
	int				m_iSize;
};

extern CFont* m_ESPFont;
extern CFont* m_HUDFont;
#endif