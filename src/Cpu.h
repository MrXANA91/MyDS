#pragma once

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include "arm_mem.h"
#include "instructions.h"
#include "breakpoints.h"

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
	Current = 0x0,
	User = 0x10,			// (non privileged)
	FIQ = 0x11,				// Fast Interrupt
	IRQ = 0x12,				// Normal Interrupt
	Supervisor = 0x13,		// (SWI)
	Abort = 0x17,			// Abort
	Undefined = 0x1B,		// Undefined
	System = 0x1F,			// (privileged 'User' mode)
};

enum ARMInstructionSet {
	ARMv4_ARM7,
	ARMv5_ARM9,
};

class Cpu {
private:
	ARM_mem* memory;
	ARMInstructionSet instructionSet;

	uint32_t bootAddress{ 0 };

	std::thread runThread;
	bool started{ false };
	Breakpoint breakpoint;
	uint64_t execInstr{ 0 };
	std::chrono::steady_clock::time_point end;
	std::chrono::steady_clock::time_point start;

	void runThreadFunc();
	void step();

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

	// Private members for instruction pointers
	Instruction instruction;

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

	void SetReg(int regID, uint32_t value, CpuMode forceCpuModeAccess = Current);
	void SetPCReg(uint32_t value);

	void SaveCPSR();
	void RestoreCPSR();

	void Fetch();
	void Decode();
	void Execute();

	bool IsConditionOK() const;

	// ========== EXCEPTIONS ============
	void ThrowReset();
	void ThrowUndefined();
	void ThrowSWI();
	void ThrowPrefetchAbort();
	void ThrowDataAbort();
	void ThrowIRQ();
	void ThrowFIQ();
	// ==================================

	// ========== INSTRUCTIONS ==========
	void DecodeInstructions();
	void DecodeMiscInstructions();
	void DecodeMultiplyOrExtraLoadStoreInstructions();
	void DecodeUndefinedInstructions();
	void DecodeMediaInstructions();
	void DecodeArchUndefinedInstructions();
	void DecodeUnconditionalInstructions();
	// Basics
	void DataProcImmShift();
	void DataProcRegShift();
	void DataProcImm();
	void LoadStoreImmOffset(sLoadStoreImmOffset* instruction);
	void LoadStoreRegOffset(sLoadStoreImmOffset* instruction);
	void LoadStoreMultiple(sLoadStoreMultiple* instruction);
	void Branch(sBranchInstruction* instruction);
	void CoprocLoadStore_DoubleRegTransf(sCoprocLoadStore_DoubleRegTransf* instruction);
	void CoprocDataProc(sCoprocDataProc* instruction);
	void CoprocRegTransf(sCoprocRegTransf* instruction);
	void SoftwareInterrupt(sSoftwareInterrupt* instruction);
	
	// Misc
	void MoveStatusRegToReg(sMoveStatusRegToReg* instruction);
	void MoveRegToStatusReg(sMoveRegToStatusReg* instruction);
	void MoveImmToStatusReg(sMoveImmToStatusReg* instruction);
	void BranchExchangeThumb(sBranchExchangeThumb* instruction);
	void BranchExchangeJava(sBranchExchangeJava* instruction);
	void CountLeadingZeros(sCountLeadingZeros* instruction);
	void BranchLinkExchangeThumb(sBranchLinkExchangeThumb* instruction);
	void SaturatingAddSub(sSaturatingAddSub* instruction);
	void SoftwareBreakpoint(sSoftwareBreakpoint* instruction);
	void SignedMultiplies(sSignedMultiplies* instruction);
	
	// Multiply
	void MultiplyInstruction(sMultiplyInstruction* instruction);
	void UnsignedMultiplyLong(sUnsignedMultiplyLong* instruction);
	void MultiplyLongInstruction(sMultiplyLongInstruction* instruction);
	
	// Extra Load/Store
	void SwapInstruction(sSwapInstruction* instruction);
	void LoadStoreHalfwordRegOffset(sLoadStoreHalfwordImmOffset* instruction);
	void LoadStoreHalfwordImmOffset(sLoadStoreHalfwordImmOffset* instruction);
	void LoadSignedHalfwordByteImmOffset(sLoadSignedHalfwordByteImmOffset* instruction);
	void LoadSignedHalfwordByteRegOffset(sLoadSignedHalfwordByteRegOffset* instruction);
	void LoadStoreDoublewordRegOffset(sLoadStoreDoublewordRegOffset* instruction);
	void LoadStoreDoublewordImmOffset(sLoadStoreDoublewordImmOffset* instruction);
	
	// Media
	void ParallelAddSub(sParallelAddSub* instruction);
	void HalfwordPack(sHalfwordPack* instruction);
	void WordSaturate(sWordSaturate* instruction);
	void ParallelHalfwordSaturate(sParallelHalfwordSaturate* instruction);
	void ByteReverseWord(sByteReverseWord* instruction);
	void ByteReversePackedHalfword(sByteReversePackedHalfword* instruction);
	void ByteReverseSignedHalfword(sByteReverseSignedHalfword* instruction);
	void SelectBytes(sSelectBytes* instruction);
	void SignZeroExtend(sSignZeroExtend* instruction);
	void Multiplies_Type3(sMultiplies_Type3* instruction);
	void UnsignedSumOfDifferences(sUnsignedSumOfDifferences* instruction);
	void UnsignedSumOfDifferencesAcc(sUnsignedSumOfDifferencesAcc* instruction);
	void UndefinedInstruction(sUndefinedInstruction* instruction);
	
	// Unconditional
	void ChangeProcessorState(sChangeProcessorState* instruction);
	void SetEndianness(sSetEndianness* instruction);
	void CachePreload(sCachePreload* instruction);
	void SaveReturnState(sSaveReturnState* instruction);
	void ReturnFromException(sReturnFromException* instruction);
	void BranchLinkChangeToThumb(sBranchLinkChangeToThumb* instruction);
	void AdditionalCoprocessorDoubleRegTransf(sAdditionalCoprocessorDoubleRegTransf* instruction);
	
	// ==================================

	bool AluExecute(eALUOpCode alu_opcode, uint32_t& Rd, uint32_t Rn, uint32_t op2, bool setFlags);
	uint32_t AluBitShift(eShiftType type, uint32_t base, uint32_t shift, bool setFlags, bool force = false);

public:
	bool Debug{ false };

	Cpu(ARMInstructionSet instructionSet);

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
	/// Start executing instructions
	/// </summary>
	void Run();

	/// <summary>
	/// Stop executing instructions
	/// </summary>
	void Stop();

	/// <summary>
	/// 
	/// </summary>
	/// <returns>True if instructions are executed, else false</returns>
	bool IsRunning() const;

	/// <summary>
	/// Execute Fetch/Decode/Execute all at ones, one time
	/// </summary>
	void DebugStep();

	uint32_t GetReg(int regID, CpuMode forceCpuModeAccess = Current) const;

	std::string eConditionToString(eCondition cond);
	std::string eALUOpCodeToString(eALUOpCode aluOpcode);
	std::string eShiftTypeToString(eShiftType shift);
	std::string eInstructCodeToString(eInstructCode instruct, std::string &othertext);

	void DisplayRegisters();

	void DisplayBreakpoints();
	bool SetBreakpoint(uint32_t address);
	bool ToggleBreakpoint(int index);
	bool RemoveBreakpoint(int index);
};