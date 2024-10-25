#pragma once

#include <cstdint>

#pragma pack(push)
#pragma pack(1)

union Instruction {
	struct {
		uint32_t code : 28;
		uint32_t condition : 4;
	};
	uint32_t opcode;
};

union BranchInstruction {
	struct {
		uint32_t nn : 24;
		uint32_t b_opcode : 1;
		uint32_t mustbe5 : 3;
		uint32_t condition:4;
	};
	struct {
		uint32_t Rn : 4;
		uint32_t bx_opcode : 4;
		uint32_t mustbe12FFF : 20;
		uint32_t condition:4;
	} thumbSwitch;
	uint32_t opcode;
};

static_assert(sizeof(BranchInstruction) == 4, "BranchInstruction size is not 4");


union ALUInstruction {
	struct {
		uint32_t Rm : 4;
		uint32_t R : 1;
		uint32_t shiftType : 2;
		uint32_t Is : 5;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t setFlags : 1;
		uint32_t alu_opcode : 4;
		uint32_t immediate : 1;
		uint32_t mustbe0_2 : 2;
		uint32_t condition : 4;
	} secOpReg_shiftImm;
	struct {
		uint32_t Rm : 4;
		uint32_t R : 1;
		uint32_t shiftType : 2;
		uint32_t mustbe0_1 : 1;
		uint32_t Rs : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t setFlags : 1;
		uint32_t alu_opcode : 4;
		uint32_t immediate : 1;
		uint32_t mustbe0_2 : 2;
		uint32_t condition : 4;
	} secOpReg_shiftReg;
	struct {
		uint32_t nn : 8;
		uint32_t rorIs : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t setFlags : 1;
		uint32_t alu_opcode : 4;
		uint32_t immediate : 1;
		uint32_t mustbe0_2 : 2;
		uint32_t condition : 4;
	} secOpImm;
	uint32_t opcode;
};

static_assert(sizeof(ALUInstruction) == 4, "ALUInstruction size is not 4");

union SingleDataTransferInstruction {
	struct {
		uint32_t immediateOffset : 12;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t load_store : 1;
		uint32_t writeBack_forceUserAccess : 1;
		uint32_t byte_word : 1;
		uint32_t up_down : 1;
		uint32_t preIndexed : 1;
		uint32_t immediate : 1;
		uint32_t mustbe1 : 2;
		uint32_t condition : 4;
	} offsetImmediate;
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe0 : 1;
		uint32_t shiftType : 2;
		uint32_t Is : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t load_store : 1;
		uint32_t writeBack_forceUserAccess : 1;
		uint32_t byte_word : 1;
		uint32_t up_down : 1;
		uint32_t preIndexed : 1;
		uint32_t immediate : 1;
		uint32_t mustbe1 : 2;
		uint32_t condition : 4;
	} offsetRegister;
	uint32_t opcode;
};

static_assert(sizeof(SingleDataTransferInstruction) == 4, "SingleDataTransferInstruction size is not 4");

union WordDataTransferInstruction {
	struct {
		uint32_t immediateOffset_lsb : 4;
		uint32_t mustbe1_bis : 1;
		uint32_t mem_opcode : 2;
		uint32_t mustbe1 : 1;
		uint32_t immediateOffset_msb : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t load_store : 1;
		uint32_t writeBack : 1; // must be 0 if preIndexed=0
		uint32_t immediate : 1;
		uint32_t upOffset : 1;
		uint32_t preIndexed : 1;
		uint32_t mustbe0 : 3;
		uint32_t condition : 4;
	} offsetImmediate;
	struct {
		uint32_t Rm : 4;
		uint32_t mustbe1_bis : 1;
		uint32_t mem_opcode : 2;
		uint32_t mustbe1 : 1;
		uint32_t mustbe0_bis : 4;
		uint32_t Rd : 4;
		uint32_t Rn : 4;
		uint32_t load_store : 1;
		uint32_t writeBack : 1; // must be 0 if preIndexed=0
		uint32_t immediate : 1;
		uint32_t upOffset : 1;
		uint32_t preIndexed : 1;
		uint32_t mustbe0 : 3;
		uint32_t condition : 4;
	} offsetRegister;
	uint32_t opcode;
};

static_assert(sizeof(WordDataTransferInstruction) == 4, "WordDataTransferInstruction size is not 4");

enum Condition : uint8_t {
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

enum ALUOpCode : uint8_t {
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

enum ShiftType : uint8_t {
	LSL = 0,	// Logical Shift Left
	LSR,		// Logical Shift Right
	ASR,		// Arithmetic Shift Right (sign bit is preserved)
	ROR			// Rotate Right
};

#pragma pack(pop)
