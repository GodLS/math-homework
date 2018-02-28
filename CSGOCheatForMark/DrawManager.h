#pragma once
#include <d3d9.h>
#include <d3dx9.h>

class DrawManager
{
public:
	DrawManager(IDirect3DDevice9* pDevice);
	~DrawManager();

	void OnLostDevice() const;
	void OnResetDevice();

	void BeginRendering() const;
	void EndRendering() const;

	void RenderLine(DWORD color, int x1, int y1, int x2, int y2) const;
	void RenderRect(DWORD color, int x, int y, int w, int h);
	void RenderCircle(DWORD color, int x, int y, int r, int resolution = 64) const;
	void RenderText(DWORD color, int x, int y, bool centered, bool dropshadow, const char* fmt, ...);

	void FillRect(DWORD color, int x, int y, int w, int h) const;
	void FillRectFix(DWORD color, int x, int y, int w, int h);
	/*
	* Below are some functions that you can implement yourself as an exercise
	*
	* void FillCircle(DWORD color, int x, int y, int r, int resolution = 64);
	* void RenderPolygon(DWORD color, int* x, int* y, int npoints);
	* void FillPolygon(DWORD color, int* x, int* y, int npoints);
	* void FillGradient(DWORD from, Color to, bool isVertical, int x, int y, int w, int h);
	*/

private:
	void ReleaseObjects() const;
	void CreateObjects();

private:
	IDirect3DDevice9*		m_pDevice;
	IDirect3DStateBlock9*	m_pStateBlock;
	ID3DXFont*				m_pDefaultFont;
};
