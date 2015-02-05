#pragma once
#include "IScreenRenderer.h"

class DummyScreenRenderer : public IScreenRenderer
{
public:
	virtual bool Initialize(HWND* hWnd);
	virtual void Shutdown(HWND* hWnd);

	virtual IImage* LoadImage(const TCHAR* filename);

	virtual void BeginDraw();
	virtual void EndDraw();

	virtual void DrawImage(int MapId, const RECT* screenPosition, const RECT* imagePosition);
	virtual void DrawText(const RECT* screenPosition, const TCHAR* string);
};

