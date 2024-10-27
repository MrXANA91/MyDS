#include "breakpoints.h"

void Breakpoint::init(uint32_t addr) {
	this->address = addr;
	this->active = true;
	this->initialised = true;
	next = nullptr;
	parent = nullptr;
}

bool Breakpoint::Check(uint32_t currentAddr) {
	if (currentAddr == address) {
		if (active && initialised) {
			active = false;
			return true;
		}
	}

	if (next != nullptr) return next->Check(currentAddr);

	return false;
}

bool Breakpoint::Add(uint32_t addr) {
	if (next != nullptr) return next->Add(addr);

	if (address == addr) {
		active = true;
		initialised = true;
		return true;
	}

	if (initialised == false) {
		init(addr);
		return true;
	}

	Breakpoint* ptr = static_cast<Breakpoint*>(malloc(sizeof(Breakpoint)));
	if (ptr == nullptr) return false;

	ptr->init(addr);

	next = ptr;
	ptr->parent = this;

	return true;
}

bool Breakpoint::Remove(int index) {
	if (index == 0) {
		if (initialised == false) return false;

		if ((parent == nullptr) && (next == nullptr)) {
			initialised = false;
			active = false;
			return true;
		}

		if (parent == nullptr) {
			Breakpoint* temp = next;
			active = next->active;
			address = next->address;

			next = next->next;
			free(temp);
			return true;
		}

		if (next == nullptr) {
			parent->next = nullptr;
			free(this);
			return true;
		}

		parent->next = next;
		next->parent = parent;
		free(this);
		return true;
	}

	if ((index > 0) && (next != nullptr)) {
		return next->Remove(index - 1);
	}

	return false;
}

bool Breakpoint::IsActive(int index) const {
	if ((index == 0) && initialised) return active;

	if ((index > 0) && (next != nullptr) && initialised) return next->IsActive(index - 1);

	return false;
}

bool Breakpoint::SetActive(int index, bool enable) {
	if ((index == 0) && initialised) {
		active = enable;
		return true;
	}

	if ((index > 0) && (next != nullptr) && initialised) return next->SetActive(index - 1, enable);

	return false;
}

int Breakpoint::GetSize() const {
	int size = 0;
	if (next != nullptr) size += next->GetSize();
	if (initialised == true) size += 1;
	return size;
}

bool Breakpoint::GetAddr(int index, uint32_t &addr) const {
	if ((index == 0) && initialised) {
		addr = address;
		return true;
	}

	if ((index > 0) && (next != nullptr) && initialised) return next->GetAddr(index - 1, addr);

	return false;
}
