#include "Cpu.h"

#pragma region Debug

void Cpu::DisplayRegisters() {
	for (int i = 0; i < 16; i++) {
		uint32_t reg = GetReg(i);
		std::cout << "R" << i << " = 0x" << std::hex << reg << std::dec << " (" << reg << ")\n";
	}
}

std::string Cpu::eConditionToString(eCondition cond)
{
	switch (cond) {
	case EQ: return "Equal / zero";
	case NE: return "Not equal";
	case CS_HS: return "unsigned higher or same";
	case CC_LO: return "unsigned lower";
	case MI:	return "signed negative";
	case PL:	return "signed positive or zero";
	case VS:	return "signed overflow";
	case VC:	return "signed no overflow";
	case HI:	return "unsigned higher";
	case LS:	return "unsigned lower or same";
	case GE:	return "signed greater or equal";
	case LT:	return "signed less than";
	case GT:	return "signed greater than";
	case LE:	return "signed less or equal";
	case AL:	return "always";
	case rsv:	return "Reserved";
	default:
		return "UNKNOWN";
	}
}

std::string Cpu::eALUOpCodeToString(eALUOpCode aluOpcode) {
	switch (aluOpcode) {
	case AND:	return "AND";
	case EOR:	return "EOR";
	case SUB:	return "SUB";
	case RSB:	return "RSB";
	case ADD:	return "ADD";
	case ADC:	return "ADC";
	case SBC:	return "SBC";
	case RSC:	return "RSC";
	case TST:	return "TST";
	case TEQ:	return "TEQ";
	case CMP:	return "CMP";
	case CMN:	return "CMN";
	case ORR:	return "ORR";
	case MOV:	return "MOV";
	case BIC:	return "BIC";
	case MVN:	return "MVN";
	default:
		return "UNKNOWN";
	}
}

std::string Cpu::eShiftTypeToString(eShiftType shift) {
	switch (shift) {
	case LSL:	return "Logical Shift Left";
	case LSR:	return "Logical Shift Right";
	case ASR:	return "Arithmetic Shift Right";
	case ROR:	return "Rotate Right";
	default:
		return "UNKNOWN";
	}
}

std::string Cpu::eInstructCodeToString(eInstructCode instruct, std::string &othertext) {
	switch (instruct) {
	default:
	case INSTRUCT_NULL:
		return "INSTRUCT_NULL";
	case INSTRUCT_DATA_PROC_IMM_SHIFT:
		othertext = " - " + eALUOpCodeToString(aluOpcode);
		return "INSTRUCT_DATA_PROC_IMM_SHIFT";
	case INSTRUCT_DATA_PROC_REG_SHIFT:
		othertext = " - " + eALUOpCodeToString(aluOpcode);
		return "INSTRUCT_DATA_PROC_REG_SHIFT";
	case INSTRUCT_DATA_PROC_IMM:
		othertext = " - " + eALUOpCodeToString(aluOpcode);
		return "INSTRUCT_DATA_PROC_IMM";
	case INSTRUCT_MOVE_IMM_STATUS_REG:
		return "INSTRUCT_MOVE_IMM_STATUS_REG";
	case INSTRUCT_LOAD_STORE_IMM_OFFSET:
		return "INSTRUCT_LOAD_STORE_IMM_OFFSET";
	case INSTRUCT_LOAD_STORE_REG_OFFSET:
		return "INSTRUCT_LOAD_STORE_REG_OFFSET";
	case INSTRUCT_LOAD_STORE_MULTIPLE:
		return "INSTRUCT_LOAD_STORE_MULTIPLE";
	case INSTRUCT_BRANCH_BRANCHLINK:
		return "INSTRUCT_BRANCH_BRANCHLINK";
	case INSTRUCT_COPROC_LOAD_STORE_DOUBLE_REG_TRANSF:
		return "INSTRUCT_COPROC_LOAD_STORE_DOUBLE_REG_TRANSF";
	case INSTRUCT_COPROC_DATA_PROC:
		return "INSTRUCT_COPROC_DATA_PROC";
	case INSTRUCT_COPROC_REG_TRANSF:
		return "INSTRUCT_COPROC_REG_TRANSF";
	case INSTRUCT_SOFTWARE_INTERRUPT:
		return "INSTRUCT_SOFTWARE_INTERRUPT";
	}
}

