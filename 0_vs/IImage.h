#pragma once
#include "main.h"
#include "Wingdi.h"
#include <string>

class IImage
{
public:
	std::string PathToFile;
	HBITMAP Bitmap;

	virtual void CreateFromFile(const TCHAR* pathToFile) = 0;
};

