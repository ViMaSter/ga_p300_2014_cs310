#include "GDIScreenRenderer.h"

bool GDIScreenRenderer::Initialize(HWND* hWnd) {
	WindowHandle = hWnd;
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
	DeviceContextHandle = BeginPaint(*WindowHandle, PaintStruct);
}

void GDIScreenRenderer::EndDraw() {
	EndPaint(*WindowHandle, PaintStruct);
}


void GDIScreenRenderer::DrawImage(int imageId, const RECT* sourcePosition, const RECT* destinationPosition) {
	SelectObject(CompatibleContextHandle, imageVector[imageId].Bitmap);
	StretchBlt(DeviceContextHandle, destinationPosition->left, destinationPosition->top, destinationPosition->right, destinationPosition->bottom, CompatibleContextHandle, sourcePosition->left, sourcePosition->top, sourcePosition->right, sourcePosition->bottom, SRCCOPY);
}

void GDIScreenRenderer::DrawText(const RECT* screenPosition, const TCHAR* string) {
	::DrawText(DeviceContextHandle, string, -1, (LPRECT)screenPosition, DT_LEFT | DT_TOP);
}