//******************************************************************************
// cc2500.c
// 
// By: Kenji Yamamoto
//     Department of Electrical Engineering, CEFNS
//     Northern Arizona University
//
// Module for using the TI/ChipCon CC2500
//
//******************************************************************************

#include <msp430x22x4.h>

#include "cc2500.h"

#include "usci_spi.h"

//////////////////////////////////////////////////////////////////////////////
// vCC2500_Init()
//
// Setups all of the hardware resources needed to use the CC2500
//////////////////////////////////////////////////////////////////////////////
void vCC2500_Init()
{
	vUSCI_B0_SPI_Init();
	
	// This is the line to CSn
	P3DIR |= BIT0;
	P3OUT |= BIT0;
}

//////////////////////////////////////////////////////////////////////////////
// vCC2500_Select()
//
// Take the CSn pin on the radio low in preperation for SPI comm
//////////////////////////////////////////////////////////////////////////////
void vCC2500_Select()
{
	// Remember, it's active low
	P3OUT &= ~BIT0;
	
	// According to CC2500 documentation, after taking CSn low, must wait for
	// the SO line before serial comm can begin
	while(P3IN & BIT2);
}

//////////////////////////////////////////////////////////////////////////////
// vCC2500_Deselect()
//
// Take the CSn pin on the radio high after SPI comm is done
//////////////////////////////////////////////////////////////////////////////
void vCC2500_Deselect()
{
	// Remember, it's active low
	P3OUT |= BIT0;
}

//////////////////////////////////////////////////////////////////////////////
// ucCC2500_ReadSingleRegister( ucAddress, pucData)
//
// Reads a single CC2500 register and stores in DATA
//
// Function returns the status byte of the radio.
//////////////////////////////////////////////////////////////////////////////
unsigned char ucCC2500_ReadSingleRegister(unsigned char ucAddress,
                                          unsigned char * pucData)
{
	vCC2500_Select();
	
	ucAddress |= 0x80;
	vUSCI_B0_SPI_SendBytes(&ucAddress, &ucAddress, 1);
	vUSCI_B0_SPI_SendBytes(&ucAddress, pucData, 1);
	
	vCC2500_Deselect();
	return ucAddress;
}

//////////////////////////////////////////////////////////////////////////////
// ucCC2500_BurstReadRegisters(ucAddress, pucData, ucCount)
//
// Reads registers indicated by COUNT using the burst method. The
// contents of the registers are written to the location pointed to by pucData.
// 
// Returns the status byte of the radio
//////////////////////////////////////////////////////////////////////////////
unsigned char ucCC2500_BurstReadRegisters(unsigned char ucAddress,
                                          unsigned char * pucData,
                                          unsigned char ucCount)
{
	vCC2500_Select();
	
	ucAddress |= 0xC0;
	vUSCI_B0_SPI_SendBytes(&ucAddress, &ucAddress, 1);
	vUSCI_B0_SPI_SendBytes(pucData, pucData, ucCount);
	
	vCC2500_Deselect();
	return ucAddress;
}

//////////////////////////////////////////////////////////////////////////////
// ucCC2500_WriteSingleRegister(ucAddress, ucData)
//
// Writes the DATA to a single register indicated by ADDRESS.
//
// Returns the status byte of the radio
//////////////////////////////////////////////////////////////////////////////
unsigned char ucCC2500_WriteSingleRegister(unsigned char ucAddress,
                                           unsigned char ucData)
{
	vCC2500_Select();
	
	ucAddress |= 0x00;
	vUSCI_B0_SPI_SendBytes(&ucAddress, &ucAddress, 1);
	vUSCI_B0_SPI_SendBytes(&ucData, &ucAddress, 1);
	
	vCC2500_Deselect();
	return ucAddress;
}

//////////////////////////////////////////////////////////////////////////////
// ucCC2500_BurstWriteRegisters( ucAddress, pucData, ucCount)
//
// Writes to a set of sequential registers starting with the register at 
// ADDRESS using the information stored at DATA.
//
// Function returns the status byte of the radio
//////////////////////////////////////////////////////////////////////////////
unsigned char ucCC2500_BurstWriteRegisters(unsigned char ucAddress,
                                           unsigned char * pucData,
                                           unsigned char ucCount)
{
	vCC2500_Select();
	
	ucAddress |= 0x40;
	vUSCI_B0_SPI_SendBytes(&ucAddress, &ucAddress, 1);
	vUSCI_B0_SPI_SendBytes(pucData, 0, ucCount);
	
	vCC2500_Deselect();
	return ucAddress;
}

//////////////////////////////////////////////////////////////////////////////
// ucCC2500_SendCommandStrobe(ucStrobe)
//
// Send the command strobe indicated by STROBE.
//
// Function returns the status byte of the radio
//////////////////////////////////////////////////////////////////////////////
unsigned char ucCC2500_SendCommandStrobe(unsigned char ucStrobe)
{
	vCC2500_Select();
	
	ucStrobe |= 0x80;
	vUSCI_B0_SPI_SendBytes(&ucStrobe, &ucStrobe, 1);
	
	vCC2500_Deselect();
	return ucStrobe;
}

