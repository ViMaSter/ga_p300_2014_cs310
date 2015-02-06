#pragma once
#include "FileHandle.h"
#include "Bitmap.h"
#include "IImage.h"
#include <vector>
#include <string>

class GLImage : public IImage
{
public:
	std::vector<ColorInfoA> PixelWithAlpha;
	
	GLImage::GLImage();

	virtual void CreateFromFile(const TCHAR* pathToFile) override;
};

