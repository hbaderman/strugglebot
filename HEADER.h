#ifndef HEADER_H
#define	HEADER_H

#define _XTAL_FREQ 8000000
#include <xc.h>


/*----------------------------------------------------------------------------
 CONTENTS:
 *
 * DC MOTOR -- Controls dc motors
 *
 * LCD -- Controls LCD display
 *
 * LED -- Sends numbers to LED array in binary
 *
 * SERIAL -- Configures serial communication for RFID
 *
 * SETUP -- General set-up functions and other functions of robot
 -----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------
 DC MOTOR
 -----------------------------------------------------------------------------*/

//definition of DC_motor structure
struct DC_motor {
    char power;                             //motor power, out of 100
    char direction;                         //motor direction, forward(1), reverse(0)
    unsigned char *dutyLowByte;             //PWM duty low byte address
    unsigned char *dutyHighByte;            //PWM duty high byte address
    char dir_pin;                           // pin that controls direction on PORTB
    int PWMperiod;                          //base period of PWM cycle
};

struct DC_motor motorL, motorR; //declare two DC_motor structures


//Function prototypes
void initPWM();                                                     //Function to setup PWM
void initMotor(void);                                               //Function to set up motor structures
void setMotorPWM(struct DC_motor *m);                               //Function to set motor PWM from values in the motor structure

void Stop(struct DC_motor *mL, struct DC_motor *mR);                //Function to stop robot
void turnLeft(struct DC_motor *mL, struct DC_motor *mR);            //Function to turn robot left
void turnRight(struct DC_motor *mL, struct DC_motor *mR);           //Function to turn robot right
void turnSlightRight(struct DC_motor *mL, struct DC_motor *mR);     //Function to turn robot slightly right whilst maintaining forwards motion
void turnSlightLeft(struct DC_motor *mL, struct DC_motor *mR);      //Function to turn robot slightly left whilst maintaining forwards motion
void turnSlightRightBack(struct DC_motor *mL, struct DC_motor *mR); //Function to turn robot slightly right in a backwards direction
void turnSlightLeftBack(struct DC_motor *mL, struct DC_motor *mR);  //Function to turn robot slightly left in a backwards direction
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR);      //Function for linear forward motion of robot
void fullSpeedBack(struct DC_motor *mL, struct DC_motor *mR);       //Function for linear reverse motion of robot

/*----------------------------------------------------------------------------
 LCD
 -----------------------------------------------------------------------------*/

// Toggle the enable bit to read data
void E_TOG(void);

//function to send four bits to the LCD
void LCDout(unsigned char number);

//function to send data/commands over a 4bit interface
void SendLCD(unsigned char Byte, char type);

//turns characters into a string
void LCD_String(char *string);

//Initialise LCD
void LCD_Init(void);

//function to put cursor to start of line
void SetLine(char line);

//function to clear LCD
void clearLCD(void);

//function to send certain custom characters to the LCD (for checksum, etc)
void customChars(void);

/*----------------------------------------------------------------------------
 LED
 -----------------------------------------------------------------------------*/

//displays number on LED array in binary
void LEDout(int number );

/*----------------------------------------------------------------------------
 SERIAL
 -----------------------------------------------------------------------------*/

//Wait for byte to be received and return received byte
char getCharSerial(void);

//function to set up EUSART registers
void setupEUSART(void);


/*----------------------------------------------------------------------------
 SETUP
 -----------------------------------------------------------------------------*/

// Resets ports to correct setting
void setAllPorts(void);

// Function delays the program in seconds
void delay_s(int sec);

// Converts ASCII characters to HEX (used for checksum)
unsigned char asciiHexBinary(unsigned char first, unsigned char last);

void setInterrupts(void); // Initalise interrupts

void setInputCapture(void); // Initialise input capture modules (1&2)

void setTimer5(void); // Initialise timer 5

void setPorts(void); // set appropriate input & digital I/O


#endif	/* HEADER_H */

