#include "GLScreenRenderer.h"

bool GLScreenRenderer::Initialize(HWND* hWnd) {
	WindowHandle = hWnd;
	ScreenBuffer = GetWindowDC(*WindowHandle);

	pfd = { 0 };
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 0;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int PixelFormat = ChoosePixelFormat(ScreenBuffer, &pfd);

	if (PixelFormat == 0) {
		return false;
	}

	if (!SetPixelFormat(ScreenBuffer, PixelFormat, &pfd)) {
		return false;
	}

	GLRenderingContext = wglCreateContext(ScreenBuffer);

	if (GLRenderingContext == NULL) {
		return false;
	}

	if (!wglMakeCurrent(ScreenBuffer, GLRenderingContext)) {
		return false;
	}

	glViewport(0, 0, 800, 600);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, 600, 800, 0, 0, 10);

	return true;
}

void GLScreenRenderer::Shutdown(HWND* hWnd) {

}


IImage* GLScreenRenderer::LoadImage(const TCHAR* filename) {
	return nullptr;
}


void GLScreenRenderer::BeginDraw() {
	glClearColor(0.0f, 0.74f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);
}

void GLScreenRenderer::EndDraw() {
	SwapBuffers(ScreenBuffer);
}


void GLScreenRenderer::DrawImage(int MapId, const RECT* screenPosition, const RECT* imagePosition) {

}

void GLScreenRenderer::DrawImage(IImage* image, const RECT* screenPosition, const RECT* imagePosition) {

}

void GLScreenRenderer::DrawText(const RECT* screenPosition, const TCHAR* string) {

}