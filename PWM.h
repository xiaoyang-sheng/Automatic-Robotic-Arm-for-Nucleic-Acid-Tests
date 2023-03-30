/*
 * PWM
 *
 * RD0:    OC1
 * RD1:    OC2
 * RD2:    OC3
 * RD3:    OC4
 * RD4:    OC5
 * TIMER2: PWM timer (no interrupt)
 *
 * in total 5 servos using PWM
 * OC1 ~ OC5 correspond servo 1 ~ servo 5 from top to bottom
 * 20ms PWM period
 * duty cycle = 1.5ms -> initial position
 *            = 2.5ms -> 90 degree counter-clockwise
 *            = 0.5ms -> 90 degree clockwise
 */

#ifndef PWM_H
#define PWM_H

#include <p32xxxx.h>

void PWM_init() {
  // config RD0 ~ RD4 (OC1 ~ OC5) as output ports
  TRISDCLR = 0x001f;
  LATDCLR = 0X001f;
  // TIMER2, CLK = PBCLK = 1MHz, T = 1us
  IEC0bits.T2IE = 0; // do not need interrupt
  T2CON = 0;
  TMR2 = 0;
  PR2 = 19999;
  T2CONbits.ON = 1;
  // OC1
  OC1RS = 700;
  OC1CON = 0;             // use TIMER2
  OC1CONbits.OCM = 0b110; // PWM mode, fault pin disabled
  OC1CONbits.ON = 1;
  // OC2
  OC2RS = 650;
  OC2CON = 0;
  OC2CONbits.OCM = 0b110;
  OC2CONbits.ON = 1;
  // OC3
  OC3RS = 1700;
  OC3CON = 0;
  OC3CONbits.OCM = 0b110;
  OC3CONbits.ON = 1;
  // OC4
  OC4RS = 2050;
  OC4CON = 0;
  OC4CONbits.OCM = 0b110;
  OC4CONbits.ON = 1;
  // OC5
  OC5RS = 1500;
  OC5CON = 0;
  OC5CONbits.OCM = 0b110;
  OC5CONbits.ON = 1;
}

#endif
