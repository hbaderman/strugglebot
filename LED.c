#include <xc.h>
#include "HEADER.h"

void LEDout(int number )
{
    //Skeleton function for displaying a binary number
    //on the LED array
    //you can write values to the whole port at once using by
    //changing LATN (e.g. LATC=something;)
    //or
    //individual pins can be changed using
    //LATNbits.LATNx (e.g. LATDbits.LATD2=1;)

    LATC=(number&0b00111100)<<2|(LATC&0b00001111);
    LATD=((number&0b00000011)<<2|(number&0b11000000)>>2)|(LATD&0b11000011);

}
