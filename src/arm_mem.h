#pragma once

#include <cstdint>

class ARM_mem {
public:
	/// <summary>
	/// Get memory pointer from virtual memory address
	/// </summary>
	/// <param name="address">Virtual ARM memory address</param>
	/// <returns>Pointer to the first byte of the provided address</returns>
	virtual uint8_t* GetPointerFromAddr(uint32_t address) = 0;

	/// <summary>
	/// Get bytes as long word at pointer (little endian). Argument pointer will not be changed during execution.
	/// </summary>
	/// <param name="startPtr">Pointer to first byte</param>
	/// <param name="size">Number of bytes (max 8)</param>
	/// <returns>64bit value</returns>
	static uint64_t GetBytesAtPointer(uint8_t* startPtr, int size);

	/// <summary>
	/// Get 32bit Word at pointer (little endian). Argument pointer will not be changed during execution.
	/// </summary>
	/// <param name="startPtr">Pointer to first byte</param>
	/// <returns>32bit Word</returns>
	static uint32_t GetWordAtPointer(uint8_t* startPtr) {
		return GetBytesAtPointer(startPtr, 4);
	}

	/// <summary>
	/// Get 16bit Half-Word at pointer (little endian). Argument pointer will not be changed during execution.
	/// </summary>
	/// <param name="startPtr">Pointer to first byte</param>
	/// <returns>16bit Half-Word</returns>
	static uint16_t GetHalfWordAtPointer(uint8_t* startPtr) {
		return GetBytesAtPointer(startPtr, 2);
	}

	/// <summary>
	/// Set a 32bit word at pointer as little endian. Argument pointer will not be changed during execution.
	/// </summary>
	/// <param name="startPtr">Pointer to first byte</param>
	/// <param name="word">32bit word to write</param>
	static void SetWordAtPointer(uint8_t* startPtr, uint32_t word) {
		*startPtr = word & 0xFF;
		*(startPtr + 1) = (word & 0xFF00) >> 8;
		*(startPtr + 2) = (word & 0xFF0000) >> 16;
		*(startPtr + 3) = (word & 0xFF000000) >> 24;
	}

	/// <summary>
	/// Set a 16bit half-word at pointer as little endian. Argument pointer will not be changed during execution.
	/// </summary>
	/// <param name="startPtr">Pointer to first byte</param>
	/// <param name="halfWord">16bit word to write</param>
	static void SetHalfWordAtPointer(uint8_t* startPtr, uint16_t halfWord) {
		*startPtr = halfWord & 0xFF;
		*(startPtr + 1) = (halfWord & 0xFF00) >> 8;
	}
};
