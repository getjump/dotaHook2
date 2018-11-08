#include "Renderer.h"

CFont* m_ESPFont = 0;
CFont* m_HUDFont = 0;

void CFont::Destroy()
{
	// cleanup
}


CFont* CRenderer::CreateFont(const char* szFace, int iSize, int iWeight, int iFlags)
{
	CFont* pFont = new CFont;

	if (!pFont)
		return NULL;

	pFont->m_uiFont = surface->CreateFont();

	if (!pFont->m_uiFont)
	{
		SAFE_DELETE(pFont);
		return NULL;
	}

	if (!surface->SetFontGlyphSet(pFont->m_uiFont, szFace, iSize, iWeight, 0, 0, iFlags))
	{
		SAFE_DELETE(pFont);
		return NULL;
	}

	pFont->m_iSize = iSize;

	return pFont;
}

void CFont::String(int x, int y, Color color, long long align, const char* text, ...)
{
	char buf[1024];
	va_list va_alist;
	int len;
	int width;
	int height;

	if (text == NULL)
		return;

	va_start(va_alist, text);
	vsprintf(buf, text, va_alist);
	va_end(va_alist);

	len = strlen(buf);

	wchar_t* pszFmt = new wchar_t[len + 1];
	mbstowcs(pszFmt, buf, len + 1);

	surface->GetTextSize(m_uiFont, pszFmt, width, height);

	if (align & RIGHT)
		x -= width;
	else if (align & CENTER_H)
		x -= width / 2;

	if (align & CENTER_V)
		y -= height / 2;

	paintSurface->DrawSetTextColor(color.r(), color.g(), color.b(), color.a());
	paintSurface->DrawSetTextFont(m_uiFont);
	paintSurface->DrawSetTextPos(x, y);
	paintSurface->DrawPrintText(pszFmt, len);

	SAFE_DELETE_ARRAY(pszFmt);
}