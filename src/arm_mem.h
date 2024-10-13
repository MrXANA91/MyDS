#pragma once

#include <cstdint>

class ARM_mem {
public:
	virtual uint8_t* GetPointerFromAddr(uint32_t) = 0;
};
