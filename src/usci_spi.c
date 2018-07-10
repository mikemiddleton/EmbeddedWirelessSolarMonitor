//******************************************************************************
// usci_spi.c
// 
// By: Kenji Yamamoto
//     Dept. of Electrical Enigineering, CEFNS
//     Northern Arizona University
//
// Module for configuring and using the USCI for SPI. There is no read function
// as communication is always sync, the received data is passed back via
// pointer. See function description
//
//
//******************************************************************************

#include <msp430x22x4.h>

//////////////////////////////////////////////////////////////////////////////
// USCI_B0_SPI_Init()
//
// Initilizes the USCI B0 for 3 wire SPI mode
/////////////////////////////////////////////////////////////////////////////
void vUSCI_B0_SPI_Init()
{
	// Make sure USCI module is held in reset
	UCB0CTL1 |= UCSWRST;
	
	// Configure for 3-wire SPI, 8-bit word, phase = 1, polarity = 0,
	//  SPI master, MSB first, bit clock is SMCLK / 1
	UCB0CTL0 = UCCKPH | UCMSB | UCMST | UCMODE_0 | UCSYNC;
	UCB0CTL1 |= UCSSEL_2;
	UCB0BR0 = 0x01;
	UCB0BR1 = 0x00;
	
	// Configure peripheral pins for proper operation
	P3SEL |= (BIT1 | BIT2 | BIT3);
	P3DIR |= (BIT1 | BIT3);
	P3DIR &= ~BIT2;
	
	// Release from reset
	UCB0CTL1 &= ~UCSWRST;
}

//////////////////////////////////////////////////////////////////////////////
// USCI_B0_SPI_SendBytes( pucTX, pucRX, ucByteCount
//
// Sends bytes on the SPI; received bytes are stored in the
// ptrToRX. If ptrToRX is 0, then discard received data
//////////////////////////////////////////////////////////////////////////////
void vUSCI_B0_SPI_SendBytes( unsigned char * pucTX,
                             unsigned char * pucRX,
                             unsigned char ucByteCount )
{
	for ( ; ucByteCount > 0; --ucByteCount )
	{
		while( (IFG2 & UCB0TXIFG) != 0x08 );
		UCB0TXBUF = *pucTX;
		// Wait for both the TX flag and the RX flag
		while( (IFG2 & 0x0C) != 0x0C );
		if ( pucRX )
		{
		    while((UCB0STAT & 0x01)); // wait until tx is complete
		    						  // to proceed with rx
		    *pucRX = UCB0RXBUF;
			++pucRX;
		}
		++pucTX;
	}
}
