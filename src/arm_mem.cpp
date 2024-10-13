#include "arm7_mem.h"
#include "arm9_mem.h"

uint8_t* ARM9_mem::GetPointerFromAddr(uint32_t address) {
	// ITCM
	if ((address & 0xEFFF0000) == 0x00000000) return itcm + (address & 0x00007FFF);

	// DTCM
	if ((address & dtcm_base_addr) == dtcm_base_addr) return dtcm + (address & 0x3FFF);

	// MAIN
	if ((address & 0xFFC00000) == 0x02000000) return main + (address & 0x003FFFFF);
	if ((address & 0xFFFFFFFE) == 0x027FFFFE) return mainMemControl + (address & 0x1);

	// Shared WRAM
	if ((address & 0xFF800000) == 0x03000000) return shared_wram + (address & 0x00007FFF);

	// IO
	if ((address & 0xFF800000) == 0x04000000) return io + (address & 0x001FFFFF);

	// MyDS debug
	if ((address & 0xFFFFFF00) == 0x04FFFA00) return myds_debug + (address & 0xFF);

	// Palettes
	if ((address & 0xFF000000) == 0x05000000) return palettes + (address & 0x7FF);

	// VRAMs - TODO : check ?
	if ((address & 0xFFF00000) == 0x06000000) return vram_A_bg + (address & 0x7FFFF);
	if ((address & 0xFFF00000) == 0x06200000) return vram_B_bg + (address & 0x1FFFF);
	if ((address & 0xFFF00000) == 0x06400000) return vram_A_obj + (address & 0x3FFFF);
	if ((address & 0xFFF00000) == 0x06600000) return vram_B_obj + (address & 0x1FFFF);
	if ((address & 0xFFF00000) == 0x06800000) return vram_lcdc + (address & 0xA3FFF);

	// OAM
	if ((address & 0xFFFFF400) == 0x07000000) return oam_A + (address & 0x3FF);
	if ((address & 0xFFFFF400) == 0x07000400) return oam_B + (address & 0x3FF);

	// GBA
	if ((address & 0xFF000000) == 0x0A000000) return gba_ram + (address & 0x0000FFFF);
	if ((address & 0xF8000000) == 0x08000000) return gba_rom + (address & 0x01FFFFFF);

	// BIOS
	if ((address & 0xFFFF0000) == 0xFFFF0000) return bios + (address & 0x00007FFF);

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