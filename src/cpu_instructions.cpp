#include "Cpu.h"

void Cpu::DataProcImmShift() {
	if (!IsConditionOK()) return;

	if (Rn == REG_PC) Rn_value += 4;
	if (Rm == REG_PC) Rm_value += 4;

	Operand = AluBitShift(Shift, Rm_value, ShiftAmount, SetFlags);

	if (AluExecute(aluOpcode, Rd_value, Rn, Operand, SetFlags)) {
		SetReg(Rd, Rd_value);
	}
}

void Cpu::DataProcRegShift() {
	ShiftAmount = Rs_value & 0xFF;
	if (Rm == REG_PC) Rm_value += 4;

	DataProcImmShift();
}

void Cpu::DataProcImm() {
	if (!IsConditionOK()) return;

	if (Rn == REG_PC) Rn_value += 4;

	Operand = AluBitShift(ROR, Immediate, Rotate * 2, SetFlags, true);

	if (AluExecute(aluOpcode, Rd_value, Rn_value, Operand, SetFlags)) {
		if ((Rd == REG_PC) && (SetFlags)) {
			RestoreCPSR();
		}
		SetReg(Rd, Rd_value);
	}
}

void Cpu::LoadStoreImmOffset(sLoadStoreImmOffset* instruction) {
	if (!IsConditionOK()) return;

	bool P_preindexed = instruction->P;
	bool U_add = instruction->U;
	bool B_byte = instruction->B;
	bool W_writeBack = instruction->W;		// if P = 1
	bool W_userMemAccess = instruction->W;	// if P = 0
	bool L_load = instruction->L;

	int size = B_byte ? 1 : 4;

	// If Pre index
	uint32_t operandAddr = Rn_value;
	if (Rn == REG_PC) operandAddr += 4; // PC+8
	if (P_preindexed) {
		if (U_add) {
			operandAddr += Immediate;
		}
		else {
			operandAddr -= Immediate;
		}
		if (W_writeBack) SetReg(Rn, operandAddr);
	}

	if (!P_preindexed && W_userMemAccess) {
		// TODO : Check if memory is User accessible
	}
	uint8_t* startPtr = memory->GetPointerFromAddr(operandAddr);
	Operand = static_cast<uint32_t>(ARM_mem::GetBytesAtPointer(startPtr, size));

	// If Post index
	if (!P_preindexed) {
		if (U_add) {
			Operand += Immediate;
		}
		else {
			Operand -= Immediate;
		}
		// WriteBack always enabled
		ARM_mem::SetWordAtPointer(startPtr, Operand);
	}

	// Execute...
	if (L_load) {	// ... load
		// TODO : LDR PC, <op> sets CPSR.T <op> bit0 (LSB) for ARMv5

		SetReg(Rd, B_byte ? static_cast<uint8_t>(Operand) : Operand);
	}
	else {			// ... store
		if (Rd == REG_PC) Rd_value += 8; // PC+12

		if (B_byte) {
			*startPtr = static_cast<uint8_t>(Rd_value);
		}
		else {
			ARM_mem::SetWordAtPointer(startPtr, Rd_value);
		}
	}
}

void Cpu::LoadStoreRegOffset(sLoadStoreImmOffset* instruction) {
	if (Rm == REG_PC) throw EXCEPTION_EXEC_MEM_REG_PC_UNAUTHORIZE;
	Immediate = AluBitShift(Shift, Rm_value, ShiftAmount, false);

	LoadStoreImmOffset(instruction);
}

