#pragma once

#include <cstdint>

#pragma pack(push)
#pragma pack(1)

enum eCondition : uint8_t {
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

bool IsConditionReserved(uint32_t opcode);

enum eALUOpCode : uint8_t {
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

enum eShiftType : uint8_t {
	LSL = 0,	// Logical Shift Left
	LSR,		// Logical Shift Right
	ASR,		// Arithmetic Shift Right (sign bit is preserved)
	ROR			// Rotate Right
};

enum eInstructCode : int {
	INSTRUCT_NULL,
	INSTRUCT_DATA_PROC_IMM_SHIFT,
	INSTRUCT_DATA_PROC_REG_SHIFT,
	INSTRUCT_DATA_PROC_IMM,
	INSTRUCT_MOVE_IMM_STATUS_REG,
	INSTRUCT_LOAD_STORE_IMM_OFFSET,
	INSTRUCT_LOAD_STORE_REG_OFFSET,
	INSTRUCT_LOAD_STORE_MULTIPLE,
	INSTRUCT_BRANCH_BRANCHLINK,
	INSTRUCT_COPROC_LOAD_STORE_DOUBLE_REG_TRANSF,
	INSTRUCT_COPROC_DATA_PROC,
	INSTRUCT_COPROC_REG_TRANSF,
	INSTRUCT_SOFTWARE_INTERRUPT
};

union sInstruction {
	struct {
		uint32_t code : 28;
		uint32_t condition : 4;
	};
	uint32_t opcode;
};

union sDataProcImmShift {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe0 : 1;
		uint32_t shift : 2;
		uint32_t shiftAmount : 5;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t S : 1;
		uint32_t opcode : 4;
		uint32_t mustbe000 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sDataProcImmShift) == 4, "DataProcImmShift size is not 4");

bool IsDataProcImmShift(uint32_t opcode);

bool IsMiscellaneous(uint32_t opcode);

union sDataProcRegShift {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe1 : 1;
		uint32_t shift : 2;
		uint32_t mustbe0 : 1;
		uint32_t Rs : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t S : 1;
		uint32_t opcode : 4;
		uint32_t mustbe000 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sDataProcRegShift) == 4, "DataProcRegShift size is not 4");

bool IsDataProcRegShift(uint32_t opcode);

bool IsMultipliesOrExtraLoadStore(uint32_t opcode);

union sDataProcImm {
	struct {
		uint32_t immediate : 8;
		uint32_t rotate : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t S : 1;
		uint32_t opcode : 4;
		uint32_t mustbe001 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sDataProcImm) == 4, "DataProcImm size is not 4");

bool IsDataProcImm(uint32_t opcode);

bool IsUndefined(uint32_t opcode);

union sMoveImmToStatusReg {
	struct {
		uint32_t immediate : 8;
		uint32_t rotate : 4;
		uint32_t mustbe1111 : 4;
		uint32_t Mask : 4;
		uint32_t mustbe10 : 2;
		uint32_t R : 1;
		uint32_t mustbe00110 : 5;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sMoveImmToStatusReg) == 4, "MoveImmToStatusReg size is not 4");

bool IsMoveImmToStatusReg(uint32_t opcode);

union sLoadStoreImmOffset {
	struct {
		uint32_t immediate : 12;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t L : 1;
		uint32_t W : 1;
		uint32_t B : 1;
		uint32_t U : 1;
		uint32_t P : 1;
		uint32_t mustbe010 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sLoadStoreImmOffset) == 4, "LoadStoreImmOffset size is not 4");

bool IsLoadStoreImmOffset(uint32_t opcode);

union sLoadStoreRegOffset {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe0 : 1;
		uint32_t shift : 2;
		uint32_t shiftAmount : 5;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t L : 1;
		uint32_t W : 1;
		uint32_t B : 1;
		uint32_t U : 1;
		uint32_t P : 1;
		uint32_t mustbe011 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sLoadStoreRegOffset) == 4, "LoadStoreRegOffset size is not 4");

bool IsLoadStoreRegOffset(uint32_t opcode);

bool IsMedia(uint32_t opcode);
bool IsArchUndefined(uint32_t opcode);

union sLoadStoreMultiple {
	struct {
		uint32_t registerList : 16; // bit0 for R0, bit1 for R1, ..., bit15 for R15(PC)
		uint32_t Rn : 4;
		uint32_t L : 1;
		uint32_t W : 1;
		uint32_t S : 1;
		uint32_t U : 1;
		uint32_t P : 1;
		uint32_t mustbe100 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sLoadStoreMultiple) == 4, "LoadStoreMultiple size is not 4");

bool IsLoadStoreMultiple(uint32_t opcode);

union sBranchInstruction {
	struct {
		uint32_t offset : 24;
		uint32_t L : 1;
		uint32_t mustbe101 : 3;
		uint32_t condition:4;
	};
	uint32_t code;
};

static_assert(sizeof(sBranchInstruction) == 4, "BranchInstruction size is not 4");

bool IsBranch(uint32_t opcode);

union sCoprocLoadStore_DoubleRegTransf {
	struct {
		uint32_t offset : 8;
		uint32_t cp_num : 4;
		uint32_t CRd : 4;
		uint32_t Rn : 4;
		uint32_t L : 1;
		uint32_t W : 1;
		uint32_t N : 1;
		uint32_t U : 1;
		uint32_t P : 1;
		uint32_t mustbe110 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sCoprocLoadStore_DoubleRegTransf) == 4, "CoprocLoadStore_DoubleRegTransf size is not 4");

bool IsCoprocLoadStore_DoubleRegTransf(uint32_t opcode);

union sCoprocDataProc {
	struct {
		uint32_t CRm : 4;
		uint32_t mustbe0 : 1;
		uint32_t opcode2 : 3;
		uint32_t cp_num : 4;
		uint32_t CRd : 4;
		uint32_t CRn : 4;
		uint32_t opcode1 : 4;
		uint32_t mustbe1110 : 4;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sCoprocDataProc) == 4, "CoprocDataProc size is not 4");

bool IsCoprocDataProc(uint32_t opcode);

union sCoprocRegTransf {
	struct {
		uint32_t CRm : 4;
		uint32_t mustbe1 : 1;
		uint32_t opcode2 : 3;
		uint32_t cp_num : 4;
		uint32_t Rd : 4;
		uint32_t CRn : 4;
		uint32_t L : 1;
		uint32_t opcode1 : 3;
		uint32_t mustbe1110 : 4;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sCoprocRegTransf) == 4, "CoprocRegTransf size is not 4");

bool IsCoprocRegTransf(uint32_t opcode);

union sSoftwareInterrupt {
	struct {
		uint32_t swiNumber : 24;
		uint32_t mustbe1111 : 4;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sSoftwareInterrupt) == 4, "SoftwareInterrupt size is not 4");

bool IsSoftwareInterrupt(uint32_t opcode);

bool IsUnconditional(uint32_t opcode);

#pragma pack(pop)
