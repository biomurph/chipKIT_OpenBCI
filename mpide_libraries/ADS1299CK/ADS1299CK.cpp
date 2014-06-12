//
//  ADS1299CK.cpp   CHIPKIT LIBRARY FOR COMMUNICATING WITH ADS1299
//  
//  Created by Conor Russomanno, Luke Travis, and Joel Murphy. Summer, 2013
//  Modified by Joel in Fall 2013
//ADS1299CK


//#include "pins_arduino.h"
#include "ADS1299CK.h"
#include "DSPI.h"

DSPI0  ADS;  // DSPI0 is connected to 13,12,11 on UNO32 board

void ADS1299CK::initialize(int _DRDY, int _RST){
    int DRDY = _DRDY;
//    CS = _CS;
//	int FREQ = _FREQ;
	int RST = _RST;
	
		delay(50);				// recommended power up sequence requiers Tpor (~32m			
		pinMode(RST,OUTPUT);
		digitalWrite(RST,LOW);
		delayMicroseconds(4);	// toggle reset pin
		digitalWrite(RST,HIGH);
		delayMicroseconds(20);	// recommended to wait 18 Tclk before using device (~8uS);
		pinMode(DRDY, INPUT);


ADS.begin(10);    
ADS.setMode(DSPI_MODE1);
ADS.setSpeed(4000000);
    


}

//System Commands
void ADS1299CK::WAKEUP() {
    ADS.setSelect(LOW); 
    ADS.transfer(_WAKEUP);
    ADS.setSelect(HIGH); 
    delayMicroseconds(3);  		//must wait 4 tCLK cycles before sending another command (Datasheet, pg. 35)
}

void ADS1299CK::STANDBY() {		// only allowed to send WAKEUP after sending STANDBY
    ADS.setSelect(LOW);
    ADS.transfer(_STANDBY);
    ADS.setSelect(HIGH);
}

void ADS1299CK::RESET() {			// reset all the registers to default settings
    ADS.setSelect(LOW);
    ADS.transfer(_RESET);
    delayMicroseconds(12);   	//must wait 18 tCLK cycles to execute this command (Datasheet, pg. 35)
    ADS.setSelect(HIGH);
}

void ADS1299CK::START() {			//start data conversion 
    ADS.setSelect(LOW);
    ADS.transfer(_START);
    ADS.setSelect(HIGH);
}

void ADS1299CK::STOP() {			//stop data conversion
    ADS.setSelect(LOW);
    ADS.transfer(_STOP);
    ADS.setSelect(HIGH);
}

void ADS1299CK::RDATAC() {
    ADS.setSelect(LOW);
    ADS.transfer(_RDATAC);
    ADS.setSelect(HIGH);
	delayMicroseconds(3);   
}
void ADS1299CK::SDATAC() {
    ADS.setSelect(LOW);
    ADS.transfer(_SDATAC);
    ADS.setSelect(HIGH);
	delayMicroseconds(3);   //must wait 4 tCLK cycles after executing this command (Datasheet, pg. 37)
}


// Register Read/Write Commands
byte ADS1299CK::getDeviceID() {			// simple hello world com check
	byte data = RREG(0x00);
	if(verbose){						// verbose otuput
		Serial.print("Device ID ");
		printHex(data);	
	}
	return data;
}

byte ADS1299CK::RREG(byte _address) {		//  reads ONE register at _address
    byte opcode1 = _address + 0x20; 	//  RREG expects 001rrrrr where rrrrr = _address
    ADS.setSelect(LOW); 				//  open SPI
    ADS.transfer(opcode1); 					//  opcode1
    ADS.transfer(0x00); 					//  opcode2
    regData[_address] = ADS.transfer(0x00);//  update mirror location with returned byte
    ADS.setSelect(HIGH); 			//  close SPI	
	if (verbose){						//  verbose output
		printRegisterName(_address);
		printHex(_address);
		Serial.print(", ");
		printHex(regData[_address]);
		Serial.print(", ");
		for(byte j = 0; j<8; j++){
			Serial.print(bitRead(regData[_address], 7-j));
			if(j!=7) Serial.print(", ");
		}
		
		Serial.println();
	}
	return regData[_address];			// return requested register value
}

