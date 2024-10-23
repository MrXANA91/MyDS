#pragma once

#include <cstdint>

class ARM_mem {
public:
	virtual uint8_t* GetPointerFromAddr(uint32_t) = 0;
	static uint32_t GetBytesAtPointer(uint8_t*, int);

	static uint32_t GetWordAtPointer(uint8_t* startPtr) {
		return GetBytesAtPointer(startPtr, 4);
	}
	static uint16_t GetHalfWordAtPointer(uint8_t* startPtr) {
		return GetBytesAtPointer(startPtr, 2);
	}
	static uint8_t GetByteAtPointer(uint8_t* ptr) {
		return *ptr;
	}

	static void SetWordAtPointer(uint8_t* startPtr, uint32_t word) {
		*startPtr = word & 0xFF;
		*(startPtr + 1) = (word & 0xFF00) >> 8;
		*(startPtr + 2) = (word & 0xFF0000) >> 16;
		*(startPtr + 3) = (word & 0xFF000000) >> 24;
	}
	static void SetHalfWordAtPointer(uint8_t* startPtr, uint16_t halfWord) {
		*startPtr = halfWord & 0xFF;
		*(startPtr + 1) = (halfWord & 0xFF00) >> 8;
	}
	static void SetByteAtPointer(uint8_t* startPtr, uint8_t byte) {
		*startPtr = byte;
	}
};
