/**************************************************************************
 File:		runTime.cpp
 About:		C++ file for Run-Time Tracking using Timer0
 Author:	Kaan "The Nugget" Yýlmaz <https://github.com/TheCodeNugget>
 Software:	AVR-GCC / Atmel Studio
 Hardware:	Atmega32
 License:	GNU General Public License v3
***************************************************************************/

#define SREG _SFR_IO8(0x3f) // <-- NO Idea why this isn't included in AVR.h

#include "runTime.h"

uint64_t _runTime = 0;
uint16_t _1ms = 0;

void runTimeInit() {
	// interrupt setup
	// prescale timer0 to 1/64th the clock rate
	TCCR0 |= (1 << CS01) | (1 << CS00);
	// enable timer overflow interrupt
	TIMSK  |= 1 << TOIE0;
	sei();
}

ISR(TIMER0_OVF_vect) {
	uint8_t sregstore = SREG;
	_1ms += 256;
	while (_1ms > 1000) {
		_runTime++;
		_1ms -= 1000;
	}
	SREG = sregstore;
}

uint64_t getrunTime() {
	uint64_t time;
	cli(); //Stop Interrupts to get clear runtime reading
	time = _runTime;
	sei();
	return time;
}