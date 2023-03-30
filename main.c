#include "DMA.h"
#include "LCD.h"
#include "PBCLK.h"
#include "PWM.h"
#include "UART.h"
#include "util.h"
#include <p32xxxx.h>

int main() {
  status = IDLE;
  INTCONbits.MVEC = 1;
  set_1MHz_PBCLK();
  asm("ei");
  T4CON = 0;
  TMR4 = 0;
  PR4 = 0;
  IFS0bits.T4IF = 0;
  IEC0bits.T4IE = 1;
  IPC4bits.T4IP = 4;
  IPC4bits.T4IS = 2;
  LCD_init();
  PWM_init();
  DMA_init();
  UART_init();
  LCD_puts("====WELCOME!====");
  while (1) {
    if (disp_ready) {
      LCD_putchar(LCD_CLR_DSP);
      t4DelayMsec(5);
      LCD_puts(lcd_str);
      t4DelayMsec(2000);
      LCD_putchar(LCD_CLR_DSP);
      t4DelayMsec(5);
      LCD_puts("====WELCOME!====");
      disp_ready = 0;
    }
  };
}
