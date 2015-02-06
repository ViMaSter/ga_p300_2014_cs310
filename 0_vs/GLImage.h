#pragma once
#include "Windows.h"
#pragma comment(lib, "opengl32.lib")
#include "Gl/Gl.h"

#include "FileHandle.h"
#include "Bitmap.h"
#include "IImage.h"
#include <vector>
#include <string>

class GLImage : public IImage
{
public:
	std::vector<ColorInfoA> PixelWithAlpha;
	POINT Size;

	GLuint InternalName;
	
	GLImage::GLImage();

	void GetTextureCoordinate(float* x, float* y);
	virtual void CreateFromFile(const TCHAR* pathToFile) override;
};

