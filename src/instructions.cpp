#include "instructions.h"

bool IsConditionReserved(uint32_t opcode) {
	sInstruction* instruction = reinterpret_cast<sInstruction*>(&opcode);
	eCondition condition = static_cast<eCondition>(instruction->condition);

	return (condition == rsv);
}

bool IsDataProcImmShift(uint32_t opcode) {
	sDataProcImmShift* instruction = reinterpret_cast<sDataProcImmShift*>(&opcode);

	return (instruction->mustbe000 == 0) &&
		(instruction->mustbe0 == 0) &&
		(!IsConditionReserved(opcode));
}

bool IsDataProcRegShift(uint32_t opcode) {
	sDataProcRegShift* instruction = reinterpret_cast<sDataProcRegShift*>(&opcode);

	return (instruction->mustbe000 == 0) &&
		(instruction->mustbe0 == 0) &&
		(instruction->mustbe1 == 1) &&
		(!IsConditionReserved(opcode));
}

bool IsDataProcImm(uint32_t opcode) {
	sDataProcImm* instruction = reinterpret_cast<sDataProcImm*>(&opcode);

	return (instruction->mustbe001 == 1) && (!IsConditionReserved(opcode));
}

bool IsMoveImmToStatusReg(uint32_t opcode) {
	sMoveImmToStatusReg* instruction = reinterpret_cast<sMoveImmToStatusReg*>(&opcode);

	return (instruction->mustbe00110 == 0b00110) &&
		(instruction->mustbe10 == 0b10) &&
		(instruction->mustbe1111 == 0b1111) &&
		(!IsConditionReserved(opcode));
}

bool IsLoadStoreImmOffset(uint32_t opcode) {
	sLoadStoreImmOffset* instruction = reinterpret_cast<sLoadStoreImmOffset*>(&opcode);

	return (instruction->mustbe010 == 0b010) && (!IsConditionReserved(opcode));
}

bool IsLoadStoreRegOffset(uint32_t opcode) {
	sLoadStoreRegOffset* instruction = reinterpret_cast<sLoadStoreRegOffset*>(&opcode);

	return (instruction->mustbe011 == 0b011) &&
		(instruction->mustbe0 == 0) &&
		(!IsConditionReserved(opcode));
}

bool IsLoadStoreMultiple(uint32_t opcode) {
	sLoadStoreMultiple* instruction = reinterpret_cast<sLoadStoreMultiple*>(&opcode);

	return (instruction->mustbe100 == 0b100) && (!IsConditionReserved(opcode));
}

bool IsBranch(uint32_t opcode) {
	sBranchInstruction* instruction = reinterpret_cast<sBranchInstruction*>(&opcode);

	return (instruction->mustbe101 == 0b101) && (!IsConditionReserved(opcode));
}

bool IsCoprocLoadStore_DoubleRegTransf(uint32_t opcode) {
	sCoprocLoadStore_DoubleRegTransf* instruction = reinterpret_cast<sCoprocLoadStore_DoubleRegTransf*>(&opcode);

	return (instruction->mustbe110 == 0b110); // condition 0b1111 is UNPREDICTABLE pour <ARMv5
}

bool IsCoprocDataProc(uint32_t opcode) {
	sCoprocDataProc* instruction = reinterpret_cast<sCoprocDataProc*>(&opcode);

	return (instruction->mustbe1110 == 0b1110) &&
		(instruction->mustbe0 == 0); // condition 0b1111 is UNPREDICTABLE pour <ARMv5
}

bool IsCoprocRegTransf(uint32_t opcode) {
	sCoprocRegTransf* instruction = reinterpret_cast<sCoprocRegTransf*>(&opcode);

	return (instruction->mustbe1 == 1) &&
		(instruction->mustbe1110 == 0b1110); // condition 0b1111 is UNPREDICTABLE pour <ARMv5
}

bool IsSoftwareInterrupt(uint32_t opcode) {
	sSoftwareInterrupt* instruction = reinterpret_cast<sSoftwareInterrupt*>(&opcode);

	return (instruction->mustbe1111 == 0b1111) && (!IsConditionReserved(opcode));
}