#pragma endregion

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
	if (regID == 15) {
		SetPCReg(value);
		return;
	}

	if ((regID < 0) || (regID > 15)) throw EXCEPTION_REG_ACCESS_OUT_OF_RANGE;

	if ((regID < 8 || regID == 15)) {
		reg[regID] = value;
		if (Debug) std::cout << "R" << regID << " := " << value << "(0x" << std::hex << value << std::dec << ")\n";
		return;
	}
	// Here, regID is from 8 to 14

	if (GetCurrentCpuMode() == FIQ) {
		reg_fiq[regID - 8] = value;
		if (Debug) std::cout << "R" << regID << "(FIQ) := " << value << "(0x" << std::hex << value << std::dec << ")\n";
		return;
	}
	// Here, we are not in FIQ mode

	if ((regID != 13) && (regID != 14)) {
		reg[regID] = value;
		if (Debug) std::cout << "R" << regID << " := " << value << "(0x" << std::hex << value << std::dec << ")\n";
		return;
	}
	// Here, we want to access either SP or LR

	switch (GetCurrentCpuMode()) {
	case System:
	case User:
		reg[regID] = value;
		if (Debug) std::cout << "R" << regID << " := " << value << "(0x" << std::hex << value << std::dec << ")\n";
		break;
	case Supervisor:
		reg_svc[regID - 13] = value;
		if (Debug) std::cout << "R" << regID << "(SVC) := " << value << "(0x" << std::hex << value << std::dec << ")\n";
		break;
	case Abort:
		reg_abt[regID - 13] = value;
		if (Debug) std::cout << "R" << regID << "(ABT) := " << value << "(0x" << std::hex << value << std::dec << ")\n";
		break;
	case IRQ:
		reg_irq[regID - 13] = value;
		if (Debug) std::cout << "R" << regID << "(IRQ) := " << value << "(0x" << std::hex << value << std::dec << ")\n";
		break;
	case Undefined:
		reg_und[regID - 13] = value;
		if (Debug) std::cout << "R" << regID << "(UND) := " << value << "(0x" << std::hex << value << std::dec << ")\n";
		break;
	case FIQ:
		reg_fiq[regID - 8] = value;
		if (Debug) std::cout << "R" << regID << "(FIQ) := " << value << "(0x" << std::hex << value << std::dec << ")\n";
		break;
	default:
		throw EXCEPTION_REG_ACCESS_IN_UNKNWOWN_MODE;
	}
}

void Cpu::SetPCReg(uint32_t value) {
	reg[15] = value;
	if (Debug) std::cout << "PC := " << value << "(0x" << std::hex << value << std::dec << ")\n";

	// TODO : PC has been changed, fetch-decode-execute cycle must be reset
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

	memset(reg, 0, 16);
	memset(reg_fiq, 0, 7);
	memset(reg_svc, 0, 2);
	memset(reg_abt, 0, 2);
	memset(reg_und, 0, 2);
	memset(reg_irq, 0, 2);

	SetReg(REG_PC, bootAddress);
}

void Cpu::DebugStep() {
	if (started) return;

	Debug = true;
	step();
	Debug = false;
}

void Cpu::step() {
	Fetch();
	if (IsThumbMode()) {
		throw "NO THUMB YET";
	}
	else {
		Decode();
		Execute();
	}
}

void Cpu::runThreadFunc() {
	using namespace std::chrono;

	execInstr = 0;
	uint32_t pc = GetReg(REG_PC);
	start = high_resolution_clock::now();
	while (started) {
		if (breakpoint.Check(pc)) {
			started = false;
			break;
		}
		step();
		execInstr++;
		pc = GetReg(REG_PC);
	}
	end = high_resolution_clock::now();

	std::cout << "Stopping.\n";
	std::cout << "Executed " << execInstr << " instructions in " << duration_cast<microseconds>(end - start) << "\n";
}

void Cpu::Run() {
	started = true;

	// Start thread
	runThread = std::thread(&Cpu::runThreadFunc, this);
	runThread.detach();
}

void Cpu::Stop() {
	started = false;
}

void Cpu::Fetch() {
	uint8_t* ptr = memory->GetPointerFromAddr(GetReg(REG_PC));
	int opsize = (IsThumbMode()) ? 2 : 4;

	uint32_t fetchedInstruction = static_cast<uint32_t>(ARM_mem::GetBytesAtPointer(ptr, opsize));
	instruction.Set(fetchedInstruction);
	if (Debug) std::cout << "Instruction = 0x" << std::hex << fetchedInstruction << std::dec;
	if (Debug) std::cout << " - Condition : " << eConditionToString(static_cast<eCondition>(instruction.pInstruction->condition));
	if (Debug) std::cout << "\n";

	SetReg(REG_PC, GetReg(REG_PC) + opsize);
}

