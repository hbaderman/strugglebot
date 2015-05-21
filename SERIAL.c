#include <xc.h>
#include "HEADER.h"

#pragma config OSC = IRCIO  // internal oscillator
#define _XTAL_FREQ 8000000 //define _XTAL_FREQ so delay routines work

//Function to wait for data to arrive over serial and to subsequently return
char getCharSerial(void) {
    while (!PIR1bits.RCIF); //wait for the data to arrive
    return RCREG; //return byte in RCREG
}

// Function to set up EUSART registers
void setupEUSART(void) {
    /*--------------SET UP EUSART REGISTERS --------------------*/

    //both need to be 1 even though RC6
    //is an output, check the datasheet!
    SPBRG = 205; //set baud rate to 9600 (set to 207 -- calibration is 205)
    SPBRGH = 0; // high byte
    BAUDCONbits.BRG16 = 1; //set baud rate scaling to 16 bit mode
    TXSTAbits.BRGH = 1; //high baud rate select bit
    RCSTAbits.CREN = 1; //continous receive mode
    RCSTAbits.SPEN = 1; //enable serial port, other settings default
    //TXSTAbits.TXEN = 1; //enable transmitter, other settings default  
    // CURRENTLY ONLY RECEIVING DATA!!!!
}
