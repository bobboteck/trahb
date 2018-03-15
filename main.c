/*
 * File:   main.c
 * Author: bobboteck
 *
 * Created on March 14, 2018, 4:46 PM
 */

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/


void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    /* TODO <INSERT USER APPLICATION CODE HERE> */

    while(1)
    {
        /* Accende il LED4 per indicare che il programma è avviato */
        LED_ROSSO = 1;
        LED_GIALLO = 1;
        LED_VERDE = 1;
        __delay_ms(1000);
        LED_ROSSO = 0;
        LED_GIALLO = 0;
        LED_VERDE = 0;
        __delay_ms(1000);
    }
}
