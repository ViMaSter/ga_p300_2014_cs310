#include "GDIScreenRenderer.h"

bool GDIScreenRenderer::Initialize(HWND* hWnd) {
	WindowHandle = hWnd;

	PaintStruct = { 0 };

	// Create a new buffer based on our window (basically just like when drawing image)
	ScreenBuffer2 = CreateCompatibleDC(GetWindowDC(*hWnd));
	// Create a bitmap based on that new buffer
	ScreenBuffer2Bitmap = CreateCompatibleBitmap(GetWindowDC(*hWnd), 800, 600);
	// Set the bitmap to be our new screen buffer (therefore, things drawn on Buffer2 will also land on Bitmap2)
	SelectObject(ScreenBuffer2, ScreenBuffer2Bitmap);

	CompatibleContextHandle = CreateCompatibleDC(GetWindowDC(*hWnd));
	ImageVector = std::vector<IImage*>();
	Images = std::vector<GDIImage>();

	BackgroundBrush = CreateSolidBrush(0x00000000);
	return true;
}

void GDIScreenRenderer::Shutdown(HWND* hWnd) {

}

IImage* GDIScreenRenderer::LoadImage(const TCHAR* filename) {
	GDIImage image = GDIImage();

	image.CreateFromFile(filename);

	Images.push_back(image);
	ImageVector.push_back(&Images[Images.size() - 1]);
	return ImageVector[Images.size() - 1];
}

void GDIScreenRenderer::BeginDraw() {
	ScreenBuffer1 = BeginPaint(*WindowHandle, &PaintStruct);
	RECT r = { 0, 0, 800, 600 };
	FillRect(ScreenBuffer2, (const RECT*)&r, BackgroundBrush);
}

void GDIScreenRenderer::EndDraw() {
	// When we're done drawing, "copy" ScreenBuffer2 (backbuffer) to ScreenBuffer1 (frontbuffer)
	StretchBlt(ScreenBuffer1, 0, 0, 800, 600, ScreenBuffer2, 0, 0, 800, 600, SRCCOPY);

	EndPaint(*WindowHandle, &PaintStruct);
}

void GDIScreenRenderer::DrawImage(int mapId, const RECT* screenPosition, const RECT* imagePosition) {
	return DrawImage((IImage*)&ImageVector[mapId], screenPosition, imagePosition);
}

void GDIScreenRenderer::DrawImage(IImage* image, const RECT* screenPosition, const RECT* imagePosition) {
	SelectObject(CompatibleContextHandle, ((GDIImage*)image)->Bitmap);
	StretchBlt(
		ScreenBuffer2,
		screenPosition->left, screenPosition->top, screenPosition->right, screenPosition->bottom,
		CompatibleContextHandle,
		imagePosition->left, imagePosition->top, imagePosition->right, imagePosition->bottom,
		SRCCOPY);
}

void GDIScreenRenderer::DrawText(const RECT* screenPosition, const TCHAR* string) {
	::DrawText(ScreenBuffer2, string, -1, (LPRECT)screenPosition, DT_LEFT | DT_TOP);
}