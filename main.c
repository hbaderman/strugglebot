/* ------------------------------------ *
 * Main code for StruggleBot EOD robot. *
 * By Hillel & Mo.                      *
 *                                      *
 * SPRING 2014                          *
 * Embedded C for Microcontroller       *
 * ------------------------------------ */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include <math.h>

#include "HEADER.h" // File contains functions for DC MOTOR, LCD, LED, SERIAL, & SETUP

#pragma config OSC = IRCIO  // Set internal oscillator
#define _XTAL_FREQ 8000000 // Set _XTAL_FREQ so that __delay_ functions work
#define PWMcycle 200

/*============================================================================*/
/* TABLE OF CONTENTS
 *
 * 1. GLOBAL VARIABLES
 *      List of variables used and their definition
 *
 * 2. GENERAL STRUCTURE/PROGRAM OVERVIEW
 *      Details how the program works. To be used as reference
 *
 * 3. MAIN FUNCTION
 *      This is the main function of the program
 *      i.  Navigate to beacon
 *      ii. Return to original location
 *
 * 4. HIGH PRIORITY INTERRUPT
 *      Triggered by the EUSART interrupt flag being flagged
 *
 * 5. LOW PRIORITY INTERRUPT
 *      Triggered by CAP1/CAP2 (IR Receivers). Stores their read values
 *
 *
 */
/*============================================================================*/



/*============================================================================*/
/*============================================================================*/
/*                             GLOBAL VARIABLES                               */
/*============================================================================*/
/*============================================================================*/
unsigned int cap1Buffer = 0; // Stores high byte CAP1BUFH - IR READINGS
unsigned int cap2Buffer = 0; // Stores high byte CAP2BUFH - IR READINGS
char lcdBuffer1; // Buffer variable used for LCD line 1
char lcdBuffer2; // Buffer variable used for LCD line 2

int rfidFlag = 0; // Goes HIGH when RFID is read
char rfidData[16]; // Holds data from RFID

int lcdFlag = 0; // Goes high to help clear LCD once

int count = 0; // count through path[255] array
char path[255]; // Array holds individual movements of robot
// Read in reverse to invert movements, and return to start

int search = 0; // Goes high when signal is found from both IR receivers

/* Goes high after initial sweep. This prevents recording the first turning
 * movements. */
