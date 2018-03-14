/* --------------------------------------------------------------------------
** 
** 	Header per la gestione del terminale del nuovo Robot comprende:
**	- Display a matrice di punti 2x16
**	- 3 LED 
**	- 1 Buzzer
**	- 4 Pulsanti
** 
**	File  : terminale.h	Ver.: 0.0.1	Ultima modifica: 26/04/06
**	
**	Autore: Roberto D'Amico (iw0gtf@libero.it)
**
**	Dipendenze: delays.h - Ricordarsi di includerlo nel progamma pricipale
**				DMLCD2R.H
**
**	Porte usate per default:
**	PORTD bit 4-7 (OUT) collegato ai pin LCD data bit 4-7, 74574 e ai 4 pulsanti
**	PORTD bit 0	(OUT) collegato al pin LCD RS
**	PORTD bit 1 (OUT) collegato al pin LCD EN
**	PORTD bit 2 (OUT) collegato al pin CLK del 74574
**	PORTD bit 3 (IN) collegato al comune dei pulsanti
**
**	Per usare queste routine:
**	- settare le porte di I/O (TRISx) in funzione delle porte
**	  che state usando;
**
**      NOTE
**	
**	La configurazione è scritta per funzionare con la RoboBoard 2.1 e il
**	display collgato sula porta D
**
**	Novità
**	
**	
** ----------------------------------------------------------------------- */
#include "DMLCD2R.h"

#define TCLK754		PORTDbits.RD2
#define TLED_VERDE	PORTDbits.RD4
#define TLED_GIALLO	PORTDbits.RD5
#define TLED_ROSSO	PORTDbits.RD6
#define TBUZZER		PORTDbits.RD7
#define TASTO_IN	PORTDbits.RD3
#define TASTO_NULL	0			// Se non viene premuto nessun tasto
#define TASTO_1		1			// Tasto 
#define TASTO_2		2			// Tasto 
#define TASTO_3		3			// Tasto 
#define TASTO_4		4			// Tasto 

/**************************************************************************
*** Esegue un colpo di clock per mandare portare il dato in out			***
**************************************************************************/
void pulse574(void)
{
	TCLK754=0;
	Delay10TCYx(20);
	TCLK754=1;
	Delay10TCYx(20);
	TCLK754=0;
}
/**************************************************************************
*** Gestione dei tre led contemporanea 1=on 0=off			  			***
***									  									***
*** rosso: led rosso, 1=ON 0=OFF					  					***
*** giallo: led giallo, 1=ON 0=OFF					  					***
*** verde: led verde, 1=ON 0=OFF					  					***
**************************************************************************/
void led_terminale(unsigned char rosso, unsigned char giallo, unsigned char verde)
{
	if(rosso==1)
		TLED_ROSSO=1;
	else
		TLED_ROSSO=0;

	if(giallo==1)
		TLED_GIALLO=1;
	else
		TLED_GIALLO=0;

	if(verde==1)
		TLED_VERDE=1;
	else
		TLED_VERDE=0;

	pulse574();
}
/**************************************************************************
*** Abilita il display a ricevere il dato inviato		  	  			***
***									  									***
*** Restituisce: Il char del tasto premuto, in caso nessun tasto è stato***
***				 restituisce TASTO_NULL									***
**************************************************************************/
unsigned char tasto_read(void)
{
unsigned char tasto;

	tasto=TASTO_NULL;

	PORTD=0b01110000;	// Controllo tasto 4
	if(TASTO_IN==0)
	{
		tasto=TASTO_4;
		while(!TASTO_IN);
	}
	PORTD=0b11100000;	// Controllo tasto 1
	if(TASTO_IN==0)
	{
		tasto=TASTO_1;
		while(!TASTO_IN);
	}
	PORTD=0b11010000;	// Controllo tasto 2
	if(TASTO_IN==0)
	{
		tasto=TASTO_2;
		while(!TASTO_IN);
	}
	PORTD=0b10110000;	// Controllo tasto 3
	if(TASTO_IN==0)
	{
		tasto=TASTO_3;
		while(!TASTO_IN);
	}

	return tasto;
}
