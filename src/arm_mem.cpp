#include "arm7_mem.h"
#include "arm9_mem.h"

uint32_t ARM9_mem::DTCM_ADDR = 0x27C0000;

uint8_t* ARM9_mem::GetPointerFromAddr(uint32_t address) {
	// ITCM
	if (((address >= ITCM_ADDR) && (address < (ITCM_ADDR + ITCM_SIZE))) || ((address > (0x10000000 + ITCM_ADDR)) && (address < (0x10000000 + ITCM_ADDR + ITCM_SIZE)))) return itcm + (address & (ITCM_SIZE-1));

	// DTCM
	if ((address >= DTCM_ADDR) && (address < (DTCM_ADDR + DTCM_SIZE))) return dtcm + (address & (DTCM_SIZE-1));

	// MAIN
	if ((address >= (MAINMEMORY_ADDR)) && (address < (MAINMEMORY_ADDR + MAINMEMORY_SIZE))) return main + (address & (MAINMEMORY_SIZE-1));
	if ((address >= (MAINMEMCTRL_ADDR)) && (address < (MAINMEMCTRL_ADDR + MAINMEMCTRL_SIZE))) return mainMemControl + (address & (MAINMEMCTRL_SIZE-1));

	// Shared WRAM
	if ((address >= (SHAREDWRAM_ADDR)) && (address < (SHAREDWRAM_ADDR + SHAREDWRAM_SIZE))) return shared_wram + (address & (SHAREDWRAM_SIZE-1));

	// IO
	if ((address >= (IO_ADDR)) && (address < (IO_ADDR + IO_SIZE))) return io + (address & (IO_SIZE - 1));

	// MyDS debug
	if ((address >= (MYDSDEBUG_ADDR)) && (address < (MYDSDEBUG_ADDR + MYDSDEBUG_SIZE))) return myds_debug + (address & (MYDSDEBUG_SIZE-1));

	// Palettes
	if ((address >= (PALETTES_ADDR)) && (address < (PALETTES_ADDR + PALETTES_SIZE))) return palettes + (address & (PALETTES_SIZE-1));

	// VRAMs - TODO : check ?
	if ((address >= (VRAMABG_ADDR)) && (address < (VRAMABG_ADDR + VRAMABG_SIZE))) return vram_A_bg + (address & (VRAMABG_SIZE-1));
	if ((address >= (VRAMBBG_ADDR)) && (address < (VRAMBBG_ADDR + VRAMBBG_SIZE))) return vram_B_bg + (address & (VRAMBBG_SIZE-1));
	if ((address >= (VRAMAOBJ_ADDR)) && (address < (VRAMAOBJ_ADDR + VRAMAOBJ_SIZE))) return vram_A_obj + (address & (VRAMAOBJ_SIZE-1));
	if ((address >= (VRAMBOBJ_ADDR)) && (address < (VRAMBOBJ_ADDR + VRAMBOBJ_SIZE))) return vram_B_obj + (address & (VRAMBOBJ_SIZE - 1));
	if ((address >= (VRAMLCDC_ADDR)) && (address < (VRAMLCDC_ADDR + VRAMLCDC_SIZE))) return vram_lcdc + (address & (VRAMLCDC_SIZE-1));

	// OAM
	if ((address >= (OAMA_ADDR)) && (address < (OAMA_ADDR + OAM_SIZE))) return oam_A + (address & (OAM_SIZE-1));
	if ((address >= (OAMB_ADDR)) && (address < (OAMB_ADDR + OAM_SIZE))) return oam_B + (address & (OAM_SIZE - 1));

	// GBA
	if ((address >= (GBARAM_ADDR)) && (address < (GBARAM_ADDR + GBARAM_SIZE))) return gba_ram + (address & (GBARAM_SIZE-1));
	if ((address >= (GBAROM_ADDR)) && (address < (GBAROM_ADDR + GBAROM_SIZE))) return gba_rom + (address & (GBAROM_SIZE - 1));

	// BIOS
	if ((address >= (BIOS_ADDR)) && (address < (BIOS_ADDR + BIOS_SIZE))) return bios + (address & (BIOS_SIZE - 1));

	// TODO : what if address invalid ?
	return nullptr;
}

uint8_t* ARM7_mem::GetPointerFromAddr(uint32_t address) {
	// BIOS
	if ((address & 0xFFFF0000) == 0x00000000) return bios + (address & 0x00003FFF);

	// MAIN
	if ((address & 0xFFC00000) == 0x02000000) return main + (address & 0x003FFFFF);

	// WRAM & Shared WRAM
	if ((address & 0xFF800000) == 0x03000000) return shared_wram + (address & 0x00007FFF);
	if ((address & 0xFF800000) == 0x03800000) return wram + (address & 0x0000FFFF);

	// IO
	if ((address & 0xFF800000) == 0x04000000) return io + (address & 0x001FFFFF);
	if ((address & 0xFF800000) == 0x04800000) return io_wifi + (address & 0x0000BFFF);

	// VRAM as WRAM
	if ((address & 0xFF000000) == 0x06000000) return vram_as_wram + (address & 0x0003FFFF);

	// GBA
	if ((address & 0xFF000000) == 0x0A000000) return gba_ram + (address & 0x0000FFFF);
	if ((address & 0xF8000000) == 0x08000000) return gba_rom + (address & 0x01FFFFFF);

	// TODO : what if address invalid ?
	return nullptr;
}

uint64_t ARM_mem::GetBytesAtPointer(uint8_t* startPtr, int size) {
	uint8_t* ptr = startPtr;
	uint64_t value = 0;
	if (size > 8) size = 8;
	for (int i = 0; i < size; i++) {
		value |= (static_cast<uint64_t>(*ptr)) << (8 * i);
		ptr++;
	}
	return value;
}

