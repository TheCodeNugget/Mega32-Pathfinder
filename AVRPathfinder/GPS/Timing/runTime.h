/**************************************************************************
 File:		runTime.h
 About:		Include file for Run-Time Tracking using Timer0
 Author:	Kaan "The Nugget" Yýlmaz <https://github.com/TheCodeNugget>
 Software:	AVR-GCC / Atmel Studio
 Hardware:	Atmega32
 License:	GNU General Public License v3
***************************************************************************/

#ifndef RUNTIME_H_
#define RUNTIME_H_

#include <avr/io.h>
#include <avr/interrupt.h>

void runTimeInit();
uint64_t getrunTime();

#endif /* RUNTIME_H_ */