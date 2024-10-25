// MyDS.cpp : définit le point d'entrée de l'application.
//

#include "MyDS.h"

static void InitArm9Memory(ARM9_mem &mem);
static void ClearArm9MemReg(ARM9_mem mem, uint32_t startAddr, size_t size, int value = 0);

static void LoadCustomTestProg(ARM9_mem &mem) {
	uint8_t* ptr;

	// Infinite loop at address 0x02000000
	ptr = mem.GetPointerFromAddr(mem.MAINMEMORY_ADDR);
	ARM_mem::SetWordAtPointer(ptr, 0xEAFFFFFE);

	// Bios is 'Load R0 with 0x02000000 & go to addr located at R0'
	ptr = mem.GetPointerFromAddr(mem.BIOS_ADDR);
	ARM_mem::SetWordAtPointer(ptr, 0xE3A00402);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0xE12FFF10);
}

int main()
{
	Cpu arm9;
	ARM9_mem mem;
	NDSRom nds("NDS-Files\\TinyFB.nds");

	arm9.SetBootAddr(nds.GetARM9StartAddress());
	arm9.SetMMU(&mem);

	InitArm9Memory(mem);

	nds.SetARM9ProgramMemory(mem);

	arm9.PrintDebug();
	for (int i = 0; i < 20; i++) {
		arm9.DebugStep();
		arm9.PrintDebug();
	}

	return 0;
}

static void InitArm9Memory(ARM9_mem &mem) {
	mem.SetTCM(static_cast<uint8_t*>(malloc(mem.ITCM_SIZE)), static_cast<uint8_t*>(malloc(mem.DTCM_SIZE)));
	ClearArm9MemReg(mem, mem.ITCM_ADDR, mem.ITCM_SIZE);
	ClearArm9MemReg(mem, mem.DTCM_ADDR, mem.DTCM_SIZE);
	mem.SetMainMemory(static_cast<uint8_t*>(malloc(mem.MAINMEMORY_SIZE)));
	ClearArm9MemReg(mem, mem.MAINMEMORY_ADDR, mem.MAINMEMORY_SIZE);
	mem.SetSharedWRAM(static_cast<uint8_t*>(malloc(mem.SHAREDWRAM_SIZE)));
	ClearArm9MemReg(mem, mem.SHAREDWRAM_ADDR, mem.SHAREDWRAM_SIZE);

	mem.SetIOMaps(static_cast<uint8_t*>(malloc(mem.IO_SIZE)));
	ClearArm9MemReg(mem, mem.IO_ADDR, mem.IO_SIZE);
	mem.SetMainMemoryControl(static_cast<uint8_t*>(malloc(mem.MAINMEMCTRL_SIZE))); // TODO : Get pointer from IO obj?
	ClearArm9MemReg(mem, mem.MAINMEMCTRL_ADDR, mem.MAINMEMCTRL_SIZE);

	mem.SetPalettes(static_cast<uint8_t*>(malloc(mem.PALETTES_SIZE)));
	ClearArm9MemReg(mem, mem.PALETTES_ADDR, mem.PALETTES_SIZE);
	mem.SetVRAM_A_BG(static_cast<uint8_t*>(malloc(mem.VRAMABG_SIZE)));
	ClearArm9MemReg(mem, mem.VRAMABG_ADDR, mem.VRAMABG_SIZE);
	mem.SetVRAM_B_BG(static_cast<uint8_t*>(malloc(mem.VRAMBBG_SIZE)));
	ClearArm9MemReg(mem, mem.VRAMBBG_ADDR, mem.VRAMBBG_SIZE);
	mem.SetVRAM_A_OBJ(static_cast<uint8_t*>(malloc(mem.VRAMAOBJ_SIZE)));
	ClearArm9MemReg(mem, mem.VRAMAOBJ_ADDR, mem.VRAMAOBJ_SIZE);
	mem.SetVRAM_B_OBJ(static_cast<uint8_t*>(malloc(mem.VRAMBOBJ_SIZE)));
	ClearArm9MemReg(mem, mem.VRAMBOBJ_ADDR, mem.VRAMBOBJ_SIZE);
	mem.SetVRAM_LCDC(static_cast<uint8_t*>(malloc(mem.VRAMLCDC_SIZE)));
	ClearArm9MemReg(mem, mem.VRAMLCDC_ADDR, mem.VRAMLCDC_SIZE);
	mem.SetOAM_A(static_cast<uint8_t*>(malloc(mem.OAM_SIZE)));
	ClearArm9MemReg(mem, mem.OAMA_ADDR, mem.OAM_SIZE);
	mem.SetOAM_B(static_cast<uint8_t*>(malloc(mem.OAM_SIZE)));
	ClearArm9MemReg(mem, mem.OAMB_ADDR, mem.OAM_SIZE);

	mem.SetBios(static_cast<uint8_t*>(malloc(mem.BIOS_SIZE)));
	ClearArm9MemReg(mem, mem.BIOS_ADDR, mem.BIOS_SIZE);
	mem.SetGBAROM(static_cast<uint8_t*>(malloc(mem.GBAROM_SIZE)));
	ClearArm9MemReg(mem, mem.GBAROM_ADDR, mem.GBAROM_SIZE);
	mem.SetGBARAM(static_cast<uint8_t*>(malloc(mem.GBARAM_SIZE)));
	ClearArm9MemReg(mem, mem.GBARAM_ADDR, mem.GBARAM_SIZE);
}

static void ClearArm9MemReg(ARM9_mem mem, uint32_t startAddr, size_t size, int value) {
	uint8_t* ptr = mem.GetPointerFromAddr(startAddr);
	memset(ptr, value, size);
}
