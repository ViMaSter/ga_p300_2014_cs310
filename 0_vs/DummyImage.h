#pragma once
#include "IImage.h"

class DummyImage : public IImage
{
public:
	TCHAR* PathToFile;
	BITMAP Bitmap;

	virtual void CreateFromFile(TCHAR* pathToFile);
};