void Cpu::Decode() {
	instruction.DecodeReset();

	do {
		if (instruction.IsDataProcImmShift()) {
			if (instruction.IsMiscellaneous()) {

				break;
			}

			aluOpcode = static_cast<eALUOpCode>(this->instruction.pDataProcImmShift->opcode);
			SetFlags = this->instruction.pDataProcImmShift->S != 0;
			Rn = this->instruction.pDataProcImmShift->Rn;
			Rd = this->instruction.pDataProcImmShift->Rd;
			ShiftAmount = this->instruction.pDataProcImmShift->shiftAmount;
			Shift = static_cast<eShiftType>(this->instruction.pDataProcImmShift->shift);
			Rm = this->instruction.pDataProcImmShift->Rm;

			this->instruction.SetDecode(INSTRUCT_DATA_PROC_IMM_SHIFT);
		}
		else if (instruction.IsDataProcRegShift()) {
			if (instruction.IsMiscellaneous()) {

				break;
			}
			else if (instruction.IsMultipliesOrExtraLoadStore()) {

				break;
			}

			aluOpcode = static_cast<eALUOpCode>(this->instruction.pDataProcRegShift->opcode);
			SetFlags = this->instruction.pDataProcRegShift->S != 0;
			Rn = this->instruction.pDataProcRegShift->Rn;
			Rd = this->instruction.pDataProcRegShift->Rd;
			Rs = this->instruction.pDataProcRegShift->Rs;
			Shift = static_cast<eShiftType>(this->instruction.pDataProcRegShift->shift);
			Rm = this->instruction.pDataProcRegShift->Rm;

			this->instruction.SetDecode(INSTRUCT_DATA_PROC_REG_SHIFT);
		}
		else if (instruction.IsDataProcImm()) {
			if (instruction.IsUndefined()) {

				break;
			}
			else if (instruction.IsMoveImmToStatusReg()) {
				Mask = this->instruction.pMoveImmToStatusReg->Mask;
				Rotate = this->instruction.pMoveImmToStatusReg->rotate;
				Immediate = this->instruction.pMoveImmToStatusReg->immediate;

				this->instruction.SetDecode(INSTRUCT_MOVE_IMM_STATUS_REG);
				break;
			}

			aluOpcode = static_cast<eALUOpCode>(this->instruction.pDataProcImm->opcode);
			SetFlags = this->instruction.pDataProcImm->S != 0;
			Rn = this->instruction.pDataProcImm->Rn;
			Rd = this->instruction.pDataProcImm->Rd;
			Rotate = this->instruction.pDataProcImm->rotate;
			Immediate = this->instruction.pDataProcImm->immediate;

			this->instruction.SetDecode(INSTRUCT_DATA_PROC_IMM);
		}
		else if (instruction.IsLoadStoreImmOffset()) {
			Rn = this->instruction.pLoadStoreImmOffset->Rn;
			Rd = this->instruction.pLoadStoreImmOffset->Rd;
			Immediate = this->instruction.pLoadStoreImmOffset->immediate;

			this->instruction.SetDecode(INSTRUCT_LOAD_STORE_IMM_OFFSET);
		}
		else if (instruction.IsLoadStoreRegOffset()) {
			if (instruction.IsMedia()) {

				break;
			}
			else if (instruction.IsArchUndefined()) {

				break;
			}

			Rn = this->instruction.pLoadStoreRegOffset->Rn;
			Rd = this->instruction.pLoadStoreRegOffset->Rd;
			ShiftAmount = this->instruction.pLoadStoreRegOffset->shiftAmount;
			Shift = static_cast<eShiftType>(this->instruction.pLoadStoreRegOffset->shift);
			Rm = this->instruction.pLoadStoreRegOffset->Rm;

			this->instruction.SetDecode(INSTRUCT_LOAD_STORE_REG_OFFSET);
		}
		else if (instruction.IsLoadStoreMultiple()) {
			Rn = this->instruction.pLoadStoreMultiple->Rn;

			this->instruction.SetDecode(INSTRUCT_LOAD_STORE_MULTIPLE);
		}
		else if (instruction.IsBranch()) {
			Offset = this->instruction.pBranchInstruction->offset;

			this->instruction.SetDecode(INSTRUCT_BRANCH_BRANCHLINK);
		}
		else if (instruction.IsCoprocLoadStore_DoubleRegTransf()) {
			Rn = this->instruction.pCoprocLoadStore_DoubleRegTransf->Rn;
			Offset = this->instruction.pCoprocLoadStore_DoubleRegTransf->offset;

			this->instruction.SetDecode(INSTRUCT_COPROC_LOAD_STORE_DOUBLE_REG_TRANSF);
		}
		else if (instruction.IsCoprocRegTransf()) {
			this->instruction.SetDecode(INSTRUCT_COPROC_REG_TRANSF);
		}
		else if (instruction.IsCoprocRegTransf()) {
			this->instruction.SetDecode(INSTRUCT_COPROC_REG_TRANSF);
		}
		else if (instruction.IsSoftwareInterrupt()) {
			this->instruction.SetDecode(INSTRUCT_SOFTWARE_INTERRUPT);
		}
		else if (instruction.IsUnconditional()) {

		}
	} while (false);

	std::string othertext = "";
	if (Debug) std::cout << "Decoded instruction : " << eInstructCodeToString(this->instruction.GetDecode(), othertext) << othertext << "\n";
}

