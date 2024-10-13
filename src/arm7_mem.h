#pragma once

#include <cstdint>
#include "arm_mem.h"

class ARM7_mem {
private:
	uint8_t* bios; // 00000000h to 00003FFFh

	uint8_t* main; // 02000000h to 023FFFFFh

	uint8_t* shared_wram; // 03000000h to 03007FFFh max
	uint8_t* wram; // 03800000h to 0380FFFFh

	uint8_t* io; // 04000000h to 04100013h
	uint8_t* io_wifi; // 04800000h to 0480BFFFh

	uint8_t* vram_as_wram; // 06000000h to 0603FFFFh max

	uint8_t* gba_rom; // 08000000h to 09FFFFFFh max
	uint8_t* gba_ram; // 0A000000h to 0A00FFFFh max

public:
	void SetBios(uint8_t* ptr) {
		bios = ptr;
	}

	void SetMainMemory(uint8_t* ptr) {
		main = ptr;
	}

	void SetSharedWRAM(uint8_t* ptr) {
		shared_wram = ptr;
	}
	void SetWRAM(uint8_t* ptr) {
		wram = ptr;
	}

	void SetIOMaps(uint8_t* ptr) {
		io = ptr;
	}
	void SetWiFiIOMaps(uint8_t* ptr) {
		io_wifi = ptr;
	}

	void SetVRAMasWRAM(uint8_t* ptr) {
		vram_as_wram = ptr;
	}

	void SetGBAROM(uint8_t* ptr) {
		gba_rom = ptr;
	}
	void SetGBARAM(uint8_t* ptr) {
		gba_ram = ptr;
	}

	uint8_t* GetPointerFromAddr(uint32_t address);
};