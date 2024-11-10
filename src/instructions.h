#pragma once

#include <cstdint>
#include <string>

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
	// Basic
	INSTRUCT_DATA_PROC_IMM_SHIFT,
	INSTRUCT_DATA_PROC_REG_SHIFT,
	INSTRUCT_DATA_PROC_IMM,
	INSTRUCT_LOAD_STORE_IMM_OFFSET,
	INSTRUCT_LOAD_STORE_REG_OFFSET,
	INSTRUCT_LOAD_STORE_MULTIPLE,
	INSTRUCT_BRANCH_BRANCHLINK,
	INSTRUCT_COPROC_LOAD_STORE_DOUBLE_REG_TRANSF,
	INSTRUCT_COPROC_DATA_PROC,
	INSTRUCT_COPROC_REG_TRANSF,
	INSTRUCT_SOFTWARE_INTERRUPT,
	// Misc
	INSTRUCT_MOVE_STATUS_REG_TO_REG,
	INSTRUCT_MOVE_REG_TO_STATUS_REG,
	INSTRUCT_MOVE_IMM_TO_STATUS_REG,
	INSTRUCT_BRANCH_EXCHANGE_THUMB,
	INSTRUCT_BRANCH_EXCHANGE_JAVA,
	INSTRUCT_COUNT_LEADING_ZEROS,
	INSTRUCT_BRANCH_LINK_EXCHANGE_THUMB,
	INSTRUCT_SATURATING_ADD_SUB,
	INSTRUCT_SOFTWARE_BREAKPOINT,
	INSTRUCT_SIGNED_MULTIPLIES,
	// Multiply
	INSTRUCT_MULTIPLY,
	INSTRUCT_UNSIGNED_MULTIPLY_LONG,
	INSTRUCT_MULTIPLY_LONG,
	// Extra Load/Store
	INSTRUCT_SWAP,
	INSTRUCT_LOAD_STORE_REG_EXCLUSIVE,
	INSTRUCT_LOAD_STORE_HALFWORD_REG_OFFSET,
	INSTRUCT_LOAD_STORE_HALFWORD_IMM_OFFSET,
	INSTRUCT_LOAD_SIGNED_HALFWORD_BYTE_IMM_OFFSET,
	INSTRUCT_LOAD_SIGNED_HALFWORD_BYTE_REG_OFFSET,
	INSTRUCT_LOAD_STORE_DOUBLEWORD_REG_OFFSET,
	INSTRUCT_LOAD_STORE_DOUBLEWORD_IMM_OFFSET,
	// Media
	INSTRUCT_PARALLEL_ADD_SUB,
	INSTRUCT_HALFWORD_PACK,
	INSTRUCT_WORD_SATURATE,
	INSTRUCT_PARALLEL_HALFWORD_SATURATE,
	INSTRUCT_BYTE_REVERSE_WORD,
	INSTRUCT_BYTE_REVERSE_PACKED_HALFWORD,
	INSTRUCT_BYTE_REVERSE_SIGNED_HALFWORD,
	INSTRUCT_SELECT_BYTES,
	INSTRUCT_SIGN_ZERO_EXTEND,
	INSTRUCT_MULTIPLIES_TYPE3,
	INSTRUCT_UNSIGNED_SUM_OF_DIFFS,
	INSTRUCT_UNSIGNED_SUM_OF_DIFFS_ACC,
	INSTRUCT_UNDEFINED,
	// Unconditional
	INSTRUCT_CHANGE_PROCESSOR_STATE,
	INSTRUCT_SET_ENDIANNESS,
	INSTRUCT_CACHE_PRELOAD,
	INSTRUCT_SAVE_RETURN_STATE,
	INSTRUCT_RETURN_FROM_EXCEPTION,
	INSTRUCT_BRANCH_LINK_CHANGE_TO_THUMB,
	INSTRUCT_ADDITIONAL_COPROC_DOUBLEREG_TRANSF,
};

union sInstruction {
	struct {
		uint32_t code : 28;
		uint32_t condition : 4;
	};
	uint32_t opcode;
};

#pragma region Basic instructions

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

union sBranchInstruction {
	struct {
		uint32_t offset : 24;
		uint32_t L : 1;
		uint32_t mustbe101 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sBranchInstruction) == 4, "BranchInstruction size is not 4");

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

