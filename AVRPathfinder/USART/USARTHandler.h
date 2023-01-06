/**************************************************************************
 File:		USARTHandler.h
 About:		Include file for Atmega32 UART/USART Handling Library
 Author:	Kaan "The Nugget" Yýlmaz <https://github.com/TheCodeNugget>
 Software:	AVR-GCC / Atmel Studio
 Hardware:	Atmega32
 License:	GNU General Public License v3
***************************************************************************/

#ifndef USARTHANDLER_H_
#define USARTHANDLER_H_

#define F_CPU 8000000ul
#define BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (BAUDRATE * 16UL))) - 1)

#include <avr/io.h>

void USART_start();				// USART initialize function
char USART_receive();			// Data receiving function
void USART_transmit(char);		// Data transmitting function
void USART_sendString(char*);	// Send string of USART data function

#endif /* USARTHANDLER_H_ */