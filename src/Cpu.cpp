#include "Cpu.h"

void Cpu::PrintDebug() {
	std::cout << "PC = 0x" << std::hex << GetReg(REG_PC) << " ; ";
	std::cout << "Code = 0x" << std::hex << fetched_opcode << " ; ";
	std::cout << "R0 = 0x" << std::hex << GetReg(0) << " ; ";
	std::cout << "R1 = 0x" << std::hex << GetReg(1) << " ; ";
	std::cout << "R2 = 0x" << std::hex << GetReg(2) << " ; ";
	std::cout << "R3 = 0x" << std::hex << GetReg(3) << "\n";
}

Cpu::Cpu() {
	cpsr.value = 0;
	cpsr.bits.Mode = Supervisor;
	cpsr.bits.I = 1;
	cpsr.bits.F = 1;
}

bool Cpu::SetBootAddr(uint32_t bootAddr) {
	bootAddress = bootAddr;

	if (started) return false;
	SetReg(REG_PC, bootAddr);
	return true;
}

void Cpu::SetMMU(ARM_mem* ptr) {
	memory = ptr;
}

CpuMode Cpu::GetCurrentCpuMode() const {
	return (CpuMode)cpsr.bits.Mode;
}

uint32_t Cpu::GetReg(int regID) {
	if ((regID < 0) || (regID > 15)) throw EXCEPTION_REG_ACCESS_OUT_OF_RANGE;

	if ((regID < 8 || regID == 15)) return reg[regID];
	// Here, regID is from 8 to 14

	if (GetCurrentCpuMode() == FIQ) return reg_fiq[regID - 8];
	// Here, we are not in FIQ mode

	if ((regID != 13) && (regID != 14)) return reg[regID];
	// Here, we want to access either SP or LR

	switch (GetCurrentCpuMode()) {
	case System:
	case User:
		return reg[regID];
	case Supervisor:
		return reg_svc[regID - 13];
	case Abort:
		return reg_abt[regID - 13];
	case IRQ:
		return reg_irq[regID - 13];
	case Undefined:
		return reg_und[regID - 13];
	case FIQ:
		return reg_fiq[regID - 8];
	default:
		throw EXCEPTION_REG_ACCESS_IN_UNKNWOWN_MODE;
	}
}

void Cpu::SetReg(int regID, uint32_t value) {
	if ((regID < 0) || (regID > 15)) throw EXCEPTION_REG_ACCESS_OUT_OF_RANGE;

	if ((regID < 8 || regID == 15)) {
		reg[regID] = value;
		return;
	}
	// Here, regID is from 8 to 14

	if (GetCurrentCpuMode() == FIQ) {
		reg_fiq[regID - 8] = value;
		return;
	}
	// Here, we are not in FIQ mode

	if ((regID != 13) && (regID != 14)) {
		reg[regID] = value;
		return;
	}
	// Here, we want to access either SP or LR

	switch (GetCurrentCpuMode()) {
	case System:
	case User:
		reg[regID] = value;
		break;
	case Supervisor:
		reg_svc[regID - 13] = value;
		break;
	case Abort:
		reg_abt[regID - 13] = value;
		break;
	case IRQ:
		reg_irq[regID - 13] = value;
		break;
	case Undefined:
		reg_und[regID - 13] = value;
		break;
	case FIQ:
		reg_fiq[regID - 8] = value;
		break;
	default:
		throw EXCEPTION_REG_ACCESS_IN_UNKNWOWN_MODE;
	}
}

void Cpu::SaveCPSR() {
	switch (GetCurrentCpuMode()) {
	case System:
	case User:
		throw EXCEPTION_SPSR_MODE_IS_USER_OR_SYSTEM;
	case Supervisor:
		spsr_svc.value = cpsr.value;
		break;
	case Abort:
		spsr_abt.value = cpsr.value;
		break;
	case IRQ:
		spsr_irq.value = cpsr.value;
		break;
	case Undefined:
		spsr_und.value = cpsr.value;
		break;
	case FIQ:
		spsr_fiq.value = cpsr.value;
		break;
	default:
		throw EXCEPTION_SPSR_UNKNOWN_MODE;
	}
}

void Cpu::RestoreCPSR() {
	switch (GetCurrentCpuMode()) {
	case System:
	case User:
		throw EXCEPTION_SPSR_MODE_IS_USER_OR_SYSTEM;
	case Supervisor:
		cpsr.value = spsr_svc.value;
		break;
	case Abort:
		cpsr.value = spsr_abt.value;
		break;
	case IRQ:
		cpsr.value = spsr_irq.value;
		break;
	case Undefined:
		cpsr.value = spsr_und.value;
		break;
	case FIQ:
		cpsr.value = spsr_fiq.value;
		break;
	default:
		throw EXCEPTION_SPSR_UNKNOWN_MODE;
	}
}