void Cpu::Execute() {
	try {
		Rd_value = GetReg(Rd);
		Rn_value = GetReg(Rn);
		Rm_value = GetReg(Rm);
		Rs_value = GetReg(Rs);
	}
	catch (int) {}

	uint32_t fetchedInstruction = this->instruction.Get();

	switch (this->instruction.GetDecode()) {
	default:
	case INSTRUCT_NULL:

		break;
	case INSTRUCT_DATA_PROC_IMM_SHIFT:
		DataProcImmShift();
		break;
	case INSTRUCT_DATA_PROC_REG_SHIFT:
		DataProcRegShift();
		break;
	case INSTRUCT_DATA_PROC_IMM:
		DataProcImm();
		break;
	//case INSTRUCT_MOVE_IMM_STATUS_REG:

		//break;
	case INSTRUCT_LOAD_STORE_IMM_OFFSET:
		LoadStoreImmOffset(this->instruction.pLoadStoreImmOffset);
		break;
	case INSTRUCT_LOAD_STORE_REG_OFFSET:
		LoadStoreRegOffset(this->instruction.pLoadStoreImmOffset);
		break;
	case INSTRUCT_LOAD_STORE_MULTIPLE:
		//LoadStoreMultiple(fetchedInstruction);
		break;
	case INSTRUCT_BRANCH_BRANCHLINK:
		Branch(this->instruction.pBranchInstruction);
		break;
	//case INSTRUCT_COPROC_LOAD_STORE_DOUBLE_REG_TRANSF:

	//	break;
	//case INSTRUCT_COPROC_DATA_PROC:

	//	break;
	//case INSTRUCT_COPROC_REG_TRANSF:

	//	break;
	//case INSTRUCT_SOFTWARE_INTERRUPT:

	//	break;
	}
}

bool Cpu::IsRunning() const {
	return started;
}

void Cpu::DisplayBreakpoints() {
	uint32_t addr{ 0 };

	int breakpointsNumber = breakpoint.GetSize();

	if (breakpointsNumber == 0) {
		std::cout << "No breakpoint\n";
		return;
	}

	for (int i = 0; i < breakpointsNumber; i++) {
		bool result = breakpoint.GetAddr(i, addr);
		std::cout << i << " - ";
		if (result) {
			std::cout << "at 0x" << std::hex << addr << std::dec << " - ";
		}
		else {
			std::cout << "(UNKNOWN ADDR) - ";
		}
		if (breakpoint.IsActive(i)) {
			std::cout << "ACTIVE";
		}
		else {
			std::cout << "INACTIVE";
		}
		if (i == 0) std::cout << " (parent)";
		std::cout << "\n";
	}
}

bool Cpu::SetBreakpoint(uint32_t address) {
	return breakpoint.Add(address);
}

bool Cpu::ToggleBreakpoint(int index) {
	bool active = breakpoint.IsActive(index);
	return breakpoint.SetActive(index, !active);
}

bool Cpu::RemoveBreakpoint(int index) {
	return breakpoint.Remove(index);
}

bool Cpu::IsConditionOK() const {
	eCondition condition = static_cast<eCondition>(this->instruction.pInstruction->condition);

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

bool Cpu::AluExecute(eALUOpCode alu_opcode, uint32_t &Rd, uint32_t Rn, uint32_t op2, bool setFlags) {
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

uint32_t Cpu::AluBitShift(eShiftType type, uint32_t base, uint32_t shift, bool setFlags, bool force) {
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

void Cpu::LoadStoreImmOffset(sLoadStoreImmOffset* instruction) {
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