union sSoftwareInterrupt {
	struct {
		uint32_t swiNumber : 24;
		uint32_t mustbe1111 : 4;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sSoftwareInterrupt) == 4, "SoftwareInterrupt size is not 4");

#pragma endregion

#pragma region Miscellaneous Instructions

union sMoveStatusRegToReg {
	struct {
		uint32_t mustbe0 : 12;
		uint32_t Rd : 4;
		uint32_t mustbe1111 : 4;
		uint32_t mustbe00 : 2;
		uint32_t R : 1;
		uint32_t mustbe00010 : 5;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sMoveStatusRegToReg) == 4, "sMoveStatusRegToReg size is not 4");

union sMoveRegToStatusReg {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe0 : 8;
		uint32_t mustbe1111 : 4;
		uint32_t Mask : 4;
		uint32_t mustbe10 : 2;
		uint32_t R : 1;
		uint32_t mustbe00010 : 5;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sMoveRegToStatusReg) == 4, "sMoveRegToStatusReg size is not 4");

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

static_assert(sizeof(sMoveImmToStatusReg) == 4, "sMoveImmToStatusReg size is not 4");

union sBranchExchangeThumb {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe_0x12FFF1 : 24;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sBranchExchangeThumb) == 4, "sBranchExchangeThumb size is not 4");

union sBranchExchangeJava {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe_0x12FFF2 : 24;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sBranchExchangeJava) == 4, "sBranchExchangeJava size is not 4");

union sCountLeadingZeros {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbeF1 : 8;
		uint32_t Rd : 4;
		uint32_t mustbe16F : 12;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sCountLeadingZeros) == 4, "sCountLeadingZeros size is not 4");

union sBranchLinkExchangeThumb {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe_0x12FFF3 : 24;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sBranchLinkExchangeThumb) == 4, "sBranchLinkExchangeThumb size is not 4");

union sSaturatingAddSub {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe_0x05 : 8;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t mustbe0 : 1;
		uint32_t opcode : 2;
		uint32_t mustbe00010 : 5;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sSaturatingAddSub) == 4, "sSaturatingAddSub size is not 4");

union sSoftwareBreakpoint {
	struct {
		uint32_t immediate2 : 4;
		uint32_t mustbe7 : 4;
		uint32_t immediate1 : 12;
		uint32_t mustbe_0x12 : 8;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sSoftwareBreakpoint) == 4, "sSoftwareBreakpoint size is not 4");

union sSignedMultiplies {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe0_2 : 1;
		uint32_t x : 1;
		uint32_t y : 1;
		uint32_t mustbe1 : 1;
		uint32_t Rs : 4;
		uint32_t Rn : 4;
		uint32_t Rd : 4;
		uint32_t mustbe0_1 : 1;
		uint32_t opcode : 2;
		uint32_t mustbe00010 : 5;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sSignedMultiplies) == 4, "sSignedMultiplies size is not 4");

#pragma endregion

#pragma region Multiply instructions

union sMultiplyInstruction {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe1001 : 4;
		uint32_t Rs : 4;
		uint32_t Rn : 4;
		uint32_t Rd : 4;
		uint32_t S : 1;
		uint32_t A : 1;
		uint32_t mustbe0 : 6;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sMultiplyInstruction) == 4, "sMultiplyInstruction size is not 4");

union sUnsignedMultiplyLong {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe1001 : 4;
		uint32_t Rs : 4;
		uint32_t RdLo : 4;
		uint32_t RdHi : 4;
		uint32_t mustbe_0x04 : 8;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sUnsignedMultiplyLong) == 4, "sUnsignedMultiplyLong size is not 4");

union sMultiplyLongInstruction {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe1001 : 4;
		uint32_t Rs : 4;
		uint32_t RdLo : 4;
		uint32_t RdHi : 4;
		uint32_t S : 1;
		uint32_t A : 1;
		uint32_t Unsigned : 1;
		uint32_t mustbe00001 : 5;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sMultiplyLongInstruction) == 4, "sMultiplyLongInstruction size is not 4");

#pragma endregion

#pragma region Extra Load/Store instructions

union sSwapInstruction {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe1001 : 4;
		uint32_t mustbe0 : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t mustbe00 : 2;
		uint32_t B : 1;
		uint32_t mustbe00010 : 5;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sSwapInstruction) == 4, "sSwapInstruction size is not 4");

