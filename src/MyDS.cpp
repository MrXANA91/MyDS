// MyDS.cpp : définit le point d'entrée de l'application.
//

#include "MyDS.h"

using namespace std;

int main()
{	
	Cpu testCpu;

	(void)testCpu.SetBootAddr(0xFFFF0000);

	return 0;
}
