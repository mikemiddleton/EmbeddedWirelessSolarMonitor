//******************************************************************************
// usci_uart.h
//
// By: Kenji Yamamoto
//     Department of Electrical Engineering, CEFNS
//     Northern Arizona University
//
// Header file for using USCI A0 in UART mode
//******************************************************************************

#ifndef _USCI_UART_H_
  #define _USCI_UART_H_
  
  void vUSCI_A0_UART_Init();
  void vUSCI_A0_UART_ClearRXBuffer(unsigned char ucWipeData);
  void vUSCI_A0_UART_SendBytes(const unsigned char * pucData, 
                               unsigned int unCount);
  
  // These are used by the USCI_A0 RX ISR for storing incoming characters.
  //  See the ISR for more details (usci_uart.c)
  extern unsigned char g_ucaUSCI_A0_RXBuffer[0x100];
  extern unsigned char g_ucUSCI_A0_RXBufferIndex;

#endif /*_USCI_UART_H_*/
