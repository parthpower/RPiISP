/*
	Programmer for InSystemProgramming (ISP) microcontrollers using Raspberry Pi.
	
	Author:			Parth Parikh
	Email:			parthpower@gmail.com
	Version:		0.0.3 (Alfa 3)
	Description:	Simple ISP programmer using Raspberry Pi SPI. Currently supporting AT89S51-52
	Reference:	AT89S51 Datasheet, BCM2835 Datasheet, and some SPI docs.	
*/

#include <stdio.h>
#include "isprog.h"

int main(int argc,char* argv[])
{
	if(argc<2)
	{
		printf("Usage %s Hexfile",argv[0]);
		return -1;
	}
	initProg();
	chipErase();
	FILE* hexfile = fopen(argv[1],"r");
	if(burnFile(hexfile)!=0)
		return -1;
	return 0;
}