void Cpu::Reset() {
	cpsr.value = 0;
	cpsr.bits.Mode = Supervisor;
	cpsr.bits.I = 1;
	cpsr.bits.F = 1;

	SetReg(REG_PC, bootAddress);
}

void Cpu::DebugStep() {
	Fetch();
	Decode();
	Execute();
}

void Cpu::Fetch() {
	uint8_t* ptr = memory->GetPointerFromAddr(GetReg(REG_PC));
	int opsize = (cpsr.bits.T == 0) ? 4 : 2;

	fetched_opcode = static_cast<uint32_t>(ARM_mem::GetBytesAtPointer(ptr, opsize));

	SetReg(REG_PC, GetReg(REG_PC) + opsize);
}

void Cpu::Decode() {
	if (IsBranch(fetched_opcode)) {
		decoded_func = 1;
	}
	else if (IsALU(fetched_opcode)) {
		decoded_func = 2;
	}
	else if (IsMemory(fetched_opcode)) {
		decoded_func = 3;
	}
	else {
		decoded_func = 0;
	}
}

void Cpu::Execute() {
	switch (decoded_func) {
	case 0:
		EXE_Nop(fetched_opcode);
		break;
	case 1:
		EXE_Branch(fetched_opcode);
		break;
	case 2:
		EXE_ALU(fetched_opcode);
		break;
	case 3:
		EXE_Memory(fetched_opcode);
		break;
	}
}

bool Cpu::IsConditionReserved(uint32_t opcode) const {
	Instruction* instruction = reinterpret_cast<Instruction*>(&opcode);
	Condition condition = static_cast<Condition>(instruction->condition);

	return (condition == rsv);
}

bool Cpu::IsConditionOK(uint32_t opcode) const {
	Instruction* instruction = reinterpret_cast<Instruction*>(&opcode);
	Condition condition = static_cast<Condition>(instruction->condition);

	switch (condition) {
	case EQ:
		return cpsr.bits.Z == 1;
	case NE:
		return cpsr.bits.Z == 0;
	case CS_HS:
		return cpsr.bits.C == 1;
	case CC_LO:
		return cpsr.bits.C == 0;
	case MI:
		return cpsr.bits.N == 1;
	case PL:
		return cpsr.bits.N == 0;
	case VS:
		return cpsr.bits.V == 1;
	case VC:
		return cpsr.bits.V == 0;
	case HI:
		return (cpsr.bits.C == 1) && (cpsr.bits.Z == 0);
	case LS:
		return (cpsr.bits.C == 0) || (cpsr.bits.Z == 1);
	case GE:
		return cpsr.bits.N == cpsr.bits.V;
	case LT:
		return cpsr.bits.N != cpsr.bits.V;
	case GT:
		return (cpsr.bits.Z == 0) && (cpsr.bits.N == cpsr.bits.V);
	case LE:
		return (cpsr.bits.Z == 1) || (cpsr.bits.N != cpsr.bits.V);
	case AL:
		return true;
	case rsv:
		return false; // TODO : exception ?
	default:
		throw EXCEPTION_CONDITION_UNKNOWN;
	}
}

bool Cpu::IsBranch(uint32_t opcode) {
	return IsBranch_B_BL(opcode) || IsBranch_BX_BLX(opcode);
}

bool Cpu::IsBranch_B_BL(uint32_t opcode) {
	BranchInstruction* instruction = reinterpret_cast<BranchInstruction*>(&opcode);
	return instruction->mustbe5 == 5;
}

bool Cpu::IsBranch_BX_BLX(uint32_t opcode) {
	BranchInstruction* instruction = reinterpret_cast<BranchInstruction*>(&opcode);
	return instruction->thumbSwitch.mustbe12FFF == 0x12FFF;
}

void Cpu::EXE_Branch(uint32_t opcode) {
	BranchInstruction* instruction = reinterpret_cast<BranchInstruction*>(&opcode);

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
}

bool Cpu::IsALU(uint32_t opcode) {
	ALUInstruction* instruction = reinterpret_cast<ALUInstruction*>(&opcode);
	return (instruction->secOpImm.mustbe0_2 == 0) || (instruction->secOpReg_shiftImm.mustbe0_2 == 0) ||
		((instruction->secOpReg_shiftReg.mustbe0_1 == 0)
			&& (instruction->secOpReg_shiftReg.mustbe0_2 == 0));
}

