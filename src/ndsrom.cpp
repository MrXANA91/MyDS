#include "ndsrom.h"

NDSRom::NDSRom(string filePath) {
	file = OpenFile(filePath, &header);
}

NDSRom::~NDSRom() {
	file.close();
}

ifstream NDSRom::OpenFile(string filepath, NDSHeader* header) {
	streampos size;
	char* memblock = nullptr;

	ifstream file(filepath, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[size];

		file.seekg(0, ios::beg);
		file.read(memblock, size);
	}

	if (memblock != nullptr) {
		if (size > sizeof(NDSHeader)) size = sizeof(NDSHeader);
		memcpy(header, (const char*)memblock, size);
	}

	return file;
}

bool NDSRom::IsOpened() { return file.is_open(); }

void NDSRom::SetARM9ProgramMemory(ARM_mem& mem) {
	uint8_t* ptr = mem.GetPointerFromAddr(header.ARM9_EntryAddress);

	file.seekg(header.ARM9_ROMOffset, ios::beg);
	file.read((char*)ptr, header.ARM9_Size);
}

uint32_t NDSRom::GetARM9StartAddress() {
	return header.ARM9_EntryAddress;
}

void NDSRom::SetARM7ProgramMemory(ARM_mem& mem) {
	uint8_t* ptr = mem.GetPointerFromAddr(header.ARM7_EntryAddress);

	file.seekg(header.ARM7_ROMOffset, ios::beg);
	file.read((char*)ptr, header.ARM7_Size);
}

uint32_t NDSRom::GetARM7StartAddress() {
	return header.ARM7_EntryAddress;
}
