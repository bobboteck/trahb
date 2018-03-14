#define USE_OR_MASKS
#include "p18f4431.h"
#include "delays.h"
#include "DMLCD2R.h"
#include "plib/adc.h"

#pragma config OSC=HSPLL	// Oscillatore PLL 4x attivo
#pragma	config FCMEN=OFF	//
#pragma	config IESO=OFF		//
#pragma config PWRTEN=ON	// Power UP timer abilitato
#pragma config BOREN=OFF	// Brown out Reset disabilitato
#pragma config BORV=42		// Tensione per Brown out Reset 4.2 Volt
#pragma config WDTEN=OFF	// Whatchdog disattivato
#pragma config WINEN=OFF	// Whatchdog Timer Enable Window
#pragma config WDPS=1		// Whatchdog postscaler
#pragma config T1OSCMX=OFF	//
#pragma config HPOL=HIGH	// High-Side Transistors Polarity
#pragma config PWMPIN=OFF	// PWM output pins Reset state control
#pragma config MCLRE=ON		// MCLR Enable
#pragma config EXCLKMX=RD0	// External clock MUX bit (RD0 o RC3)
#pragma config PWM4MX=RB5	// PWM4 MUX bit (RD5 o RB5)
#pragma config SSPMX=RC7	// SOLO PER 40pin, imposta RC4 e RC5 per l'I2C perchè sono multiplexati
#pragma config FLTAMX=RC1	// FLTA MUX bit (RD4 o RC1)
#pragma config STVREN=OFF	// Stack overflow reset
#pragma config LVP=OFF		// Programmazione LVP disabilitata
#pragma config DEBUG=OFF	//
#pragma config CP0=OFF		// Da qui in giu protezione aree di memoria
#pragma config CP1=OFF		//
#pragma config CP2=OFF		//
#pragma config CP3=OFF		//
#pragma config CPB=OFF		//
#pragma config CPD=OFF		//
#pragma config WRT0=OFF		//
#pragma config WRT1=OFF		//
#pragma config WRT2=OFF		//
#pragma config WRT3=OFF		//
#pragma config WRTB=OFF		//
#pragma config WRTC=OFF		//
#pragma config WRTD=OFF		//
#pragma config EBTR0=OFF	//
#pragma config EBTR1=OFF	//
#pragma config EBTR2=OFF	//
#pragma config EBTR3=OFF	//
#pragma config EBTRB=OFF	//
/**********
 * DEFINE *
 **********/
#define LED_ROSSO   LATEbits.LATE2
#define LED_GIALLO  LATEbits.LATE1
#define LED_VERDE   LATEbits.LATE0
#define SLED_SSX    LATCbits.LATC0
#define SLED_SX     LATCbits.LATC1
#define SLED_CX     LATCbits.LATC2
#define SLED_DX     LATCbits.LATC3
#define SLED_DDX    LATCbits.LATC4
#define EMOTDX      LATBbits.LATB5
#define EMOTSX      LATBbits.LATB4
#define SPEED_MAX   1800
#define SPEED_MED   1600
#define SPEED_MIN   1350

//PROTOTIPI
unsigned int SensoreRigaSSX(void);
unsigned int SensoreRigaSX(void);
unsigned int SensoreRigaCX(void);
unsigned int SensoreRigaDX(void);
unsigned int SensoreRigaDDX(void);
unsigned int GetCaricaBatteria(void);
void ShowCaricaBatteria(unsigned int tensione);
void ImpostaVelocita(unsigned int *valori);

void main(void)
{
    unsigned int batteryVolt = 0;
    unsigned int sensors[5];
    unsigned int dutycycle = 2000;
    /* Reset iniziale delle porte */
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    ANSEL0 = 0b00111111;
    ANSEL1 = 0b00000000;
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
    /* Configurazione Modulo AD */
    OpenADC(ADC_CONV_SINGLE_SHOT | ADC_MODE_SINGLE_CH | ADC_CONV_SEQ_SEQM1 | ADC_INT_OFF,
            ADC_REF_VDD_VSS | ADC_FIFO_DIS | ADC_TRIG_TMR_5,
            ADC_RIGHT_JUST | ADC_10_TAD | ADC_FOSC_8);
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
    OVDCOND = 0b00111111; // L'output Ã? controllato dal DutyCycle e TimeBase
    OVDCONS = 0b00000000; // Tutti i pin sono INACTIVE quando in override
    // Fault Configuration
    FLTCONFIG = 0x00; // Fault A e B disabilitati
    // Impostazione DutyCycle
    PDC0H = dutycycle >> 8;PDC0L = dutycycle;
    PDC1H = dutycycle >> 8;PDC1L = dutycycle;

    /* Accende il LED4 per indicare che il programma è avviato */
    LED_ROSSO = 1;
    LED_GIALLO = 1;
    LED_VERDE = 1;
    Delay10KTCYx(250);
    Delay10KTCYx(250);
    Delay10KTCYx(250);
    Delay10KTCYx(250);
    LED_ROSSO = 0;
    LED_GIALLO = 0;
    LED_VERDE = 0;
    /* Abilita i motori */
    EMOTDX = 1;
    EMOTSX = 1;

    while (1)
    {
        sensors[0] = SensoreRigaSSX();
        sensors[1] = SensoreRigaSX();
        sensors[2] = SensoreRigaCX();
        sensors[3] = SensoreRigaDX();
        sensors[4] = SensoreRigaDDX();
        ImpostaVelocita(sensors);
        // Verifica carica batteria
        batteryVolt = GetCaricaBatteria();
        ShowCaricaBatteria(batteryVolt);
    }
}

