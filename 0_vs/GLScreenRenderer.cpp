#include "GLScreenRenderer.h"

bool GLScreenRenderer::Initialize(HWND* hWnd) {
	WindowHandle = hWnd;
	ScreenBuffer = GetWindowDC(*WindowHandle);
	ImageVector = std::vector<IImage*>();
	Images = std::vector<GLImage>();


	PixelFormatDescriptor = { 0 };
	PixelFormatDescriptor.nSize = sizeof(PixelFormatDescriptor);
	PixelFormatDescriptor.nVersion = 1;
	PixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	PixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	PixelFormatDescriptor.cColorBits = 24;
	PixelFormatDescriptor.cDepthBits = 0;
	PixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;


	// Request a pixel format, that's as close as the specified one available 
	int PixelFormat = ChoosePixelFormat(ScreenBuffer, &PixelFormatDescriptor);

	if (PixelFormat == 0) {
		return false;
	}


	// Set the pixel format we got from ChoosePixelFormat
	if (!SetPixelFormat(ScreenBuffer, PixelFormat, &PixelFormatDescriptor)) {
		return false;
	}


	// Create a rendering context
	GLRenderingContext = wglCreateContext(ScreenBuffer);

	if (GLRenderingContext == NULL) {
		return false;
	}


	// Make the rendering context the current one
	if (!wglMakeCurrent(ScreenBuffer, GLRenderingContext)) {
		return false;
	}


	// Set up the viewport dimensions
	glViewport(0, 0, 800, 600);


	// Set up the projection matrix to the identity matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	// Set up the orthographic matrix
	glOrtho(0, 800, 600, 0, 0, 10);

	return true;
}


void GLScreenRenderer::Shutdown(HWND* hWnd) {

}


IImage* GLScreenRenderer::LoadImage(const TCHAR* filename) {
	GLImage image = GLImage();
	image.CreateFromFile(filename);
	Images.push_back(image);
	ImageVector.push_back(&Images[Images.size() - 1]);
	return nullptr;
}


void GLScreenRenderer::BeginDraw() {
	glClearColor(0.0f, 0.74f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
}


void GLScreenRenderer::EndDraw() {
	SwapBuffers(ScreenBuffer);
}


void GLScreenRenderer::DrawImage(int MapId, const RECT* screenPosition, const RECT* imagePosition) {
	return DrawImage(ImageVector[MapId], screenPosition, imagePosition);
}


void GLScreenRenderer::DrawImage(IImage* image, const RECT* screenPosition, const RECT* imagePosition) {
	glBegin(GL_QUADS);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glVertex2f(screenPosition->left, screenPosition->top);
	glVertex2f(screenPosition->left, screenPosition->top + screenPosition->bottom);
	glVertex2f(screenPosition->left + screenPosition->right, screenPosition->top + screenPosition->bottom);
	glVertex2f(screenPosition->left + screenPosition->right, screenPosition->top);

	glEnd();

	glDrawPixels(128, 120, GL_BGRA_EXT, GL_UNSIGNED_BYTE, &((GLImage*)image)->PixelWithAlpha[0]);
}


void GLScreenRenderer::DrawText(const RECT* screenPosition, const TCHAR* string) {

}