// MyDS.cpp : définit le point d'entrée de l'application.
//

#include "MyDS.h"

static void InitArm9Memory(ARM9_mem &mem);
static void ClearArm9MemReg(ARM9_mem mem, uint32_t startAddr, size_t size, int value = 0);
static void LoadARM9BIOS(ARM9_mem& mem, std::string biospath);

static Cpu arm9;
static ARM9_mem mem;
static uint64_t execInstr{ 0 };
static std::chrono::steady_clock::time_point start;
static std::chrono::steady_clock::time_point end;
static std::chrono::milliseconds waitTime{ 0 };
uint32_t pc{ 0 };
uint32_t bpAddr{ 0xFFFFFFFF };

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

	NDSRom nds("..\\NDS-Files\\TinyFB.nds");

	std::cout << "CPU : arm9\n";
	if (nds.IsOpened()) {
		std::cout << "TinyFB.nds successfully opened\n";
	}
	else {
		std::cout << "Could not load TinyFB.nds file, exiting.\n";
		return -1;
	}

	arm9.SetMMU(&mem);
	InitArm9Memory(mem);
	nds.SetARM9ProgramMemory(mem);
	//LoadARM9BIOS(mem, "..\\NDS-Files\\Bios\\biosnds9.rom");
	//arm9.SetBootAddr(mem.BIOS_ADDR);
	arm9.SetBootAddr(nds.GetARM9StartAddress());

	std::cout << "Emulator setup finished.\n> ";

	char command[50];

	uint32_t addr{ 0 };
	int bpIndex{ 0 };
	pc = arm9.GetReg(REG_PC);

	while (true) {
		std::cin >> command;

		switch (command[0]) {
		case 's':
			arm9.DebugStep();
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
			if (command[1] == '\n' || command[1] == '\0' || command[1] == 'd') {
				arm9.DisplayBreakpoints();
				if (command[1] == 'd') break;
				std::cout << "n: new breakpoint / t: toggle breakpoint / r: remove breakpoint\nBREAKPOINT > ";
				std::cin >> command;
			}
			else {
				command[0] = command[1];
			}
			switch (command[0]) {
			case 'n':
				std::cout << "Enter program address to stop execution : 0x";
				std::cin >> std::hex >> bpAddr >> std::dec;
				if (arm9.SetBreakpoint(bpAddr)) {
					std::cout << "Program will stop at address 0x" << std::hex << bpAddr << std::dec << "\n";
				}
				else {
					std::cout << "Failed to setup breakpoint\n";
				}
				break;
			case 't':
				std::cout << "Enter breakpoint id to toggle : ";
				std::cin >> bpIndex;
				if (arm9.ToggleBreakpoint(bpIndex)) {
					std::cout << "Successfully toggled breakpoint " << bpIndex << "\n";
				}
				else {
					std::cout << "Failed to toggle breakpoint\n";
				}
				break;
			case 'r':
				std::cout << "Enter breakpoint id to remove : ";
				std::cin >> bpIndex;
				if (arm9.RemoveBreakpoint(bpIndex)) {
					std::cout << "Successfully removed breakpoint " << bpIndex << "\n";
				}
				else {
					std::cout << "Failed to remove breakpoint\n";
				}
				break;
			default:
				break;
			}
			break;
		case 'e':
			if (arm9.IsRunning()) {
				arm9.Stop();
				std::cout << "CPU stopped.\n";
			}
			else {
				arm9.Run();
				std::cout << "CPU running...\n";
			}
			break;
		case 'h':
			std::cout << "r: reset CPU / s: single step\n";
			std::cout << "e: continuous execution (until breakpoint) / b: breakpoint sub console (bd to only display breakpoints)\n";
			std::cout << "m: print a memory address / d: display registers\n";
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

static void LoadARM9BIOS(ARM9_mem& mem, std::string biospath) {
	std::streampos size;
	char* memblock = nullptr;

	std::ifstream file(biospath, std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[size];

		file.seekg(0, std::ios::beg);
		file.read(memblock, size);
	}

	if (memblock != nullptr) {
		if (static_cast<size_t>(size) > mem.BIOS_SIZE) size = mem.BIOS_SIZE;
		uint8_t* ptr = mem.GetPointerFromAddr(mem.BIOS_ADDR);
		memcpy(ptr, const_cast<const char*>(memblock), size);
	}
}
