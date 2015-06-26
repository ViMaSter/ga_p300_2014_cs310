#pragma once
#include "IImage.h"
#include "d3d11.h"
#pragma comment(lib, "d3d11.lib")

#include "FileHandle.h"
#include "Bitmap.h"
#include "IImage.h"
#include <vector>
#include <string>

class DXImage : public IImage
{
public:
	std::vector<ColorInfoA> PixelWithAlpha;
	POINT Size;

	ID3D11Texture2D* InternalTexture;
	D3D11_SUBRESOURCE_DATA InternalData;

	DXImage::DXImage();

	void GetTextureCoordinate(float* x, float* y);
	virtual void CreateFromFile(const TCHAR* pathToFile) override;
};

