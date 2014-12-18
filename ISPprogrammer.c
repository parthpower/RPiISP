/*
	Programmer for InSystemProgramming (ISP) microcontrollers using Raspberry Pi.
	
	Author:			Parth Parikh
	Email:			parthpower@gmail.com
	Version:		0.0.3 (Alfa 3)
	Description:	Simple ISP programmer using Raspberry Pi SPI. Currently supporting AT89S51-52
	Reference:	AT89S51 Datasheet, BCM2835 Datasheet, and some SPI docs.	
*/

/*To-Do: 12/17/2014
	->Checksum matching in parseline function.
	->More Error handling structure.
	->Add verbose
	->Revision Control
	->Add functions to Read Memory, Lock bits, Signature reading implementation.
	->Get rid of bcm2835 lib.
	->Make Library.
	->Auto-detect microcontroller controller.
	->AVR and other 8-bit ISP compatible microcontroller support
	->Make Parallel Programming interface.
	->4-byte sending instead of one at a time.
*/

#include <stdio.h>
#include <math.h> 
#include <string.h>
#include "bcm2835.h"

//Structure for soring the Intel Hex 
typedef struct {
	int addr;
	unsigned char count;
	unsigned char recordtype;
	unsigned char data[255];
	unsigned char checksum;
}hexline;
//Every function returns 0 on success and -1 on failure 

unsigned int strtohex(char*);					 //Convert hex string to equivalent value , the string must end with NULL char, maximum 2-byte output as unsigned int
int parseline(char*,hexline*); 					//Parse a record string of Intel Hex to the hexline structure
int sendline(hexline*);								 //Send single record uses writeByte function 
int writeByte(unsigned char,int); 				//Arguments: a byte to write (1-byte) unsigned char and address (2-byte) int to write on Flash.
unsigned char sendByte(unsigned char); 	//Send a single byte and returns the value from MISO. This function is for future improvements.
int initProg(); 											//Initialize the SPI, set the SCLK, SPI mode and send Program Enable signal, must call this before calling any of the function.
int burnFile(FILE*) 									//Parse the Hexfile and do all the things needed to burn it

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

unsigned int strtohex(char* in)
{
	unsigned int i=0,out=0,len=strlen(in);
	
	for(i=0;i<len;i++)
	{
		in[i] = (char)tolower(in[i]);
		if(in[i]>='a' && in[i]<='f')
			out = out + (in[i] - 'a'+10)*pow(16,len-1-i);
		else if(in[i]>='0' && in[i] <= '9')
			out = out + (in[i]-'0')*pow(16,len-1-i);
		else
			return -1;
	}
	return out;
}


int parseline(char* line,hexline* out)
{
	unsigned char tmp[5];
	unsigned char sum;
	
	int i;
	if(line[0]!=':')
		return -1;
	
	tmp[0]=line[1];	//First two bytes are Count, max count FFh = 255
	tmp[1]=line[2];
	tmp[3]='\0';		//Must add '\0' for strtohex to know where to stop
	
	out->count = (unsigned char)strtohex(tmp);  
	
	for(i=0;i<4;i++)
		tmp[i]=line[i+3];	//Second 2-bytes of starting page address (where only 12-bits are meaningful)
	tmp[i] = '\0';
	
	out->addr = strtohex(tmp); //For int (2-byte) no typecasting
	
	for(i=0;i<2;i++)
		tmp[i]=line[i+7];	//3rd byte is Record type
	tmp[i]='\0';
	
	out->recordtype = (unsigned char)strtohex(tmp); 
	
	for(i=0;i<(out->count);i++)	//Store the data till the end of count 
	{
		tmp[0]=line[i*2+9];	//Every data is 1 byte worth of 2 hex digits so move 2 step after a data is read
		tmp[1]=line[i*2+9+1];
		tmp[2]='\0';
		out->data[i] = (unsigned char)strtohex(tmp);
		sum += out->data[i]; //For checking checksum not used now to avoid complexity Future Proofing
	}
	
	tmp[0] = line[(out->count)*2+9]; //To get checksum location, 1byte = 2 hex digits 
	tmp[1] = line[(out->count)*2+9+1];
	tmp[2] = '\0';
	out->checksum = (unsigned char)strtohex(tmp);
	return 0;
}

int sendline(hexline* in) //Using pointer to reduce memory usage 
{
	int i;
	for(i=0;i<(in->count);i++)
	{
		writeByte(in->data[i],in->addr + i); //This can be done by simple loop even in main but a function is always portable and I personally love portable things :D
	}
}

int writeByte(unsigned char in,int addr)
{
	// Write On flash in Byte mode
	//Format : 0100-0000	    xxxx-A11 A10 A9 A8      A7 A6 A5 A4 - A3 A2 A1 A0     1-byte DATA
	
	sendByte(0x40);
	sendByte((unsigned char)(addr << 8)); //Higher byte first first nibble is don't care so just don't care.
	sendByte((unsigned char)(addr >> 8)); //
	sendByte(in);
	usleep(600); //It takes 0.5ms in write cycle so to be safe 0.6ms delay
}

unsigned char sendByte(unsigned char in)
{
	return  bcm2835_spi_transfer(in); //Future proofing
}

void chipErase()
{
	sendByte(0xac);
	sendByte(0x80);
	sendByte(0x00);
	sendByte(0x00);
	sleep(1);				//It takes time to erase flash
}
 
int initProg()
{
	if (!bcm2835_init())
        return -1;
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // Set MSB first
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // CPHA(clock phase)=0 CPOL(clock polarity) = 0
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536); // Lowest Speed for accurate writing 
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // Chip select-0 not in use but should be on to SPI work
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // CS is active low
	sleep(1);
	//Program Enable 
	sendByte(0xac);
	sendByte(0x53);
	sendByte(0x00);
	if(sendByte(0x00) != 0x69) //In last byte MISO returns 0x69 on sucess
		return -1;
	else
		return 0;
} 

int burnFile(FILE* hexfile)
{
	if(hexfile==NULL)	//You don't mess with null pointer or system dies like hell
		return -1;

	char linestr[550];	//Buffer to store record data, max size ':' + 2-digit count + 4-digit address + 2-digit record type + max(count = 255) * 2-digit = 510 digit + 2-digit checksum + '\0' = 524 +26 to avoid buffer overflow
	hexline rec;			//Buffer to store record structure

	while(fscanf(hexfile,"%s",linestr)!=EOF)	//Get each line till EOF reached
	{
		praseline(linestr,&rec);	//Parse the line and send it to processor
		sendline(&rec);
		if(in->recordtype == 0x01) //If final line to write
			break;
	}
	return 0;
}
