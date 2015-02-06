#include "memory.h"
#include "stdlib.h"
#include <iostream>
#include <fstream>

#pragma pack(2)
struct BitmapInformationHeader {
	unsigned long biSize;
	signed long biWidth;
	signed long biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long biCompression;
	unsigned long biSizeImage;
	signed long biXPelsPerMeter;
	signed long biYPelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
};

struct BitmapFileHeader {
	unsigned short bfType;
	unsigned long bfSize;
	unsigned long bfReserved;
	unsigned long bfOffBits;
};

struct BMP {
	BitmapFileHeader Header;
	BitmapInformationHeader Body;

	void print() {
		char* comp[] =
		{
			"BI_RGB",
			"BI_RLE8",
			"BI_RLE4",
			"BI_BITFIELDS"
		};

		std::cout << "--- FILEINFO START ---\r\n";
		std::cout << "Width:                " << Body.biWidth << "\r\n";
		std::cout << "Height:               " << Body.biHeight << "\r\n";
		std::cout << "Compression: (int):   " << Body.biCompression << "\r\n";
		std::cout << "Compression: (beaut): " << comp[Body.biCompression] << "\r\n";
		std::cout << "--- FILEINFO  END  ---\r\n";
	}
};

struct ColorInfo {
	unsigned char B;
	unsigned char G;
	unsigned char R;

	void Invert() {
		B = 255 - B;
		G = 255 - G;
		R = 255 - R;
	}
};

struct ColorInfoA : ColorInfo {
	unsigned char A;

	ColorInfoA() {
		B = 0;
		G = 0;
		R = 0;
		A = 255;
	}

	ColorInfoA(ColorInfo c) {
		B = c.B;
		G = c.G;
		R = c.R;
		A = (c.R == 255 && c.G == 255 && c.B == 255) ? 0 : 255;
	}
};