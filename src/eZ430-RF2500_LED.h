/*
 * eZ430-RF2500_LED.h
 *
 *  Created on: Oct 16, 2012
 *      Author: pgf
 */

#ifndef EZ430_RF2500_LED_H_
#define EZ430_RF2500_LED_H_

#define GREEN_LED BIT1
#define RED_LED BIT0
#define BOTH_LED ( BIT1 | BIT0 )

#define LED_ON(led) ( P1OUT |= led )

#define LED_OFF(led) ( P1OUT &= ~led )

#define LED_TOGGLE(led) ( P1OUT ^= led )

// Flashes LED in current state:
// if off, flash LED on; if on , flash LED off
// dependent on MCU speed
#define LED_FLASH(led) do { \
		LED_TOGGLE( led ); \
		__delay_cycles(64000); \
		__delay_cycles(64000); \
		LED_TOGGLE( led ); \
		__delay_cycles(64000); \
} while( 0 )

#endif /* EZ430_RF2500_LED_H_ */
