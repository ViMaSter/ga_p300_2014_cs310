#include "GDIScreenRenderer.h"

bool GDIScreenRenderer::Initialize(HWND* hWnd) {
	WindowHandle = hWnd;

	// Create a new buffer based on our window (basically just like when drawing image)
	ScreenBuffer2 = CreateCompatibleDC(GetWindowDC(*hWnd));
	// Create a bitmap based on that new buffer
	ScreenBuffer2Bitmap = CreateCompatibleBitmap(GetWindowDC(*hWnd), 800, 600);
	// Set the bitmap to be our new screen buffer (therefore, things drawn on Buffer2 will also land on Bitmap2)
	SelectObject(ScreenBuffer2, ScreenBuffer2Bitmap);

	CompatibleContextHandle = CreateCompatibleDC(GetWindowDC(*hWnd));
	imageVector = std::vector<GDIImage>();
	return true;
}

void GDIScreenRenderer::Shutdown(HWND* hWnd) {

}


IImage* GDIScreenRenderer::LoadImage(const TCHAR* filename) {
	GDIImage image = GDIImage();
	image.CreateFromFile(filename);
	imageVector.push_back(image);
	return nullptr;
}

void GDIScreenRenderer::BeginDraw() {
	ScreenBuffer1 = BeginPaint(*WindowHandle, PaintStruct);
}

void GDIScreenRenderer::EndDraw() {
	// When we're done drawing, "copy" ScreenBuffer2 to ScreenBuffer1
	StretchBlt(ScreenBuffer1, 0, 0, 800, 600, ScreenBuffer2, 0, 0, 800, 600, SRCCOPY);

	EndPaint(*WindowHandle, PaintStruct);
}


void GDIScreenRenderer::DrawImage(int imageId, const RECT* sourcePosition, const RECT* destinationPosition) {
	SelectObject(CompatibleContextHandle, imageVector[imageId].Bitmap);
	StretchBlt(ScreenBuffer2, destinationPosition->left, destinationPosition->top, destinationPosition->right, destinationPosition->bottom, CompatibleContextHandle, sourcePosition->left, sourcePosition->top, sourcePosition->right, sourcePosition->bottom, SRCCOPY);
}

void GDIScreenRenderer::DrawText(const RECT* screenPosition, const TCHAR* string) {
	::DrawText(ScreenBuffer2, string, -1, (LPRECT)screenPosition, DT_LEFT | DT_TOP);
}