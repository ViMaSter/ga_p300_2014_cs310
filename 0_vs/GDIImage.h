#pragma once
#include "IImage.h"
#include <string>

class GDIImage : public IImage
{
public:
	std::string PathToFile;
	HBITMAP Bitmap;

	virtual void CreateFromFile(const TCHAR* pathToFile);
};

