#pragma once

#include "Windows.h"
#pragma comment(lib, "opengl32.lib")
#include "Gl/Gl.h"

#include "IScreenRenderer.h"
#include "GLImage.h"

class GLScreenRenderer : public IScreenRenderer
{
public:
	PIXELFORMATDESCRIPTOR PixelFormatDescriptor;
	HWND* WindowHandle;
	HGLRC GLRenderingContext;
	HDC ScreenBuffer;

	virtual bool Initialize(HWND* hWnd) override;
	virtual void Shutdown(HWND* hWnd);

	virtual IImage* LoadImage(const TCHAR* filename);

	virtual void BeginDraw() override;
	virtual void EndDraw() override;

	virtual void DrawImage(int mapId, const RECT* screenPosition, const RECT* imagePosition) override;
	virtual void DrawImage(IImage* image, const RECT* screenPosition, const RECT* imagePosition) override;
	virtual void DrawText(const RECT* screenPosition, const TCHAR* string) override;
};

