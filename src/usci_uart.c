// *************************************************************************************
// File  : usci_uart.c
// Author: Karl Groll, Eric Hammerstrom. Help from Kenji Yamamoto
// Date  : March 02, 2009
//
// Uses Universal Serial Communication Interfaces (USCI) to communicate between the 
//  eZ430 development tool and the host PC. This file implements the functionality for 
//  the three functions defined in 'usci_uart.h'
// The functions allow initialization of the MSP430 USCI registers, clearing of the
//  receive buffer, and the sending of data from eZ430 to PC.
// *************************************************************************************

#include <msp430x22x4.h>

unsigned char g_ucaUSCI_A0_RXBuffer[0x100];
unsigned char g_ucUSCI_A0_RXBufferIndex;

void vUSCI_A0_UART_Init()
{
    UCA0CTL1 |= UCSWRST;   //Hold USCI_A0 in reset
    UCA0CTL0 = 0x00;       //Set up UCACTL0: Async UART mode, 
                           // no parity, LSB first, 8-bit char 
                           // length, 1 stop bit
   
    UCA0CTL1 |= UCSSEL_2;  // Set source to SMCLK
    UCA0CTL1 &= ~UCRXEIE;  // Disable interrupts on RX error
    UCA0CTL1 &= ~UCBRKIE;  // Disable interrupts on break character
    UCA0CTL1 &= ~UCDORM;   // Not dormant
    UCA0CTL1 &= ~UCTXADDR; // Next frame is data (not address)
    UCA0CTL1 &= ~UCTXBRK;  // Next frame is data (not break)
    
	// Baud rate = SMCLK / 0x01A0 = ~9600
	// See Table 15-4 MSP430x2xx Family User's Guide (0x01A0=416)
	UCA0BR1 = 0x01;
	UCA0BR0 = 0xA0;
	UCA0MCTL |= UCBRS_6;	
	UCA0MCTL &= ~UCOS16;
	
	UCA0STAT = 0x00;       // No loopback mode and clear status register
	UCA0IRTCTL &= ~UCIREN; // No IrDA encoding
	UCA0IRRCTL &= ~UCIRRXFE;
	UCA0ABCTL &= ~UCABDEN; // No auto-baud rate detection
	
	P3SEL |= (BIT4 | BIT5);// Configure pins
    g_ucUSCI_A0_RXBufferIndex = 0x00; // Set RX index to 0x00
    UCA0CTL1 &= ~UCSWRST;  // Release from reset
	IE2 |= UCA0RXIE;       // Enable RX interrupt	
}

void vUSCI_A0_UART_ClearRXBuffer(unsigned char ucWipeData)
{
    for(g_ucUSCI_A0_RXBufferIndex = 0; g_ucUSCI_A0_RXBufferIndex < 0xFF; ++g_ucUSCI_A0_RXBufferIndex)
    {
        g_ucaUSCI_A0_RXBuffer[g_ucUSCI_A0_RXBufferIndex] = ucWipeData;
    }
    g_ucUSCI_A0_RXBufferIndex = 0x000;
}

/////////////////////////////////////////////////////////////////////////
// USCI_A0_UART_SendBytes( pucData: * uint8, unCount : uint16)
//
// Send unCount bytes pointed to by pucData on USCI_A0
/////////////////////////////////////////////////////////////////////////
void vUSCI_A0_UART_SendBytes( unsigned char *pucData, unsigned int unCount )
{
	for (; unCount > 0; --unCount)
	{
		while (!(IFG2 & UCA0TXIFG));
		UCA0TXBUF = *pucData;
		++pucData;
	}
}