union sLoadStoreRegExclusive {
	struct {
		uint32_t mustbeF9F : 12;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t L : 1;
		uint32_t mustbe0001100 : 7;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sLoadStoreRegExclusive) == 4, "sLoadStoreRegExclusive size is not 4");

union sLoadStoreHalfwordRegOffset {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe1011 : 4;
		uint32_t mustbe0000 : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t L : 1;
		uint32_t W : 1;
		uint32_t mustbe0 : 1;
		uint32_t U : 1;
		uint32_t P : 1;
		uint32_t mustbe000 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sLoadStoreHalfwordRegOffset) == 4, "sLoadStoreHalfwordRegOffset size is not 4");

union sLoadStoreHalfwordImmOffset {
	struct {
		uint32_t LoOffset : 4;
		uint32_t mustbe1011 : 4;
		uint32_t HiOffset : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t L : 1;
		uint32_t W : 1;
		uint32_t mustbe1 : 1;
		uint32_t U : 1;
		uint32_t P : 1;
		uint32_t mustbe000 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sLoadStoreHalfwordImmOffset) == 4, "sLoadStoreHalfwordImmOffset size is not 4");

union sLoadSignedHalfwordByteImmOffset {
	struct {
		uint32_t LoOffset : 4;
		uint32_t mustbe1_3 : 1;
		uint32_t H : 1;
		uint32_t mustbe11 : 2;
		uint32_t HiOffset : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t mustbe1_2 : 1;
		uint32_t W : 1;
		uint32_t mustbe1_1 : 1;
		uint32_t U : 1;
		uint32_t P : 1;
		uint32_t mustbe000 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sLoadSignedHalfwordByteImmOffset) == 4, "sLoadSignedHalfwordByteImmOffset size is not 4");

union sLoadSignedHalfwordByteRegOffset {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe1_2 : 1;
		uint32_t H : 1;
		uint32_t mustbe11 : 2;
		uint32_t mustbe0000 : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t mustbe1_1 : 1;
		uint32_t W : 1;
		uint32_t mustbe0 : 1;
		uint32_t U : 1;
		uint32_t P : 1;
		uint32_t mustbe000 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sLoadSignedHalfwordByteRegOffset) == 4, "sLoadSignedHalfwordByteRegOffset size is not 4");

union sLoadStoreDoublewordRegOffset {
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe1 : 1;
		uint32_t St : 1;
		uint32_t mustbe11 : 2;
		uint32_t mustbe0000 : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t mustbe0_2 : 1;
		uint32_t W : 1;
		uint32_t mustbe0_1 : 1;
		uint32_t U : 1;
		uint32_t P : 1;
		uint32_t mustbe000 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sLoadStoreDoublewordRegOffset) == 4, "sLoadStoreDoublewordRegOffset size is not 4");

union sLoadStoreDoublewordImmOffset {
	struct {
		uint32_t LoOffset : 4;
		uint32_t mustbe1_2 : 1;
		uint32_t St : 1;
		uint32_t mustbe11 : 2;
		uint32_t HiOffset : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t mustbe0 : 1;
		uint32_t W : 1;
		uint32_t mustbe1_1 : 1;
		uint32_t U : 1;
		uint32_t P : 1;
		uint32_t mustbe000 : 3;
		uint32_t condition : 4;
	};
	uint32_t code;
};

static_assert(sizeof(sLoadStoreDoublewordImmOffset) == 4, "sLoadStoreDoublewordImmOffset size is not 4");

#pragma endregion

#pragma region Media instructions

union sParallelAddSub {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sParallelAddSub) == 4, "sParallelAddSub size is not 4");

union sHalfwordPack {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sHalfwordPack) == 4, "sHalfwordPack size is not 4");

union sWordSaturate {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sWordSaturate) == 4, "sWordSaturate size is not 4");

union sParallelHalfwordSaturate {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sParallelHalfwordSaturate) == 4, "sParallelHalfwordSaturate size is not 4");

union sByteReverseWord {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sByteReverseWord) == 4, "sByteReverseWord size is not 4");

union sByteReversePackedHalfword {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sByteReversePackedHalfword) == 4, "sByteReversePackedHalfword size is not 4");

union sByteReverseSignedHalfword {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sByteReverseSignedHalfword) == 4, "sByteReverseSignedHalfword size is not 4");

union sSelectBytes {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sSelectBytes) == 4, "sSelectBytes size is not 4");

