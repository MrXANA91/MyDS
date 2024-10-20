#pragma once

#include <cstdint>
#include <iostream>
#include <iomanip>
#include "arm_mem.h"

constexpr auto REG_SP = 13;
constexpr auto REG_LR = 14;
constexpr auto REG_PC = 15;

constexpr auto EXCEPTION_REG_ACCESS_IN_UNKNWOWN_MODE = 1;
constexpr auto EXCEPTION_REG_ACCESS_OUT_OF_RANGE = 2;
constexpr auto EXCEPTION_SPSR_UNKNOWN_MODE = 3;
constexpr auto EXCEPTION_SPSR_MODE_IS_USER_OR_SYSTEM = 4;
constexpr auto EXCEPTION_CONDITION_UNKNOWN = 5;
constexpr auto EXCEPTION_EXEC_BRANCH_DECODE_FAILURE = 6;
constexpr auto EXCEPTION_ALU_BITSHIFT_UNKNOWN_SHIFTTYPE = 7;

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

		uint32_t J : 1;		// Jazelle Mode (not supported here)
		uint32_t : 2;		// Reserved
		uint32_t Q : 1;		// Sticky Overflow
		uint32_t V : 1;		// Overflow flag
		uint32_t Z : 1;		// Zero flag
		uint32_t C : 1;		// Carry flag
		uint32_t N : 1;		// Sign flag
	} bits;
	uint32_t value;
};

enum Condition {
	EQ = 0,	// Equal / zero
	NE,		// Not equal
	CS_HS,	// unsigned higher or same
	CC_LO,	// unsigned lower
	MI,		// signed negative
	PL,		// signed positive or zero
	VS,		// signed overflow
	VC,		// signed no overflow
	HI,		// unsigned higher
	LS,		// unsigned lower or same
	GE,		// signed greater or equal
	LT,		// signed less than
	GT,		// signed greater than
	LE,		// signed less or equal
	AL,		// always
	rsv		// Reserved (previously never)
};

enum ALUOpCode {
	AND = 0,	// Rd = Rn & Op2 (and)
	EOR,		// Rd = Rn ^ Op2 (xor)
	SUB,		// Rd = Rn - Op2 (substract)
	RSB,		// Rd = Op2 - Rn (reversed sub)
	ADD,		// Rd = Rn + Op2 (add)
	ADC,		// Rd = Rn + Op2 + C (add with carry)
	SBC,		// Rd = Rd - Op2 + C - 1 (sub with carry)
	RSC,		// Rd = Op2 - Rn + C - 1 (reversed sub with carry)
	TST,		// Rn & Op2 (test)
	TEQ,		// Rn ^ Op2 (test exclusive)
	CMP,		// Rn - Op2 (compare)
	CMN,		// Rn + Op2 (compare negative)
	ORR,		// Rd = Rn | Op2 (or)
	MOV,		// Rd = Op2
	BIC,		// Rd = Rn & ~Op2 (bit clear, Rn AND NOT Op2)
	MVN			// Rd = ~Op2 (not)
};

enum CpuMode {
	User = 0x10,			// (non privileged)
	FIQ = 0x11,				// Fast Interrupt
	IRQ = 0x12,				// Normal Interrupt
	Supervisor = 0x13,		// (SWI)
	Abort = 0x17,			// Abort
	Undefined = 0x1B,		// Undefined
	System = 0x1F,			// (privileged 'User' mode)
};

enum ShiftType {
	LSL = 0,	// Logical Shift Left
	LSR,		// Logical Shift Right
	ASR,		// Arithmetic Shift Right (sign bit is preserved)
	ROR			// Rotate Right
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

	uint32_t fetched_opcode{ 0 };
	int decoded_func{ 0 };

	uint32_t GetReg(int regID);
	void SetReg(int regID, uint32_t value);

	void SaveCPSR();
	void RestoreCPSR();

	void Fetch();
	void Decode();
	void Execute();

	bool IsConditionReserved(uint32_t opcode) const;
	bool IsConditionOK(uint32_t opcode) const;
	bool IsBranch(uint32_t opcode);
	bool IsBranch_B_BL(uint32_t opcode);
	bool IsBranch_BX_BLX(uint32_t opcode);
	bool IsALU(uint32_t opcode);
	bool IsMemory(uint32_t opcode);

	void EXE_Branch(uint32_t opcode);
	void EXE_ALU(uint32_t opcode);
	bool AluExecute(ALUOpCode opcode, uint32_t& Rd, uint32_t Rn, uint32_t op2, bool setFlags);
	uint32_t AluBitShift(ShiftType type, uint32_t base, uint32_t shift, bool setFlags);
	void EXE_Memory(uint32_t opcode);
	void EXE_Nop(uint32_t opcode) { }

public:
	Cpu();
	bool SetBootAddr(uint32_t bootAddr);
	void SetMMU(ARM_mem* ptr);

	CpuMode GetCurrentCpuMode() const;

	void DebugStep();
	void Reset();

	void PrintDebug();
};