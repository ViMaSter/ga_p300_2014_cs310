#include "DXImage.h"

DXImage::DXImage() {
	PixelWithAlpha = std::vector<ColorInfoA>();
}

void DXImage::CreateFromFile(const TCHAR* pathToFile) {
	PathToFile = std::string(pathToFile);
	Bitmap = (HBITMAP) ::LoadImage(NULL, PathToFile.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	BMP Bitmap;

	FileHandle bitmapFileHandle((char*)pathToFile);

	bool v = bitmapFileHandle.IsValid();
	if (v)
		bitmapFileHandle.Read(Bitmap, true);

	Bitmap.print();

	int currentPosition = (int)bitmapFileHandle.FileStream.tellg();

	Size.x = Bitmap.Body.biWidth;
	Size.y = Bitmap.Body.biHeight;

	ColorInfo pixel = ColorInfo();
	for (int x = 0; x < Bitmap.Body.biWidth; x++) {
		for (int y = 0; y < Bitmap.Body.biHeight; y++) {
			bitmapFileHandle.Read(pixel, false);
			PixelWithAlpha.push_back(ColorInfoA(pixel));
		}
	}
}

void DXImage::GetTextureCoordinate(float* x, float* y) {
	*x /= Size.x;
	*y /= Size.y;

	*x = *x;
	*y = 1 - *y;
}