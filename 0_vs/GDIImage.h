#pragma once
#include "IImage.h"
#include <string>

class GDIImage : public IImage
{
public:
	virtual void CreateFromFile(const TCHAR* pathToFile) override;
};

