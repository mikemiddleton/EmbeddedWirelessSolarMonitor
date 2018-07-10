//******************************************************************************
// Project 7
// An MSP430 connected to a CLIO board (REMOTE) with a solar panel samples the
// photovoltaic output from the solar panel, converts the analog signal to
// digital with the ADC10, then sends the data wireless with the CC2500
// on-board to another MSP430 (BASE). The BASE MSP430 is connected to a computer
// via USB, then reports information over UART to a Python UART polling program
//
// eZ430-RF2500
//
// Description:
//
// Bryce Carey and Michael Middleton
// Northern Arizona University
// Built with CCE for MSP430 V5.2
// 10 Dec 2012
//******************************************************************************

#include "msp430x22x4.h"
#include <stdint.h>
#include <stdlib.h>
#include "usci_spi.h"
#include "usci_uart.h"
#include "cc2500.h"
#include "eZ430-RF2500_LED.h"

//******************************************************************************
// Global variables
//******************************************************************************

// Holds value from ADC10MEM - split in two
unsigned char g_ucaRXInput[2];

// The value that is retrieved from the ADC10MEM - only ten bits
unsigned int g_uiSolar;

// Flag for whether or not the device is receiving or sending data with CC2500
unsigned char g_ucRXFlag = 0;



//******************************************************************************
// Define MSP430 functionality: BASE or REMOTE
//******************************************************************************

#define BASE
//#define REMOTE


//******************************************************************************
// Main Function
//******************************************************************************

void main(void)
{
    // Stop watch dog timer
    WDTCTL = WDTPW + WDTHOLD;

    // DCO = 16 MHz and calibrated with BCSCTL1 register
    DCOCTL = CALDCO_16MHZ;
    BCSCTL1 = CALBC1_16MHZ;

	// 16 MHz MCLK -- 4 MHz SMCLK
    BCSCTL2 = SELM_0 | DIVM_0 | DIVS_2;

    // Set up red and green LEDs
    P1DIR |= (BIT0 | BIT1);

    // Clear LEDs
    P1OUT &= 0;

	// Initialize CC2500
    vCC2500_Init();

	// Write CC2500 registers with correct configurations for transmission
    vCC2500_LoadProfile(0);

	// Set transmission power
    vCC2500_SetTXPower(POS_01_DBM);

	// Idle the CC2500 to begin accepting SRX/STX strobes
    ucCC2500_SendCommandStrobe(SIDLE);

    // Select pins for digital I/O
	P2SEL &= ~(BIT6);

	// P2.6 as an input
	P2DIR &= ~BIT6;

	// Selects interrupt edge with P2.6
	P2IES |= BIT6;

	// Enable interrupt for P2.6
	P2IE |= BIT6;

	// Sets bit when selected transition has been detected on the input
	P2IFG &= ~BIT6;

    // Enable general interrupts
    __bis_SR_register(GIE);

    // Clear the receiver FIFO with strobe command
	ucCC2500_SendCommandStrobe(SFRX);

	// Clear the transmit FIFO with strobe command
	ucCC2500_SendCommandStrobe(SFTX);

    // Sets the channel for transmission to 131 (13th independent channel in classroom hopefully)
    ucCC2500_WriteSingleRegister(CHANNR, 0x83);


    // If BASE is defined, the following code is executed
	#ifdef BASE

    	// Initialize UART communication on for BASE once; Not needed for REMOTE
        vUSCI_A0_UART_Init();

        // Loop continues forever
        while(1)
		{
				// Clear the receiver buffer with strobe command
				ucCC2500_SendCommandStrobe(SFRX);

				// Enable interrupt for P2.6
				P2IE |= BIT6;

				// Sets bit when selected transition has been detected on the input
				P2IFG &= ~BIT6;

				// Set receive flag for BASE to receive input from REMOTE
				g_ucRXFlag = 1;

				// Enable CC2500 packet TX interrupt
				P2IE |= BIT6;

				// Command the CC250 to transmit the string – STX strobe
				ucCC2500_SendCommandStrobe(SRX);

				// Wait for finish (sleep)
				__bis_SR_register( LPM3_bits + GIE );

				// Enable packet RXinterrupt
				P2IE  |=  BIT6;

				// Received the packet
				P2IE  &=  ~BIT6;

				// Read the 2 bytes from the receive buffer (RX_FIFO)
				ucCC2500_BurstReadRegisters(RX_FIFO, g_ucaRXInput, 2);

				// Light red LED
				LED_FLASH(RED_LED);

				// Light green LED
				LED_FLASH(GREEN_LED);

				// Send data to Python UART polling program
				vUSCI_A0_UART_SendBytes(g_ucaRXInput, 2);

				// Clear the receiver buffer with strobe command
				ucCC2500_SendCommandStrobe(SFRX);

				// Enable interrupts once again
				P2IE |= BIT6;
    	}

        // BASE code ends
		#endif

    // If REMOTE is defined, the following code is executed
	#ifdef REMOTE

				// Use the VLO for clock
				BCSCTL3 |= LFXT1S_2; // VLO used

				// Set Capture/Compare Control Register 1
				TACCTL0 |= CCIE;        // PWM mode set to reset/set mode and enable capture/compare interrupt

				// Maximum value for TACCR0 in up mode (about one second)
				TACCR0 = 14000;

				// Set up Timer_A Control Register
				TACTL |= TASSEL_1;    // Set Timer_A source to ACLK
				TACTL |= MC_1;        // Set Timer_A mode control to up mode
				TACTL |= TACLR;		  // Clear Timer_A


				//******************************************************************************
				// ADC INITIALIZATION
				//******************************************************************************

				// Straight binary is default data format over 2s complement
				// ADC10CTL1 |= ~ADC10DF;

				// No inversion is default
				// ADC10CTL1 |= ~ISSH;

				// Select ADC100SC as clock source
				ADC10CTL1 |= ADC10SSEL_0;

				// Select /4 for divider of clock source
				ADC10CTL1 |= ADC10DIV_3;

				// Select A0 for communication with CLIO board
				ADC10CTL1 |= INCH_0;

				// Vr+ = Vref+ = 1.5 V and Vr- = Vss = 0V
				ADC10CTL0 |= SREF_1;

				// Sample and hold time = 64 ADC10CLKs
				ADC10CTL0 |= ADC10SHT_3;

				// Reference generator on
				ADC10CTL0 |= REFON;

				// ADC10 turned on
				ADC10CTL0 |= ADC10ON;

				// ADC10 reference-generator voltage is set to 2.5
				ADC10CTL0 |= REF2_5V;

				while(1)
				{
					// Prepare to sample ADC
					ADC10CTL0 |= (REFON + ADC10ON);

					// Sample A0 from CLIO board (solar panel)
					ADC10CTL0 |= ENC + ADC10SC;

					// Enter LPM3 and wait for ADC10 to finish calculations
					__bis_SR_register(GIE+LPM3_bits);

					// Store value from ADC10 in global int variable - value is only ten bits
					g_uiSolar = ADC10MEM;

					// Stop converting, then shutoff reference generator and ADC to save energy
					ADC10CTL0 &= ~ENC;
					ADC10CTL0 &= ~(REFON + ADC10ON);

					// Split the values from the ADC into two different bytes to be sent:

						// Stores only the first two bits
						// Example:
						// 1 0 1 0 1 0 1 0 1 0
						// |__|

						g_ucaRXInput[0] = g_uiSolar >> 8;


						// Stores only the last eight bits
						// Example:
						// 1 0 1 0 1 0 1 0 1 0
						//     |_____________|

						g_ucaRXInput[1] = (g_uiSolar & 255);

					// Reset interrupt enable
					P2IE &= ~BIT6;

					// Reset interrupt flag
					P2IFG &= ~BIT6;

					// Clear the transmit FIFO with strobe command
					ucCC2500_SendCommandStrobe(SFTX);

					// Renable interrupts
					P2IE |= BIT6;

					// Reset interrupt flag
					P2IFG &= ~BIT6;

					// Send flag
					g_ucRXFlag = 0;



					// Flash green LED
					LED_FLASH(GREEN_LED);

					// Send the 16-bit array (with only 10-bits possible of actual data from ADC10)
					ucCC2500_BurstWriteRegisters(TX_FIFO, g_ucaRXInput, 2);

					// Send strobe command to send data to BASE
					ucCC2500_SendCommandStrobe(STX);

					// Enter sleep mode until finished and ready to sample ADC10 again
					__bis_SR_register(GIE+LPM3_bits);

					// Data is received so disable interrupts
					P2IE &= ~BIT6;

				}
		// REMOTE code ends
		#endif
}


