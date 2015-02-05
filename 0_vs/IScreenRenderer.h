#pragma once
#include "main.h"
#include "IImage.h"
#include <vector>

// Handle
// Bitmap
// Draw crop from image somewhere on screen
// Draw text
// Clean init+shutdown

class IScreenRenderer
{
protected:

public:

	virtual bool Initialize(HWND* hWnd) = 0;
	virtual void Shutdown(HWND* hWnd) = 0;

	virtual IImage* LoadImage(const TCHAR* filename) = 0;

	virtual void BeginDraw() = 0;
	virtual void EndDraw() = 0;

	virtual void DrawImage(int MapId, const RECT* screenPosition, const RECT* imagePosition) = 0;
	virtual void DrawText(const RECT* screenPosition, const TCHAR* string) = 0;

};

