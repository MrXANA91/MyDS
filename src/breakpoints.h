#pragma once

#include <cstdint>
#include <iostream>

class Breakpoint {
private:
	bool initialised{ false };

	bool active{ false };
	uint32_t address{ 0 };

	Breakpoint* next{ nullptr };
	Breakpoint* parent{ nullptr };

	void init(uint32_t bpAddr);
public:
	bool Check(uint32_t currentAddr);
	bool Add(uint32_t addr);
	bool Remove(int index);

	bool IsActive(int index) const;

	bool SetActive(int index, bool enable);

	bool GetAddr(int index, uint32_t &addr) const;
	int GetSize() const;
};