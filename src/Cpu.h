#pragma once

#include <cstdint>
#include <iostream>
#include <iomanip>
#include "arm_mem.h"
#include "instructions.h"

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
constexpr auto EXCEPTION_EXEC_MEM_REG_PC_UNAUTHORIZE = 8;
constexpr auto EXCEPTION_EXEC_MEM_DECODE_FAILURE = 9;

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

enum CpuMode : uint16_t {
	User = 0x10,			// (non privileged)
	FIQ = 0x11,				// Fast Interrupt
	IRQ = 0x12,				// Normal Interrupt
	Supervisor = 0x13,		// (SWI)
	Abort = 0x17,			// Abort
	Undefined = 0x1B,		// Undefined
	System = 0x1F,			// (privileged 'User' mode)
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

	// Private members for instructions
	uint32_t fetchedInstruction{ 0 };
	eInstructCode decodedInstructCode{ INSTRUCT_NULL };

	eALUOpCode aluOpcode{ 0 };
	bool SetFlags{ false };
	uint32_t Rn{ 0 };
	uint32_t Rn_value{ 0 };
	uint32_t Rd{ 0 };
	uint32_t Rd_value{ 0 };
	uint32_t Rm{ 0 };
	uint32_t Rm_value{ 0 };
	uint32_t Rs{ 0 };
	uint32_t Rs_value{ 0 };
	uint32_t ShiftAmount{ 0 };
	eShiftType Shift{ 0 };
	uint32_t Rotate{ 0 };
	uint32_t Mask{ 0 };
	uint32_t Immediate{ 0 };
	uint32_t Offset{ 0 };
	uint32_t Operand{ 0 };

	bool IsThumbMode() {
		return cpsr.bits.T == 1;
	}

	void SetReg(int regID, uint32_t value);

	void SaveCPSR();
	void RestoreCPSR();

	void Fetch();
	void Decode();
	void Execute();

	bool IsConditionOK(uint32_t opcode) const;

	// Branch
	void Branch(uint32_t opcode);

	// Data processing
	void DataProcImmShift(uint32_t opcode);
	void DataProcRegShift(uint32_t opcode);
	void DataProcImm(uint32_t opcode);
	bool AluExecute(eALUOpCode alu_opcode, uint32_t& Rd, uint32_t Rn, uint32_t op2, bool setFlags);
	uint32_t AluBitShift(eShiftType type, uint32_t base, uint32_t shift, bool setFlags, bool force = false);

	// Multiply

	// Misc arithmetic (CLZ)

	// Status register access
	void MoveImmToStatusReg(uint32_t opcode);

	// Load and store
	void LoadStoreImmOffset(uint32_t opcode);
	void LoadStoreRegOffset(uint32_t opcode);
	void LoadStoreMultiple(uint32_t opcode);

	// Semaphore

	// Exeption-generating
	void SoftwareInterrupt(uint32_t opcode);

	// Coprocessor
	void CoprocLoadStore_DoubleRegTransf(uint32_t opcode);
	void CoprocDataProc(uint32_t opcode);
	void CoprocRegTransf(uint32_t opcode);

public:
	bool Debug{ false };

	Cpu();

	/// <summary>
	/// Set starting CPU (or boot) address. PC will not be changed if CPU already running.
	/// </summary>
	/// <param name="bootAddr">Address in virtual ARM memory</param>
	/// <returns>false if CPU has already started, true otherwise</returns>
	bool SetBootAddr(uint32_t bootAddr);

	/// <summary>
	/// Set virtual ARM memory object pointer
	/// </summary>
	/// <param name="ptr">Pointer to virtual memory object</param>
	void SetMMU(ARM_mem* ptr);

	/// <summary>
	/// Returns the current CPU profile mode (User, FIQ, IRQ, Supervisor, Abort, Undefined, System)
	/// </summary>
	/// <returns></returns>
	CpuMode GetCurrentCpuMode() const;

	/// <summary>
	/// Reset the CPU and the PC to boot address
	/// </summary>
	void Reset();

	/// <summary>
	/// Execute Fetch/Decode/Execute all at ones, one time
	/// </summary>
	void DebugStep();

	uint32_t GetReg(int regID);

	static std::string eConditionToString(eCondition cond);
	static std::string eALUOpCodeToString(eALUOpCode aluOpcode);
	static std::string eShiftTypeToString(eShiftType shift);
	static std::string eInstructCodeToString(eInstructCode instruct);

	void DisplayRegisters();
};