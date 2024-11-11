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

/// <summary>
/// Instructions must already be DataProcImmShift or DataProcRegShift
/// </summary>
/// <returns></returns>
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

/// <summary>
/// Instructions must already be DataProcRegShift
/// </summary>
/// <returns></returns>
bool Instruction::IsMultipliesOrExtraLoadStore() const {
	return (pDataProcRegShift->mustbe0 == 1);
}

bool Instruction::IsDataProcImm() const {
	return (pDataProcImm->mustbe001 == 1) && (!IsConditionReserved());
}

/// <summary>
/// Instructions must already be DataProcImm
/// </summary>
/// <returns></returns>
bool Instruction::IsUndefined() const {
	return ((pDataProcImm->opcode & 0b1101) == 0b1000) && (pDataProcImm->S == 0);
}

bool Instruction::IsLoadStoreImmOffset() const {
	return (pLoadStoreImmOffset->mustbe010 == 0b010) && (!IsConditionReserved());
}

bool Instruction::IsLoadStoreRegOffset() const {
	return (pLoadStoreRegOffset->mustbe011 == 0b011) &&
		/*(instruction->mustbe0 == 0) &&*/
		(!IsConditionReserved());
}

/// <summary>
/// Instruction must be LoadStoreRegOffset
/// </summary>
/// <returns></returns>
bool Instruction::IsMedia() const {
	return (pLoadStoreRegOffset->mustbe0 == 1) && (!IsArchUndefined());
}

/// <summary>
/// Instruction must be LoadStoreRegOffset
/// </summary>
/// <returns></returns>
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
	pLoadStoreImmOffset = reinterpret_cast<sLoadStoreImmOffset*>(&opcode);
	pLoadStoreRegOffset = reinterpret_cast<sLoadStoreRegOffset*>(&opcode);
	pLoadStoreMultiple = reinterpret_cast<sLoadStoreMultiple*>(&opcode);
	pBranchInstruction = reinterpret_cast<sBranchInstruction*>(&opcode);
	pCoprocLoadStore_DoubleRegTransf = reinterpret_cast<sCoprocLoadStore_DoubleRegTransf*>(&opcode);
	pCoprocDataProc = reinterpret_cast<sCoprocDataProc*>(&opcode);
	pCoprocRegTransf = reinterpret_cast<sCoprocRegTransf*>(&opcode);
	pSoftwareInterrupt = reinterpret_cast<sSoftwareInterrupt*>(&opcode);

	pMoveStatusRegToReg = reinterpret_cast<sMoveStatusRegToReg*>(&opcode);
	pMoveRegToStatusReg = reinterpret_cast<sMoveRegToStatusReg*>(&opcode);
	pMoveImmToStatusReg = reinterpret_cast<sMoveImmToStatusReg*>(&opcode);
	pBranchExchangeThumb = reinterpret_cast<sBranchExchangeThumb*>(&opcode);
	pBranchExchangeJava = reinterpret_cast<sBranchExchangeJava*>(&opcode);
	pCountLeadingZeros = reinterpret_cast<sCountLeadingZeros*>(&opcode);
	pBranchLinkExchangeThumb = reinterpret_cast<sBranchLinkExchangeThumb*>(&opcode);
	pSaturatingAddSub = reinterpret_cast<sSaturatingAddSub*>(&opcode);
	pSoftwareBreakpoint = reinterpret_cast<sSoftwareBreakpoint*>(&opcode);
	pSignedMultiplies = reinterpret_cast<sSignedMultiplies*>(&opcode);

	pMultiplyInstruction = reinterpret_cast<sMultiplyInstruction*>(&opcode);
	pUnsignedMultiplyLong = reinterpret_cast<sUnsignedMultiplyLong*>(&opcode);
	pMultiplyLongInstruction = reinterpret_cast<sMultiplyLongInstruction*>(&opcode);

	pSwapInstruction = reinterpret_cast<sSwapInstruction*>(&opcode);
	pLoadStoreHalfwordRegOffset = reinterpret_cast<sLoadStoreHalfwordRegOffset*>(&opcode);
	pLoadStoreHalfwordImmOffset = reinterpret_cast<sLoadStoreHalfwordImmOffset*>(&opcode);
	pLoadSignedHalfwordByteImmOffset = reinterpret_cast<sLoadSignedHalfwordByteImmOffset*>(&opcode);
	pLoadSignedHalfwordByteRegOffset = reinterpret_cast<sLoadSignedHalfwordByteRegOffset*>(&opcode);
	pLoadStoreDoublewordRegOffset = reinterpret_cast<sLoadStoreDoublewordRegOffset*>(&opcode);
	pLoadStoreDoublewordImmOffset = reinterpret_cast<sLoadStoreDoublewordImmOffset*>(&opcode);

	pParallelAddSub = reinterpret_cast<sParallelAddSub*>(&opcode);
	pHalfwordPack = reinterpret_cast<sHalfwordPack*>(&opcode);
	pWordSaturate = reinterpret_cast<sWordSaturate*>(&opcode);
	pParallelHalfwordSaturate = reinterpret_cast<sParallelHalfwordSaturate*>(&opcode);
	pByteReverseWord = reinterpret_cast<sByteReverseWord*>(&opcode);
	pByteReversePackedHalfword = reinterpret_cast<sByteReversePackedHalfword*>(&opcode);
	pByteReverseSignedHalfword = reinterpret_cast<sByteReverseSignedHalfword*>(&opcode);
	pSelectBytes = reinterpret_cast<sSelectBytes*>(&opcode);
	pSignZeroExtend = reinterpret_cast<sSignZeroExtend*>(&opcode);
	pMultiplies_Type3 = reinterpret_cast<sMultiplies_Type3*>(&opcode);
	pUnsignedSumOfDifferences = reinterpret_cast<sUnsignedSumOfDifferences*>(&opcode);
	pUnsignedSumOfDifferencesAcc = reinterpret_cast<sUnsignedSumOfDifferencesAcc*>(&opcode);
	pUndefinedInstruction = reinterpret_cast<sUndefinedInstruction*>(&opcode);

	pChangeProcessorState = reinterpret_cast<sChangeProcessorState*>(&opcode);
	pSetEndianness = reinterpret_cast<sSetEndianness*>(&opcode);
	pCachePreload = reinterpret_cast<sCachePreload*>(&opcode);
	pSaveReturnState = reinterpret_cast<sSaveReturnState*>(&opcode);
	pReturnFromException = reinterpret_cast<sReturnFromException*>(&opcode);
	pBranchLinkChangeToThumb = reinterpret_cast<sBranchLinkChangeToThumb*>(&opcode);
	pAdditionalCoprocessorDoubleRegTransf = reinterpret_cast<sAdditionalCoprocessorDoubleRegTransf*>(&opcode);
}

void Instruction::Set(uint32_t opcode) {
	this->opcode = opcode;
}

const uint32_t Instruction::Get() const {
	return opcode;
}

void Instruction::DecodeReset() {
	SetDecode(INSTRUCT_NOP);
}

void Instruction::SetDecode(eInstructCode instruct) {
	decodedInstructCode = instruct;
}

const eInstructCode Instruction::GetDecode() const {
	return decodedInstructCode;
}

std::string Instruction::ToString() const {
	return "";
}