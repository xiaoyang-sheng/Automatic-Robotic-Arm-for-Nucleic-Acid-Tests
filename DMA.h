#ifndef DMA_H
#define DMA_H

#include "LCD.h"
#include "util.h"
#include <p32xxxx.h>
#include <plib.h>
#include <stdlib.h>
#pragma interrupt DMA0_ISR ipl5 vector 36 // sub prio = 3
#pragma interrupt DMA2_ISR ipl5 vector 38 // sub prio = 3
#pragma interrupt DMA3_ISR ipl5 vector 39 // sub prio = 3
#pragma interrupt DMA4_ISR ipl5 vector 40 // sub prio = 3
#pragma interrupt DMA5_ISR ipl5 vector 41 // sub prio = 3
#pragma interrupt DMA7_ISR ipl5 vector 43 // sub prio = 3

static ushort up4_duty[200];
static ushort up3_duty[200];
static ushort up2_duty[200];
static ushort exec5_duty[200];
static ushort exec2_duty[200];
static ushort down4_duty[200];
static ushort down3_duty[200];
static ushort down2_duty[200];
static ushort up4_sz = 0;
static ushort up3_sz = 0;
static ushort up2_sz = 0;
static ushort exec5_sz = 0;
static ushort exec2_sz = 0;
static ushort down4_sz = 0;
static ushort down3_sz = 0;
static ushort down2_sz = 0;

void gen_duty(ushort *duty_arr, ushort *duty_sz, ushort start, ushort end, short delta) {
  ushort i = 0;
  ushort num = (end - start + delta - (delta > 0 ? 1 : -1)) / delta + 1;
  ushort *head = duty_arr + *duty_sz / sizeof(ushort);
  for (; i < num; ++i) {
    head[i] = start + i * delta;
  }
  head[num - 1] = end;
  *duty_sz += sizeof(ushort) * num;
}

// servo 4 up, trigger servo 3 & servo 2 up
void DMA0_ISR() {
  DelayMsec(8);
  DCH1ECONbits.CFORCE = 1;
  DelayMsec(8);
  DCH2ECONbits.CFORCE = 1;
  DelayMsec(8);
  DCH0INTbits.CHCCIF = 0;  // clear Cell Complete interrupt flag
  IFS1bits.DMA0IF = 0;     // clear channel interrupt flag
  DCH0ECONbits.CFORCE = 1; // trigger next cell transfer
}

// up finished, trigger servo 5 exec
void DMA2_ISR() {
  DelayMsec(1000);
  DCH0INTbits.CHCCIF = 0; // clear Cell Complete interrupt flag
  IFS1bits.DMA0IF = 0;    // clear channel interrupt flag
  DCH2INTbits.CHBCIF = 0;
  IFS1bits.DMA2IF = 0;
  DCH3CONbits.CHEN = 1;
  DCH4CONbits.CHEN = 1;
  DCH3ECONbits.CFORCE = 1;
}

// servo 5 exec, trigger servo 2
void DMA3_ISR() {
  DelayMsec(10);
  DCH4ECONbits.CFORCE = 1;
  DelayMsec(10);
  DCH3INTbits.CHCCIF = 0;
  IFS1bits.DMA3IF = 0;
  DCH3ECONbits.CFORCE = 1;
}

// exec finished, trigger servo 4 down
void DMA4_ISR() {
  DelayMsec(800);
  DCH4INTbits.CHBCIF = 0;
  IFS1bits.DMA4IF = 0;
  DCH5CONbits.CHEN = 1;
  DCH6CONbits.CHEN = 1;
  DCH7CONbits.CHEN = 1;
  DCH5ECONbits.CFORCE = 1;
}

// servo 4 down, trigger servo 3 & servo 2 down
void DMA5_ISR() {
  DelayMsec(8);
  DCH6ECONbits.CFORCE = 1;
  DelayMsec(8);
  DCH7ECONbits.CFORCE = 1;
  DelayMsec(8);
  DCH5INTbits.CHCCIF = 0;
  IFS1bits.DMA5IF = 0;
  DCH5ECONbits.CFORCE = 1;
}

// down finished, release clamp
void DMA7_ISR() {
  DCH7INTbits.CHBCIF = 0;
  IFS1bits.DMA7IF = 0;
  OC1RS = 700;
  DelayMsec(1000);
  status = IDLE;
}

