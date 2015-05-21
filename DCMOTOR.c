#include <xc.h>
#include "HEADER.h"

/* 
 * 
 * 
 * NOTE:
 * m_L - refers to RIGHT motor
 * m_R - refers to LEFT motor!!
 * 
 * 
 *  */

/*
 * The power figures appear to be very inconsistent due to the constant
 * tweaking to compensate for terrain-related issues and non-ideal motors.
 *
 * The power changes immediately each time a different movement function
 * is called as gradual changes were proving to be very difficult to work with.
 *
 *
 *
 * ALSO NOTE: Capacitors were placed across the motors in order to filter
 * noise that the motors caused when moving. */


//Function to setup PWM
void initPWM(void) {
    // your code to set up the PWM module
    PTCON0 = 0b00000000; // free running mode, FOSC/16 1:1 prescale
    PTCON1 = 0b10000000; // enable PWM timer

    PWMCON0 = 0b01101111; // PWM0/1 and 2/3 enabled, all independent mode PWM1, PWM3 pins are enabled for PWM output
    PWMCON1 = 0x00; // special features, all 0 (default)

    //    PTPERL = 49; // base PWM period low byte
    //    PTPERH = 49 >> 8; // base PWM period high byte
    PTPERL = (0b11000111); // base PWM period low byte
    PTPERH = (0b00000000); // base PWM period high byte

}                                                       

//Function to set up motor structures
void initMotor(void) {
    motorL.power = 0; //zero power to start
    motorL.direction = 0; //set default motor direction
    motorL.dutyLowByte = (unsigned char *) (&PDC0L); //store address of PWM duty low byte
    motorL.dutyHighByte = (unsigned char *) (&PDC0H); //store address of PWM duty high byte
    motorL.dir_pin = 0; //pin RB0/PWM0 controls direction
    motorL.PWMperiod = 200; //store PWMperiod for motor

    motorR.power = 0; //zero power to start
    motorR.direction = 0; //set default motor direction
    motorR.dutyLowByte = (unsigned char *) (&PDC1L); //store address of PWM duty low byte
    motorR.dutyHighByte = (unsigned char *) (&PDC1H); //store address of PWM duty high byte
    motorR.dir_pin = 2; //pin RB2/PWM0 controls direction
    motorR.PWMperiod = 200; //store PWMperiod for motor
}                                                     

//Function to set motor PWM from values in the motor structure
void setMotorPWM(struct DC_motor *m) {
    int PWMduty;

    PWMduty = (m->power * m->PWMperiod) / 100;

    if (m->direction) {
        PWMduty = m->PWMperiod - PWMduty;
        LATB = LATB | (1 << (m->dir_pin));
    } else {
        LATB = LATB & (~(1 << (m->dir_pin)));
    }

    *(m->dutyLowByte) = PWMduty << 2;
    *(m->dutyHighByte) = PWMduty >> 6;
}                                     

//Function to stop robot
void Stop(struct DC_motor *m_L, struct DC_motor *m_R) {
    for (m_L->power; (m_L->power) > 0; (m_L->power)--) { //increase motor power until 100
        m_R->power = m_L->power;
        setMotorPWM(m_L); //pass pointer to setMotorSpeed function (not &m here)
        setMotorPWM(m_R);
        
    }
}                    

//Function to turn robot left
void turnLeft(struct DC_motor *m_L, struct DC_motor *m_R) {
    
    m_L->direction = 1; // set direction of RIGHT motor
    m_R->direction = 0; // set direction of LEFT motor


    m_R->power = 72; // set power
    m_L->power = 69; // set power
    setMotorPWM(m_L); //pass pointer to setMotorSpeed function (not &m here)
    setMotorPWM(m_R);

}                

//Function to turn robot right
void turnRight(struct DC_motor *m_L, struct DC_motor *m_R) {
    
    m_L->direction = 0;
    m_R->direction = 1;

    m_R->power = 64; // set power
    m_L->power = 69; // set power
    setMotorPWM(m_R); //pass pointer to setMotorSpeed function (not &m here)
    setMotorPWM(m_L);


}               

//Function to turn robot slightly right whilst maintaining forwards motion
void turnSlightRight(struct DC_motor *m_L, struct DC_motor *m_R) {

    m_L->direction = 0;
    m_R->direction = 0;

    m_R->power = 75;
    m_L->power = 45;

    setMotorPWM(m_R); //pass pointer to setMotorSpeed function (not &m here)
    setMotorPWM(m_L);

}     

//Function to turn robot slightly right in a backwards direction
void turnSlightRightBack(struct DC_motor *m_L, struct DC_motor *m_R) {

    m_L->direction = 1;
    m_R->direction = 1;

    m_R->power = 70;
    m_L->power = 85;
    setMotorPWM(m_R); //pass pointer to setMotorSpeed function (not &m here)
    setMotorPWM(m_L);


}     

//Function to turn robot slightly left in a backwards direction
void turnSlightLeftBack(struct DC_motor *m_L, struct DC_motor *m_R) {

    m_L->direction = 1;
    m_R->direction = 1;


    m_R->power = 90; // 70
    m_L->power = 70; // 40
    setMotorPWM(m_L); //pass pointer to setMotorSpeed function (not &m here)
    setMotorPWM(m_R);


}    

//Function to turn robot slightly left whilst maintaining forwards motion
void turnSlightLeft(struct DC_motor *m_L, struct DC_motor *m_R) {

    m_L->direction = 0;
    m_R->direction = 0;


    m_R->power = 45;
    m_L->power = 85;
    setMotorPWM(m_L); //pass pointer to setMotorSpeed function (not &m here)
    setMotorPWM(m_R);


}         

//Function for linear forward motion of robot
void fullSpeedAhead(struct DC_motor *m_L, struct DC_motor *m_R) {
    
    m_L->direction = 0;
    m_R->direction = 0;

    m_L->power = 93; // 80 75 97 95 98
    m_R->power = 95; // 90 85 99 97 95

    setMotorPWM(m_L); //pass pointer to setMotorSpeed function (not &m here)
    setMotorPWM(m_R);

}          

//Function for linear reverse motion of robot
void fullSpeedBack(struct DC_motor *m_L, struct DC_motor *m_R) {
    
    m_L->direction = 1;
    m_R->direction = 1;

    m_L->power = 98; //78 98
    m_R->power = 95; //75 95

    setMotorPWM(m_L); //pass pointer to setMotorSpeed function (not &m here)
    setMotorPWM(m_R);

}           