union sSignZeroExtend {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sSignZeroExtend) == 4, "sSignZeroExtend size is not 4");

union sMultiplies_Type3 {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sMultiplies_Type3) == 4, "sMultiplies_Type3 size is not 4");

union sUnsignedSumOfDifferences {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sUnsignedSumOfDifferences) == 4, "sUnsignedSumOfDifferences size is not 4");

union sUnsignedSumOfDifferencesAcc {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sUnsignedSumOfDifferencesAcc) == 4, "sUnsignedSumOfDifferencesAcc size is not 4");

union sUndefinedInstruction {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sUndefinedInstruction) == 4, "sUndefinedInstruction size is not 4");

#pragma endregion

#pragma region Unconditional instructions

union sChangeProcessorState {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sChangeProcessorState) == 4, "sChangeProcessorState size is not 4");

union sSetEndianness {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sSetEndianness) == 4, "sSetEndianness size is not 4");

union sCachePreload {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sCachePreload) == 4, "sCachePreload size is not 4");

union sSaveReturnState {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sSaveReturnState) == 4, "sSaveReturnState size is not 4");

union sReturnFromException {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sReturnFromException) == 4, "sReturnFromException size is not 4");

union sBranchLinkChangeToThumb {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sBranchLinkChangeToThumb) == 4, "sBranchLinkChangeToThumb size is not 4");

union sAdditionalCoprocessorDoubleRegTransf {
	struct {

	};
	uint32_t code;
};

static_assert(sizeof(sAdditionalCoprocessorDoubleRegTransf) == 4, "sAdditionalCoprocessorDoubleRegTransf size is not 4");

#pragma endregion

#pragma pack(pop)

class Instruction {
private:
	uint32_t opcode{ 0 };
public:
	eInstructCode decodedInstructCode{ INSTRUCT_NULL };
	sInstruction* pInstruction;

	// ======= Instructions =======
	// Basics
	sDataProcImmShift* pDataProcImmShift;
	sDataProcRegShift* pDataProcRegShift;
	sDataProcImm* pDataProcImm;
	sLoadStoreImmOffset* pLoadStoreImmOffset;
	sLoadStoreRegOffset* pLoadStoreRegOffset;
	sLoadStoreMultiple* pLoadStoreMultiple;
	sBranchInstruction* pBranchInstruction;
	sCoprocLoadStore_DoubleRegTransf* pCoprocLoadStore_DoubleRegTransf;
	sCoprocDataProc* pCoprocDataProc;
	sCoprocRegTransf* pCoprocRegTransf;
	sSoftwareInterrupt* pSoftwareInterrupt;

	// Misc
	sMoveStatusRegToReg* pMoveStatusRegToReg;
	sMoveRegToStatusReg* pMoveRegToStatusReg;
	sMoveImmToStatusReg* pMoveImmToStatusReg;
	sBranchExchangeThumb* pBranchExchangeThumb;
	sBranchExchangeJava* pBranchExchangeJava;
	sCountLeadingZeros* pCountLeadingZeros;
	sBranchLinkExchangeThumb* pBranchLinkExchangeThumb;
	sSaturatingAddSub* pSaturatingAddSub;
	sSoftwareBreakpoint* pSoftwareBreakpoint;
	sSignedMultiplies* pSignedMultiplies;

	// Multiply
	sMultiplyInstruction* pMultiplyInstruction;
	sUnsignedMultiplyLong* pUnsignedMultiplyLong;
	sMultiplyLongInstruction* pMultiplyLongInstruction;

	// Extra LoadStore
	sSwapInstruction* pSwapInstruction;
	sLoadStoreRegExclusive* pLoadStoreRegExclusive;
	sLoadStoreHalfwordRegOffset* pLoadStoreHalfwordRegOffset;
	sLoadStoreHalfwordImmOffset* pLoadStoreHalfwordImmOffset;
	sLoadSignedHalfwordByteImmOffset* pLoadSignedHalfwordByteImmOffset;
	sLoadSignedHalfwordByteRegOffset* pLoadSignedHalfwordByteRegOffset;
	sLoadStoreDoublewordRegOffset* pLoadStoreDoublewordRegOffset;
	sLoadStoreDoublewordImmOffset* pLoadStoreDoublewordImmOffset;

