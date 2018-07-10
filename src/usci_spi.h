//******************************************************************************
// usci_spi.h
//
// By: Kenji Yamamoto
//     Dept. of Electrical Engineering, CEFNS
//     Northern Arizon University
//
// Module for configuring the USCI for SPI
//
// Part of the Hardware Abstraction Layer (HAL)
//
//******************************************************************************

#ifndef _USCI_SPI_H_
  #define _USCI_SPI_H_
  
  void vUSCI_B0_SPI_Init();
  void vUSCI_B0_SPI_SendBytes(unsigned char * pucTX,
                              unsigned char * pucRX,
                              unsigned char ucByteCount);

#endif /*_USCI_SPI_H_*/
