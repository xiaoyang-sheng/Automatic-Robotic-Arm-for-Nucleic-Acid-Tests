/*
 * 4-bit LCD
 *
 * RE:     data bits
 * RF4:    R/S bit
 * RF5:    ENABLE bit
 * TIMER1: generate delay
 *
 * R/W bit is grounded (always write to LCD)
 * by default data type is LCD instructions (R/S bit = 0)
 */

#ifndef LCD_H
#define LCD_H

#include "util.h"
#include <p32xxxx.h>
// LCD instructions
#define LCD_IDLE 0x33
#define LCD_2_LINE_4_BITS 0x28
#define LCD_DSP_CSR 0x0c
#define LCD_CLR_DSP 0x01 // clear display
#define LCD_CSR_INC 0x06
#define LCD_SFT_MOV 0x14
// LCD ports
#define Data PORTE       // data bits
#define RS PORTFbits.RF4 // R/S bit
#define E PORTFbits.RF5  // ENABLE bit

void T1_ISR();
void LCD_init();
void LCD_putchar(uchar c);
void LCD_puts(const uchar *s);
void LCD_goto(uchar addr);

void LCD_init() {
  // configure RE, RF4 and RF5 as output ports
  TRISE = 0;
  LATE = 0;
  TRISFbits.TRISF4 = 0;
  LATFbits.LATF4 = 0;
  TRISFbits.TRISF5 = 0;
  LATFbits.LATF5 = 0;
  // TIMER1, CLK = PBCLK = 1MHz, T = 1us
  T1CON = 0;
  TMR1 = 0;
  PR1 = 0;
  // TIMER1 interrupt
  IFS0bits.T1IF = 0;
  IPC1bits.T1IP = 6;
  IPC1bits.T1IS = 3;
  IEC0bits.T1IE = 1;
  // initialize LCD
  DelayMsec(20);
  RS = 0;
  Data = LCD_IDLE;
  DelayMsec(20);
  Data = LCD_IDLE;
  DelayMsec(20);
  Data = LCD_IDLE;
  DelayMsec(20);
  Data = LCD_IDLE;
  DelayMsec(20);
  LCD_putchar(LCD_2_LINE_4_BITS);
  DelayMsec(20);
  LCD_putchar(LCD_DSP_CSR);
  DelayMsec(20);
  LCD_putchar(LCD_CLR_DSP);
  DelayMsec(20);
  LCD_putchar(LCD_CSR_INC);
  DelayMsec(20);
}

// send in total 8-bit data to LCD
// always add delay after calling this function
void LCD_putchar(uchar c) {
  E = 1;
  Data = c;
  E = 0;
  E = 1;
  Data <<= 4;
  E = 0;
}

// send a string to LCD for display
void LCD_puts(const uchar *s) {
  ushort i = 0;
  RS = 1; // data type is normal characters
  while (*s) {
    if (i == 16) {
      RS = 0;
      LCD_goto(0x40);
      RS = 1;
    }
    LCD_putchar(*s++);
    ++i;
    t4DelayUsec(100);
  }
  RS = 0;
}

// move cursor to addr
void LCD_goto(uchar addr) {
  LCD_putchar(0x80 | addr);
  t4DelayUsec(100);
}

#endif
