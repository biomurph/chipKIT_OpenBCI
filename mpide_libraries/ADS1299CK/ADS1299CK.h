//
//  ADS1299CK.h
//  Part of the CHIPKIT Library
//  Created by Conor Russomanno, Luke Travis, and Joel Murphy. Summer 2013.
//	Modified by Joel in Fall 2013
//ADS1299CK
#ifndef ____ADS1299CK__
#define ____ADS1299CK__

#include <stdio.h>
#include "WProgram.h"#include <DSPI.h>
// #include "Definitions.h"  ERASE THIS

//#define  RESET_PIN 9
//#define  DRDY 8

//SPI Command Definition Byte Assignments (Datasheet, p35)
#define _WAKEUP 0x02 // Wake-up from standby mode
#define _STANDBY 0x04 // Enter Standby mode
#define _RESET 0x06 // Reset the device registers to default
#define _START 0x08 // Start and restart (synchronize) conversions
#define _STOP 0x0A // Stop conversion
#define _RDATAC 0x10 // Enable Read Data Continuous mode (default mode at power-up)
#define _SDATAC 0x11 // Stop Read Data Continuous mode
#define _RDATA 0x12 // Read data by command; supports multiple read back


//Register Addresses
#define ID 0x00
#define CONFIG1 0x01
#define CONFIG2 0x02
#define CONFIG3 0x03
#define LOFF 0x04
#define CH1SET 0x05
#define CH2SET 0x06
#define CH3SET 0x07
#define CH4SET 0x08
#define CH5SET 0x09
#define CH6SET 0x0A
#define CH7SET 0x0B
#define CH8SET 0x0C
#define BIAS_SENSP 0x0D
#define BIAS_SENSN 0x0E
#define LOFF_SENSP 0x0F
#define LOFF_SENSN 0x10
#define LOFF_FLIP 0x11
#define LOFF_STATP 0x12
#define LOFF_STATN 0x13
#define GPIO 0x14
#define MISC1 0x15
#define MISC2 0x16
#define CONFIG4 0x17

//#endif


class ADS1299CK {
	
//	DSPI *ADS;
	
public:

//	ADS1299CK (DSPI0 *a) : ADS(a) {}
      
    void initialize(int _DRDY, int _RST);
    
    //ADS1299 SPI Command Definitions (Datasheet, p35)
    //System Commands
    void WAKEUP();
    void STANDBY();
    void RESET();
    void START();
    void STOP();
    
    //Data Read Commands
    void RDATAC();
    void SDATAC();
    void RDATA();
    
    //Register Read/Write Commands
    byte getDeviceID();
    byte RREG(byte _address);
    void RREGS(byte _address, byte _numRegistersMinusOne);     
    void printRegisterName(byte _address);
    void WREG(byte _address, byte _value); 
    void WREGS(byte _address, byte _numRegistersMinusOne); 
    void printHex(byte _data);
    void updateChannelData();

//    int DRDY; 			// pin numbers for DRDY and CS 
    long stat;			// used to hold the status register
    byte regData [24];	// array is used to mirror register data
    long channelData [9];	// array used when reading channel data
    boolean verbose;		// turn on/off Serial feedback
    
    
};

#endif