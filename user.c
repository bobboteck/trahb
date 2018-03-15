/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>         /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    unsigned int dutycycle = 2000;
    
    /* TODO Initialize User Ports/Peripherals/Project here */
    /* Reset iniziale delle porte */
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    ANSEL0 = 0b00111111;
    ANSEL1 = 0b00000000;

    /* Setup analog functionality and port direction */
    
    /* PORTA tutti input per il convertitore AD */
    TRISA = 0b00111111;
    /* PORTB utilizzata per i segnali PWM, enable del PonteH, programmazione */
    TRISB = 0b00000000;
    /* Da RC0 a RC4 sono OUT per pilotare i led dei sensori
     * (RC6 e RC7 possono essere usati per la seriale) */
    TRISC = 0b10100000;
    /* Tutti OUT usato per il display */
    TRISD = 0b00000000;
    /* Impostati come OUT per i 3 LED di monitoraggio della batteria */
    TRISE = 0b11111000;

    /* Initialize peripherals */

    /* Configurazione Modulo AD */
    /*OpenADC(ADC_CONV_SINGLE_SHOT | ADC_MODE_SINGLE_CH | ADC_CONV_SEQ_SEQM1 | ADC_INT_OFF,
            ADC_REF_VDD_VSS | ADC_FIFO_DIS | ADC_TRIG_TMR_5,
            ADC_RIGHT_JUST | ADC_10_TAD | ADC_FOSC_8);  */
    /* CONFIGURAZIONE PWM */
    // Power Timer Control
    PTCON0 = 0b11110000; // 1:16 postscaler, 1:1 prescale, free running
    PTCON1 = 0b10000000; // PWM time base ON, count up
    // Impostazione periodo
    PTPERH = 0x01;
    PTPERL = 0xff;
    // PWM Control
    //PWMCON0 = 0b01000000;	// PWM1:PWM5 Abilitati
    PWMCON0 = 0b00110000; // PWM0:PWM4 Abilitati
    PWMCON1 = 0b11110000; // 1:16 postscaler, updates abilitati, override asincrono
    DTCON = 0x00; // dead time 0ns
    // Output Override
    OVDCOND = 0b00111111; // L'output è controllato dal DutyCycle e TimeBase
    OVDCONS = 0b00000000; // Tutti i pin sono INACTIVE quando in override
    // Fault Configuration
    FLTCONFIG = 0x00; // Fault A e B disabilitati
    // Impostazione DutyCycle
    PDC0H = dutycycle >> 8;PDC0L = dutycycle;
    PDC1H = dutycycle >> 8;PDC1L = dutycycle;
    
    /* Configure the IPEN bit (1=on) in RCON to turn on/off int priorities */

    /* Enable interrupts */
}


