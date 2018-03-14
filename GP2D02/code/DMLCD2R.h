/* --------------------------------------------------------------------------
** 
** 	Header per la gestione di Display a matrice di punti su 2 righe
**  Versione modificata per il compilatore C18 
** 
**	File  : DMLCD2R.H	Ver.: 1.2.1	Ultima modifica: 10/04/06
**		
**	Autore: Roberto D'Amico (iw0gtf@libero.it)
**
**	Dipendenze: delays.h - Ricordarsi di includerlo nel progamma pricipale
**
**	Porte usate per default:
**	PORTD bit 4-7 collegato ai pin LCD data bit 4-7
**	PORTD bit 0 collegato al pin LCD RS
**	PORTD bit 1 collegato al pin LCD EN
**	
**	Per usare queste routine:
**	- settare le porte di I/O (TRISx) in funzione delle porte
**	  che state usando;
**	- modificare le impostazioni di lcd_init() se volete usare il display
**	  in modalità diversa da quella di default;
**	- modificare le DEFINE (in questo file) LCD_RS, LCD_EN e 
**	  LCD_PORT_DATA, se volete usare porte del PIC diverse da quelle di
**	  default;
**	- la funzione lcd_write() usa i 4 bit più significativi della porta
**	  indicata da LCD_PORT_DATA.
**
**      NOTE
**	
**	L configurazione è scritta per funzionare con la RoboBoard 2.1 e il
**	display collgato sula porta D
**
**	Novità
**	
**	- Aggiunta la funzione lcd_put_uchar		(1.2)
**	- Aggiunta la funzione lcd_put_uint			(1.2)
**	- Cambiate le chiamate al delay per il C18	(1.2.1)
**	
** ----------------------------------------------------------------------- */

/* ========================================================================================= */
/* Modificare questi paramentri per cambiare le porte utilizzate per la gestione del display */
/* !!!ATTENZIONE!!! - Per i dati vengono utilizzati i 4 bit più significativi della porta    */
/* indicata da LCD_PORT_DATA								     							 */
/* ----------------------------------------------------------------------------------------- */
#define LCD_RS			PORTDbits.RD0	// Register select
#define LCD_EN			PORTDbits.RD1	// Enable
#define	LCD_PORT_DATA	PORTD	// Porta di connesione per le linee Data

#define	LCD_CMD			0		// Definisce modalità Comando
#define	LCD_DATA		1		// Definisce modalità Data 
/* ========================================================================================= */

/* ========================================================================================= */
/* Comandi da inviare al display per la gestione di alcune funzioni			     			 */
/* !!!ATTENZIONE!!! - Per ogni comando devono essere usati in OR un elemento per ogni coppia */
/* di settaggi.										     									 */
/* ----------------------------------------------------------------------------------------- */

#define	LCD_CMD_CLEAR		0b00000001	// Comando Clear, per la cancellazione del display
#define	LCD_CMD_HOME		0b00000010	// Comando Home, porta il cursore nella posizione Home
#define LCD_CMD_ADDRESS		0b10000000	// Per posizionare il cursore all'indirizzo stabilito

// ---------------
// Entry Mode Set
// ---------------
#define LCD_CMD_EMS			0b00000100	// Comando per Entry Mode Set, da mettere in OR con i seguenti

#define LCD_EMS_INCREASE	0b00000010	// Imposta la scrittura da sinistra a destra
#define LCD_EMS_DECREASE	0b00000000	// Imposta la scrittura da destra a sinistra

#define LCD_EMS_SHIFT_ON	0b00000001	// Consente al testo di scorrere
#define LCD_EMS_SHIFT_OFF	0b00000000	// Impedisce al testo di scorrere
/* ----------------------------------------------------------------------------------------- */

// ------------------------
// Display On/Off Controll
// ------------------------
#define LCD_CMD_DC			0b00001000	// Comando per Display On/Off Controll da mettere in OR con i seguenti

#define LCD_DC_DISPLAY_ON	0b00000100	// Imposta la modalità Display On
#define LCD_DC_DISPLAY_OFF	0b00000000	// Imposta la modalità Display Off, i dati visalizzati restano comunque in memoria

#define LCD_DC_CURSOR_ON	0b00000010	// Imposta la modalità Cursore On
#define LCD_DC_CURSOR_OFF	0b00000000	// Imposta la modalità Cursore Off