int startFlag = 0;



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                     GENERAL STRUCTURE/PROGRAM OVERVIEW    
 * 
 * The program uses Input Capture to detect pulses from, and navigate to, an 
 * InfraRed (IR) beacon. The robot uses 2 IR receivers in order to determine
 * whether the beacon is directly ahead, and if not, compare the signal strengths
 * to decide which way to turn.
 * The robot starts by spinning left, in order to sweep the room to locate the
 * beacon. Once the robot detects that the beacon is ahead it proceeds towards it.
 * However, as the robot does not drive perfectly straight, it is necessary to 
 * make small adjustments to its direction. 
 * When the beacon is reached, the robot reads the RFID card. It then rewinds its
 * movements in order to return approximately to its starting position. Once it 
 * has returned to this location, it displays the 'disarm code', read from the
 * RFID. It also displays a 'bomb' character if the Checksum has been validated
 * correctly. This means that the 'disarm code' has been read correctly.
 *
 * Interrupts:
 *
 * 1. High Priority (RFID read)
 *
 * When the RFID card is detected/interrupt flag (serial sends 0x02 header byte),
 * the interrupt reads the 16 bits of data (10 ASCII data bits, 2 Checksum bits)
 * and stores each bit in a separate location in an array. Then, the checksum
 * calculation is performed. All the 10 ASCII data bytes are converted to 5 Hex
 * data bytes, which are then consecutively XOR'd. If the result of this calculation
 * equals the checksum Hex byte (which is also found by converting its 2 respective
 * ASCII bytes), then the data has been read correctly. A variable (rfidFlag) is
 * then set to high. If there was an error when reading the data, this variable
 * is not set to high, and error message is displayed on the LCD. The RFID interrupt
 * flag is then set back to 0.
 *
 * 2. Low Priority (IR read)
 *
 * When either the CAP1 or CAP2 (IR receivers) detects a pulse, their interrupt
 * flag is triggered. These are set to low priority, as they are constantly
 * reading. The high byte of CAP(1/2)BUF is stored in a variable called
 * cap(1/2)Buffer. The low byte (CAP1/2BUFL) is ignored, as readings were found
 * to be excessively noisy. When the value is stored, the flag is reset.
 *
 *
 * Main Function:
 *
 * 1. Initialisation
 *
 * Ports are reset and initialised. Timer 5 module is set up in order to use
 * Input Capture function (MFM - Chapter 17 PIC18F Datasheet). Interrupts are
 * intialised. Motors are also set up (e.g. PWM). LCD is set up and so is serial
 * communication (for the RFID). It also sends the LCD a custom character
 * (bomb symbol), which is later used for the checksum. The LCD then displays
 * a start screen ('STRUGGLE BOT v1').
 *
 * 2. While(rfidFlag != 1)
 *
 * Until the RFID is read, this 'while' loop iterates. This loop is essentially
 * the routine which navigates the robot to the IR beacon.
 *
 * The 'search' variable in the first 'while' loop indicates whether the IR signal
 * has been detected. It was found through experimentation, that when the signal
 * reads the value '195' in both IR receivers, the beacon is directly ahead of the
 * robot. The 'while' loop tests that this condition is true. Until this condition
 * is met, the robot spins left. The LED array also flashes on and off during this
 * loop, for aesthetic purposes. When both receivers are equal, and read 195, the
 * robot stops spinning, and exits the while loop. The 'search' variable is set to
 * '1'. The robot will now proceed towards the beacon.
 *
 * Along the way it tests certain conditions to ensure it is on the direct path
 * towards the beacon. If both cap1/2Buffer variables read 195, it moves straight.
 * If there is a difference between the IR readings, it moves in the appropriate
 * direction by enacting a slight right/left turn. Due to the characteristics of
 * the IR beacon and IR receivers, occasional anomalous conditions occur and are
 * accounted for by entering a spin to re-establish the location of the beacon.
 * It was found that sometimes these 'anomalies' would change in value, depending
 * on the terrain, and therefore had to be adjusted. The adjustments were made
 * through trial and error, using the LCD display to show readings.
 *
 * In one particular condition, the signal is considered as 'lost'. In this case,
 * the 'search' variable is reset to 0, and the robot now acts like it is searching
 * for the signal again (by spinning left, etc).
 *
 * It was also found that the robot base was prone to veering in one direction,
 * possibly due to the misalingment of the wheels, or slipping of the tracks.
 * Therefore, it was found that this could be compensated for by making the
 * turnSlightRight function to run for 3 times as long as turnSlightLeft.
 *
 * Also, while this loop iterates, each time a new movement occurs, a count
 * variable is incremented. This variable is used to move to a new location in
 * the 'path' array, which records the type of movement
 * For example, if the first movement is fullSpeedAhead, the number '3' is stored
 * in the first location in the 'path' array. If the next movement is turnSlightRight
 * the number '1' is stored in the second location in the 'path' array. And so on.
 *
 * 3. if (rfidFlag == 1)
 *
 * From the high priority interrupt handler for the RFID, a variable, 'rfidFlag'.
 * is set to high. This exits the navigation While loop, and enters this if-statement.
 * First, all unneccessary interrupts are disabled, to prevent interference. The motors
 * are stopped, as the RFID has been read.
 *
 * The robot then enters a for loop in order to return to its start position. In order
 * to do this, the path array is read in reverse, and the movements used to
 * navigate to the beacon are inverted.
 * An iteration variable, 'i', is declared (the size of the path array), in order
 * to move through the array in reverse. As the array is read, the number stored
 * at each location is tested. For example, if a '1' is read in the second location
 * in the path array, (originally a turnSlightRight in forwards) a turnSlightRightBack
 * is called. When the beginning of the path array is reached (end of for loop),
 * the robot should have returned to its start location.
 * The RFID bomb disarm code is then displayed on the LCD, while the LED array flashes.
 *
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


/*============================================================================*/
/*============================================================================*/
/*                             MAIN FUNCTION                                  */
/*============================================================================*/

/*============================================================================*/