// DmaChnSetTxfer(DMA_src_addr, DMA_dest_addr, src_size, dest_size, transfer_size (cell_size))
// do not need to enable channels here, enable channels in UART ISR & channel ISRs
void DMA_init() {
  gen_duty(up4_duty, &up4_sz, 2075, 1150, -25);
  gen_duty(up3_duty, &up3_sz, 1675, 500, -38);
  gen_duty(up2_duty, &up2_sz, 675, 1300, 25);

  gen_duty(exec5_duty, &exec5_sz, 1525, 1650, 25);
  gen_duty(exec5_duty, &exec5_sz, 1625, 1350, -25);
  gen_duty(exec5_duty, &exec5_sz, 1375, 1500, 25);

  gen_duty(exec2_duty, &exec2_sz, 1275, 1000, -25);
  gen_duty(exec2_duty, &exec2_sz, 1025, 1300, 25);

  gen_duty(down4_duty, &down4_sz, 1175, 2100, 25);
  gen_duty(down3_duty, &down3_sz, 525, 1700, 38);
  gen_duty(down2_duty, &down2_sz, 1275, 650, -25);
  // channel 0: servo 4 up
  DmaChnSetTxfer(DMA_CHANNEL0, (void *)up4_duty, (void *)&OC4RS, up4_sz, 2, 2);
  IFS1bits.DMA0IF = 0;
  IPC9bits.DMA0IP = 5;
  IPC9bits.DMA0IS = 3;
  IEC1bits.DMA0IE = 1;
  DCH0INT = 0;
  DCH0INTbits.CHCCIE = 1; // enable channel 0 Cell Complete interrupt
  DCH0CON = 0;
  // channel 1: servo 3 up
  DmaChnSetTxfer(DMA_CHANNEL1, (void *)up3_duty, (void *)&OC3RS, up3_sz, 2, 2);
  DCH1CON = 0;
  // channel 2: servo 2 up
  DmaChnSetTxfer(DMA_CHANNEL2, (void *)up2_duty, (void *)&OC2RS, up2_sz, 2, 2);
  IFS1bits.DMA2IF = 0;
  IPC9bits.DMA2IP = 5;
  IPC9bits.DMA2IS = 3;
  IEC1bits.DMA2IE = 1;
  DCH2INT = 0;
  DCH2INTbits.CHBCIE = 1;
  DCH2CON = 0;
  // channel 3: servo 5 exec
  DmaChnSetTxfer(DMA_CHANNEL3, (void *)exec5_duty, (void *)&OC5RS, exec5_sz, 2, 2);
  IFS1bits.DMA3IF = 0;
  IPC9bits.DMA3IP = 5;
  IPC9bits.DMA3IS = 3;
  IEC1bits.DMA3IE = 1;
  DCH3INT = 0;
  DCH3INTbits.CHCCIE = 1;
  DCH3CON = 0;
  // channel 4: servo 2 exec
  DmaChnSetTxfer(DMA_CHANNEL4, (void *)exec2_duty, (void *)&OC2RS, exec2_sz, 2, 2);
  IFS1bits.DMA4IF = 0;
  IPC10bits.DMA4IP = 5;
  IPC10bits.DMA4IS = 3;
  IEC1bits.DMA4IE = 1;
  DCH4INT = 0;
  DCH4INTbits.CHBCIE = 1;
  DCH4CON = 0;
  // channel 5: servo 4 down
  DmaChnSetTxfer(DMA_CHANNEL5, (void *)down4_duty, (void *)&OC4RS, down4_sz, 2, 2);
  IFS1bits.DMA5IF = 0;
  IPC10bits.DMA5IP = 5;
  IPC10bits.DMA5IS = 3;
  IEC1bits.DMA5IE = 1;
  DCH5INT = 0;
  DCH5INTbits.CHCCIE = 1;
  DCH5CON = 0;
  // channel 6: servo 3 down
  DmaChnSetTxfer(DMA_CHANNEL6, (void *)down3_duty, (void *)&OC3RS, down3_sz, 2, 2);
  DCH6CON = 0;
  // channel 7: servo 2 down
  DmaChnSetTxfer(DMA_CHANNEL7, (void *)down2_duty, (void *)&OC2RS, down2_sz, 2, 2);
  IFS1bits.DMA7IF = 0;
  IPC10bits.DMA7IP = 5;
  IPC10bits.DMA7IS = 3;
  IEC1bits.DMA7IE = 1;
  DCH7INT = 0;
  DCH7INTbits.CHBCIE = 1;
  DCH7CON = 0;
  // enable DMA
  DMACONbits.ON = 1;
}

#endif
