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
	using namespace std::chrono;

	std::cout << "=============================" << "\n";
	std::cout << "MyDS Emulator - Debug console" << "\n";
	std::cout << "=============================" << "\n";
	std::cout << "\n";

	Cpu arm9;
	ARM9_mem mem;
	NDSRom nds("NDS-Files\\TinyFB.nds");

	std::cout << "CPU : arm9\n";
	if (nds.IsOpened()) {
		std::cout << "TinyFB.nds successfully opened\n";
	}
	else {
		std::cout << "Could not load TinyFB.nds file, exiting.\n";
		return -1;
	}

	arm9.SetBootAddr(nds.GetARM9StartAddress());
	arm9.SetMMU(&mem);
	InitArm9Memory(mem);
	nds.SetARM9ProgramMemory(mem);

	std::cout << "Emulator setup finished.\n> ";

	char command[50];

	uint32_t addr{ 0 };
	uint32_t bpAddr{ 0xFFFFFFFF };
	uint32_t pc = arm9.GetReg(REG_PC);
	uint64_t execInstr{ 0 };
	uint8_t reg{ 0 };
	uint8_t reg_value{ 0 };
	steady_clock::time_point start;
	steady_clock::time_point end;

	while (true) {
		std::cin >> command;

		switch (command[0]) {
		case 's':
			arm9.Debug = true;
			arm9.DebugStep();
			arm9.Debug = false;
			break;
		case 'm':
			std::cout << "Enter mem address as hex address to read : 0x";
			std::cin >> std::hex >> addr >> std::dec;
			std::cout << "Word = 0x" << std::hex << mem.GetWordAtPointer(mem.GetPointerFromAddr(addr)) << "\n";
			break;
		case 'd':
			arm9.DisplayRegisters();
			break;
		case 'b':
			std::cout << "Enter program address to stop execution : 0x";
			std::cin >> std::hex >> bpAddr >> std::dec;
			std::cout << "Program will stop at address 0x" << std::hex << bpAddr << std::dec << "\n";
			break;
		case 'e':
			pc = arm9.GetReg(REG_PC);
			execInstr = 0;
			start = high_resolution_clock::now();
			while (pc != bpAddr) {
				//std::cout << "PC : 0x" << std::hex << pc << std::dec << "\n";
				arm9.DebugStep();
				execInstr++;
				pc = arm9.GetReg(REG_PC);
				//std::cout << "R2 : 0x" << std::hex << arm9.GetReg(2) << std::dec << "\n";
			}
			end = high_resolution_clock::now();
			std::cout << "Breakpoint at 0x" << std::hex << pc << std::dec << "\n";
			std::cout << "Executed " << execInstr << " instructions in " << duration_cast<microseconds>(end - start).count() << "us \n";
			break;
		case 'h':
			std::cout << "r: reset CPU\n";
			std::cout << "s: single step / e: continuous execution (until breakpoint) / b: setup breakpoint address for continuous execution\n";
			std::cout << "m: display a memory address / d: display registers\n";
			std::cout << "q: exit program\n";
			break;
		case 'r':
			arm9.Reset();
			pc = arm9.GetReg(REG_PC);
			std::cout << "PC : 0x" << std::hex << pc << std::dec << "\n";
			std::cout << "CPU has been reset.\n";
			break;
		case 'q':
			std::cout << "Exiting program.\n";
			return 1;
		}
		
		std::cout << "> ";
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