#define LCD_DC_CURBLINK_ON	0b00000001	// Imposta la modalità Cursore lampeggiante
#define LCD_DC_CURBLINK_OFF	0b00000000	// Imposta la modalità Cursore non lampeggiante
/* ----------------------------------------------------------------------------------------- */

// ------------------------
// Cursor or Display Shift
// ------------------------
#define LCD_CMD_CDS		0b00010000	// Comando per lo shift del display o del cursore

#define LCD_CDS_MCL		0b00000000	// Sposta il cursore a sinistra
#define LCD_CDS_MCR		0b00000100	// Sposta il cursore a destra

#define LCD_CDS_MDL		0b00001000	// Sposta il display a sinistra
#define LCD_CDS_MDR		0b00001100	// Sposta il display a destra
/* ----------------------------------------------------------------------------------------- */

// -------------
// Function Set
// -------------
#define LCD_CMD_FS		0b00100000	// Comando per la definizione del modo di funzionamento

#define LCD_FS_DL8		0b00010000	// Imposta la modalità di comunicazione dati a 8 bit
#define LCD_FS_DL4		0b00000000	// Imposta la modalità di comunicazione dati a 4 bit

#define LCD_FS_NRM		0b00001000	// Imposta la modalità a + righe
#define LCD_FS_NRS		0b00000000	// Imposta la modalità a 1 riga

#define LCD_FS_F5X10	0b00000100	// Imposta la dimensione Font 5x10
#define LCD_FS_F5X7		0b00000000	// Imposta la dimensione Font 5x7
/* --------------------------------------------------------------------- */
/* ===================================================================== */

/**************************************************************************
*** Abilita il display a ricevere il dato inviato		  	  			***
**************************************************************************/
void lcd_strobe(void)
{
	LCD_EN=1;
//	DelayUs(1);
	Delay10TCYx(1);
	LCD_EN=0;
}

/**************************************************************************
*** Invia un byte al display in modalita 4 bit				  			***
***									  									***
*** c: byte da inviare al display					  					***
**************************************************************************/
void lcd_write(unsigned char c)
{
	LCD_PORT_DATA=(LCD_PORT_DATA & 0x0F) | (c & 0xF0);
	lcd_strobe();
	LCD_PORT_DATA=(LCD_PORT_DATA & 0x0F) | (c << 4);
	lcd_strobe();
//	DelayUs(50);
	Delay10TCYx(50);
}

/**************************************************************************
*** Invia un comando al display 					  					***
***									  									***
*** c: il comando da inviare al display					  				***
**************************************************************************/
void lcd_command(char c)
{
	LCD_RS=LCD_CMD;
	lcd_write(c);
}

/**************************************************************************
*** Posiziona il cursore alla riga e colonna indicata dai parametri  	***
***									  									***
*** row: indica la righa a cui posizionare il cursore (indice da 0) 	***
*** col: indica la colonna a cui posizionare il	cursore (indice da 0)	***
**************************************************************************/
void lcd_pos_cur(unsigned char row, unsigned char col)
{
unsigned char pos;

	LCD_RS=LCD_CMD;

	// imposta indirizzo base 
	switch(row)
	{
		case 0 : pos=0x00;
			break;
		case 1 : pos=0x40;
			break;
	}

	lcd_write(LCD_CMD_ADDRESS | (pos+col));
}

/**************************************************************************
*** Canella il display e posiziona il cursore alla posizione home  	  	***
**************************************************************************/
void lcd_clear(void)
{
	LCD_RS=LCD_CMD;
	lcd_write(LCD_CMD_CLEAR);
//	DelayMs(3);
	Delay10KTCYx(3);
}

/**************************************************************************
*** Posiziona il cursore alla posizione home  	  						***
**************************************************************************/
void lcd_home(void)
{
	LCD_RS=LCD_CMD;
	lcd_write(LCD_CMD_HOME);
//	DelayMs(3);
	Delay10KTCYx(3);
}

/**************************************************************************
*** Scrive un carattere sul display					  					***
***									  									***
*** c: carattere da visualizzare sul display	 			  			***
**************************************************************************/
void lcd_putch(unsigned char c)  // Modificata da char a unsigned
{
	LCD_RS=LCD_DATA;
	lcd_write(c);
}

/**************************************************************************
*** Scrive una stringa di caratteri sul display				  			***
***									  									***
*** s: stringa da visualizzare sul display				  				***
**************************************************************************/
void lcd_puts(const char *s)
{
	LCD_RS=LCD_DATA;
	while(*s)
		lcd_write(*s++);
}