void Cpu::LoadStoreMultiple(sLoadStoreMultiple* instruction) {
	if (!IsConditionOK()) return;

	bool P_excluded = instruction->P;
	bool U_upward = instruction->U;
	bool S_CPSRfromSPSR = instruction->S;	// if (Load and Rn == REG_PC)
	bool S_useUserReg = instruction->S;		// if (Load and Rn != REG_PC) or (Store)
	bool W_writeBack = instruction->W;
	bool L_load = instruction->L;

	Rn_value = GetReg(Rn);
	if (P_excluded) {
		if (U_upward) {
			Rn_value += 4;
		}
		else {
			Rn_value -= 4;
		}
	}

	uint32_t bitShift = 1;
	CpuMode forceCpuModeAccess = Current;
	for (int i = (U_upward ? 0 : 15); i < (U_upward ? 15 : 0); (U_upward ? i++ : i--)) {
		uint8_t* ptr = memory->GetPointerFromAddr(Rn_value);

		if ((instruction->registerList & bitShift) != 0) {
			if (L_load) {
				// Load
				if (S_CPSRfromSPSR && (i == REG_PC)) {
					RestoreCPSR();
				}
				else if (S_useUserReg && (i != REG_PC)) {
					forceCpuModeAccess = User;
				}
				SetReg(i, memory->GetWordAtPointer(ptr), forceCpuModeAccess);
			}
			else {
				// Store
				if (S_useUserReg) {
					forceCpuModeAccess = User;
				}
				memory->SetWordAtPointer(ptr, GetReg(i, forceCpuModeAccess));
			}

			if (U_upward) {
				Rn_value += 4;
			}
			else {
				Rn_value -= 4;
			}
		}

		bitShift << 1;
	}

	if (W_writeBack) {
		SetReg(Rn, Rn_value);
	}
}

/*void Cpu::EXE_Branch(uint32_t opcode) {
	sBranchInstruction* instruction = reinterpret_cast<sBranchInstruction*>(&opcode);

	uint32_t oldPC = GetReg(REG_PC); // Here, REG_PC has already been incremented by 4
	if (IsBranch_B_BL(instruction->opcode)) {
		int32_t offset = static_cast<int32_t>(instruction->nn);
		if ((offset & 0x00800000) != 0) offset += 0xFF000000;
		uint32_t newPC = oldPC + 4 + offset * 4;
		if (IsConditionReserved(instruction->opcode)) {
			// ARM9 ONLY : BLX_imm, branch with link and thumb
			bool halfword_offset = instruction->b_opcode;
			newPC += (halfword_offset ? 2 : 0);
			SetReg(REG_PC, newPC);
			SetReg(REG_LR, oldPC);
			cpsr.bits.T = 1;
			return;
		}

		if (!IsConditionOK(instruction->opcode)) return;

		if (instruction->b_opcode != 0) {
			// branch with link
			SetReg(REG_LR, oldPC);
		}
		SetReg(REG_PC, newPC);
	}
	else if (IsBranch_BX_BLX(instruction->opcode)) {

		if (!IsConditionOK(instruction->opcode)) return;

		uint8_t bx_opcode = instruction->thumbSwitch.bx_opcode;
		int regID = instruction->thumbSwitch.Rn;
		if ((bx_opcode == 0x1) || (bx_opcode == 0x2)) {
			// BX (or Jazelle, but Jazelle not supported so behaving like BX)
			uint32_t Rn = GetReg(regID);
			SetReg(REG_PC, Rn);
			cpsr.bits.T = Rn & 0x1;
		}
		else if (bx_opcode == 0x3) {
			// BX with link
			uint32_t Rn = GetReg(regID);
			SetReg(REG_PC, Rn);
			cpsr.bits.T = Rn & 0x1;
			SetReg(REG_LR, oldPC);
		}
	}
	else {
		throw EXCEPTION_EXEC_BRANCH_DECODE_FAILURE;
	}
}*/

void Cpu::Branch(sBranchInstruction* instruction) {
	if (!IsConditionOK()) return;

	int32_t signedOffset = Offset;
	uint32_t oldPC = GetReg(REG_PC); // Here, REG_PC has already been incremented by 4
	if ((signedOffset & 0x00800000) != 0) signedOffset += 0xFF000000;
	uint32_t newPC = oldPC + 4 + signedOffset * 4;

	if (instruction->L != 0) {
		// Branch with Link
		SetReg(REG_LR, oldPC);
	}
	SetReg(REG_PC, newPC);
}