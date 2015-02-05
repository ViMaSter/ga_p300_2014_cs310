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
	HDC DeviceContextHandle;
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

