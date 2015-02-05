#include "GDIImage.h"

void GDIImage::CreateFromFile(const TCHAR* pathToFile) {
	PathToFile = std::string(pathToFile);
	Bitmap = (HBITMAP) ::LoadImage(NULL, PathToFile.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}