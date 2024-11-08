#include "instructions.h"

bool Instruction::IsConditionReserved() const {
	eCondition condition = static_cast<eCondition>(pInstruction->condition);

	return (condition == rsv);
}

bool Instruction::IsDataProcImmShift() const {
	return (pDataProcImmShift->mustbe000 == 0) &&
		(pDataProcImmShift->mustbe0 == 0) &&
		(!IsConditionReserved());
}

bool Instruction::IsMiscellaneous() const {
	return ((pDataProcImmShift->opcode & 0b1100) == 0b1000) && (pDataProcImmShift->S == 0) &&
		((((pDataProcImmShift->shiftAmount & 0x1) == 0) && (pDataProcImmShift->mustbe0 == 1)) || (pDataProcImmShift->mustbe0 == 0));
}

bool Instruction::IsDataProcRegShift() const {
	return (pDataProcRegShift->mustbe000 == 0) &&
		/*(instruction->mustbe0 == 0) &&*/
		(pDataProcRegShift->mustbe1 == 1) &&
		(!IsConditionReserved());
}

bool Instruction::IsMultipliesOrExtraLoadStore() const {
	return (pDataProcRegShift->mustbe0 == 1);
}

bool Instruction::IsDataProcImm() const {
	return (pDataProcImm->mustbe001 == 1) && (!IsConditionReserved());
}

bool Instruction::IsUndefined() const {
	return ((pDataProcImm->opcode & 0b1101) == 0b1000) && (pDataProcImm->S == 0);
}

bool Instruction::IsMoveImmToStatusReg() const {
	return (pMoveImmToStatusReg->mustbe00110 == 0b00110) &&
		(pMoveImmToStatusReg->mustbe10 == 0b10) &&
		(pMoveImmToStatusReg->mustbe1111 == 0b1111) &&
		(!IsConditionReserved());
}

bool Instruction::IsLoadStoreImmOffset() const {
	return (pLoadStoreImmOffset->mustbe010 == 0b010) && (!IsConditionReserved());
}

bool Instruction::IsLoadStoreRegOffset() const {
	return (pLoadStoreRegOffset->mustbe011 == 0b011) &&
		/*(instruction->mustbe0 == 0) &&*/
		(!IsConditionReserved());
}

bool Instruction::IsMedia() const {
	return (pLoadStoreRegOffset->mustbe0 == 1) && (!IsArchUndefined());
}

bool Instruction::IsArchUndefined() const {
	return (pLoadStoreRegOffset->P == 1) && (pLoadStoreRegOffset->U == 1) && (pLoadStoreRegOffset->B == 1) && (pLoadStoreRegOffset->W == 1) && (pLoadStoreRegOffset->L == 1) &&
		((pLoadStoreRegOffset->shiftAmount&0x1)==1) && (pLoadStoreRegOffset->shift == 0b11) && (pLoadStoreRegOffset->mustbe0 == 1);
}

bool Instruction::IsLoadStoreMultiple() const {
	return (pLoadStoreMultiple->mustbe100 == 0b100) && (!IsConditionReserved());
}

bool Instruction::IsBranch() const {
	return (pBranchInstruction->mustbe101 == 0b101) && (!IsConditionReserved());
}

bool Instruction::IsCoprocLoadStore_DoubleRegTransf() const {
	return (pCoprocLoadStore_DoubleRegTransf->mustbe110 == 0b110); // condition 0b1111 is UNPREDICTABLE for ARMv4
}

bool Instruction::IsCoprocDataProc() const {
	return (pCoprocDataProc->mustbe1110 == 0b1110) &&
		(pCoprocDataProc->mustbe0 == 0); // condition 0b1111 is UNPREDICTABLE for ARMv4
}

bool Instruction::IsCoprocRegTransf() const {
	return (pCoprocRegTransf->mustbe1 == 1) &&
		(pCoprocRegTransf->mustbe1110 == 0b1110); // condition 0b1111 is UNPREDICTABLE for ARMv4
}

bool Instruction::IsSoftwareInterrupt() const {
	return (pSoftwareInterrupt->mustbe1111 == 0b1111) && (!IsConditionReserved());
}

bool Instruction::IsUnconditional() const {
	return IsConditionReserved();
}

Instruction::Instruction() {
	pInstruction = reinterpret_cast<sInstruction*>(&opcode);
	pDataProcImmShift = reinterpret_cast<sDataProcImmShift*>(&opcode);
	pDataProcRegShift = reinterpret_cast<sDataProcRegShift*>(&opcode);
	pDataProcImm = reinterpret_cast<sDataProcImm*>(&opcode);
	pMoveImmToStatusReg = reinterpret_cast<sMoveImmToStatusReg*>(&opcode);
	pLoadStoreImmOffset = reinterpret_cast<sLoadStoreImmOffset*>(&opcode);
	pLoadStoreRegOffset = reinterpret_cast<sLoadStoreRegOffset*>(&opcode);
	pLoadStoreMultiple = reinterpret_cast<sLoadStoreMultiple*>(&opcode);
	pBranchInstruction = reinterpret_cast<sBranchInstruction*>(&opcode);
	pCoprocLoadStore_DoubleRegTransf = reinterpret_cast<sCoprocLoadStore_DoubleRegTransf*>(&opcode);
	pCoprocDataProc = reinterpret_cast<sCoprocDataProc*>(&opcode);
	pCoprocRegTransf = reinterpret_cast<sCoprocRegTransf*>(&opcode);
	pSoftwareInterrupt = reinterpret_cast<sSoftwareInterrupt*>(&opcode);
}

void Instruction::Set(uint32_t opcode) {
	this->opcode = opcode;
}

const uint32_t Instruction::Get() const {
	return opcode;
}

void Instruction::DecodeReset() {
	SetDecode(INSTRUCT_NULL);
}

void Instruction::SetDecode(eInstructCode instruct) {
	decodedInstructCode = instruct;
}

const eInstructCode Instruction::GetDecode() const {
	return decodedInstructCode;
}