void Cpu::EXE_ALU(uint32_t opcode) {
	ALUInstruction* instruction = reinterpret_cast<ALUInstruction*>(&opcode);

	if (!IsConditionOK(instruction->opcode)) return;

	bool immediate = instruction->secOpImm.immediate;
	bool setConditionCodes = instruction->secOpImm.setFlags;
	ALUOpCode alu_opcode = (ALUOpCode)instruction->secOpImm.alu_opcode;

	int Rn = instruction->secOpImm.Rn;
	int Rd = instruction->secOpImm.Rd;
	uint32_t Rn_value = GetReg(Rn);
	uint32_t Rd_value = 0;

	if (Rn == REG_PC) {
		Rn_value += 4;
	}

	if (immediate) {
		int Is = instruction->secOpImm.rorIs;
		int nn = instruction->secOpImm.nn;
		uint32_t op2 = AluBitShift(ROR, nn, Is*2, setConditionCodes, true);
		if (AluExecute(alu_opcode, Rd_value, Rn_value, op2, setConditionCodes)) {
			if ((Rd == REG_PC) && (setConditionCodes)) {
				RestoreCPSR();
			}
			SetReg(Rd, Rd_value);
		}
		return;
	}

	bool shiftByRegister = instruction->secOpReg_shiftImm.R;
	if (shiftByRegister && (Rn == REG_PC)) Rn_value += 4;
	ShiftType shiftType = (ShiftType)instruction->secOpReg_shiftImm.shiftType;
	int Is = shiftByRegister ? (GetReg(instruction->secOpReg_shiftReg.Rs) & 0xFF) : ((instruction->secOpReg_shiftImm.Is) >> 7);
	int Rm = instruction->secOpReg_shiftImm.Rm;
	uint32_t Rm_value = GetReg(Rm);
	if (Rm == REG_PC) {
		Rm_value += 4;
		if (shiftByRegister) Rm_value += 4;
	}
	uint32_t op2 = AluBitShift(shiftType, Rm_value, Is, setConditionCodes);
	if (AluExecute(alu_opcode, Rd_value, Rn_value, op2, setConditionCodes)) {
		SetReg(Rd, Rd_value);
	}
}

bool Cpu::AluExecute(ALUOpCode alu_opcode, uint32_t &Rd, uint32_t Rn, uint32_t op2, bool setFlags) {
	uint32_t result = 0;
	bool updateRd = true;
	int arithmetic = 0;

	switch (alu_opcode) {
	case TST:
		updateRd = false;
		[[fallthrough]]; // fallthrough is explicit
	case AND:
		result = Rn & op2;
		break;
	case TEQ:
		updateRd = false;
		[[fallthrough]]; // fallthrough is explicit
	case EOR:
		result = Rn ^ op2;
		break;
	case CMP:
		updateRd = false;
		[[fallthrough]]; // fallthrough is explicit
	case SUB:
		arithmetic = -1;
		result = Rn - op2;
		break;
	case RSB:
		arithmetic = -2;
		result = op2 - Rn;
		break;
	case CMN:
		updateRd = false;
		[[fallthrough]]; // fallthrough is explicit
	case ADD:
		arithmetic = 1;
		result = Rn + op2;
		break;
	case ADC:
		arithmetic = 1;
		result = Rn + op2 + cpsr.bits.C;
		break;
	case SBC:
		arithmetic = -1;
		result = Rn - op2 + cpsr.bits.C - 1;
		break;
	case RSC:
		arithmetic = -2;
		result = op2 - Rn + cpsr.bits.C - 1;
		break;
	case ORR:
		result = Rn | op2;
		break;
	case MOV:
		result = op2;
		break;
	case BIC:
		result = Rn & ~op2;
		break;
	case MVN:
		result = ~op2;
		break;
	}

	if (setFlags) {
		if (arithmetic == 1) cpsr.bits.C = (result < Rn);
		if (arithmetic == -1) cpsr.bits.C = (result > Rn);
		if (arithmetic == -2) cpsr.bits.C = (result > op2);

		if (arithmetic != 0) {
			if (((Rn & 0x80000000) == (op2 & 0x80000000)) && ((result & 0x80000000) != (Rn & 0x80000000))) {
				cpsr.bits.V = 1;
			}
			else {
				cpsr.bits.V = 0;
			}
		}
		cpsr.bits.Z = result == 0 ? 1 : 0;
		cpsr.bits.N = (result & 0x80000000) != 0 ? 1 : 0;
	}

	if (updateRd) Rd = result;

	return updateRd;
}

