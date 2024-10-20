#pragma once

#include <cstdint>
#include "arm_mem.h"

class ARM9_mem : public ARM_mem {
private:
	uint8_t* itcm; // 00000000h to 00007FFFh & 10000000h to 10007FFFh
	uint8_t* dtcm; // base+0000h to base+3FFFh, default base 027C0000h

	uint8_t* main; // 02000000h to 023FFFFFh
	//uint32_t debugVector; // 027FFD9Ch
	uint8_t* mainMemControl; // 027FFFFEh (2 bytes)

	uint8_t* shared_wram; // 03000000h to 03007FFFh max

	uint8_t* io; // 04000000h to 040010FFh
	uint8_t* myds_debug; // 04FFFAxxh ... if needed

	uint8_t* palettes; // 05000000h to 050007FFh
	uint8_t* vram_A_bg; // 06000000h to 0607FFFFh max
	uint8_t* vram_B_bg; // 06200000h to 0621FFFFh max
	uint8_t* vram_A_obj; // 06400000h to 0643FFFFh max
	uint8_t* vram_B_obj; // 06600000h to 0661FFFFh max
	uint8_t* vram_lcdc; // 06800000h to 068A3FFFh
	uint8_t* oam_A; // 07000000h to 070003FFh
	uint8_t* oam_B; // 07000400h to 070007FFh

	uint8_t* gba_rom; // 08000000h to 09FFFFFFh
	uint8_t* gba_ram; // 0A000000h to 0A00FFFFh

	uint8_t* bios; // FFFF0000h to FFFF7FFFh

public:
	static const uint32_t ITCM_ADDR = 0x0;
	static const size_t ITCM_SIZE = 0x8000;
	static const uint32_t DEFAULT_DTCM_ADDR = 0x27C0000;
	static uint32_t DTCM_ADDR;
	static const size_t DTCM_SIZE = 0x4000;
	static const uint32_t MAINMEMORY_ADDR = 0x2000000;
	static const size_t MAINMEMORY_SIZE = 0x40000;
	static const uint32_t SHAREDWRAM_ADDR = 0x03000000;
	static const size_t SHAREDWRAM_SIZE = 0x8000;

	static const uint32_t IO_ADDR = 0x04000000;
	static const size_t IO_SIZE = 0x1100;
	static const uint32_t MAINMEMCTRL_ADDR = 0x27FFFFE;
	static const size_t MAINMEMCTRL_SIZE = 0x2;
	static const uint32_t MYDSDEBUG_ADDR = 0x04FFFA00;
	static const size_t MYDSDEBUG_SIZE = 0x0;

	static const uint32_t PALETTES_ADDR = 0x5000000;
	static const size_t PALETTES_SIZE = 0x800;
	static const uint32_t VRAMABG_ADDR = 0x06000000;
	static const size_t VRAMABG_SIZE = 0x80000;
	static const uint32_t VRAMBBG_ADDR = 0x06200000;
	static const size_t VRAMBBG_SIZE = 0x20000;
	static const uint32_t VRAMAOBJ_ADDR = 0x06400000;
	static const size_t VRAMAOBJ_SIZE = 0x40000;
	static const uint32_t VRAMBOBJ_ADDR = 0x06600000;
	static const size_t VRAMBOBJ_SIZE = 0x20000;
	static const uint32_t VRAMLCDC_ADDR = 0x06800000;
	static const size_t VRAMLCDC_SIZE = 0xA4000;
	static const uint32_t OAMA_ADDR = 0x7000000;
	static const uint32_t OAMB_ADDR = 0x7000400;
	static const size_t OAM_SIZE = 0x400;

	static const uint32_t BIOS_ADDR = 0xFFFF0000;
	static const size_t BIOS_SIZE = 0x8000;
	static const uint32_t GBAROM_ADDR = 0x8000000;
	static const size_t GBAROM_SIZE = 0x2000000;
	static const uint32_t GBARAM_ADDR = 0xA000000;
	static const size_t GBARAM_SIZE = 0x10000;

	void SetTCM(uint8_t* instructions, uint8_t* data, uint32_t dtcm_base_addr = DEFAULT_DTCM_ADDR) {
		itcm = instructions;
		dtcm = data;
		DTCM_ADDR = dtcm_base_addr;
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