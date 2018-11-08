#ifndef RENDERER_H
#define RENDERER_H

#ifndef SAFE_DELETE
#define SAFE_DELETE( x ) if( x ){ delete x; x = NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY( x ) if( x ){ delete [] x; x = NULL; }
#endif

#pragma comment(lib, "winmm.lib")

#include "Font.h"
#include "ISurface.h"

#include <stdio.h>
#include <Windows.h>

class CRenderer
{
public:
	CRenderer(void);

	~CRenderer(void);

	CFont*          CreateFont(const char* szFace, int iSize, int iWeight, int iFlags);

	void			Rect(int x, int y, int w, int h, Color color);

	void			RectOut(int x, int y, int w, int h, int t, Color color, Color out);

	void			BorderBox(int x, int y, int w, int h, int t, Color color);

	void			BorderBoxOut(int x, int y, int w, int h, int t, Color color, Color out);

	void			Line(int x0, int y0, int x1, int y1, Color color);
};
extern CRenderer* g_pRenderer;
#endif // RENDERER_H