#include "ndsrom.h"

NDSRom::NDSRom(std::string filePath) {
	file = OpenFile(filePath, &header);
}

NDSRom::~NDSRom() {
	file.close();
}

std::ifstream NDSRom::OpenFile(std::string filepath, NDSHeader* header) {
	std::streampos size;
	char* memblock = nullptr;

	std::ifstream file(filepath, std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[size];

		file.seekg(0, std::ios::beg);
		file.read(memblock, size);
	}

	if (memblock != nullptr) {
		if (size > sizeof(NDSHeader)) size = sizeof(NDSHeader);
		memcpy(header, const_cast<const char *>(memblock), size);
	}

	return file;
}

bool NDSRom::IsOpened() { return file.is_open(); }

void NDSRom::WriteProgramToARM9Memory(ARM_mem& mem) {
	uint8_t* ptr = mem.GetPointerFromAddr(header.ARM9_EntryAddress);

	file.seekg(header.ARM9_ROMOffset, std::ios::beg);
	file.read(reinterpret_cast<char*>(ptr), header.ARM9_Size);
}

uint32_t NDSRom::GetARM9StartAddress() {
	return header.ARM9_EntryAddress;
}

void NDSRom::SetARM7ProgramMemory(ARM_mem& mem) {
	uint8_t* ptr = mem.GetPointerFromAddr(header.ARM7_EntryAddress);

	file.seekg(header.ARM7_ROMOffset, std::ios::beg);
	file.read(reinterpret_cast<char*>(ptr), header.ARM7_Size);
}

uint32_t NDSRom::GetARM7StartAddress() {
	return header.ARM7_EntryAddress;
}
