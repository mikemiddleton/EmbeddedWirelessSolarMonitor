//******************************************************************************
// cc2500.h
//
// By: Kenji Yamamoto
//     Department of Electrical Engineering, CEFNS
//     Northern Arizona University
//
// Module for using the TI/Chipcon CC2500
//******************************************************************************

#ifndef _RADIO_H_
  #define _RADIO_H_
  
  void vCC2500_Init();
  void vCC2500_Select();
  void vCC2500_Deselect();
  unsigned char ucCC2500_ReadSingleRegister(unsigned char ucAddress,
                                            unsigned char * pucData);
  
  unsigned char ucCC2500_BurstReadRegisters(unsigned char ucAddress,
                                            unsigned char * pucData,
                                            unsigned char ucCount);
  
  unsigned char ucCC2500_WriteSingleRegister(unsigned char ucAddress,
                                             unsigned char ucData);
  
  unsigned char ucCC2500_BurstWriteRegisters(unsigned char ucAddress,
                                             unsigned char * pucData,
                                             unsigned char ucCount);
  
  unsigned char ucCC2500_SendCommandStrobe(unsigned char ucStrobe);
  unsigned char ucCC2500_GetReadStatus();
  unsigned char ucCC2500_GetWriteStatus();
 
  void vCC2500_SetTXPower(unsigned char ucPower);
  void vCC2500_SetupRFPacketMode();
  void vCC2500_LoadProfile(unsigned char ucProfile);
  
  // Register addresses
  #define    IOCFG2    0x00
  #define    IOCFG1    0x01
  #define    IOCFG0    0x02
  #define    FIFOTHR   0x03
  #define    SYNC1     0x04
  #define    SYNC0     0x05
  #define    PKTLEN    0x06
  #define    PKTCTRL1  0x07
  #define    PKTCTRL0  0x08
  #define    ADDR      0x09
  #define    CHANNR    0x0A
  #define    FSCTRL1   0x0B
  #define    FSCTRL0   0x0C
  #define    FREQ2     0x0D
  #define    FREQ1     0x0E
  #define    FREQ0     0x0F
  #define    MDMCFG4   0x10
  #define    MDMCFG3   0x11
  #define    MDMCFG2   0x12
  #define    MDMCFG1   0x13
  #define    MDMCFG0   0x14
  #define    DEVIATN   0x15
  #define    MCSM2     0x16
  #define    MCSM1     0x17
  #define    MCSM0     0x18
  #define    FOCCFG    0x19
  #define    BSCFG     0x1A
  #define    AGCCTRL2  0x1B
  #define    AGCCTRL1  0x1C
  #define    AGCCTRL0  0x1D
  #define    WOREVT1   0x1E
  #define    WOREVT0   0x1F
  #define    WORCTRL   0x20
  #define    FREND1    0x21
  #define    FREND0    0x22
  #define    FSCAL3    0x23
  #define    FSCAL2    0x24
  #define    FSCAL1    0x25
  #define    FSCAL0    0x26
  #define    RCCTRL1   0x27
  #define    RCCTRL0   0x28
  #define    FSTEST	   0x29
  #define    PTEST     0x2A
  #define    AGCTEST   0x2B
  #define    TEST2     0x2C
  #define    TEST1     0x2D
  #define    TEST0     0x2E
 
  #define    PARTNUM        0x30
  #define    VERSION        0x31
  #define    FREQEST        0x32
  #define    LQI            0x33
  #define    RSSI           0x34
  #define    MARCSTATE      0x35
  #define    WORTIME1       0x36
  #define    WORTIME0       0x37
  #define    PKTSTATUS      0x38
  #define    VCO_VC_DAC     0x39
  #define    TXBYTES        0x3A
  #define    RXBYTES        0x3B
  #define    RCCTRL1_STATUS	0x3C
  #define    RCCTRL0_STATUS	0x3D

  // Multi-byte registers
  #define    PATABLE        0x3E
  #define    TX_FIFO        0x3F
  #define    RX_FIFO        0x3F
   
  // Command strobes
  #define    SRES           0x30
  #define    SFSTXON        0x31
  #define    SXOFF          0x32
  #define    SCAL           0x33
  #define    SRX            0x34
  #define    STX            0x35
  #define    SIDLE          0x36
  #define    SWOR           0x38
  #define    SPWD           0x39
  #define    SFRX           0x3A
  #define    SFTX           0x3B
  #define    SWORRST        0x3C
  #define    SNOP           0x3D
  
  // PATABLE values
  #define    POS_01_DBM     0xFF
  #define    ZERO_DBM       0xFE
  #define    NEG_02_DBM     0xBB
  #define    NEG_04_DBM     0xA9
  #define    NEG_06_DBM     0x7F
  #define    NEG_08_DBM	    0x6E
  #define    NEG_10_DBM	    0x97
  #define    NEG_12_DBM	    0xC6
  #define    NEG_14_DBM	    0x8D
  #define    NEG_16_DBM	    0x55
  #define    NEG_18_DBM	    0x93
  #define    NEG_20_DBM	    0x46
  #define    NEG_22_DBM	    0x81
  #define    NEG_24_DBM	    0x84
  #define    NEG_26_DBM	    0xC0
  #define    NEG_28_DBM	    0x44
  #define    NEG_30_DBM	    0x50
  
#endif /*_RADIO_H_*/
