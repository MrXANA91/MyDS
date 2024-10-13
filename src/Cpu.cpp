#include "Cpu.h"

Cpu::Cpu() {
	cpsr.bits.Mode = Supervisor;
	cpsr.bits.I = 1;
	cpsr.bits.F = 1;
	cpsr.bits.J = 0;
	cpsr.bits.T = 0;
}

bool Cpu::SetBootAddr(uint32_t bootAddr) {
	bootAddress = bootAddr;

	if (started) return false;
	reg[REG_PC] = bootAddr;
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
		spsr_svc = cpsr;
		break;
	case Abort:
		spsr_abt = cpsr;
		break;
	case IRQ:
		spsr_irq = cpsr;
		break;
	case Undefined:
		spsr_und = cpsr;
		break;
	case FIQ:
		spsr_fiq = cpsr;
		break;
	default:
		throw EXCEPTION_SPSR_UNKNOWN_MODE;
	}
}

void Cpu::Reset() {
	cpsr.bits.Mode = Supervisor;
	cpsr.bits.I = 1;
	cpsr.bits.F = 1;
	cpsr.bits.J = 0;
	cpsr.bits.T = 0;

	// Empty Fetch fifo
	// Empty Decode fifo
	// Empty Execute fifo

	reg[REG_PC] = bootAddress;
}

void Cpu::DebugStep() {
	Fetch();
	Decode();
	Execute();
}

void Cpu::Fetch() {

}

void Cpu::Decode() {

}

void Cpu::Execute() {

}