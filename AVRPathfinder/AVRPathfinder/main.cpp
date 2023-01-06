/*
 * tinygpsport.cpp
 *
 * Created: 03/01/2023 10:31:29
 * Author : Nugget
 */ 

#define F_CPU 8000000ul		// <-- Set this for the target MCU clock (in Hz) for timing purposes
#define SREG _SFR_IO8(0x3f) // <-- SREG Address

#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "GPS/TinyGPSPlus.h"	// <--NMEA Parser by Mikal Hart ported to Atmega32
#include "LCD/easyAVRLCD.h"		// <--LCD Library for easyAVRv7 dev board by MikroE
#include "USART/USARTHandler.h" // <-- USART Functions for Atmega32
#include "EEPROM/EEPROM.h"		//AVR EEPROM Library by Christopher Andrews

char lcdBuf[16];		// Buffer to Hold one line of chars for the LCD
bool working = false;	// Bool for switching device between Main Menu and Functions

TinyGPSPlus gps; //GPS Parsing Object

const char *gpsStream = //Pre-Recorded Data for Diagnostic purposes - Pull PinA7 high to switch to Diag Mode
"$GPGGA,103153.517,4112.430,N,02904.558,E,1,12,1.0,0.0,M,0.0,M,,*6E\r\n"
"$GPRMC,103153.517,A,4112.430,N,02904.558,E,,,060123,000.0,W*7C\r\n";

struct waypoint {	//Waypoint Structure for saving to EEPROM
	char name[6];	//Waypoint Name 5 Chars
	float lat, lng; //Waypoint Coordinates
};

void bootScreen();
void mainScreen();
void tndScreen();
void posScreen();
void saveLocation(double, double);
void navScreen();
void diagScreen();
uint8_t slotSelect();
static void readGPS();

int main(void) {
	GICR = (1 << INT2);		// INT2 Setup
	MCUCSR = (1 << ISC2);
	sei();
	
	runTimeInit();	// Start Runtime Tracker For GPS Parser
	LCDInit();		// Initialize LCD
	USART_start();	// Start USART
	bootScreen();	// Boot Sequence
	
	while(1) {
		mainScreen(); // Main Menu Always Active
	}
}


void bootScreen() { // Booting Screen
	moveCursor(0,0);
	string2LCD("AVR Pathfinder"); // Device Name
	moveCursor(0,1);
	string2LCD("Connecting...");
	while (!(gps.location.isValid())) { // Await Valid Data From GPS
		readGPS();
	}
	moveCursor(0,1);
	string2LCD("Connected    "); // Device Ready
	_delay_ms(1000);
}

void mainScreen() { // Main Screen of the Device
	// Menu Layout to LCD
	LCDClr();
	moveCursor(0,0);
	string2LCD("PB7:T&D  PB6:POS");
	moveCursor(0,1);
	string2LCD("PB5:NAV  PB4:Dx");
	
	// Awaits Button Press
	while (PINB == 0);
	LCDClr();
	working = true;			// Device Switches to functions
	switch (PINB) {			//Determine Pressed Pin
		case _BV(7):		//Time & Date Screen
		while (PINB != 0);	//Awaits Button Release
		tndScreen();
		break;
		
		case _BV(6): //Position Tracker Screen
		while (PINB != 0);
		posScreen();
		break;
		
		case _BV(5): //Navigation Screen
		while (PINB != 0);
		navScreen();
		break;

		case _BV(4): //Diagnostic Screen
		while (PINB != 0);
		diagScreen();
		break;
	}
}

void tndScreen () { // Time & Date Screen
	// Print Static Layout
	string2LCD("Time:");
	moveCursor(0,1);
	string2LCD("Date:");
	
	while (working) {
		if (gps.time.isValid()) { // Print Data if valid
			moveCursor(5,0);
			sprintf(lcdBuf, "%d:%d:%dUTC", gps.time.hour(), gps.time.minute(), gps.time.second());
			string2LCD(lcdBuf);
			moveCursor(5,1);
			sprintf(lcdBuf, "%d/%d/%d", gps.date.day(), gps.date.month(), gps.date.year());
			string2LCD(lcdBuf);
		}
		readGPS(); //Retrieve GPS Data
	}
}

void posScreen() { // Position Tracking Screen
	// Print Static Layout
	string2LCD("LAT:");
	moveCursor(0,1);
	string2LCD("LON:");
	
	while (working) {
		if (gps.location.isValid()) { // Print Data if valid
			moveCursor(4,0);
			sprintf(lcdBuf, "%f", gps.location.lat());
			string2LCD(lcdBuf);
			moveCursor(4,1);
			sprintf(lcdBuf, "%f", gps.location.lng());
			string2LCD(lcdBuf);
		}
		
		if (PINB == _BV(3)) { // Save Current Location to EEPROM
			saveLocation(gps.location.lat(), gps.location.lng());
		}
		
		readGPS(); //Retrieve GPS Data
	}
}

