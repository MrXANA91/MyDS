#pragma once

#include <cstdint>
#include <iostream>
#include <fstream>
#include "arm_mem.h"

#pragma pack(push)
#pragma pack(1)	// https://github.com/devkitPro/ndstool/blob/master/source/header.h
struct NDSHeader {
	char GameTitle[12];
	char GameCode[4];							// 0 : Homebrew
	char Makercode[2];							// "01" : Nintendo / 0 : Homebrew
	uint8_t Unitcode;							// 0x00 : NDS / 0x02 : NDS+DSi / 0x03 : DSi
	uint8_t EncryptionSeedSelect;				// usually 0x00
	uint8_t DeviceCapacity;						// Chipsize = 128KB SHL {this} (7 = 16MB)
	uint8_t Reserved_1[7];
	uint8_t ReservedDSI_1;
	uint8_t NDSRegion;
	uint8_t ROMVersion;
	uint8_t Autostart;
	// Program offsets
	uint32_t ARM9_ROMOffset;
	uint32_t ARM9_EntryAddress;
	uint32_t ARM9_RamAddress;
	uint32_t ARM9_Size;
	uint32_t ARM7_ROMOffset;
	uint32_t ARM7_EntryAddress;
	uint32_t ARM7_RamAddress;
	uint32_t ARM7_Size;
	// File Name Table
	uint32_t FNT_Offset;
	uint32_t FNT_Size;
	// File Allocation Table
	uint32_t FAT_Offset;
	uint32_t FAT_Size;
	// Overlays
	uint32_t ARM9_OverlayOffset;
	uint32_t ARM9_OverlaySize;
	uint32_t ARM7_OverlayOffset;
	uint32_t ARM7_OverlaySize;
	uint32_t PortSettingNormalCommands;			// Port 0x40001A4 setting for normal commands (usually 0x00586000)
	uint32_t PortSettingKEY1Commands;			// Port 0x40001A4 setting for KEY1 commands (usually 0x001808F8)
	uint32_t IconTitleOffset;					// 0 : None
	uint16_t SecureAreaChecksum;				// CRC-16 of [[020h]..00007FFFh]
	uint16_t SecureAreaDelay;					// in 131kHz units
	uint32_t ARM9_AutoLoadListHookRAMAddress;	// ?
	uint32_t ARM7_AutoLoadListHookRAMAddress;	// ?
	uint64_t SecureAreaDisable;					// usually 0
	uint32_t TotalUsedROMSize;					// Remaning/unused bytes usually 0xFF padded
	uint32_t ROMHeaderSize;						// usually 0x4000
	uint32_t Unknown;
	uint64_t ReversedDSI_2;
	uint16_t NAND_EndOfROMArea;					// in 0x20000-byte units
	uint16_t NAND_StartOfRWArea;				// 0 : None (usually same as previous)
	uint8_t Reserved_2[0x18];
	uint8_t Reserved_3[0x10];
	uint8_t NintendoLogo[0x9C];					// Compressed bitmap, see GBA Headers
	uint16_t NintendoLogoChecksum;				// CRC-16 of NintendoLogo - fixed 0xCF56
	uint16_t HeaderChecksum;					// CRC-16 of Header until this address
	uint32_t Debug_ROMOffset;
	uint32_t Debug_Size;
	uint32_t Debug_RAMAddress;
	uint32_t Reserved_4;
};
#pragma pack(pop)

class NDSRom {
private:
	NDSHeader header;
	std::ifstream file;

	std::ifstream OpenFile(std::string, NDSHeader *);

public:
	/// <summary>
	/// Initialise a NDS ROM from a .NDS file
	/// </summary>
	/// <param name="filepath">Path to .NDS file</param>
	NDSRom (std::string filepath);

	~NDSRom();

	/// <summary>
	/// Returns whether or not ROM file is still opened.
	/// </summary>
	/// <returns></returns>
	bool IsOpened();

	/// <summary>
	/// Write ARM9 program into virtual ARM memory
	/// </summary>
	/// <param name="mem">Reference to ARM9 virtual memory</param>
	void SetARM9ProgramMemory(ARM_mem &mem);

	/// <summary>
	/// Get ARM9 entry address in virtual ARM memory
	/// </summary>
	/// <returns>ARM9 start address</returns>
	uint32_t GetARM9StartAddress();

	/// <summary>
	/// Write ARM7 program into virtual ARM memory
	/// </summary>
	/// <param name="mem">Reference to ARM7 virtual memory</param>
	void SetARM7ProgramMemory(ARM_mem& mem);

	/// <summary>
	/// Get ARM7 entry address in virtual ARM memory
	/// </summary>
	/// <returns>ARM7 start address</returns>
	uint32_t GetARM7StartAddress();
};