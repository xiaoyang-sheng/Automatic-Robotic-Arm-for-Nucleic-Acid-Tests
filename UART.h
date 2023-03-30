/*
 * * * UART * * *
 * RF2: UART1 receiver port (U1RX)
 */

#ifndef UART_H
#define UART_H

#include "LCD.h"
#include "util.h"
#include <p32xxxx.h>
#include <stdio.h>
#include <string.h>
#pragma interrupt UART_ISR ipl4 vector 24 // sub prio = 3

void UART_ISR() {
  uchar ch;
  // fetch received string
  while (U1STAbits.URXDA) {
    ch = U1RXREG;
    if (ch == ';') { // input ends
      lcd_str[offs] = '\0';
      offs = 0;
      // deal with commands
      if (strcmp(lcd_str, "exec") == 0) {
        if (status == IDLE) {
          status = EXEC;
          OC1RS = 1400;
          DelayMsec(1000);
          DCH0CONbits.CHEN = 1;
          DCH1CONbits.CHEN = 1;
          DCH2CONbits.CHEN = 1;
          DCH0ECONbits.CFORCE = 1;
        }
      } else {
        disp_ready = 1;
      }
    } else if (offs == 32) {
      offs = 0;
    } else {
      lcd_str[offs++] = ch;
    }
  }
  IFS0bits.U1RXIF = 0;
}

void UART_init() {
  TRISFbits.TRISF2 = 1; // configure RF2 (U1RX) as input port
  U1MODE = 0;           // 8-bit, no parity, 1 stop bit
  U1MODEbits.BRGH = 1;  // use high speed baud rate generator
  U1BRG = 25;           // baud rate = 9600, consistent with the HM-10 module
  U1STA = 0;
  U1STAbits.URXISEL = 0; // interrupt when receiver buffer is not empty
  IFS0bits.U1RXIF = 0;
  IPC6bits.U1IP = 4;
  IPC6bits.U1IS = 3;
  IEC0bits.U1RXIE = 1;
  U1MODEbits.ON = 1;   // enable UART1
  U1STAbits.URXEN = 1; // enable receiver
}

#endif
