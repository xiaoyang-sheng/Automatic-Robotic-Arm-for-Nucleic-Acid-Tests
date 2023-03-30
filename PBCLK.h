#include <p32xxxx.h>

// DEVCFG3
// USERID = No Setting
#pragma config FSRSSEL = PRIORITY_0 // SRS Select (SRS Priority 0)
#pragma config FCANIO = OFF         // CAN I/O Pin Select (Alternate CAN I/O)
#pragma config FUSBIDIO = OFF       // USB USID Selection (Controlled by Port Function)
#pragma config FVBUSONIO = OFF      // USB VBUS ON Selection (Controlled by Port Function)

// DEVCFG2
#pragma config FPLLIDIV = DIV_2 // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_15 // PLL Multiplier (15x Multiplier)
#pragma config UPLLIDIV = DIV_1 // USB PLL Input Divider (1x Divider)
#pragma config UPLLEN = OFF     // USB PLL Enable (Disabled and Bypassed)
#pragma config FPLLODIV = DIV_1 // System PLL Output Clock Divider (PLL Divide by 1)

// DEVCFG1
#pragma config FNOSC = PRIPLL // Oscillatoits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = ON   // Secondary Oscillator Enable (Enabled)
#pragma config IESO = ON      // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = HS   // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC = ON  // CLKO Output Signal Active on the OSCO Pin (Enabled)
#pragma config FPBDIV = DIV_2 // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/2)
#pragma config FCKSM = CSECMD // Clock Switching and Monitor Selection (Clock Switch Enable, FSCM Disabled)
#pragma config WDTPS = PS1    // Watchdog Timer Postscaler (1:1)
#pragma config FWDTEN = OFF   // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))

// DEVCFG0
#pragma config DEBUG = OFF       // Background Debugger Enable (Debugger is disabled)
#pragma config ICESEL = ICS_PGx2 // ICE/ICD Comm Channel Select (ICE EMUC2/EMUD2 pins shared with PGC2/PGD2)
#pragma config PWP = OFF         // Program Flash Write Protect (Disable)
#pragma config BWP = OFF         // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF          // Code Protect (Protection Disabled)

// Set peripheral bus clock frequency to 1MHz
void set_1MHz_PBCLK() {
  /* configure the oscillator */
  SYSKEY = 0x0;              // Ensure OSCCON is lock
  SYSKEY = 0xAA996655;       // Unlock sequence part 1 of 2 back to back instructions.
  SYSKEY = 0x556699AA;       // Unlock sequence part 2 of 2 back to back instructions.
  OSCCONbits.NOSC = 0x0007;  // select FRC, with original freqeuncy as 8 MHz
  OSCCONbits.FRCDIV = 0x3;   // the prescale of FRC is 8
  OSCCONbits.PBDIV = 0x0;    // divided by 1, PBCLK has freqeuncy 1 MHz
  OSCCONbits.OSWEN = 0x0001; // Initiate clock switch by setting OSWEN bit.
  SYSKEY = 0x0;              // Write non-key value to perform a re-lock.
}
