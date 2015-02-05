#pragma once
#include "IScreenRenderer.h"
#include "GDIImage.h"
#include <vector>

class GDIScreenRenderer : public IScreenRenderer
{
public:
	std::vector<GDIImage> imageVector;

	HINSTANCE* InstanceHandler;
	HWND* WindowHandle;
	
	// Double buffering (buffer = device context)
	HDC ScreenBuffer1;		// shown buffer
	HDC ScreenBuffer2;		// buffer that's drawn on in the background

	// Requires a bitmap of ScreenBuffer2, since we can't draw ScreenBuffer2 directly onto ScreenBuffer1
	HBITMAP ScreenBuffer2Bitmap;
	HDC CompatibleContextHandle;

	PAINTSTRUCT* PaintStruct;

	virtual bool Initialize(HWND* hWnd) override;
	virtual void Shutdown(HWND* hWnd);

	virtual IImage* LoadImage(const TCHAR* filename);

	virtual void BeginDraw() override;
	virtual void EndDraw() override;

	virtual void DrawImage(int MapId, const RECT* screenPosition, const RECT* imagePosition);
	virtual void DrawText(const RECT* screenPosition, const TCHAR* string);
};

