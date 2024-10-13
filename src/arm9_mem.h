#pragma once

#include <cstdint>
#include "arm_mem.h"

class ARM9_mem : public ARM_mem {
private:
	uint8_t* itcm; // 00000000h to 00007FFFh & 10000000h to 10007FFFh
	uint32_t dtcm_base_addr{ 0x27C0000 };
	uint8_t* dtcm; // base+0000h to base+3FFFh, default base 027C0000h

	uint8_t* main; // 02000000h to 023FFFFFh
	//uint32_t debugVector; // 027FFD9Ch
	uint8_t* mainMemControl; // 027FFFFEh (2 bytes)

	uint8_t* shared_wram; // 03000000h to 03007FFFh max

	uint8_t* io; // 04000000h to 040010FFh
	uint8_t* myds_debug; // 04FFFXxxh ... if needed

	uint8_t* palettes; // 05000000h to 050007FFh

	uint8_t* vram_A_bg; // 06000000h to 0607FFFFh max
	uint8_t* vram_B_bg; // 06200000h to 0621FFFFh max
	uint8_t* vram_A_obj; // 06400000h to 0643FFFFh max
	uint8_t* vram_B_obj; // 06600000h to 0661FFFFh max

	uint8_t* vram_lcdc; // 06800000h to 068A3FFFh

	uint8_t* oam_A; // 07000000h to 070003FFh
	uint8_t* oam_B; // 07000400h to 070007FFh

	uint8_t* gba_rom; // 08000000h to 09FFFFFFh
	uint8_t* gba_ram; // 0A00000h to 0A00FFFFh

	uint8_t* bios; // FFFF0000h to FFFF7FFFh

public:
	uint32_t GetDTCMBaseAddr() const { return dtcm_base_addr; }

	void SetTCM(uint8_t* instructions, uint8_t* data, uint32_t dtcm_base_addr = 0x27C0000) {
		itcm = instructions;
		dtcm = data;
		this->dtcm_base_addr = dtcm_base_addr;
	}

	void SetMainMemory(uint8_t* ptr) {
		main = ptr;
	}
	/*void SetDebugVector(uint32_t addr) {
		debugVector = addr;
	}*/
	void SetMainMemoryControl(uint8_t* ptr) {
		mainMemControl = ptr;
	}

	void SetSharedWRAM(uint8_t* ptr) {
		shared_wram = ptr;
	}

	void SetIOMaps(uint8_t* ptr) {
		io = ptr;
	}
	void SetMyDSDebug(uint8_t* ptr) {
		myds_debug = ptr;
	}

	void SetPalettes(uint8_t* ptr) {
		palettes = ptr;
	}

	void SetVRAM_A_BG(uint8_t* ptr) {
		vram_A_bg = ptr;
	}
	void SetVRAM_B_BG(uint8_t* ptr) {
		vram_B_bg = ptr;
	}
	void SetVRAM_A_OBJ(uint8_t* ptr) {
		vram_A_obj = ptr;
	}
	void SetVRAM_B_OBJ(uint8_t* ptr) {
		vram_B_obj = ptr;
	}

	void SetVRAM_LCDC(uint8_t* ptr) {
		vram_lcdc = ptr;
	}

	void SetOAM_A(uint8_t* ptr) {
		oam_A = ptr;
	}
	void SetOAM_B(uint8_t* ptr) {
		oam_B = ptr;
	}

	void SetGBAROM(uint8_t* ptr) {
		gba_rom = ptr;
	}
	void SetGBARAM(uint8_t* ptr) {
		gba_ram = ptr;
	}

	void SetBios(uint8_t* ptr) {
		bios = ptr;
	}

	uint8_t* GetPointerFromAddr(uint32_t address);
};