//**************************************************************************/
// PORT2 - CC2500 communication - Interrupt Service Routine
// vPort2_ISR()
// When STX is called the data is sent with ISR when it is completed to be sent over transmission
// When SRX is called the data that is received is completed, this interrupt is called
//**************************************************************************/

#pragma vector=PORT2_VECTOR
__interrupt void vPort2_ISR()
{
    unsigned char ucLength = 0x00;
    if ( g_ucRXFlag ) // If in RX mode
    {
        ucCC2500_ReadSingleRegister( RXBYTES, &ucLength );
        if ( ucLength )
        {
        // CRC has passed and hence packet is valid, so wake up on exit
        __bic_SR_register_on_exit( LPM3_bits );
        }
    }
    // If in TX mode, we are done sending the packet, so wake on exit
    else
    {
    __bic_SR_register_on_exit( LPM3_bits );
    }

    P2IFG &= ~BIT6; // Clear interrupt flag so the interrupt can be called again

}


//**************************************************************************/
// TIMERA0 Interrupt Service Routine
// With TACCR0 = 14000 on VLO, this is about one second
//**************************************************************************/

#pragma vector = TIMERA0_VECTOR;
__interrupt void Timer_A (void)
{
	_bic_SR_register_on_exit(LPM3_bits);
}


//**************************************************************************/
// ADC10 Interrupt Service Routine
// Interrupt triggered by the completion of the ADC conversion. This will
// pull the MSP430 out of LPM3 where it can resume its tasks
//**************************************************************************/

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(LPM3_bits);        // Clear CPUOFF bit from 0(SR)
}