void main(void) {

    OSCCON = 0x72; // Set internal oscillator to 8MHz
    while (!OSCCONbits.IOFS); // Wait for OSC to stablise


    setAllPorts(); // Clear all LAT registers and set all TRIS ports as outputs
    setPorts(); // Sets input ports for CAP1/CAP2
    setTimer5(); // Set up for IC falling-to-rising edge capture
    setInputCapture(); // Initialise input capture module
    setInterrupts(); // Initialise interrupts
    initPWM(); // Initialise PWM modules
    initMotor(); // Function to initialise motor structures
    LCD_Init(); // Initialise the LCD
    customChars(); // Send LCD the custom characters
    setupEUSART(); // Initialise serial communication


    // Start Screen
    SetLine(1); // Set cursor to line 1 on LCD
    LCD_String("STRUGGLE BOT v1");
    delay_s(1); // Delay for 1 second
    clearLCD(); // Clear the LCD display

    /*----------------------------------------------------------------------------*/
    /*                             FIND BEACON                                    */
    /*----------------------------------------------------------------------------*/

    while (rfidFlag != 1) {

        //USED FOR DEBUG
        /*-----------------*/
        SetLine(1);
        sprintf(lcdBuffer1, "C1 %d C2 %d       ", cap1Buffer, cap2Buffer);
        LCD_String(lcdBuffer1);
        /*-----------------*/


        while (search != 1) { // Loop to spin robot round and locate beacon
            turnLeft(&motorL, &motorR);

            if (startFlag == 1) {
                /* If this is the first sweep (startFlag is 1), then don't store
                 * the turn left movement. This prevents the robot from unnecessarily
                 * spinning on return to its initial orientation */
            } else {
                count++; // increment counter
                path[count] = 4; // store in path movement 'reference code'
            }
            LEDout(0); // for debug
            __delay_ms(89); // movement lasts about 89ms

            LEDout(15); // for debug

            SetLine(2); // cursor to line 2
            LCD_String("SEARCHING     "); // for debug - the robot is in the search loop

            if (cap1Buffer == 195 && cap2Buffer == 195) { // Both sensors found beacon
                startFlag = 0; /* If this is the first sweep (ie not signal lost), set
                                * flag to zero, and start storing all movements */
                search = 1; // Flag changes to exit while loop
                SetLine(2); // cursor to line 2
                LCD_String("BOMB LOCATED"); // beacon location is found
                Stop(&motorL, &motorR); // Stop spinning
            }
        }



        if (cap1Buffer == 195 && cap2Buffer == 195) { // beacon is ahead
            fullSpeedAhead(&motorL, &motorR); // move forward

            count++; // increment counter to move to next position in path array
            path[count] = 3; // store movement 'reference code'
            __delay_ms(89); // length of movement time

        } else {

            if (cap1Buffer > cap2Buffer) { // difference in readings of IR

                turnSlightRight(&motorL, &motorR); // make adjustment

                count++; // increment counter to move to next position in path array
                path[count] = 1; // store movement 'reference code'

                __delay_ms(89); // movement time is slightly longer, for tweaking
                __delay_ms(89);
                __delay_ms(89);


            }
            if (cap2Buffer > cap1Buffer) { // difference in readings of IR


                turnSlightLeft(&motorL, &motorR); // make adjustment

                count++; // increment counter to move to next position in path array
                path[count] = 2; // store movement 'reference code'
                __delay_ms(89);


            }
            if (cap2Buffer == cap1Buffer && cap1Buffer != 195 && cap2Buffer != 195) {
                // anomalous condition (see above)

                fullSpeedAhead(&motorL, &motorR);

                count++;
                path[count] = 3;
                __delay_ms(89);

            }
            if (cap1Buffer == 0 && cap2Buffer <= 5) {
                // anomalous condition - presented itself when signal was lost

                search = 0; // debug corrections - this occurs when signal lost
                // this causes re-entry to earlier while loop
                // note that this time, the startFlag is tripped and the movement
                // will be recorded

            }

        }




    }




    /*----------------------------------------------------------------------------*/
    /*             BEACON FOUND; RFID FLAGGED                                     */
    /*----------------------------------------------------------------------------*/

    if (rfidFlag == 1) { // set to high when RFID is read


        PIE3bits.IC1IE = 0; // Turn off the interrupt for CAP1 to prevent issues
        PIE3bits.IC2QEIE = 0; // Turn off the interrupt for CAP2 to prevent issues
        PIE1bits.RCIE = 0; // Turn off the interrupt for RFID to prevent further reads



        if (lcdFlag == 0) {
            //clears LCD once in the whole code
            clearLCD();
            lcdFlag = 1;

            __delay_ms(5); // ensure that the LCD is cleared properly
        }


        SetLine(1); // cursor to line 1
        LCD_String("REVERSING");


        Stop(&motorL, &motorR); // stop motors, as robot is next to beacon
        int i = sizeof (path); // iterative variable to count backwards through array
        while (i != 0) { // while array position is not at the beginning



            int x;
            x = path[i];
            if (x == 1) { // is the movement at position 'i' referred to as '1'?
                turnSlightLeftBack(&motorL, &motorR); // function to invert turnSlightLeft
                __delay_ms(89);


                LEDout(1); // for debug

            } else if (x == 2) {
                turnSlightRightBack(&motorL, &motorR);
                __delay_ms(89); // length of time is identical to the inverse of this movement
                __delay_ms(89); // (i.e. turnSlightRightBack)
                __delay_ms(89);


                LEDout(2); // for debug

            } else if (x == 3) {
                fullSpeedBack(&motorL, &motorR);
                __delay_ms(89);


                LEDout(3); // for debug

            } else if (x == 4) {
                turnRight(&motorL, &motorR);
                __delay_ms(89);

                LEDout(4); // for debug
            }

            i--; // decrement counter to move to next position in path array


        }


        /*-------------------------------------------*/
        // when loop has exited the RFID disarm code is displayed:


        if (lcdFlag == 0) { // as before
            //clears LCD once in the whole code
            clearLCD();
            lcdFlag = 1;

            __delay_ms(5);
        }

        SetLine(1); // Set the cursor to the LCD's first line
        LCD_String("DISARM CODE: "); // The line underneath is the code

        // DISPLAY THE CODE

        SetLine(2); // Set the cursor to the LCD's second line
        for (int j = 1; j <= 10; j++) {
            /* Iterate through the stored data array, showing only the 10
             * ASCII data bytes (i.e. the code). */

            SendLCD(rfidData[j], 1); // Diplay byte by byte
        }

        // DISPLAY THE CHECKSUM LOGO

        LCD_String(" "); // Space
        SendLCD(0x01, 1); // Send the bomb custom character
        LCD_String("CS"); // "CS" stands for "checksum"


        Stop(&motorL, &motorR); // ensure motors are stopped

        while (1) { // flash LED array indefinitely (FOR AESTHETICS)
            LEDout(15);
            __delay_ms(89);
            LEDout(0);
            __delay_ms(89);
        }


    }


}


