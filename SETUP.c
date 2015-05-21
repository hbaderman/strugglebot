#include <xc.h>
#include "HEADER.h"

#pragma config OSC = IRCIO
#define _XTAL_FREQ 8000000 //define _XTAL_FREQ so delay routines work

//Function clears LAT registers and sets TRIS ports as outputs
void setAllPorts(void) {

    //REGISTERS
    LATA = 0;
    LATB = 0;
    LATC = 0;
    LATD = 0;

    //PORTS
    TRISA = 0; //Output
    TRISB = 0; //Output
    TRISC = 0b11000000; //set data direction registers
    TRISD = 0; //Output

}

//Function to delay in seconds
void delay_s(int sec) {
    
    for (int i = 0; i <= (sec*20); i++) {
        __delay_ms(50);
    }
}

// Converts ASCII characters to HEX (used for checksum)
unsigned char asciiHexBinary(unsigned char first, unsigned char last) {
    if (first > '9') first += 9;
    if (last > '9') last += 9;
    return (last << 4) | (first & 0x0F);
}


void setInterrupts(void) {

    /* SEE DATASHEET FOR FURTHER CLARIFICATION.
     * REFER TO INTCON, PIE3, PIE1, IPR3, IPR1
     * REGISTERS FOR RELEVANT ENABLE/PRIORITY
     * BITS */

    // GENERAL
    RCONbits.IPEN = 1; // Enable priority levels on interrupts
    INTCONbits.GIEH = 1; // Global Interrupt Enable bit
    INTCONbits.GIEL = 1; // Global Interrupt Low Priority Enable bit


    // INPUT CAPTURE MODULE 1 (CAP1)
    PIE3bits.IC1IE = 1; // CAP1 enable
    IPR3bits.IC1IP = 0; // Set CAP1 interrupt as LOW priority

    // INPUT CAPTURE MODULE 2 (CAP2)
    PIE3bits.IC2QEIE = 1;
    IPR3bits.IC2QEIP = 0; // Set CAP2 interrupt as LOW priority

    // RFID
    PIE1bits.RCIE = 1; // Interrupt EUSART Receive Interrupt Enabled
    IPR1bits.RC1IP = 1; // Set EUSART receive interrupt as HIGH priority

}

void setInputCapture(void) {
    // CAP1CON SETUP
    CAP1CON = 0b01000110; // Pulse-width measurement mode, every falling to rising
    // CAP2CON SETUP
    CAP2CON = 0b01000110; // Pulse-width measurement mode, every falling to rising

}

void setTimer5(void) {
    // TIMER5 SETUP
    T5CON = 0b00000001; // Enable the TIMER5 module
    T5CONbits.T5PS = 0b01; // Enable and set Timer5 Prescaler 1:4

    /* [Timer5 overflows every 2^16 bits (65535). The period at which it counts
     * at is 1/2E6 (50us). (FOSC/4 = 8Mhz/4 = 2Mhz). So the total period (without
     * pre-scaler) is  33ms. With a 1:2 prescaler, the overflow period is
     * 65.54ms for TIMER5. THE OVERFLOW PERIOD SHOULD BE LONGER THAN THE IR
     * BEACON PULSE DURATION*/

    DFLTCON = 0b00111011; // DIGITAL NOISE FILTER REGISTER ENABLED
                          // SEE PAGE 169 OF DATASHEET

    TMR5H = 0; // SET INITIAL VALUES TO ZERO
    TMR5L = 0;
}

void setPorts(void) {
    // SET PORTS
    TRISAbits.RA2 = 1; // Input for CAP1
    TRISAbits.RA3 = 1; // Input for CAP2
    ANSEL0bits.ANS2 = 0; // Force digital I/O
    ANSEL0bits.ANS3 = 0; // Force digital I/O

}