/****************************
 * FUNZIONI
 ****************************/
unsigned int SensoreRigaSSX(void)
{
    int result = 0;
    SLED_SSX = 1;
    SelChanConvADC(0);
    Delay10TCYx(5); // Delay for 50TCY
    // Gruppo A
    ADCON0bits.ACMOD0 = 0;
    ADCON0bits.ACMOD1 = 0;
    ConvertADC();
    while (BusyADC());
    result = ReadADC();
    SLED_SSX = 0;
    return result;
}

unsigned int SensoreRigaSX(void)
{
    int result = 0;
    SLED_SX = 1;
    SelChanConvADC(1);
    Delay10TCYx(5); // Delay for 50TCY
    // Gruppo B
    ADCON0bits.ACMOD0 = 1;
    ADCON0bits.ACMOD1 = 0;
    ConvertADC();
    while (BusyADC());
    result = ReadADC();
    SLED_SX = 0;
    return result;
}

unsigned int SensoreRigaCX(void)
{
    int result = 0;
    SLED_CX = 1;
    SelChanConvADC(2);
    Delay10TCYx(5); // Delay for 50TCY
    // Gruppo C
    ADCON0bits.ACMOD0 = 0;
    ADCON0bits.ACMOD1 = 1;
    ConvertADC();
    while (BusyADC());
    result = ReadADC();
    SLED_CX = 0;
    return result;
}

unsigned int SensoreRigaDX(void)
{
    int result = 0;
    SLED_DX = 1;
    SelChanConvADC(3);
    Delay10TCYx(5); // Delay for 50TCY
    // Gruppo D
    ADCON0bits.ACMOD0 = 1;
    ADCON0bits.ACMOD1 = 1;
    ConvertADC();
    while (BusyADC());
    result = ReadADC();
    SLED_DX = 0;
    return result;
}

unsigned int SensoreRigaDDX(void)
{
    int result = 0;
    SLED_DDX = 1;
    SelChanConvADC(4);
    Delay10TCYx(5); // Delay for 50TCY
    // Gruppo A
    ADCON0bits.ACMOD0 = 0;
    ADCON0bits.ACMOD1 = 0;
    ConvertADC();
    while (BusyADC());
    result = ReadADC();
    SLED_DDX = 0;
    return result;
}

unsigned int GetCaricaBatteria(void)
{
    int result = 0;
    SelChanConvADC(5);
    Delay10TCYx(5); // Delay for 50TCY
    // Gruppo B
    ADCON0bits.ACMOD0 = 1;
    ADCON0bits.ACMOD1 = 0;
    ConvertADC();
    while (BusyADC());
    result = ReadADC();
    return result;
}

void ShowCaricaBatteria(unsigned int tensione)
{
    if (tensione >= 800)
    {
        LED_ROSSO = 1;
        LED_GIALLO = 1;
        LED_VERDE = 1;
    }
    else if (tensione < 800 && tensione >= 715)
    {
        LED_ROSSO = 1;
        LED_GIALLO = 1;
        LED_VERDE = 0;
    }
    else if (tensione < 715 && tensione >= 696)
    {
        LED_ROSSO = 1;
        LED_GIALLO = 0;
        LED_VERDE = 0;
    }
    else
    {
        LED_ROSSO = 0;
        LED_GIALLO = 0;
        LED_VERDE = 0;
    }
}

void ImpostaVelocita(unsigned int *valori)
{
    unsigned int dcsx, dcdx;

    if(valori[0]>900)
    {
        dcsx=SPEED_MIN; dcdx=SPEED_MAX;
        PDC0H = dcdx >> 8;PDC0L = dcdx;
        PDC1H = dcsx >> 8;PDC1L = dcsx;
    }
    else if(valori[1]>900)
    {
        dcsx=SPEED_MED; dcdx=SPEED_MAX;
        PDC0H = dcdx >> 8;PDC0L = dcdx;
        PDC1H = dcsx >> 8;PDC1L = dcsx;
    }
    else if(valori[2]>980)
    {
        dcsx=SPEED_MAX; dcdx=SPEED_MAX;
        PDC0H = dcdx >> 8;PDC0L = dcdx;
        PDC1H = dcsx >> 8;PDC1L = dcsx;
    }
    else if(valori[3]>900)
    {
        dcsx=SPEED_MAX; dcdx=SPEED_MED;
        PDC0H = dcdx >> 8;PDC0L = dcdx;
        PDC1H = dcsx >> 8;PDC1L = dcsx;
    }
    else if(valori[4]>900)
    {
        dcsx=SPEED_MAX; dcdx=SPEED_MIN;
        PDC0H = dcdx >> 8;PDC0L = dcdx;
        PDC1H = dcsx >> 8;PDC1L = dcsx;
    }
    else
    {
        dcsx=SPEED_MAX; dcdx=SPEED_MAX;
        PDC0H = dcdx >> 8;PDC0L = dcdx;
        PDC1H = dcsx >> 8;PDC1L = dcsx;
    }
}
