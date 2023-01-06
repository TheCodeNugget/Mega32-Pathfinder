/**************************************************************************
 File:		EEPROMLoader.cpp
 About:		C++ file for Loading Waypoint Data to Atmega32 Pathfinder
 Author:	Kaan "The Nugget" Yýlmaz <https://github.com/TheCodeNugget>
 Software:	AVR-GCC / Atmel Studio
 Hardware:	Atmega32
 License:	GNU General Public License v3
***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "easyAVRLCD.h"
#include "EEPROM.h"

struct waypoint {
	char name[6];
	float lat, lng;
	};
	
waypoint defaultWP {
	"EMPTY",
	0,
	0
};

int main(void) {
    for (int i = 0; i < 20; i++) {
		EEPROM.put(i*sizeof(waypoint), defaultWP);
	}
}

