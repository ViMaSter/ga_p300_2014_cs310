#include "GLScreenRenderer.h"

bool GLScreenRenderer::Initialize(HWND* hWnd) {
	WindowHandle = hWnd;
	ScreenBuffer = GetWindowDC(*WindowHandle);
	ImageVector = std::vector<IImage*>();

	PixelFormatDescriptor = { 0 };
	PixelFormatDescriptor.nSize = sizeof(PixelFormatDescriptor);
	PixelFormatDescriptor.nVersion = 1;
	PixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	PixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	PixelFormatDescriptor.cColorBits = 24;
	PixelFormatDescriptor.cDepthBits = 0;
	PixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;

	// Set up our variables
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

	/// Set up matrices
	// Set up the viewport dimensions
	glViewport(0, 0, 800, 600);

	// Set up the projection matrix to the identity matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set up the orthographic matrix
	glOrtho(0, 800, 600, 0, 0, 10);

	/// Ready-up the featureset of GL
	// Enable texture usage
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}


void GLScreenRenderer::Shutdown(HWND* hWnd) {

}


IImage* GLScreenRenderer::LoadImage(const TCHAR* filename) {
	// Prepare variables
	GLenum e = 0;

	// Create our own image
	GLImage image = GLImage();
	image.CreateFromFile(filename);

	// Register a texture slot with OpenGL
	glGenTextures(1, &image.InternalName);
	e = glGetError();

	// Define what kind of texture we're using
	glBindTexture(GL_TEXTURE_2D, image.InternalName);
	e = glGetError();

	// Set the values we prepared (size + actual image-data)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Size.x, image.Size.y, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, &(image.PixelWithAlpha[0]));
	e = glGetError();

	// Add it to our internal storage
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
	// Set a filter-type for mini- and magnifing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Bind the image we have specified to our texture
	glBindTexture(GL_TEXTURE_2D, ((GLImage*)image)->InternalName);

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);

	float texCoord[] = { 0.0f, 0.0f };
	
	texCoord[0] = imagePosition->left;
	texCoord[1] = imagePosition->top;
	((GLImage*)image)->GetTextureCoordinate(&texCoord[0], &texCoord[1]);
	glTexCoord2f(texCoord[0], texCoord[1]);
	glVertex2f(screenPosition->left, screenPosition->top);

	texCoord[0] = imagePosition->left;
	texCoord[1] = imagePosition->top + imagePosition->bottom;
	((GLImage*)image)->GetTextureCoordinate(&texCoord[0], &texCoord[1]);
	glTexCoord2f(texCoord[0], texCoord[1]);
	glVertex2f(screenPosition->left, screenPosition->top + screenPosition->bottom);

	texCoord[0] = imagePosition->right;
	texCoord[1] = imagePosition->top + imagePosition->bottom;





	((GLImage*)image)->GetTextureCoordinate(&texCoord[0], &texCoord[1]);
	glTexCoord2f(texCoord[0], texCoord[1]);
	glVertex2f(screenPosition->left + screenPosition->right, screenPosition->top + screenPosition->bottom);

	texCoord[0] = imagePosition->right;
	texCoord[1] = imagePosition->top;
	((GLImage*)image)->GetTextureCoordinate(&texCoord[0], &texCoord[1]);
	glTexCoord2f(texCoord[0], texCoord[1]);
	glVertex2f(screenPosition->left + screenPosition->right, screenPosition->top);

	glEnd();
}


void GLScreenRenderer::DrawText(const RECT* screenPosition, const TCHAR* string) {

}