/*============================================================================*/
/*============================================================================*/
/*                             RFID INTERRUPT                                 */
/*============================================================================*/

/*============================================================================*/

void interrupt high_priority RFIDinterrupt() {

    if (PIR1bits.RCIF) {

        /* Function reads RFID card data into an array
         and then displays the disarm code in the LCD screen */

        // Variables for serial communication
        int i = 1; // Iteration variable


        rfidData[0] = getCharSerial(); // Read the first byte

        if (rfidData[0] == 0x02) { // If the header byte is detected, proceed
            rfidData[i] = getCharSerial(); // Store the next byte in the array

            while (rfidData[i] != 0x03) {
                /* Until the end byte is received, store consecutive bytes
                 * in the buf array */

                i++; // increase the counter to move to the next location in array

                rfidData[i] = getCharSerial(); // Store the byte read from the RFID

            }


            /*---------- CHECKSUM CALCULATION ---------*/


            /* The following is the checksum calculation to ensure that the data
             * has been read correctly from the RFID. This is done by converting
             * each pair of the 10 ASCII data bytes into a single hex byte
             * (using the asciiHexBinary function), and then XORing them all
             * together. If the data has been read correctly, the result should
             * equal the single hexadecimal of the checksum bytes read at
             * location 12 and 13 of the 16 bits of data read from the RFID.*/

            char hexArray[5]; // Stores the converted array of hex data bytes

            for (int k = 1; k <= 10; k++) { // Iterates through the buffer data array

                for (int l = 0; l <= 5; l++) { // Iterates through the hexArray
                    hexArray[l] = asciiHexBinary(rfidData[k], rfidData[k + 1]); 

                }


            }

            // Store checksum data byte
            char checksumBytes = asciiHexBinary(rfidData[11], rfidData[12]);

            // Store checksum XOR values
            char checksumHolder = hexArray[1] ^ hexArray[2];

            for (int a = 3; a <= 5; a++) { // consecutively XORs the data bytes
                checksumHolder ^= hexArray[a];

            }


            if (checksumBytes == checksumHolder) { // If the result matches the databyte
                TRISCbits.RC7 = 0; // turn off the RFID input pin
                rfidFlag = 1;



            } else {

                // DISPLAY THE ERROR MESSAGE
                SetLine(1);
                LCD_String("read error."); // The checksum doesn't work; an error occured.
            }

        }

        PIR1bits.RCIF = 0; // Reset the flag to ZERO

    }

}


/*============================================================================*/
/*============================================================================*/
/*                             IR INTERRUPT                                   */
/*============================================================================*/

/*============================================================================*/

void interrupt low_priority IRinterrupt() {

    /* The low priority interrupt handles only the readings from the MFM module
     * - Input Capture (Chapter 17 of PIC18F Data Sheet). It stores the values
     * read by the IR receivers. */

    if (PIR3bits.IC1IF) { // CAP1 interrupt triggered when pulse measured

        cap1Buffer = CAP1BUFH; // Store only the high byte of CAP1BUF in a variable

        PIR3bits.IC1IF = 0; // Reset the flag

    }

    if (PIR3bits.IC2QEIF) { // CAP2 interrupt triggered when pulse measured

        cap2Buffer = CAP2BUFH; // Store only the high byte of CAP2BUF in a variable

        PIR3bits.IC2QEIF = 0; // Reset the flag

    }


}