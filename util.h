#ifndef UTIL_H
#define UTIL_H

#include <p32xxxx.h>
#pragma interrupt T1_ISR ipl6 vector 4  // sub prio = 3
#pragma interrupt T4_ISR ipl6 vector 16 // sub prio = 2

typedef unsigned char uchar;
typedef unsigned short ushort;

#define EXEC 1
#define IDLE 0
ushort status = IDLE;

uchar lcd_str[33];
ushort offs = 0;
ushort disp_ready = 0;

void T1_ISR() {
  T1CONbits.ON = 0;
  IFS0bits.T1IF = 0;
}

void DelayUsec(ushort num) {
  TMR1 = 0;
  PR1 = num;
  T1CONbits.ON = 1;
  while (1) {
    if (!T1CONbits.ON) {
      break;
    }
  }
}

void DelayMsec(ushort num) {
  ushort i = 0;
  for (; i < num; i++) {
    GenMsec();
  }
}

void GenMsec() {
  TMR1 = 0;
  PR1 = 1000;
  T1CONbits.ON = 1;
  while (1) {
    if (!T1CONbits.ON) {
      break;
    }
  }
}

void T4_ISR() {
  T4CONbits.ON = 0;
  IFS0bits.T4IF = 0;
}

void t4DelayUsec(ushort num) {
  TMR4 = 0;
  PR4 = num;
  T4CONbits.ON = 1;
  while (1) {
    if (!T4CONbits.ON) {
      break;
    }
  }
}

void t4DelayMsec(ushort num) {
  ushort i = 0;
  for (; i < num; i++) {
    t4GenMsec();
  }
}

void t4GenMsec() {
  TMR4 = 0;
  PR4 = 1000;
  T4CONbits.ON = 1;
  while (1) {
    if (!T4CONbits.ON) {
      break;
    }
  }
}

#endif
