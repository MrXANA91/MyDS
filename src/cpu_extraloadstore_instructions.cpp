#include "Cpu.h"
#include "instructions.h"

#pragma region Decode
void Cpu::DecodeMultiplyOrExtraLoadStoreInstructions() {
	if (instruction.IsSwapInstruction()) {
		Rn = instruction.pSwapInstruction->Rn;
		Rd = instruction.pSwapInstruction->Rd;
		Rm = instruction.pSwapInstruction->Rm;

		instruction.SetDecode(INSTRUCT_SWAP);
	}
	else if (instruction.IsLoadStoreHalfwordRegOffset()) {
		Rn = instruction.pLoadStoreHalfwordRegOffset->Rn;
		Rd = instruction.pLoadStoreHalfwordRegOffset->Rd;
		Rm = instruction.pLoadStoreHalfwordRegOffset->Rm;

		instruction.SetDecode(INSTRUCT_LOAD_STORE_HALFWORD_REG_OFFSET);
	}
	else if (instruction.IsLoadStoreHalfwordImmOffset()) {
		Rn = instruction.pLoadStoreHalfwordImmOffset->Rn;
		Rd = instruction.pLoadStoreHalfwordImmOffset->Rd;
		Offset = (((uint8_t)instruction.pLoadStoreHalfwordImmOffset->HiOffset) << 4) | ((uint8_t)instruction.pLoadStoreHalfwordImmOffset->LoOffset);

		instruction.SetDecode(INSTRUCT_LOAD_STORE_HALFWORD_IMM_OFFSET);
	}
	//else if (instruction.IsLoadSignedHalfwordByteImmOffset()) {

	//}
	//else if (instruction.IsLoadSignedHalfwordByteRegOffset()) {

	//}
	//else if (instruction.IsLoadStoreDoublewordRegOffset() && (instructionSet == ARMv5_ARM9)) {	// ARMv5TE only

	//}
	//else if (instruction.IsLoadStoreDoublewordImmOffset() && (instructionSet == ARMv5_ARM9)) {	// ARMv5TE only

	//}
	else {
		instruction.SetDecode(INSTRUCT_NOP);
	}
}

bool Instruction::IsSwapInstruction() const {
	return (pSwapInstruction->mustbe00010 == 0b10) &&
		(pSwapInstruction->mustbe00 == 0) &&
		(pSwapInstruction->mustbe0 == 0) &&
		(pSwapInstruction->mustbe1001 == 0b1001);
}

bool Instruction::IsLoadStoreHalfwordRegOffset() const {
	return (pLoadStoreHalfwordRegOffset->mustbe000 == 0) &&
		(pLoadStoreHalfwordRegOffset->mustbe0 == 0) &&
		(pLoadStoreHalfwordRegOffset->mustbe0000 == 0) &&
		(pLoadStoreHalfwordRegOffset->mustbe1011 == 0b1011);
}

bool Instruction::IsLoadStoreHalfwordImmOffset() const {
	return (pLoadStoreHalfwordImmOffset->mustbe000 == 0) &&
		(pLoadStoreHalfwordImmOffset->mustbe1 == 1) &&
		(pLoadStoreHalfwordImmOffset->mustbe1011 == 0b1011);
}

#pragma endregion

#pragma region Execute
void Cpu::SwapInstruction(sSwapInstruction* instruction) {
	if (!IsConditionOK()) return;

	bool B_byte = instruction->B;

	uint8_t* ptr = memory->GetPointerFromAddr(Rn_value);
	uint32_t data = 0;
	if (B_byte) {
		data = *ptr;
		*ptr = (Rm_value & 0xFF);
	}
	else {
		data = memory->GetWordAtPointer(ptr);
		memory->SetWordAtPointer(ptr, Rm_value);
	}
	SetReg(Rd, data);
}

void Cpu::LoadStoreHalfwordRegOffset(sLoadStoreHalfwordImmOffset* instruction) {
	if (Rm == REG_PC) throw EXCEPTION_EXEC_MEM_REG_PC_UNAUTHORIZE;
	Offset = GetReg(Rm);

	LoadStoreHalfwordImmOffset(instruction);
}

void Cpu::LoadStoreHalfwordImmOffset(sLoadStoreHalfwordImmOffset* instruction) {
	if (!IsConditionOK()) return;

	bool P_offsetAddress = instruction->P;
	bool U_added = instruction->U;
	bool W_writeBack = instruction->W;
	bool L_load = instruction->L;

	uint32_t operandAddr = Rn_value;
	if (Rn == REG_PC) operandAddr += 4; // PC+8
	if (P_offsetAddress) {
		if (U_added) {
			operandAddr += Offset;
		}
		else {
			operandAddr -= Offset;
		}
		if (W_writeBack) SetReg(Rn, operandAddr);
	}

	uint8_t* startPtr = memory->GetPointerFromAddr(operandAddr);
	Operand = memory->GetWordAtPointer(startPtr);

	if (!P_offsetAddress) {
		if (U_added) {
			Operand += Offset;
		}
		else {
			Operand -= Offset;
		}
		// WriteBack always enabled
		SetReg(Rn, U_added ? Rn_value + Operand : Rn_value - Operand);
	}

	if (L_load) {
		SetReg(Rd, static_cast<uint16_t>(Operand));
	}
	else {
		if (Rd == REG_PC) Rd_value += 8; // PC+12

		memory->SetHalfWordAtPointer(startPtr, Rd_value);
	}

	if (false) {
		std::cout << "BLABLA";
	}
}

#pragma endregion
