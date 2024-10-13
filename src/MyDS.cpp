// MyDS.cpp : définit le point d'entrée de l'application.
//

#include "MyDS.h"

using namespace std;

int main()
{	
	Cpu arm9;

	ARM9_mem arm9_mem;

	arm9.SetMMU(&arm9_mem);

	(void)arm9.SetBootAddr(0xFFFF0000);

	return 0;
}
