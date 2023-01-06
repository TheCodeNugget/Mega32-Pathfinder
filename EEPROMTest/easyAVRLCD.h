/*************************************************************************
 Title	:   include file for using LCD module on easyAVRv7
 Author:    Kaan "The Nugget" Yýlmaz <https://github.com/TheCodeNugget>
 File:	    easyAVRLCD.h
 Software:  AVR-GCC / Atmel Studio
 Hardware:  easyAVRv7 Dev Board
 License:   GNU Public License v3
***************************************************************************/

#ifndef LCD_H_
#define LCD_H_

//Define Keywords
#define RS 2 //RS Pin
#define E 6 //E Pin
#define D4 4 //Data-4 Pin
#define D5 5 //Data-5 Pin
#define D6 6 //Data-6 Pin
#define D7 7 //Data-7 Pin
#define RSP PORTA //RS Port
#define EP PORTD //E Port
#define DP PORTC //Data Port
#define RSD DDRA //RS Direction Register
#define ED DDRD //E Direction Register
#define DD DDRC //Data Direction Register

//Declare Functions
void LCDCmd(uint8_t); //Sends Command to LCD
void LCDChr(uint8_t); //Sends Char to LCD
void LCDInit(); //LCD Init
void pulseEnable(); //Pulses Enable Bit for Other Functions
void LCDClr(); //Clears LCD
void cursorHome(); //Moves Cursor Home
void moveCursor(uint8_t, uint8_t);
void cursorOff(); //Turns Cursor Off
void cursorOn(); //Turns Cursor On
void cursorBlink(); //Toggle Cursor Blink
void cursorLeft(uint8_t); //Move Cursor Left by given amount
void cursorRight(uint8_t); //Move Cursor Right by given amount
void displayLeft(uint8_t); //Shifts Display Left by given amount
void displayRight(uint8_t); //Shifts Display Right by given amount
void string2LCD(const char *s);

#endif /* LCD_H_ */