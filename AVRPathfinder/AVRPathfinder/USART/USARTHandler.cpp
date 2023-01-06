/**************************************************************************
 File:		USARTHandler.cpp
 About:		C++ file for Atmega32 UART/USART Handling Library
 Author:	Kaan "The Nugget" Yýlmaz <https://github.com/TheCodeNugget>
 Software:	AVR-GCC / Atmel Studio
 Hardware:	Atmega32
 License:	GNU General Public License v3
***************************************************************************/

#include "USARTHandler.h"

void USART_start() {									// USART initialize function
	UBRRL = (unsigned char) BAUD_PRESCALE;				// Set Baud Rate
	UBRRH = (unsigned char) (BAUD_PRESCALE >> 8);		// Set Baud Rate
	UCSRB |= (1 << RXEN) | (1 << TXEN) | (0 << RXCIE);	// Enable USART transmitter and receiver
	UCSRC |= (1 << URSEL)| (1 << UCSZ0) | (1 << UCSZ1);	// Write USCRC for 8 bit data and 1 stop bit (Modify For Device Needs)
}

char USART_receive() {				// Retrieves Char from USART-RX
	while (!(UCSRA & (1 << RXC)));	// Waits Until Receive Register Valid
	return(UDR);					// Returns Receive Register
}

void USART_transmit(char data) {	// Sends Char Over USART-TX
	UDR = data;						// Write data to be transmitting in UDR
	while (!(UCSRA & (1<<UDRE)));	// Wait until data transmit and buffer get empty
}

void USART_sendString(char *str) {	// Send string of USART data function
	int i=0;
	while (str[i]!=0) {
		USART_transmit(str[i]);		// Send each char of string till the NULL
		i++;
	}
}