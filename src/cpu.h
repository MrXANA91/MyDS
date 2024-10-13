#pragma once

#include <cstdint>
#include "arm_mem.h"

constexpr auto REG_SP = 13;
constexpr auto REG_LR = 14;
constexpr auto REG_PC = 15;

constexpr auto EXCEPTION_REG_ACCESS_IN_UNKNWOWN_MODE = 1;
constexpr auto EXCEPTION_REG_ACCESS_OUT_OF_RANGE = 2;
constexpr auto EXCEPTION_SPSR_UNKNOWN_MODE = 3;
constexpr auto EXCEPTION_SPSR_MODE_IS_USER_OR_SYSTEM = 4;

// https://problemkaputt.de/gbatek-arm-cpu-flags-condition-field-cond.htm
union CPSR {
	struct {
		uint32_t Mode : 5;	// Current operating mode
		uint32_t T : 1;		// State bit (0: ARM ; 1: THUMB)
		uint32_t F : 1;		// FIQ disable
		uint32_t I : 1;		// IRQ disable

		uint32_t : 1;		// Abort disable (ARM11 only)
		uint32_t : 1;		// Endian (ARM11?)
		uint32_t : 6;		// Reserved

		uint32_t : 8;		// Reserved

		uint32_t J : 1;		// Jazelle Mode
		uint32_t : 2;		// Reserved
		uint32_t Q : 1;		// Sticky Overflow
		uint32_t V : 1;		// Overflow flag
		uint32_t Z : 1;		// Zero flag
		uint32_t C : 1;		// Carry flag
		uint32_t N : 1;		// Sign flag
	} bits;
	uint32_t value;
};

enum CpuMode {
	User = 16,				// (non privileged)
	FIQ = 17,				// Fast Interrupt
	IRQ = 18,				// Normal Interrupt
	Supervisor = 19,		// (SWI)
	Abort = 23,				// Abort
	Undefined = 27,			// Undefined
	System = 31,			// (privileged 'User' mode)
};

class Cpu {
private:
	ARM_mem* memory;

	bool started{ false };

	uint32_t bootAddress{ 0 };

	// Registers
	uint32_t reg[16]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint32_t reg_fiq[7]{ 0, 0, 0, 0, 0, 0, 0 };
	uint32_t reg_svc[2]{ 0, 0 };
	uint32_t reg_abt[2]{ 0, 0 };
	uint32_t reg_irq[2]{ 0, 0 };
	uint32_t reg_und[2]{ 0, 0 };

	CPSR cpsr{ 0 };
	CPSR spsr_fiq{ 0 };
	CPSR spsr_svc{ 0 };
	CPSR spsr_abt{ 0 };
	CPSR spsr_irq{ 0 };
	CPSR spsr_und{ 0 };

	uint32_t GetReg(int regID);
	void SetReg(int regID, uint32_t value);

	void SaveCPSR();

	void Fetch();
	void Decode();
	void Execute();

public:
	Cpu();
	bool SetBootAddr(uint32_t bootAddr);
	void SetMMU(ARM_mem* ptr);

	CpuMode GetCurrentCpuMode() const;

	void DebugStep();
	void Reset();
};