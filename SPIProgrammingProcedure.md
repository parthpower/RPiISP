#SPIProgrammingProcedure
RST ---> H
SCK must be ---> L for 64 sys cycle after RST ---> H

#SPI CONFIG FOR BCM2835 lib
SCLK must be XTAL/16 at most, BCM2835_SPI_CLOCK_DIVIDER_65536 ---> ~3KHz to be safe and sound
CPHA=0 CPOL=0 ----> BCM2835_SPI_MODE0
MSB First ----> BCM2835_SPI_BIT_ORDER_MSBFIRST

#BEFORE DOING ANYTHING
First send Programming enable berfore doing anything.
Must erase before writing
4-byte for any instruction