void saveLocation(double lat, double lng) {
	waypoint temp {
		"USWYP", // Name for User Saved Waypoints
		lat,
		lng
	};
	
	uint8_t addr = slotSelect();					// Select Save Slot
	if (!(addr == 20)) {							// 20 for Address Value Indicates Exit
		EEPROM.put(addr*sizeof(waypoint), temp);	// Write Waypoint to selected Slot
		LCDClr();
		string2LCD("Position Saved");
		_delay_ms(1000);
	}
	
	// Re-Print Static Layout for Position Screen
	LCDClr();
	string2LCD("LAT:");
	moveCursor(0,1);
	string2LCD("LON:");
}

void navScreen() { // Navigation Screen
	waypoint temp;
	double dist = 0;
	int addr = slotSelect(); // Select Destination from EEPROM
	if (addr == 20) return;
	EEPROM.get(addr*sizeof(waypoint), temp);
	
	// Print Static Layout
	LCDClr();
	sprintf(lcdBuf, "Dest:%s", temp.name);
	string2LCD(lcdBuf);
	
	while (working) {
		if (gps.location.isValid()) {
			dist = gps.distanceBetween (
			gps.location.lat(),
			gps.location.lng(),
			temp.lat,
			temp.lng
			);
			
			moveCursor(0,1);
			sprintf(lcdBuf, "%.3fKm %s",
			dist/ 1000,
			gps.cardinal(gps.courseTo (
			gps.location.lat(),
			gps.location.lng(),
			temp.lat,
			temp.lng
			)));
			string2LCD(lcdBuf);
		}
		readGPS();
		
		if (dist < 10) { // Destination Readched When within 10m of destination
			LCDClr();
			string2LCD("Destination");
			moveCursor(0,1);
			string2LCD("Reached");
			_delay_ms(1000);
			working = false;
		}
	}
}

void diagScreen() { // Screen For Diagnostic Data
	// Print Static Layout
	string2LCD("Sat:");
	moveCursor(7,0);
	string2LCD("HDOP:");
	moveCursor(0,1);
	string2LCD("ChksmFail:");
	
	while (working) {
		if (gps.satellites.isValid()) {
			moveCursor(4,0);
			sprintf(lcdBuf, "%lu", gps.satellites.value());
			string2LCD(lcdBuf);
			moveCursor(12,0);
			sprintf(lcdBuf, "%lu", gps.hdop.value());
			string2LCD(lcdBuf);
			moveCursor(10,1);
			sprintf(lcdBuf, "%lu", gps.failedChecksum());
			string2LCD(lcdBuf);
		}
		readGPS();
	}
}

uint8_t slotSelect() { // Selecting WP Slots in EEPROM - 20 Slots Available
	uint8_t addr = 0;
	waypoint temp;
	
	while (1) {
		// Print 1st Slot in EEPROM
		LCDClr();
		EEPROM.get(addr*sizeof(waypoint), temp);
		sprintf(lcdBuf, "SLT-%d:%s", addr + 1, temp.name);
		string2LCD(lcdBuf);
		sprintf(lcdBuf, "%.4f  %.4f", temp.lat, temp.lng);
		moveCursor(0,1);
		string2LCD(lcdBuf);
		
		// Navigation Buttons
		while (PINB == 0);
		switch (PINB) {
			case _BV(7): //Go Back
			while (PINB != 0);
			return 20;
			
			case _BV(6): //Slot Up
			while (PINB != 0);
			addr = (addr == 0) ? 19 : addr - 1;
			break;
			
			case _BV(5): //Slot Down
			while (PINB != 0);
			addr = (addr == 19) ? 0 : addr + 1;
			break;

			case _BV(4): //Select Slot
			while (PINB != 0);
			return addr;
			break;
		}
		while (PINB != 0);
	}
}

static void readGPS() {
	if ((PINA & 0b10000000) == 0b10000000) {
		while (!(gps.encode(*gpsStream++))); //Use Pre-Recorded Data
	} else {
		while(!(gps.encode((USART_receive()))) & working); //Retrive Data from GPS until Sentence Complete
	}
}

ISR(INT2_vect) { // Interrupt Vector for Returning to Main Menu
	uint8_t sregstore = SREG;
	while (PINB == 4); // Wait for pin Release
	working = false;
	SREG = sregstore;
}