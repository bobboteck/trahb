/* --------------------------------------------------------------------------
** 
** 	Programma di sviluppo sul nuovo Robot Fire
**	Scritto in C18 utilizza una RoboBoard 2.1 con PIC 18F2640 a 40MHz
** 
**	File  : EEPIIC.C	Ver.: 0.0.1	Ultima modifica: 22/03/07
**	
**	Autore: Roberto D'Amico (iw0gtf@libero.it)
**
**	Dipendenze: p18F4620.h
**				delays.h
**				terminale.h
**
**	Porte usate:
**	PORTC bit 1	(OUT) PWM
**	PORTC bit 2 (OUT) PWM
**	PORTD bit 4-7 (OUT) collegato ai pin LCD data bit 4-7, 74574 e ai 4 pulsanti
**	PORTD bit 0	(OUT) collegato al pin LCD RS
**	PORTD bit 1 (OUT) collegato al pin LCD EN
**	PORTD bit 2 (OUT) collegato al pin CLK del 74574
**	PORTD bit 3 (IN) collegato al comune dei pulsanti
**	PORTE bit 0-2 (OUT) collegato ai led della RB2.1
**	PORTB bit 4 (OUT) enable motori
**
**	NOTE
**	
**	Definizione delle funzioni base del terminale nel file terminale.h
**	
**
**	Novità
**	-------
**	- 
**
** ----------------------------------------------------------------------- */
#include <p18F4620.h>
#include <delays.h>
#include "terminale.h"

#pragma config OSC	= HSPLL		// Usare con quarzo da 10MHz e PPL 4x
#pragma config WDT	= OFF
#pragma config PWRT	= ON
#pragma config LVP	= OFF
#pragma config MCLRE = ON
//#pragma config BOR	= OFF
//#pragma config BORV	= 42

#define LED_VERDE	PORTEbits.RE0
#define LED_GIALLO	PORTEbits.RE1
#define LED_ROSSO	PORTEbits.RE2
#define TRIG_S1		LATBbits.LATB0		// OUT
#define PULSE_S1	PORTBbits.RB1 		// IN
#define TRIG_S2		LATBbits.LATB2		// OUT
#define PULSE_S2	PORTBbits.RB3 		// IN

unsigned char IRS1,gp2_1_read,IRS2,gp2_2_read;

/*************************************************************/
void GP2D02_dx (void)
{
unsigned char i;

	IRS1 = 0;
	for (i=0;i<8;i++)
	{
		TRIG_S1 = 1;
		Delay10TCYx(125);
		TRIG_S1 = 0;
		Delay10TCYx(125);

 		if (PULSE_S1 == 1) IRS1 += 1;
 
 		if (i < 7)
 		{
 			IRS1 = IRS1 << 1;
		}
	}

	TRIG_S1 = 1;
	Delay100TCYx(250);
	TRIG_S1 = 0;

	gp2_1_read=IRS1;

	if (IRS1 < 85) IRS1 = 85;
	IRS1 = 1387 / (IRS1-71);
	if (IRS1>85) IRS1=100;
	if (IRS1<8) IRS1=8;
} 
/*************************************************************/
void GP2D02_sx (void)
{
unsigned char i;

	IRS2 = 0;
	for (i=0;i<8;i++)
	{
		TRIG_S2 = 1;
		Delay10TCYx(125);
		TRIG_S2 = 0;
		Delay10TCYx(125);

 		if (PULSE_S2 == 1) IRS2 += 1;
 
 		if (i < 7)
 		{
 			IRS2 = IRS2 << 1;
		}
	}

	TRIG_S2 = 1;
	Delay100TCYx(250);
	TRIG_S2 = 0;

	gp2_2_read=IRS2;

	if (IRS2 < 85) IRS2 = 85;
	IRS2 = 1387 / (IRS2-71);
	if (IRS2>85) IRS2=100;
	if (IRS2<8) IRS2=8;
} 
/*************************************************************/
void main()
{
char buf_msg_1[]= "GP2D02 Ver 0.0.1";
char buf_msg_2[]= "In attesa ...   ";
char buf_tasto1[]="DX     SX       ";
unsigned char cod_tasto, modifica;
unsigned char dato=0;
unsigned char indirizzo=0;

	ADCON1=0x06;			// PORTA all I/O mode, PORTE I/O mode

	PORTA=0x00;
	PORTB=0x00;
	PORTC=0x00;
	PORTD=0x00;
	PORTE=0x00;

	TRISA=0x00;
	TRISB=0b00001010;
	TRISC=0b00000000;
	TRISD=0b00001000;		// Completamente utilizzata per il terminale, per inormazioni sulla configurazione guarda terminale.h
	TRISE=0x00;

	lcd_init();
	lcd_clear();

	lcd_puts(buf_msg_1);		// Mostra messaggio di start
	lcd_pos_cur(1,0);
	lcd_puts(buf_msg_2);

	led_terminale(0,0,0);		// Lampeggio led del terminale TEST
	led_terminale(1,0,0);
	Delay10KTCYx(250);
	led_terminale(0,0,0);
	Delay10KTCYx(250);
	led_terminale(0,1,0);
	Delay10KTCYx(250);
	led_terminale(0,0,0);
	Delay10KTCYx(250);
	led_terminale(0,0,1);
	Delay10KTCYx(250);
	led_terminale(0,0,0);
	Delay10KTCYx(250);

	lcd_pos_cur(1,0);
	lcd_puts(buf_tasto1);

	while(1)
	{

		GP2D02_dx();
		//lcd_put_uchar(IRS1,1,9);
		//lcd_put_uchar(gp2_1_read,1,13);
		lcd_put_uchar(gp2_1_read,1,3);
		Delay10KTCYx(50);
		GP2D02_sx();
		//lcd_put_uchar(IRS2,1,9);
		lcd_put_uchar(gp2_2_read,1,10);
		
/*		cod_tasto=tasto_read();
		if(cod_tasto!=0)
		{
			if(cod_tasto==1)
			{
				led_terminale(1,0,0);
				lcd_pos_cur(1,0);
				lcd_puts(buf_tasto1);
				GP2D02();
				lcd_put_uchar(IRS1,1,9);
				led_terminale(0,0,0);
			}
		} */
	}
}

