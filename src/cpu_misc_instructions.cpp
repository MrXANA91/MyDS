#include "Cpu.h"
#include "instructions.h"

#pragma region Decode
void Cpu::DecodeMiscInstructions() {

}

bool Instruction::IsMoveStatusRegToReg() const {
	return false;
}

bool Instruction::IsMoveRegToStatusReg() const {
	return false;
}

/// <summary>
/// Instructions must already be DataProcImm
/// </summary>
/// <returns></returns>
bool Instruction::IsMoveImmToStatusReg() const {
	return (pMoveImmToStatusReg->mustbe00110 == 0b00110) &&
		(pMoveImmToStatusReg->mustbe10 == 0b10) &&
		(pMoveImmToStatusReg->mustbe1111 == 0b1111) &&
		(!IsConditionReserved());
}

bool Instruction::IsBranchExchangeThumb() const {
	return false;
}

bool Instruction::IsBranchExchangeJava() const {
	return false;
}

bool Instruction::IsCountLeadingZeros() const {
	return false;
}

bool Instruction::IsBranchLinkExchangeThumb() const {
	return false;
}

bool Instruction::IsSaturatingAddSub() const {
	return false;
}

bool Instruction::IsSoftwareBreakpoint() const {
	return false;
}

bool Instruction::IsSignedMultiplies() const {
	return false;
}
#pragma endregion

#pragma region Execute
void Cpu::MoveStatusRegToReg(sMoveStatusRegToReg* instruction) {

}

void Cpu::MoveRegToStatusReg(sMoveRegToStatusReg* instruction) {

}

void Cpu::MoveImmToStatusReg(sMoveImmToStatusReg* instruction) {

}

void Cpu::BranchExchangeThumb(sBranchExchangeThumb* instruction) {

}

void Cpu::BranchExchangeJava(sBranchExchangeJava* instruction) {

}

void Cpu::CountLeadingZeros(sCountLeadingZeros* instruction) {

}

void Cpu::BranchLinkExchangeThumb(sBranchLinkExchangeThumb* instruction) {

}
#pragma endregion
