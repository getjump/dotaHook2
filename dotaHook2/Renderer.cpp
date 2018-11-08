#include "Renderer.h"

CRenderer* g_pRenderer = new CRenderer();

CRenderer::CRenderer(void)
{

}

CRenderer::~CRenderer(void)
{

}

void CRenderer::Rect(int x, int y, int w, int h, Color color)
{
	paintSurface->DrawSetColor(color.r(), color.g(), color.b(), color.a());
	paintSurface->DrawFilledRect(x, y, x + w, y + h);
}

void CRenderer::RectOut(int x, int y, int w, int h, int t, Color color, Color out)
{
	Rect(x, y, w, h, color);
	BorderBox(x - 1, y - 1, w + 1, h + 1, 1, out);
}

void CRenderer::BorderBox(int x, int y, int w, int h, int t, Color color)
{
	Rect(x, y, w, t, color);
	Rect(x, y, t, h, color);
	Rect(x + w, y, t, h, color);
	Rect(x, y + h, w + t, t, color);
}

void CRenderer::BorderBoxOut(int x, int y, int w, int h, int t, Color color, Color out)
{
	BorderBox(x, y, w, h, t, color);
	BorderBox(x - 1, y - 1, w + 2, h + 2, 1, out);
	BorderBox(x + 1, y + 1, w - 2, h - 2, 1, out);
}

void CRenderer::Line(int x0, int y0, int x1, int y1, Color color)
{
	paintSurface->DrawSetColor(color.r(), color.g(), color.b(), color.a());
	paintSurface->DrawLine(x0, y0, x1, y1);
}