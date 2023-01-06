/*************************************************************************
 Title	:   C file for using LCD module on easyAVRv7
 Author:    Kaan "The Nugget" Yýlmaz <https://github.com/TheCodeNugget>
 File:	    easyAVRLCD.C
 Software:  AVR-GCC / Atmel Studio
 Hardware:  easyAVRv7 Dev Board
 License:   GNU Public License v3
***************************************************************************/

#define F_CPU 16000000ul

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include "easyAVRLCD.h"

void LCDCmd(uint8_t cmd) {
	DP = (cmd & 0xf0) | (DP & 0x0f);
	pulseEnable();
	_delay_us(45);
	DP = ((cmd & 0x0f) << 4) | (DP & 0x0f);
	pulseEnable();
	_delay_us(45);
}

void LCDChr(uint8_t c) {
	RSP |= (1 << RS); //RS High for Char
	DP = (c & 0xf0) | (DP & 0x0f);
	pulseEnable();
	_delay_us(45);
	DP = ((c & 0x0f) << 4) | (DP & 0x0f);
	pulseEnable();
	RSP &= ~(1 << RS); //Clear RS on Done
	_delay_us(45);
}

void LCDInit() {
	//Clear Used Ports
	RSP &= ~(1 << RS);
	DP &= ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7));
	EP &= ~(1 << E);
	//Set Used Port Dirs
	RSD |= (1 << RS);
	DD |= ((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7));
	ED |= (1 <<E);
	
	//Send Boot Up Commands 4-Bit
	_delay_ms(20); //~15ms Powerup time
	
	DP = 0b00110000 | (DP & 0x0f); //Power On Controller
	pulseEnable();
	_delay_ms(5);
	
	DP = 0b00110000 | (DP & 0x0f); //4-Bit Init
	pulseEnable();
	_delay_us(110);
	
	DP = 0b00110000 | (DP & 0x0f); //4-Bit Init
	pulseEnable();
	_delay_us(110);
	
	DP = 0b00100000 | (DP & 0x0f); //Initialize Controller
	pulseEnable();
	_delay_us(110);
	
	LCDCmd(0x28); // 2-Line 5x8 display mode set
	
	LCDCmd(0x06); // Increment Mode, No Shift
	
	LCDCmd(0x0f);
	
	LCDClr(); // Clear LCD
}

void pulseEnable() {
	EP |= (1 << E);
	EP &= ~(1 << E);
}

void LCDClr() {
	LCDCmd(0x01);
	_delay_ms(2);
}

void cursorHome() {
	LCDCmd(0x02);
	_delay_ms(2);
}

void moveCursor(uint8_t x, uint8_t y) {
	register uint8_t DDRAMAddr;
	switch(y) {
		case 0:
		DDRAMAddr = x;
		break;
		case 1:
		DDRAMAddr = 0x40 + x;
		break;
		case 2:
		DDRAMAddr = 0x14 + x;
		break;
		case 3:
		DDRAMAddr = 0x56 + x;
		break;
		defaultWP:
		DDRAMAddr = x;
	}
	LCDCmd(1 << 7 | DDRAMAddr);
	_delay_us(40);
}

void cursorOff() {
	LCDCmd(0x0c);
	_delay_us(45);
}

void cursorOn() {
	LCDCmd(0x0e);
	_delay_us(45);
}

void cursorLeft(uint8_t n) {
	for (uint8_t i=0;i<n;i++) {
		LCDCmd(0x10);
		_delay_us(45);
	}
}

void cursorRight(uint8_t n) {
	for (uint8_t i=0;i<n;i++) {
		LCDCmd(0x14);
		_delay_us(45);
	}
}

void displayLeft(uint8_t n) {
	for (uint8_t i=0;i<n;i++) {
		LCDCmd(0x1E);
		_delay_us(45);
	}
}

void displayRight(uint8_t n) {
	for (uint8_t i=0;i<n;i++) {
		LCDCmd(0x18);
		_delay_us(45);
	}
}

void string2LCD(const char *s) {
	register char c;

	while ((c = *s++)) {
		LCDChr(c);
	}
}