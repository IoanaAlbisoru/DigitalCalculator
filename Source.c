//Displaying "HELLO" on LCD for Dragon12+ Trainer Board 
//with HCS12 Serial Monitor Program installed. This code is for CodeWarrior IDE
//Modified from Mazidi's book with contribution from Travis Chandler
//On Dragon12+ LCD data pins of D7-D4 are connected to Pk5-Pk2, En=Pk1,and RS=Pk0,

 
#include <hidef.h>      /* common defines and macros */
#include "mc9s12dg256.h"      /* derivative-specific definitions */
  
  
#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02
//#define KEYPAD PORTA folosim direct PORTA la verificari

void COMWRT4(unsigned char);
void DATWRT4(unsigned char);
void KEYPADSCAN(); // functie pentru scanarea tastaturii si afisare operator/operand specific
void MSDelay(unsigned int);


void main(void) 
{

        DDRK = 0xFF; 
        DDRA = 0x0F;  //randuri - intrare; coloane - iesiri
	
        COMWRT4(0x33);   //reset sequence provided by data sheet
        MSDelay(1);
        COMWRT4(0x32);   //reset sequence provided by data sheet
        MSDelay(1);
        COMWRT4(0x28);   //Function set to four bit data length
                                         //2 line, 5 x 7 dot format
        MSDelay(1);
        COMWRT4(0x06);  //entry mode set, increment, no shift
        MSDelay(1);
        COMWRT4(0x0E);  //Display set, disp on, cursor on, blink off
        MSDelay(1);
        COMWRT4(0x01);  //Clear display
        MSDelay(1);
        COMWRT4(0x80);  //set start posistion, home position
        MSDelay(1);
        
        /*DATWRT4('H');
        MSDelay(1);
        DATWRT4('E');
        MSDelay(1);
        DATWRT4('L');
        MSDelay(1);
        DATWRT4('L');
        MSDelay(1);
        DATWRT4('O');
        */ 
	// Sa facem direct in interiorul KeypadScan o bucla infinita
        KEYPADSCAN();



	
}
void COMWRT4(unsigned char command)
  {
        unsigned char x;
        
        x = (command & 0xF0) >> 2;         //shift high nibble to center of byte for Pk5-Pk2
        LCD_DATA =LCD_DATA & ~0x3C;          //clear bits Pk5-Pk2
        LCD_DATA = LCD_DATA | x;          //sends high nibble to PORTK
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~RS;         //set RS to command (RS=0)
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | EN;          //rais enable
        MSDelay(5);
        LCD_CTRL = LCD_CTRL & ~EN;         //Drop enable to capture command
        MSDelay(15);                       //wait
        x = (command & 0x0F)<< 2;          // shift low nibble to center of byte for Pk5-Pk2
        LCD_DATA =LCD_DATA & ~0x3C;         //clear bits Pk5-Pk2
        LCD_DATA =LCD_DATA | x;             //send low nibble to PORTK
        LCD_CTRL = LCD_CTRL | EN;          //rais enable
        MSDelay(5);
        LCD_CTRL = LCD_CTRL & ~EN;         //drop enable to capture command
        MSDelay(15);
  }

void DATWRT4(unsigned char data)
  {
    unsigned char x;
            
        x = (data & 0xF0) >> 2;
        LCD_DATA =LCD_DATA & ~0x3C;                     
        LCD_DATA = LCD_DATA | x;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | RS;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | EN;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~EN;
        MSDelay(5);
       
        x = (data & 0x0F)<< 2;
        LCD_DATA =LCD_DATA & ~0x3C;                     
        LCD_DATA = LCD_DATA | x;
        LCD_CTRL = LCD_CTRL | EN;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~EN;
        MSDelay(15);
  }
  
void KEYPADSCAN() {
	unsigned char row, column;
	
	while(1){ //stai aici
		//trebuie facut initial deubounce-ul pentru a nu avea probleme la citire normala
		//citim orice tasta
			do{
				PORTA = PORTA | 0x0F; //setarea coloanelor
				row = PORTA & 0xF0; //citirea randurilor
			}while(!(row | 0x00));	// asteapta pana o tasta este apasata
		
			do{
				do{
					MSDelay(1);
					row = PORTA & 0xF0; //citire rand
				}while(!(row | 0x00)); // verificarea apasarii unei taste
				
				MSDelay(15); //asteapa pentru debounce
				row = PORTA & 0xF0;
			}while(!(row | 0x00)); // fake key press
		//sfarsit initializare/debounce
		
		//citire normala
		while(1){
			//coloana 3
			PORTA = PORTA & 0xF0; //sterge configuratia anterioara a coloanelor
			PORTA = PORTA | 0x08; //seteaza coloana 3
			row = PORTA & 0xF0;
			if(row | 0x00){ //tasta apasata se afla in coloana 3
				if(row & 0x10){
					DATWRT4('+');
				}
				else if(row & 0x20){
					DATWRT4('-');
				}
				else if(row & 0x40){
					DATWRT4('*');
				}
				else if(row & 0x80){
					DATWRT4('/');
				}
				break; //iesi din bucla infinita
			}
			
			
			//coloana 2
			PORTA = PORTA & 0xF0; //sterge configuratia anterioara a coloanelor
			PORTA = PORTA | 0x04; //seteaza coloana 3
			row = PORTA & 0xF0;
			if(row | 0x00){ //tasta apasata se afla in coloana 3
				if(row & 0x10){
					DATWRT4('3');
				}
				else if(row & 0x20){
					DATWRT4('6');
				}
				else if(row & 0x40){
					DATWRT4('9');
				}
				else if(row & 0x80){
					DATWRT4('.');
				}
				break; //iesi din bucla infinita
			}
			
			
			//coloana 1
			PORTA = PORTA & 0xF0; //sterge configuratia anterioara a coloanelor
			PORTA = PORTA | 0x02; //seteaza coloana 3
			row = PORTA & 0xF0;
			if(row | 0x00){ //tasta apasata se afla in coloana 3
				if(row & 0x10){
					DATWRT4('2');
				}
				else if(row & 0x20){
					DATWRT4('5');
				}
				else if(row & 0x40){
					DATWRT4('8');
				}
				else if(row & 0x80){
					DATWRT4('0');
				}
				break; //iesi din bucla infinita
			}
			
			
			//coloana 0
			PORTA = PORTA & 0xF0; //sterge configuratia anterioara a coloanelor
			PORTA = PORTA | 0x01; //seteaza coloana 3
			row = PORTA & 0xF0;
			if(row | 0x00){ //tasta apasata se afla in coloana 3
				if(row & 0x10){
					DATWRT4('1');
				}
				else if(row & 0x20){
					DATWRT4('4');
				}
				else if(row & 0x40){
					DATWRT4('7');
				}
				else if(row & 0x80){
					DATWRT4('=');
				}
				break; //iesi din bucla infinita
			}
			
			row = 0; //tasta negasita
			break; //iesi din bucla
		}
	}
	     
}

 void MSDelay(unsigned int itime)
  {
    unsigned int i; unsigned int j;
    for(i=0;i<itime;i++)
      for(j=0;j<4000;j++);
 }