// Read more than one register starting at _address
void ADS1299CK::RREGS(byte _address, byte _numRegistersMinusOne) {
//	for(byte i = 0; i < 0x17; i++){
//		regData[i] = 0;					//  reset the regData array
//	}
    byte opcode1 = _address + 0x20; 	//  RREG expects 001rrrrr where rrrrr = _address
    ADS.setSelect(LOW); 				//  open SPI
    ADS.transfer(opcode1); 					//  opcode1
    ADS.transfer(_numRegistersMinusOne);	//  opcode2
    for(int i = 0; i <= _numRegistersMinusOne; i++){
        regData[_address + i] = ADS.transfer(0x00); 	//  add register byte to mirror array
		}
    ADS.setSelect(HIGH); 			//  close SPI
	if(verbose){						//  verbose output
		for(int i = 0; i<= _numRegistersMinusOne; i++){
			printRegisterName(_address + i);
			printHex(_address + i);
			Serial.print(", ");
			printHex(regData[_address + i]);
			Serial.print(", ");
			for(int j = 0; j<8; j++){
				Serial.print(bitRead(regData[_address + i], 7-j));
				if(j!=7) Serial.print(", ");
			}
			Serial.println();
		}
    }
    
}

void ADS1299CK::WREG(byte _address, byte _value) {	//  Write ONE register at _address
    byte opcode1 = _address + 0x40; 	//  WREG expects 010rrrrr where rrrrr = _address
    ADS.setSelect(LOW); 				//  open SPI
    ADS.transfer(opcode1);					//  Send WREG command & address
    ADS.transfer(0x00);						//	Send number of registers to read -1
    ADS.transfer(_value);					//  Write the value to the register
    ADS.setSelect(HIGH); 			//  close SPI
	regData[_address] = _value;			//  update the mirror array
	if(verbose){						//  verbose output
		Serial.print("Register ");
		printHex(_address);
		Serial.println(" modified.");
	}
}

void ADS1299CK::WREGS(byte _address, byte _numRegistersMinusOne) {
    byte opcode1 = _address + 0x40;		//  WREG expects 010rrrrr where rrrrr = _address
    ADS.setSelect(LOW); 				//  open SPI
    ADS.transfer(opcode1);					//  Send WREG command & address
    ADS.transfer(_numRegistersMinusOne);	//	Send number of registers to read -1	
	for (int i=_address; i <=(_address + _numRegistersMinusOne); i++){
		ADS.transfer(regData[i]);			//  Write to the registers
	}	
	ADS.setSelect(HIGH);				//  close SPI
	if(verbose){
		Serial.print("Registers ");
		printHex(_address); Serial.print(" to ");
		printHex(_address + _numRegistersMinusOne);
		Serial.println(" modified");
	}
}


void ADS1299CK::updateChannelData(){
	byte inByte;
	ADS.setSelect(LOW);				//  open SPI
	for(int i=0; i<3; i++){		//  read 24 bits of channel data in 8 3 byte chunks
		inByte = ADS.transfer(0x00);
		stat = (stat<<8) | inByte;			//  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
	}
	for(int i = 0; i<8; i++){
		for(int j=0; j<3; j++){		//  read 24 bits of channel data in 8 3 byte chunks
			inByte = ADS.transfer(0x00);
			channelData[i] = (channelData[i]<<8) | inByte;
		}
	}
	ADS.setSelect(HIGH);				//  close SPI
	
	for(int i=0; i<8; i++){			// convert 3 byte 2's compliment to 4 byte 2's compliment	
		if(bitRead(channelData[i],23) == 1){	
			channelData[i] |= 0xFF000000;
		}else{
			channelData[i] &= 0x00FFFFFF;
		}
	}
//	if(verbose){
//		Serial.print(stat); Serial.print(", ");
//		for(int i=0; i<8; i++){
//			Serial.print(channelData[i]);
//			if(i<7){Serial.print(", ");}
//		}
//		Serial.println();
//	}
}
	



