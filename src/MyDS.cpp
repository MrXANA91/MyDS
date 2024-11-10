// MyDS.cpp : définit le point d'entrée de l'application.
//

#include "MyDS.h"

static void InitArm9Memory(ARM9_mem &mem);
static void ClearArm9MemReg(ARM9_mem mem, uint32_t startAddr, size_t size, int value = 0);
static bool LoadARM9BIOS(ARM9_mem& mem, std::string biospath);

static Cpu* arm9 = new Cpu(ARMv5_ARM9);
static Cpu* arm7 = new Cpu(ARMv4_ARM7);
static ARM9_mem mem9;
static ARM9_mem mem7;

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

	std::cout << "CPU : arm9\n";
	arm9->SetMMU(&mem9);
	InitArm9Memory(mem9);

	NDSRom nds("..\\NDS-Files\\TinyFB.nds");
	if (nds.IsOpened()) {
		std::cout << "TinyFB.nds successfully opened :\n";
		nds.WriteProgramToARM9Memory(mem9);
		std::cout << "\t- ARM9 Start address : 0x" << std::hex << nds.GetARM9StartAddress() << std::dec << "\n";
		std::cout << "\t- ARM7 Start address : 0x" << std::hex << nds.GetARM7StartAddress() << std::dec << "\n";
	}
	else {
		std::cout << "Could not load TinyFB.nds file\n";
	}

	if (LoadARM9BIOS(mem9, "..\\NDS-Files\\Bios\\biosnds9.rom")) {
		std::cout << "ARM9 bios successfully loaded\n";
	}
	else {
		std::cout << "Could not load ARM9 bios file\n";
	}

	arm9->SetBootAddr(mem9.BIOS_ADDR);
	std::cout << "Boot address set to : 0x" << std::hex << mem9.BIOS_ADDR << std::dec << "\n";
	std::cout << "Emulator setup finished.\n> ";

	Cpu* currentCpu = arm9;

	char command[50];

	uint32_t addr{ 0 };
	int bpIndex{ 0 };
	pc = currentCpu->GetReg(REG_PC);


	bool programRunning = true;
	while (programRunning) {
		std::cin >> command;

		switch (command[0]) {
		case 's':
			currentCpu->DebugStep();
			break;
		case 'm':
			std::cout << "Enter mem address as hex address to read : 0x";
			std::cin >> std::hex >> addr >> std::dec;
			std::cout << "Word = 0x" << std::hex << mem9.GetWordAtPointer(mem9.GetPointerFromAddr(addr)) << "\n";
			break;
		case 'd':
			currentCpu->DisplayRegisters();
			break;
		case 'b':
			if (command[1] == '\n' || command[1] == '\0' || command[1] == 'd') {
				currentCpu->DisplayBreakpoints();
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
				if (currentCpu->SetBreakpoint(bpAddr)) {
					std::cout << "Program will stop at address 0x" << std::hex << bpAddr << std::dec << "\n";
				}
				else {
					std::cout << "Failed to setup breakpoint\n";
				}
				break;
			case 't':
				std::cout << "Enter breakpoint id to toggle : ";
				std::cin >> bpIndex;
				if (currentCpu->ToggleBreakpoint(bpIndex)) {
					std::cout << "Successfully toggled breakpoint " << bpIndex << "\n";
				}
				else {
					std::cout << "Failed to toggle breakpoint\n";
				}
				break;
			case 'r':
				std::cout << "Enter breakpoint id to remove : ";
				std::cin >> bpIndex;
				if (currentCpu->RemoveBreakpoint(bpIndex)) {
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
			if (currentCpu->IsRunning()) {
				currentCpu->Stop();
				std::cout << "CPU stopped.\n";
			}
			else {
				currentCpu->Run();
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
			if (command[1] != 'a') {
				currentCpu->Reset();
				pc = currentCpu->GetReg(REG_PC);
				std::cout << "PC : 0x" << std::hex << pc << std::dec << "\n";
				std::cout << "CPU has been reset.\n";
			}
			else {
				std::cout << "Enter new boot address as hex address : 0x";
				std::cin >> std::hex >> addr >> std::dec;
				if (currentCpu->SetBootAddr(addr)) {
					std::cout << "New boot address set" << "\n";
				}
				else {
					std::cout << "Could not set boot address" << "\n";
				}
			}
			break;
		case 'q':
			std::cout << "Exiting program.\n";
			programRunning = false;
		}
		
		std::cout << "> ";
	}

	delete arm9;
	delete arm7;
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

static bool LoadARM9BIOS(ARM9_mem& mem, std::string biospath) {
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
	else {
		return false;
	}

	if (memblock != nullptr) {
		if (static_cast<size_t>(size) > mem.BIOS_SIZE) size = mem.BIOS_SIZE;
		uint8_t* ptr = mem.GetPointerFromAddr(mem.BIOS_ADDR);
		memcpy(ptr, const_cast<const char*>(memblock), size);
	}
	else {
		return false;
	}

	return true;
}
