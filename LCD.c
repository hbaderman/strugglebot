#include <xc.h>
#include <string.h>
#include <stdio.h>
#include "HEADER.h"

#pragma config OSC = IRCIO
#define _XTAL_FREQ 8000000 //define _XTAL_FREQ so delay routines work

#define LCD_RS LATAbits.LA6 //LCD RS bit
#define LCD_EN LATCbits.LC0 //LCD Enable bit
#define LCD_DB4 LATCbits.LC1 //LCD Databit 4
#define LCD_DB5 LATCbits.LC2 //LCD Databit 5
#define LCD_DB6 LATDbits.LD0 //LCD Databit 6
#define LCD_DB7 LATDbits.LD1 //LCD Databit 7


//Function to toggle the enable bit to read data
void E_TOG(void) {
    //don?t forget to put a delay between the on and off
    //commands! 5us will be plenty.
    LCD_EN = 1;
    __delay_us(5); // 5us delay ? remember to define _XTAL_FREQ
    LCD_EN = 0;
}

//Function to send four bits to the LCD
void LCDout(unsigned char number) {
    //set data pins using the four bits from number

    LCD_DB4 = (number & 0b00000001);
    LCD_DB5 = ((number >> 1) & 0b00000001); //shift number down and & it so that DB5 only shows second number
    LCD_DB6 = ((number >> 2) & 0b00000001);
    LCD_DB7 = ((number >> 3) & 0b00000001);

    //toggle the enable bit to send data
    E_TOG();
    __delay_us(5); // 5us delay
}

//Function to send data/commands over a 4bit interface
void SendLCD(unsigned char Byte, char type) {
    // set RS pin whether it is a Command (0) or Data/Char (1)
    LCD_RS = type;
    // using type as the argument
    // send high bits of Byte using LCDout function
    LCDout((Byte >> 4) & 0b00001111);
    __delay_us(10); // 10us delay
    // send low bits of Byte using LCDout function
    LCDout(Byte & 0b00001111);
}

//Turns characters into a string
void LCD_String(char *string) {
    //While the data pointed to isn?t a 0x00 do below
    while (*string != 0) {
        //Send out the current byte pointed to
        // and increment the pointer
        SendLCD(*string++, 1);
        __delay_us(50); //so we can see each character
        //being printed in turn (remove delay if you want
        //your message to appear almost instantly)
    }
}

//Function to intialise LCD
void LCD_Init(void) {
    // set initial LAT output values (they start up in a random state)
    //    LATA = 0;
    //    LATC = 0;
    //    LATD = 0;
    // set LCD pins as output (TRIS registers)
    // Initialisation sequence code - see the data sheet
    //delay 15mS
    __delay_ms(15);
    //send 0b0011 using LCDout
    LCDout(0b0011);
    //delay 5ms
    __delay_ms(5);
    //send 0b0011 using LCDout
    LCDout(0b0011);
    //delay 200us
    __delay_us(200);
    //send 0b0011 using LCDout
    LCDout(0b0011);
    //delay 50us
    __delay_us(50);
    //send 0b0010 using LCDout set to four bit mode
    LCDout(0b0010);
    // now use SendLCD to send whole bytes ? send function set, clear
    // screen, set entry mode, display on etc to finish initialisation
    SendLCD(0b00101000, 0); //
    SendLCD(0b00001000, 0); //display off
    SendLCD(0b00000001, 0); //display clear
    __delay_ms(2);
    SendLCD(0b00000110, 0); //entry mode on, cursor direction increase, display not shifted
    SendLCD(0b00001100, 0); //display on, cursor off, blinking off
}

//Function to put cursor to start of line
void SetLine(char line) {
    //Send 0x80 to set line to 1 (0x00 ddram address)
    if (line == 1) {
        SendLCD(0x80, 0);
    }
    //Send 0xC0 to set line to 2 (0x40 ddram address)
    if (line == 2) {
        SendLCD(0xC0, 0);
    }
    __delay_us(50); // 50us delay
}

//Function to clear LCD
void clearLCD(void) {
    SendLCD(0b00000001, 0);
}

// Custom characters function
void customChars(void) {
    //CUSTOM CHAR 1 BOMB
    SendLCD(0x40, 0); // COMMAND
    SendLCD(0b00010, 1); // THE REST IS DATA
    SendLCD(0b00101, 1);
    SendLCD(0b00100, 1);
    SendLCD(0b01110, 1);
    SendLCD(0b11111, 1);
    SendLCD(0b11111, 1);
    SendLCD(0b11111, 1);
    SendLCD(0b01110, 1);
    //CUSTOM CHAR 2 BOMB
    SendLCD(0b00010, 1);
    SendLCD(0b00101, 1);
    SendLCD(0b00100, 1);
    SendLCD(0b01110, 1);
    SendLCD(0b11111, 1);
    SendLCD(0b11111, 1);
    SendLCD(0b11111, 1);
    SendLCD(0b01110, 1);
}