	// Media
	sParallelAddSub* pParallelAddSub;
	sHalfwordPack* pHalfwordPack;
	sWordSaturate* pWordSaturate;
	sParallelHalfwordSaturate* pParallelHalfwordSaturate;
	sByteReverseWord* pByteReverseWord;
	sByteReversePackedHalfword* pByteReversePackedHalfword;
	sByteReverseSignedHalfword* pByteReverseSignedHalfword;
	sSelectBytes* pSelectBytes;
	sSignZeroExtend* pSignZeroExtend;
	sMultiplies_Type3* pMultiplies_Type3;
	sUnsignedSumOfDifferences* pUnsignedSumOfDifferences;
	sUnsignedSumOfDifferencesAcc* pUnsignedSumOfDifferencesAcc;
	sUndefinedInstruction* pUndefinedInstruction;

	// Unconditional
	sChangeProcessorState* pChangeProcessorState;
	sSetEndianness* pSetEndianness;
	sCachePreload* pCachePreload;
	sSaveReturnState* pSaveReturnState;
	sReturnFromException* pReturnFromException;
	sBranchLinkChangeToThumb* pBranchLinkChangeToThumb;
	sAdditionalCoprocessorDoubleRegTransf* pAdditionalCoprocessorDoubleRegTransf;

	// ============================

	Instruction();

	void Set(uint32_t opcode);
	const uint32_t Get() const;

	void DecodeReset();
	void SetDecode(eInstructCode instruct);
	const eInstructCode GetDecode() const;

	// ======== Instruction Decoders ========
	// Basic
	bool IsDataProcImmShift() const;
	bool IsDataProcRegShift() const;
	bool IsDataProcImm() const;
	bool IsLoadStoreImmOffset() const;
	bool IsLoadStoreMultiple() const;
	bool IsLoadStoreRegOffset() const;
	bool IsBranch() const;
	bool IsCoprocLoadStore_DoubleRegTransf() const;
	bool IsCoprocDataProc() const;
	bool IsCoprocRegTransf() const;
	bool IsSoftwareInterrupt() const;

	// Misc
	bool IsMiscellaneous() const;

	bool IsMoveStatusRegToReg() const;
	bool IsMoveRegToStatusReg() const;
	bool IsMoveImmToStatusReg() const;
	bool IsBranchExchangeThumb() const;
	bool IsBranchExchangeJava() const;
	bool IsCountLeadingZeros() const;
	bool IsBranchLinkExchangeThumb() const;
	bool IsSaturatingAddSub() const;
	bool IsSoftwareBreakpoint() const;
	bool IsSignedMultiplies() const;

	// Multiply
	bool IsMultipliesOrExtraLoadStore() const;

	bool IsMultiplyInstruction() const;
	bool IsUnsignedMultiplyLong() const;
	bool IsMultiplyLongInstruction() const;

	// Extra Load/Store
	bool IsSwapInstruction() const;
	bool IsLoadStoreRegExclusive() const;
	bool IsLoadStoreHalfwordRegOffset() const;
	bool IsLoadStoreHalfwordImmOffset() const;
	bool IsLoadSignedHalfwordByteImmOffset() const;
	bool IsLoadSignedHalfwordByteRegOffset() const;
	bool IsLoadStoreDoublewordRegOffset() const;
	bool IsLoadStoreDoublewordImmOffset() const;

	// Media
	bool IsMedia() const;

	bool IsParallelAddSub() const;
	bool IsHalfwordPack() const;
	bool IsWordSaturate() const;
	bool IsParallelHalfwordSaturate() const;
	bool IsByteReverseWord() const;
	bool IsByteReversePackedHalfword() const;
	bool IsByteReverseSignedHalfword() const;
	bool IsSelectBytes() const;
	bool IsSignZeroExtend() const;
	bool IsMultiplies_Type3() const;
	bool IsUnsignedSumOfDifferences() const;
	bool IsUnsignedSumOfDifferencesAcc() const;
	bool IsUndefined() const;

	bool IsArchUndefined() const;

	// Unconditional
	bool IsUnconditional() const;

	bool IsChangeProcessorState() const;
	bool IsSetEndianness() const;
	bool IsCachePreload() const;
	bool IsSaveReturnState() const;
	bool IsReturnFromException() const;
	bool IsBranchLinkChangeToThumb() const;
	bool IsAdditionalCoprocessorDoubleRegTransf() const;

	// Others
	bool IsConditionReserved() const;

	// ======================================

	// Assembly string display
	std::string ToString() const;
};