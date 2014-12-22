//isprog.h
#ifndef ISPROG_H
#define ISPROG_H

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
int burnFile(FILE*);									//Parse the Hexfile and do all the things needed to burn it
void chipErase();									//Erase the flash
#endif