void ADS1299CK::RDATA() {					//  use in Stop Read Continuous mode when DRDY goes low
	byte inByte;						//  to read in one sample of the channels
    ADS.setSelect(LOW);				//  open SPI
    ADS.transfer(_RDATA);					//  send the RDATA command
	for(int i=0; i<3; i++){		//  read 24 bits of channel data in 8 3 byte chunks
			inByte = ADS.transfer(0x00);
			stat = (stat<<8) | inByte;			//  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
		}
	for(int i = 0; i<8; i++){
		for(int j=0; j<3; j++){		//  read in the status register and new channel data
			inByte = ADS.transfer(0x00);
			channelData[i] = (channelData[i]<<8) | inByte;
		}
	}
	ADS.setSelect(HIGH);				//  close SPI
	
	for(int i=0; i<8; i++){
		if(bitRead(channelData[i],23) == 1){	// convert 3 byte 2's compliment to 4 byte 2's compliment
			channelData[i] |= 0xFF000000;
		}else{
			channelData[i] &= 0x00FFFFFF;
		}
	}
    
}


// String-Byte converters for RREG and WREG
void ADS1299CK::printRegisterName(byte _address) {
    if(_address == ID){
        Serial.print("ID, "); //the "F" macro loads the string directly from Flash memory, thereby saving RAM
    }
    else if(_address == CONFIG1){
        Serial.print("CONFIG1, ");
    }
    else if(_address == CONFIG2){
        Serial.print("CONFIG2, ");
    }
    else if(_address == CONFIG3){
        Serial.print("CONFIG3, ");
    }
    else if(_address == LOFF){
        Serial.print("LOFF, ");
    }
    else if(_address == CH1SET){
        Serial.print("CH1SET, ");
    }
    else if(_address == CH2SET){
        Serial.print("CH2SET, ");
    }
    else if(_address == CH3SET){
        Serial.print("CH3SET, ");
    }
    else if(_address == CH4SET){
        Serial.print("CH4SET, ");
    }
    else if(_address == CH5SET){
        Serial.print("CH5SET, ");
    }
    else if(_address == CH6SET){
        Serial.print("CH6SET, ");
    }
    else if(_address == CH7SET){
        Serial.print("CH7SET, ");
    }
    else if(_address == CH8SET){
        Serial.print("CH8SET, ");
    }
    else if(_address == BIAS_SENSP){
        Serial.print("BIAS_SENSP, ");
    }
    else if(_address == BIAS_SENSN){
        Serial.print("BIAS_SENSN, ");
    }
    else if(_address == LOFF_SENSP){
        Serial.print("LOFF_SENSP, ");
    }
    else if(_address == LOFF_SENSN){
        Serial.print("LOFF_SENSN, ");
    }
    else if(_address == LOFF_FLIP){
        Serial.print("LOFF_FLIP, ");
    }
    else if(_address == LOFF_STATP){
        Serial.print("LOFF_STATP, ");
    }
    else if(_address == LOFF_STATN){
        Serial.print("LOFF_STATN, ");
    }
    else if(_address == GPIO){
        Serial.print("GPIO, ");
    }
    else if(_address == MISC1){
        Serial.print("MISC1, ");
    }
    else if(_address == MISC2){
        Serial.print("MISC2, ");
    }
    else if(_address == CONFIG4){
        Serial.print("CONFIG4, ");
    }
}


// Used for printing HEX in verbose feedback mode
void ADS1299CK::printHex(byte _data){
	Serial.print("0x");
    if(_data < 0x10) Serial.print("0");
    Serial.print(_data, HEX);
}

//-------------------------------------------------------------------//
//-------------------------------------------------------------------//
//-------------------------------------------------------------------//



