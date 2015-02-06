#include "memory.h"
#include "stdlib.h"
#include <iostream>
#include <fstream>

template < typename T > class MemoryBuffer {
public:
	T* MemoryPointer;
	unsigned int Count;

	// Con- + Destructor
	MemoryBuffer(const MemoryBuffer<T>& source) {
		Count = source.GetCount();
		MemoryPointer = (T*)malloc(sizeof(T)*Count);
		if (MemoryPointer == nullptr) {
			throw "Out of Memory";
		}
		memcpy(MemoryPointer, source.MemoryPointer, source.GetSize());
	}

	MemoryBuffer(unsigned int count) {
		Count = count;
		MemoryPointer = (T*)malloc(sizeof(T)*count);
		if (MemoryPointer == nullptr) {
			throw "Out of Memory";
		}
	}

	~MemoryBuffer() {
		CleanUp();
	}

	void CleanUp() {
		if (MemoryPointer == NULL)
			free(MemoryPointer);

		Count = 0;
		MemoryPointer = 0;
	}

	operator T*() {
		return GetBuffer();
	}

	MemoryBuffer& operator =(const MemoryBuffer< T >& aOther) {
		CleanUp();

		Count = aOther.GetCount();
		MemoryPointer = (T*)malloc(sizeof(T)*Count);
		if (MemoryPointer == nullptr) {
			throw "Out of Memory";
		}
		memcpy(MemoryPointer, aOther.MemoryPointer, aOther.GetSize());
	}

	// Functions
	T* GetBuffer() {
		return (T*)MemoryPointer;
	};

	void SetBuffer(int index, T value) {
		((T*)MemoryPointer)[index] = value;
	}

	unsigned int GetCount() const {
		return Count;
	}

	unsigned int GetSize() const {
		return sizeof(T)*Count;
	}
};