/**************************************************************************
*** Scrive sul display il numero passato come parametro in forma di		***
*** striga, posizionandolo nella possizione indicata					***
***																		***
*** numero: numero da visualizzare sul display							***
*** riga: indica la righa a cui posizionare il cursore (indice da 0) 	***
*** colonna: indica la colonna a cui posizionare il cursore (indice da 0)**
**************************************************************************/
void lcd_put_uchar(unsigned char numero, unsigned char riga, unsigned char colonna)
{
	/***** Posiziona il cursore *****/
	lcd_pos_cur(riga,colonna);

	/***** Estrazione centinaia *****/
	if((numero/100)>0)
		lcd_putch('0'+(numero/100));
	else
		lcd_putch('0');
	numero-=(numero/100)*100;
	
	/***** Estrazione decine *****/
	if((numero/10)>0)
		lcd_putch('0'+(numero/10));
	else
		lcd_putch('0');
	numero-=(numero/10)*10;

	/***** Estrazione unità *****/
	if(numero!=0)
		lcd_putch('0'+numero);
	else
		lcd_putch('0');
}

/**************************************************************************
*** Scrive sul display il numero passato come parametro in forma di		***
*** striga, posizionandolo nella possizione indicata					***
***																		***
*** numero: numero da visualizzare sul display							***
*** riga: indica la righa a cui posizionare il cursore (indice da 0) 	***
*** colonna: indica la colonna a cui posizionare il cursore (indice da 0)**
**************************************************************************/
void lcd_put_uint(unsigned int numero, unsigned char riga, unsigned char colonna)
{
	/***** Posiziona il cursore *****/
	lcd_pos_cur(riga,colonna);

	/***** Estrazione decine di migliaia *****/
	if((numero/10000)>0)
		lcd_putch('0'+(numero/10000));
	else
		lcd_putch('0');
	numero-=(numero/10000)*10000;

	/***** Estrazione migliaia *****/
	if((numero/1000)>0)
		lcd_putch('0'+(numero/1000));
	else
		lcd_putch('0');
	numero-=(numero/1000)*1000;

	/***** Estrazione centinaia *****/
	if((numero/100)>0)
		lcd_putch('0'+(numero/100));
	else
		lcd_putch('0');
	numero-=(numero/100)*100;
	
	/***** Estrazione decine *****/
	if((numero/10)>0)
		lcd_putch('0'+(numero/10));
	else
		lcd_putch('0');
	numero-=(numero/10)*10;

	/***** Estrazione decine *****/
	if(numero!=0)
		lcd_putch('0'+numero);
	else
		lcd_putch('0');
}

/**************************************************************************
*** Inizializza il display in modalita 4 bit il cursore è OFF, per	  	***
*** attivare il cursore cambiare il commento				  			***
**************************************************************************/
void lcd_init(void)
{
	LCD_RS=LCD_CMD;
//	DelayMs(15);		// Ritardo per l'accensione del display
	Delay10KTCYx(15);

	LCD_PORT_DATA=(LCD_PORT_DATA & 0x0F) | 0x30;

	lcd_strobe();
//	DelayMs(5);
	Delay10KTCYx(5);
	lcd_strobe();
//	DelayUs(100);
	Delay10TCYx(100);
	lcd_strobe();
//	DelayMs(5);
	Delay10KTCYx(5);

	LCD_PORT_DATA=(LCD_PORT_DATA & 0x2F) | 0x20;	// set 4 bit mode

	lcd_strobe();
//	DelayUs(40);
	Delay10TCYx(40);

	// 4 bit mode, 2 or more lines, 5x8 font
	lcd_write(LCD_CMD_FS | LCD_FS_DL4 | LCD_FS_NRM | LCD_FS_F5X7);
	// display off
	lcd_write(LCD_CMD_DC | LCD_DC_DISPLAY_OFF | LCD_DC_CURSOR_OFF | LCD_DC_CURBLINK_OFF);
	// display on, curson off, blink off
	lcd_write(LCD_CMD_DC | LCD_DC_DISPLAY_ON | LCD_DC_CURSOR_OFF | LCD_DC_CURBLINK_OFF);
	// shift entry mode, display not shifted
	lcd_write(LCD_CMD_EMS | LCD_EMS_INCREASE | LCD_EMS_SHIFT_OFF);
}
