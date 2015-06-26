#pragma once
#include "memory.h"
#include "stdlib.h"
#include <iostream>
#include <fstream>

class FileHandle {
public:
	std::fstream FileStream;
	bool IsValid() {
		return FileStream.good();
	}

	FileHandle(char* filename) {
		FileStream.open(filename, std::fstream::in | std::fstream::out | std::fstream::binary);
	}

	~FileHandle() {
		FileStream.close();
	}

	template <typename T > bool Read(T& writeTo, bool resetPointer) {
		try {
			if (resetPointer) {
				FileStream.seekg(0, FileStream.beg);
			}

			FileStream.read((char*)&writeTo, sizeof(T));
		}
		catch (std::ifstream::failure e) {
			return false;
		}
		return true;
	}

	template <typename T > bool Write(T& content) {
		return Write<T>(content, false);
	}

	template <typename T > bool Write(T& content, bool append) {
		try {
			if (!append) {
				FileStream.seekg(0, FileStream.beg);
			}

			char* buffer = new char[sizeof(T)];
			memcpy(buffer, &content, sizeof(T));

			FileStream.write(buffer, sizeof(T));
		}
		catch (std::ifstream::failure e) {
			return false;
		}
		return true;
	}
};