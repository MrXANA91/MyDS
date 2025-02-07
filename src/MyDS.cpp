﻿// MyDS.cpp : définit le point d'entrée de l'application.
//

#include "MyDS.h"

static void ClearMemReg(ARM_mem &mem, uint32_t startAddr, size_t size, int value = 0);

static void InitArm9Memory(ARM9_mem &mem);
static void InitArm7Memory(ARM7_mem& mem, ARM9_mem &mem9);

static bool LoadBios(ARM_mem& mem, std::string biospath, uint32_t biosAddr, uint32_t biosSize);

static Cpu* arm9 = new Cpu(ARMv5_ARM9);
static Cpu* arm7 = new Cpu(ARMv4_ARM7);
static ARM9_mem mem9;
static ARM7_mem mem7;

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

	arm9->SetMMU(&mem9);
	InitArm9Memory(mem9);
	arm7->SetMMU(&mem7);
	InitArm7Memory(mem7, mem9);

	NDSRom nds("..\\NDS-Files\\TinyFB.nds");
	if (nds.IsOpened()) {
		std::cout << "TinyFB.nds successfully opened :\n";
		nds.WriteProgramToARM9Memory(mem9);
		std::cout << "\t- ARM9 Start address : 0x" << std::hex << nds.GetARM9StartAddress() << std::dec << "\n";
		nds.WriteProgramToARM7Memory(mem7);
		std::cout << "\t- ARM7 Start address : 0x" << std::hex << nds.GetARM7StartAddress() << std::dec << "\n";
	}
	else {
		std::cout << "Could not load TinyFB.nds file\n";
	}

	if (LoadBios(mem9, "..\\NDS-Files\\Bios\\biosnds9.rom", mem9.BIOS_ADDR, mem9.BIOS_SIZE)) {
		std::cout << "ARM9 bios successfully loaded\n";
	}
	else {
		std::cout << "Could not load ARM9 bios file\n";
	}

	if (LoadBios(mem7, "..\\NDS-Files\\Bios\\biosnds7.rom", mem7.BIOS_ADDR, mem7.BIOS_SIZE)) {
		std::cout << "ARM7 bios successfully loaded\n";
	}
	else {
		std::cout << "Could not load ARM7 bios file\n";
	}

	arm9->SetBootAddr(mem9.BIOS_ADDR);
	std::cout << "ARM9 boot address set to : 0x" << std::hex << mem9.BIOS_ADDR << std::dec << "\n";
	arm7->SetBootAddr(mem7.BIOS_ADDR);
	std::cout << "ARM7 boot address set to : 0x" << std::hex << mem7.BIOS_ADDR << std::dec << "\n";

	std::cout << "Selected CPU : ARM9\n";
	Cpu* selectedCpu = arm9;

	std::cout << "Emulator setup finished.\n> ";

	char command[50];

	uint8_t* ptr{ 0 };
	uint32_t addr{ 0 };
	int bpIndex{ 0 };
	pc = selectedCpu->GetReg(REG_PC);


	bool programRunning = true;
	while (programRunning) {
		std::cin >> command;

		switch (command[0]) {
		case 's':
			selectedCpu->DebugStep();
			break;
		case 'm':
			std::cout << "Enter mem address as hex address to read : 0x";
			std::cin >> std::hex >> addr >> std::dec;
			if (selectedCpu == arm9) {
				ptr = mem9.GetPointerFromAddr(addr);
			}
			else {
				ptr = mem7.GetPointerFromAddr(addr);
			}
			if (ptr == nullptr) {
				std::cout << "Unknown address '0x" << std::hex << addr << std::dec << "'\n";
			}
			else {
				std::cout << "Word = 0x" << std::hex << ARM_mem::GetWordAtPointer(ptr) << std::dec << "\n";
			}
			break;
		case 'd':
			selectedCpu->DisplayRegisters();
			break;
		case 'b':
			if (command[1] == '\n' || command[1] == '\0' || command[1] == 'd') {
				selectedCpu->DisplayBreakpoints();
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
				if (selectedCpu->SetBreakpoint(bpAddr)) {
					std::cout << "Program will stop at address 0x" << std::hex << bpAddr << std::dec << "\n";
				}
				else {
					std::cout << "Failed to setup breakpoint\n";
				}
				break;
			case 't':
				std::cout << "Enter breakpoint id to toggle : ";
				std::cin >> bpIndex;
				if (selectedCpu->ToggleBreakpoint(bpIndex)) {
					std::cout << "Successfully toggled breakpoint " << bpIndex << "\n";
				}
				else {
					std::cout << "Failed to toggle breakpoint\n";
				}
				break;
			case 'r':
				std::cout << "Enter breakpoint id to remove : ";
				std::cin >> bpIndex;
				if (selectedCpu->RemoveBreakpoint(bpIndex)) {
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
			if (selectedCpu->IsRunning()) {
				selectedCpu->Stop();
				std::cout << "CPUs stopped.\n";
			}
			else {
				selectedCpu->Run();
				std::cout << "CPUs running...\n";
			}
			break;
		case 'r':
			if (command[1] != 'a') {
				selectedCpu->Reset();
				pc = selectedCpu->GetReg(REG_PC);
				std::cout << "PC : 0x" << std::hex << pc << std::dec << "\n";
				std::cout << "CPU has been reset.\n";
			}
			else {
				std::cout << "Enter new boot address as hex address : 0x";
				std::cin >> std::hex >> addr >> std::dec;
				if (selectedCpu->SetBootAddr(addr)) {
					std::cout << "New boot address set" << "\n";
				}
				else {
					std::cout << "Could not set boot address" << "\n";
				}
			}
			break;
		case 'c':
			if (selectedCpu == arm9) {
				selectedCpu = arm7;
				std::cout << "Selected CPU : ARM7\n";
			}
			else {
				selectedCpu = arm9;
				std::cout << "Selected CPU : ARM9\n";
			}
			break;
		case 'p':
			selectedCpu->Debug = !selectedCpu->Debug;
			if (selectedCpu->Debug) {
				std::cout << "Print debug is now enabled\n";
			}
			else {
				std::cout << "Print debug is disabled\n";
			}
			break;
		case 'h':
			std::cout << "c: switch current selected CPU\n";
			std::cout << "r: reset CPU (ra: change boot address) / s: single step\n";
			std::cout << "e: continuous execution (until breakpoint) / b: breakpoint sub console (bd to only display breakpoints) / p: toggle print debug\n";
			std::cout << "m: print a memory address / d: display registers\n";
			std::cout << "q: exit program\n";
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
	ClearMemReg(mem, mem.ITCM_ADDR, mem.ITCM_SIZE);
	ClearMemReg(mem, mem.DTCM_ADDR, mem.DTCM_SIZE);
	mem.SetMainMemory(static_cast<uint8_t*>(malloc(mem.MAINMEMORY_SIZE)));
	ClearMemReg(mem, mem.MAINMEMORY_ADDR, mem.MAINMEMORY_SIZE);
	mem.SetSharedWRAM(static_cast<uint8_t*>(malloc(mem.SHAREDWRAM_SIZE)));
	ClearMemReg(mem, mem.SHAREDWRAM_ADDR, mem.SHAREDWRAM_SIZE);

	mem.SetIOMaps(static_cast<uint8_t*>(malloc(mem.IO_SIZE)));
	ClearMemReg(mem, mem.IO_ADDR, mem.IO_SIZE);
	mem.SetMainMemoryControl(static_cast<uint8_t*>(malloc(mem.MAINMEMCTRL_SIZE))); // TODO : Get pointer from IO obj?
	ClearMemReg(mem, mem.MAINMEMCTRL_ADDR, mem.MAINMEMCTRL_SIZE);

	mem.SetPalettes(static_cast<uint8_t*>(malloc(mem.PALETTES_SIZE)));
	ClearMemReg(mem, mem.PALETTES_ADDR, mem.PALETTES_SIZE);
	mem.SetVRAM_A_BG(static_cast<uint8_t*>(malloc(mem.VRAMABG_SIZE)));
	ClearMemReg(mem, mem.VRAMABG_ADDR, mem.VRAMABG_SIZE);
	mem.SetVRAM_B_BG(static_cast<uint8_t*>(malloc(mem.VRAMBBG_SIZE)));
	ClearMemReg(mem, mem.VRAMBBG_ADDR, mem.VRAMBBG_SIZE);
	mem.SetVRAM_A_OBJ(static_cast<uint8_t*>(malloc(mem.VRAMAOBJ_SIZE)));
	ClearMemReg(mem, mem.VRAMAOBJ_ADDR, mem.VRAMAOBJ_SIZE);
	mem.SetVRAM_B_OBJ(static_cast<uint8_t*>(malloc(mem.VRAMBOBJ_SIZE)));
	ClearMemReg(mem, mem.VRAMBOBJ_ADDR, mem.VRAMBOBJ_SIZE);
	mem.SetVRAM_LCDC(static_cast<uint8_t*>(malloc(mem.VRAMLCDC_SIZE)));
	ClearMemReg(mem, mem.VRAMLCDC_ADDR, mem.VRAMLCDC_SIZE);
	mem.SetOAM_A(static_cast<uint8_t*>(malloc(mem.OAM_SIZE)));
	ClearMemReg(mem, mem.OAMA_ADDR, mem.OAM_SIZE);
	mem.SetOAM_B(static_cast<uint8_t*>(malloc(mem.OAM_SIZE)));
	ClearMemReg(mem, mem.OAMB_ADDR, mem.OAM_SIZE);

	mem.SetBios(static_cast<uint8_t*>(malloc(mem.BIOS_SIZE)));
	ClearMemReg(mem, mem.BIOS_ADDR, mem.BIOS_SIZE);
	mem.SetGBAROM(static_cast<uint8_t*>(malloc(mem.GBAROM_SIZE)));
	ClearMemReg(mem, mem.GBAROM_ADDR, mem.GBAROM_SIZE);
	mem.SetGBARAM(static_cast<uint8_t*>(malloc(mem.GBARAM_SIZE)));
	ClearMemReg(mem, mem.GBARAM_ADDR, mem.GBARAM_SIZE);
}

static void InitArm7Memory(ARM7_mem &mem, ARM9_mem &mem9) {
	mem.SetBios(static_cast<uint8_t*>(malloc(mem.BIOS_SIZE)));
	ClearMemReg(mem, mem.BIOS_ADDR, mem.BIOS_SIZE);

	mem.SetMainMemory(mem9.GetPointerFromAddr(mem9.MAINMEMORY_ADDR));

	mem.SetSharedWRAM(mem9.GetPointerFromAddr(mem9.SHAREDWRAM_ADDR));

	mem.SetWRAM(static_cast<uint8_t*>(malloc(mem.WRAM_SIZE)));
	ClearMemReg(mem, mem.WRAM_ADDR, mem.WRAM_SIZE);

	mem.SetIOMaps(static_cast<uint8_t*>(malloc(mem.IO_SIZE)));
	ClearMemReg(mem, mem.IO_ADDR, mem.IO_SIZE);
	mem.SetWiFiIOMaps(static_cast<uint8_t*>(malloc(mem.IOWIFI_SIZE)));
	ClearMemReg(mem, mem.IOWIFI_ADDR, mem.IOWIFI_SIZE);

	mem.SetVRAMasWRAM(static_cast<uint8_t*>(malloc(mem.VRAM_AS_WRAM_SIZE)));
	ClearMemReg(mem, mem.VRAM_AS_WRAM_ADDR, mem.VRAM_AS_WRAM_SIZE);

	mem.SetGBARAM(mem9.GetPointerFromAddr(mem9.GBARAM_ADDR));
	mem.SetGBAROM(mem9.GetPointerFromAddr(mem9.GBAROM_ADDR));
}

static void ClearMemReg(ARM_mem &mem, uint32_t startAddr, size_t size, int value) {
	uint8_t* ptr = mem.GetPointerFromAddr(startAddr);
	memset(ptr, value, size);
}

static bool LoadBios(ARM_mem &mem, std::string biospath, uint32_t biosAddr, uint32_t biosSize) {
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
		if (static_cast<size_t>(size) > biosSize) size = biosSize;
		uint8_t* ptr = mem.GetPointerFromAddr(biosAddr);
		memcpy(ptr, const_cast<const char*>(memblock), size);
	}
	else {
		return false;
	}

	return true;
}