uint32_t Cpu::AluBitShift(ShiftType type, uint32_t base, uint32_t shift, bool setFlags, bool force) {
	switch (type) {
	default:
		throw EXCEPTION_ALU_BITSHIFT_UNKNOWN_SHIFTTYPE;
	case LSL:
		if (setFlags) {
			cpsr.bits.C = ((base & (0x80000000 >> (shift - 1))) != 0) ? 1 : 0;
		}
		return base << shift;
	case LSR:
		if ((shift == 0) && (!force)) shift = 32;
		if (setFlags) {
			cpsr.bits.C = ((base & (1 << (shift - 1))) != 0) ? 1 : 0;
		}
		return base >> shift;
	case ASR:
		if ((shift == 0) && (!force)) shift = 32;
		if (setFlags) {
			cpsr.bits.C = ((base & (1 << (shift - 1))) != 0) ? 1 : 0;
		}
		return ((base >> shift) | (base & 0x80000000));
	case ROR:
		if ((shift == 0) && (!force)) {
			// ROR#0 interpreted as RRX#1 : REALLY ??
			// Rotates the number to the right by one place
			// but the original bit 31 is filled by the value of the Carry flag
			// and the original bit 0 is moved into the Carry flag
			uint32_t carry = cpsr.bits.C;
			if (setFlags) cpsr.bits.C = base & 0x1;
			return (base >> 1) | (carry << 31);
		}
		uint32_t left = base >> shift;
		uint32_t preserveMask = (0xFFFFFFFF >> (32 - shift));
		uint32_t right = (base & preserveMask) << (32 - shift);
		return (left) | (right);
	}
}

bool Cpu::IsMemory(uint32_t opcode) {
	SingleDataTransferInstruction* instr1 = reinterpret_cast<SingleDataTransferInstruction*>(&opcode);
	//WordDataTransferInstruction* instr2 = reinterpret_cast<WordDataTransferInstruction*>(&opcode);

	return (instr1->offsetImmediate.mustbe1 == 1);/* ||
		((instr2->offsetRegister.mustbe0 == 0) && (instr2->offsetRegister.mustbe0_bis == 0) &&
			(instr2->offsetRegister.mustbe1 == 1) && (instr2->offsetRegister.mustbe1_bis == 1));*/
}

void Cpu::EXE_Memory(uint32_t opcode) {
	SingleDataTransferInstruction* instruction = reinterpret_cast<SingleDataTransferInstruction*>(&opcode);

	bool immediate = instruction->offsetImmediate.immediate == 0;
	bool addOffsetPreIndex = instruction->offsetImmediate.preIndexed;
	bool addToBase = instruction->offsetImmediate.up_down;
	bool byteTransfer = instruction->offsetImmediate.byte_word;
	int size = byteTransfer ? 1 : 4;

	bool forceUserAccess = instruction->offsetImmediate.writeBack_forceUserAccess;
	bool writeBack = forceUserAccess;

	bool load = instruction->offsetImmediate.load_store;

	int Rn = instruction->offsetImmediate.Rn;
	int Rd = instruction->offsetImmediate.Rd;

	int immediateOffset = instruction->offsetImmediate.immediateOffset;

	int shiftAmount = instruction->offsetRegister.Is;
	ShiftType shiftType = static_cast<ShiftType>(instruction->offsetRegister.shiftType);
	bool isBit4Zero = instruction->offsetRegister.mustbe0;
	int Rm = instruction->offsetRegister.Rm;

	// Offset calculation
	uint32_t offset = 0;
	if (immediate) {
		offset = immediateOffset;
	}
	else {
		if (Rm == REG_PC) throw EXCEPTION_EXEC_MEM_REG_PC_UNAUTHORIZE;
		uint32_t Rm_value = GetReg(Rm);
		offset = AluBitShift(shiftType, Rm_value, shiftAmount, false);
	}

	// If Pre index
	uint32_t operandAddr = GetReg(Rn);
	if (Rn == REG_PC) operandAddr += 4; // PC+8
	if (addOffsetPreIndex) {
		if (addToBase) {
			operandAddr += offset;
		}
		else {
			operandAddr -= offset;
		}
		if (writeBack) {
			SetReg(Rn, operandAddr);
		}
	}
	uint8_t *startPtr = memory->GetPointerFromAddr(operandAddr);
	uint32_t operand = ARM_mem::GetWordAtPointer(startPtr);
	// If Post index
	if (!addOffsetPreIndex) {
		if (addToBase) {
			operand += offset;
		}
		else {
			operand -= offset;
		}
		// WriteBack always enabled
		ARM_mem::SetWordAtPointer(startPtr, operand);
	}

	// Execute...
	if (load) { // ... load
		// TODO : LDR PC, <op> sets CPSR.T bot <op> bit0 for ARMv5

		uint32_t Rd_value = GetReg(Rd);
		if (Rd == REG_PC) Rd_value += 8; // PC+12
		ARM_mem::SetWordAtPointer(startPtr, Rd_value);
	}
	else { // ... store
		SetReg(Rd, operand);
	}
}