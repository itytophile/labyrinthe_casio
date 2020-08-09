#include "simpleTime.h"

//syscall GetTicks;
const static int SysCallCode[] = {0xD201422B,0x60F20000,0x80010070};
const static int (*SysCall)(int R4, int R5, int R6, int R7, int FNo ) = (void*)&SysCallCode;

int GetTicks() {
	return (*SysCall)(0, 0, 0, 0, 0x3B);
}

static unsigned int lastrandom=0x12345678;

unsigned int rand() {
	lastrandom = ( 0x41C64E6D * lastrandom ) + 0x3039;
	return (lastrandom >> 16);
}

void srand(int s) {
	lastrandom+=s;
}
