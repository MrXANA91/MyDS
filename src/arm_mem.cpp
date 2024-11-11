#include "arm7_mem.h"
#include "arm9_mem.h"

#define RETURN_PTR_IF_IN_RANGE(address, startRangeAddress, rangeSize, rangePtr) if ((address >= startRangeAddress) && (address < (startRangeAddress + rangeSize))) return rangePtr + ((address - startRangeAddress) % rangeSize);

uint32_t ARM9_mem::DTCM_ADDR = 0x27C0000;

uint8_t* ARM9_mem::GetPointerFromAddr(uint32_t address) {
	// ITCM
	RETURN_PTR_IF_IN_RANGE(address, ITCM_ADDR, ITCM_SIZE, itcm);
	RETURN_PTR_IF_IN_RANGE(address, ITCM_ADDR + 0x10000000, ITCM_SIZE, itcm);

	// DTCM
	RETURN_PTR_IF_IN_RANGE(address, DTCM_ADDR, DTCM_SIZE, dtcm);

	// MAIN
	RETURN_PTR_IF_IN_RANGE(address, MAINMEMORY_ADDR, MAINMEMORY_SIZE, main);
	RETURN_PTR_IF_IN_RANGE(address, MAINMEMCTRL_ADDR, MAINMEMCTRL_SIZE, mainMemControl);

	// Shared WRAM
	RETURN_PTR_IF_IN_RANGE(address, SHAREDWRAM_ADDR, SHAREDWRAM_SIZE, shared_wram);

	// IO
	RETURN_PTR_IF_IN_RANGE(address, IO_ADDR, IO_SIZE, io);

	// MyDS debug
	if ((address >= (MYDSDEBUG_ADDR)) && (address < (MYDSDEBUG_ADDR + MYDSDEBUG_SIZE))) return myds_debug + ((address - MYDSDEBUG_ADDR) % MYDSDEBUG_SIZE);
	RETURN_PTR_IF_IN_RANGE(address, IO_ADDR, IO_SIZE, io);

	// Palettes
	if ((address >= (PALETTES_ADDR)) && (address < (PALETTES_ADDR + PALETTES_SIZE))) return palettes + ((address - PALETTES_ADDR) % PALETTES_SIZE);
	RETURN_PTR_IF_IN_RANGE(address, IO_ADDR, IO_SIZE, io);

	// VRAMs - TODO : check ?
	RETURN_PTR_IF_IN_RANGE(address, VRAMABG_ADDR, VRAMABG_SIZE, vram_A_bg);
	RETURN_PTR_IF_IN_RANGE(address, VRAMBBG_ADDR, VRAMBBG_SIZE, vram_B_bg);
	RETURN_PTR_IF_IN_RANGE(address, VRAMAOBJ_ADDR, VRAMAOBJ_SIZE, vram_A_obj);
	RETURN_PTR_IF_IN_RANGE(address, VRAMBOBJ_ADDR, VRAMBOBJ_SIZE, vram_B_obj);
	RETURN_PTR_IF_IN_RANGE(address, VRAMLCDC_ADDR, VRAMLCDC_SIZE, vram_lcdc);

	// OAM
	RETURN_PTR_IF_IN_RANGE(address, OAMA_ADDR, OAM_SIZE, oam_A);
	RETURN_PTR_IF_IN_RANGE(address, OAMB_ADDR, OAM_SIZE, oam_B);

	// GBA
	RETURN_PTR_IF_IN_RANGE(address, GBARAM_ADDR, GBARAM_SIZE, gba_ram);
	RETURN_PTR_IF_IN_RANGE(address, GBAROM_ADDR, GBAROM_SIZE, gba_rom);

	// BIOS
	RETURN_PTR_IF_IN_RANGE(address, BIOS_ADDR, BIOS_SIZE, bios);

	// TODO : what if address invalid ?f
	return nullptr;
}

uint8_t* ARM7_mem::GetPointerFromAddr(uint32_t address) {
	// BIOS
	RETURN_PTR_IF_IN_RANGE(address, BIOS_ADDR, BIOS_SIZE, bios);

	// MAIN
	RETURN_PTR_IF_IN_RANGE(address, MAINMEMORY_ADDR, MAINMEMORY_SIZE, main);

	// WRAM & Shared WRAM
	RETURN_PTR_IF_IN_RANGE(address, SHAREDWRAM_ADDR, SHAREDWRAM_SIZE, shared_wram);
	RETURN_PTR_IF_IN_RANGE(address, WRAM_ADDR, WRAM_SIZE, wram);

	// IO
	RETURN_PTR_IF_IN_RANGE(address, IO_ADDR, IO_SIZE, io);
	RETURN_PTR_IF_IN_RANGE(address, IOWIFI_ADDR, IOWIFI_SIZE, io_wifi);

	// VRAM as WRAM
	RETURN_PTR_IF_IN_RANGE(address, VRAM_AS_WRAM_ADDR, VRAM_AS_WRAM_SIZE, vram_as_wram);

	// GBA
	RETURN_PTR_IF_IN_RANGE(address, GBAROM_ADDR, GBAROM_SIZE, gba_rom);
	RETURN_PTR_IF_IN_RANGE(address, GBARAM_ADDR, GBARAM_SIZE, gba_ram);

	// TODO : what if address invalid ?
	return nullptr;
}

uint64_t ARM_mem::GetBytesAtPointer(uint8_t* startPtr, int size) {
	if (startPtr == nullptr) {
		return 0;
	}
	uint8_t* ptr = startPtr;
	uint64_t value = 0;
	if (size > 8) size = 8;
	for (int i = 0; i < size; i++) {
		value |= (static_cast<uint64_t>(*ptr)) << (8 * i);
		ptr++;
	}
	return value;
}

