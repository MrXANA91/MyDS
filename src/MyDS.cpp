// MyDS.cpp : définit le point d'entrée de l'application.
//

#include "MyDS.h"

using namespace std;

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

static void LoadTinyNDSProg(ARM9_mem& mem) {
	uint8_t* ptr;

	ptr = mem.GetPointerFromAddr(mem.BIOS_ADDR);
	ARM_mem::SetWordAtPointer(ptr, 0xE3A00301);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0xE3A01003);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0xE3A02802);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0x00000000);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0x000001A0);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0xE5801304);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0xE5802000);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0xE5803240);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0xE3A0051A);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0xE3A0101F);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0xE3A02903);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0xE0C010B2);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0xE2522001);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0x1AFFFFFC);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0x53534150);
	ptr += 4;
	ARM_mem::SetWordAtPointer(ptr, 0xEAFFFFFE);
	ptr += 4;
}

int main()
{	
	Cpu arm9;
	ARM9_mem mem;

	(void)arm9.SetBootAddr(mem.BIOS_ADDR);
	arm9.SetMMU(&mem);

	InitArm9Memory(mem);

	LoadTinyNDSProg(mem);

	arm9.PrintDebug();
	for (int i = 0; i < 20; i++) {
		arm9.DebugStep();
		arm9.PrintDebug();
	}

	return 0;
}

static void InitArm9Memory(ARM9_mem &mem) {
	mem.SetTCM((uint8_t*)malloc(mem.ITCM_SIZE), (uint8_t*)malloc(mem.DTCM_SIZE));
	ClearArm9MemReg(mem, mem.ITCM_ADDR, mem.ITCM_SIZE);
	ClearArm9MemReg(mem, mem.DTCM_ADDR, mem.DTCM_SIZE);
	mem.SetMainMemory((uint8_t*)malloc(mem.MAINMEMORY_SIZE));
	ClearArm9MemReg(mem, mem.MAINMEMORY_ADDR, mem.MAINMEMORY_SIZE);
	mem.SetSharedWRAM((uint8_t*)malloc(mem.SHAREDWRAM_SIZE));
	ClearArm9MemReg(mem, mem.SHAREDWRAM_ADDR, mem.SHAREDWRAM_SIZE);

	mem.SetIOMaps((uint8_t*)malloc(mem.IO_SIZE));
	ClearArm9MemReg(mem, mem.IO_ADDR, mem.IO_SIZE);
	mem.SetMainMemoryControl((uint8_t*)malloc(mem.MAINMEMCTRL_SIZE)); // TODO : Get pointer from IO obj?
	ClearArm9MemReg(mem, mem.MAINMEMCTRL_ADDR, mem.MAINMEMCTRL_SIZE);

	mem.SetPalettes((uint8_t*)malloc(mem.PALETTES_SIZE));
	ClearArm9MemReg(mem, mem.PALETTES_ADDR, mem.PALETTES_SIZE);
	mem.SetVRAM_A_BG((uint8_t*)malloc(mem.VRAMABG_SIZE));
	ClearArm9MemReg(mem, mem.VRAMABG_ADDR, mem.VRAMABG_SIZE);
	mem.SetVRAM_B_BG((uint8_t*)malloc(mem.VRAMBBG_SIZE));
	ClearArm9MemReg(mem, mem.VRAMBBG_ADDR, mem.VRAMBBG_SIZE);
	mem.SetVRAM_A_OBJ((uint8_t*)malloc(mem.VRAMAOBJ_SIZE));
	ClearArm9MemReg(mem, mem.VRAMAOBJ_ADDR, mem.VRAMAOBJ_SIZE);
	mem.SetVRAM_B_OBJ((uint8_t*)malloc(mem.VRAMBOBJ_SIZE));
	ClearArm9MemReg(mem, mem.VRAMBOBJ_ADDR, mem.VRAMBOBJ_SIZE);
	mem.SetVRAM_LCDC((uint8_t*)malloc(mem.VRAMLCDC_SIZE));
	ClearArm9MemReg(mem, mem.VRAMLCDC_ADDR, mem.VRAMLCDC_SIZE);
	mem.SetOAM_A((uint8_t*)malloc(mem.OAM_SIZE));
	ClearArm9MemReg(mem, mem.OAMA_ADDR, mem.OAM_SIZE);
	mem.SetOAM_B((uint8_t*)malloc(mem.OAM_SIZE));
	ClearArm9MemReg(mem, mem.OAMB_ADDR, mem.OAM_SIZE);

	mem.SetBios((uint8_t*)malloc(mem.BIOS_SIZE));
	ClearArm9MemReg(mem, mem.BIOS_ADDR, mem.BIOS_SIZE);
	mem.SetGBAROM((uint8_t*)malloc(mem.GBAROM_SIZE));
	ClearArm9MemReg(mem, mem.GBAROM_ADDR, mem.GBAROM_SIZE);
	mem.SetGBARAM((uint8_t*)malloc(mem.GBARAM_SIZE));
	ClearArm9MemReg(mem, mem.GBARAM_ADDR, mem.GBARAM_SIZE);
}

static void ClearArm9MemReg(ARM9_mem mem, uint32_t startAddr, size_t size, int value) {
	uint8_t* ptr = mem.GetPointerFromAddr(startAddr);
	memset(ptr, value, size);
}