//////////////////////////////////////////////////////////////////////////////
// ucCC2500_GetReadStatus()
//
// Gets the status byte of the radio with the RX FIFO count
//////////////////////////////////////////////////////////////////////////////
unsigned char ucCC2500_GetReadStatus()
{
	unsigned char ucAddress;
	vCC2500_Select();
	
	ucAddress = SNOP | 0x80;
	vUSCI_B0_SPI_SendBytes(&ucAddress, &ucAddress, 1);
	
	vCC2500_Deselect();
	return ucAddress;
}

//////////////////////////////////////////////////////////////////////////////
// ucCC2500_GetWriteStatus()
//
// Gets the status byte of the radio with the TX FIFO count
//////////////////////////////////////////////////////////////////////////////
unsigned char ucCC2500_GetWriteStatus()
{
	unsigned char ucAddress;
	vCC2500_Select();
	
	ucAddress = SNOP | 0x00;
	vUSCI_B0_SPI_SendBytes(&ucAddress, &ucAddress, 1);
	
	vCC2500_Deselect();
	return ucAddress;
}

//////////////////////////////////////////////////////////////////////////////
// vCC2500_SetTXPower( ucPower )
// 
// Set the TX output power
//////////////////////////////////////////////////////////////////////////////
void vCC2500_SetTXPower(unsigned char ucPower)
{
	ucCC2500_WriteSingleRegister(PATABLE, ucPower);
}

//////////////////////////////////////////////////////////////////////////////
// vCC2500_SetupRFPacketMode()
//
// Setups the radio for packet mode, see SmartRF studio and CC2500
// documentation for more info
//////////////////////////////////////////////////////////////////////////////
void vCC2500_SetupRFPacketMode()
{
	ucCC2500_WriteSingleRegister(MDMCFG1,  0x20);
	ucCC2500_WriteSingleRegister(MDMCFG2,  0x03);
	ucCC2500_WriteSingleRegister(PKTCTRL0, 0x05);
	ucCC2500_WriteSingleRegister(PKTCTRL1, 0x04);
	ucCC2500_WriteSingleRegister(IOCFG0,   0x06);
	ucCC2500_WriteSingleRegister(ADDR,     0x00);
	ucCC2500_WriteSingleRegister(PKTLEN,   0x3D);
}

//////////////////////////////////////////////////////////////////////////////
// vCC2500_LoadProfile( ucProfile)
//
// Loads the profile specified by PROFILE into the CC2500
/////////////////////////////////////////////////////////////////////////////
void vCC2500_LoadProfile(unsigned char ucProfile)
{
	unsigned char ucAddress;
  unsigned char ucaProfiles[][0x2F] = 
	{
    { // 1.2 kBaud, 28 kHz Deviation, 2-FSK, 203 kHz RX filterbandwidth,
      0x29,  // GDO2 output pin configuration.
      0x2E,  // GDO1 output pin configuration.
      0x06,  // GDO0 output pin configuration.
      0x07,  // RXFIFO and TXFIFO thresholds.
      0xD3,  // Sync word, high byte
      0x91,  // Sync word, low byte
      0x20,  // Packet length.
      0x08,  // Packet automation control.
      0x04,  // Packet automation control.
      0x00,  // Device address.
      0x80,  // Channel number.
      0x08,  // Frequency synthesizer control.
      0x00,  // Frequency synthesizer control.
      0x5C,  // Frequency control word, high byte.
      0x58,  // Frequency control word, middle byte.
      0x9D,  // Frequency control word, low byte.
      0x85,  // Modem configuration.
      0x83,  // Modem configuration.
      0x03,  // Modem configuration.
      0x22,  // Modem configuration.
      0xF8,  // Modem configuration.
      0x44,  // Modem deviation setting (when FSK modulation is enabled).
      0x07,  // Main Radio Control State Machine configuration
      0x30,  // Main Radio Control State Machine configuration
      0x18,  // Main Radio Control State Machine configuration.
      0x16,  // Frequency Offset Compensation Configuration.
      0x6C,  // Bit synchronization Configuration.
      0x03,  // AGC control.
      0x40,  // AGC control.
      0x91,  // AGC control.
      0x87,  // High Byte Event0 Timeout
      0x6B,  // Low Byte Event0 Timeout
      0xF8,  // Wake On Radio Control
      0x56,  // Front end RX configuration.
      0x10,  // Front end TX configuration.
      0xA9,  // Frequency synthesizer calibration.
      0x0A,  // Frequency synthesizer calibration.
      0x00,  // Frequency synthesizer calibration.
      0x11,  // Frequency synthesizer calibration.
      0x41,  // RC Oscillator Configuration
      0x00,  // RC Oscillator Configuration
      0x59,  // Frequency synthesizer calibration.
      0x7F,  // Production Test
      0x3F,  // AGC Test
      0x88,  // Various test settings.
      0x31,  // Various test settings.
      0x0B  // Various test settings.   
    }
	};
	
	// Power up the radio and issue the reset command, then wait for reset to 
	// finish
	vCC2500_Select();
	ucAddress = SRES | 0x80;
	vUSCI_B0_SPI_SendBytes(&ucAddress, &ucAddress, 1);
	while (P3IN & BIT2);
	vCC2500_Deselect();
	
	// Write the registers
	ucCC2500_BurstWriteRegisters(0x00, &ucaProfiles[ucProfile][0], 0x2F);
}
