#include <hidef.h>      /* common defines and macros */
#include "mc9s12dg256.h"      /* derivative-specific definitions */
#include<string.h>
#include<stdlib.h>
 
  
#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02
//#define KEYPAD PORTA folosim direct PORTA la verificari

void COMWRT4(unsigned char);
void DATWRT4(unsigned char);
void KEYPADSCAN(); // functie pentru scanarea tastaturii si afisare operator/operand specific
void MSDelay(unsigned int);
void OPERATIE();

unsigned int operand1, operand2, operand3;
/*
  codificare operatii: 
  0 -> adunare
  1 -> scadere
  2 -> inmultire
  3 -> impartire
  4 -> operatie nedefinita
*/
unsigned int operatie = 4, operatie2 = 4;
unsigned char rez[8];


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
  
void itoa(int rezultat){
    int i = 0;
    if(rezultat == 0)
      rez[i++] = '0';
    while(rezultat != 0){
      rez[i] =  rezultat % 10 + '0';
      rezultat = rezultat / 10;
      MSDelay(10);
      i++;
    }
    rez[i] = '\0';
}

void print(){
      int i;
      for(i = strlen(rez) - 1; i >= 0; i--){
      	DATWRT4(rez[i]);
      	MSDelay(50);
    }
}

void printError(){
	COMWRT4(0xC0);
	MSDelay(1);
	DATWRT4('E');
	MSDelay(1);
	DATWRT4('R');
	MSDelay(1);
	DATWRT4('O');
	MSDelay(1);
	DATWRT4('A');
	MSDelay(1);
	DATWRT4('R');
	MSDelay(1);
	DATWRT4('E');
	MSDelay(1);
}
  
void OPERATIE(){
    int rezultat, parteR = 0;
    int i, parteReala;
    float rez;
    
    if(operatie2 == 2 && (operatie != 2 && operatie != 3)){
      
        operand2 = operand2 * operand3;
        itoa(operand2);
        print();
        operatie2 = 4;
    }
    else if(operatie2 == 3 && (operatie != 2 && operatie != 3))
        if(operand3 == 0){
           printError();
           exit(0);
        }
         else 
             	if(operand2%operand3 != 0){
	               	operand2 = operand2/operand3;
	              	parteReala = operand2%operand3;  
	              	rez = parteReala/operand3;
              		rez = rez * 100; 
              		parteR = (unsigned int)parteReala;
              	
            	 } 
            	 else {
            	  operatie2=4;
            	    operand2 = operand2 / operand3;
            	 }
            	 
    if(operatie == 0){
      if(operatie2 != 4){
         MSDelay(10); 
        operand2 = operand1 + operand2;
      
      } else{
      itoa(operand1);
       print();
       itoa(operand2);
       print();  
      rezultat = operand1 + operand2;
      /*if(parteR != 0){
          itoa(rezultat);
          print();
          DATWRT4('.');
          itoa(parteR);
          print(); 
      } else{  */
          itoa(rezultat);
	        print();
      //}
      }
     
    } 
    else if(operatie == 1){
      if(operatie2 != 4){
        MSDelay(10);
        operand2 = operand1 - operand2;       
      }else{
      
       rezultat = operand1 - operand2;
       itoa(operand1);
       print();
       
       /*if(parteR != 0){
          itoa(rezultat);
          print();
          DATWRT4('.');
          itoa(parteR);
          print(); 
      } else{     */
          itoa(rezultat);
	        print();
      //}
      }
    
    } 
    else if(operatie == 2){
    if(operatie2 != 4){
      operand2 = operand1 * operand2; 
      itoa(operand2);
      print();  
    } else {
      rezultat = operand1 * operand2;
      /*if(parteR != 0){
          itoa(rezultat);
          print();
          DATWRT4('.');
          itoa(parteR);
          print(); 
      } else{*/
          itoa(rezultat);
	        print();
      //}*/
    }
    }  
    else if(operatie == 3){
      if(operatie2 == 4){
        
        if(operand2 == 0){
          printError();
          exit(0);
        }
      	if(operand1%operand2 != 0){
      		int parteIntreaga = operand1/operand2;
      		int parteReala = operand1%operand2;  
      		float rez = parteReala/operand2;
      		rez = rez * 100;     
      		itoa(parteIntreaga);
      		print();
      		DATWRT4('.');
      		parteR = (unsigned int)parteReala;
      		itoa(parteR);
      		print();
      	}
  	    else{
        		rezultat = operand1 / operand2;
        		itoa(rezultat);
  	      	print();
  	    } 
      } else operand2=operand1/operand2;
    }
    else{
      	printError();
      	 exit(0); 
    }
    if(operatie2 == 0){
      rezultat = operand2 + operand3;
      /*if(parteR != 0){
          itoa(rezultat);
          print();
          DATWRT4('.');
          itoa(parteR);
          print(); 
      } else{*/
          itoa(rezultat);
	        print();
      //}
    } 
    else if(operatie2 == 1){
      rezultat = operand2 - operand3;
       /*if(parteR != 0){
          itoa(rezultat);
          print();
          DATWRT4('.');
          itoa(parteR);
          print(); 
      } else{  */
          itoa(rezultat);
	        print();
      //}
    
    } else if(operatie2 == 2){
      rezultat = operand2 * operand3;
      itoa(rezultat);
      print();
    } else if(operatie2 == 3){
        rezultat = operand2/operand3;
      itoa(rezultat);
      print();
    }
}
  
void KEYPADSCAN() {
	unsigned char row;
	unsigned int contorOperanzi = 1;
	
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
				
				MSDelay(100); //asteapta pentru debounce
				row = PORTA & 0xF0;
			}while(!(row | 0x00)); // fake key press
		//sfarsit initializare/debounce
		
		//citire normala
		while(1){
			//coloana 3
			PORTA = PORTA & 0xF0; //sterge configuratia anterioara a coloanelor
			PORTA = PORTA | 0x08; //seteaza coloana 3
			MSDelay(10);
			row = PORTA & 0xF0;
			if(row | 0x00){ //tasta apasata se afla in coloana 3
				if(row & 0x10){
					DATWRT4('+');
					if(contorOperanzi == 2)
					     operatie2 = 0;
					else operatie = 0;
					  
					contorOperanzi++;
				}
				else if(row & 0x20){
					DATWRT4('-');
				  if(contorOperanzi == 2)
					     operatie2 = 1;
					else operatie = 1;
	
				  contorOperanzi++; 
				}
				else if(row & 0x40){
					DATWRT4('*');
					if(contorOperanzi == 2)
					     operatie2 = 2;
					else operatie = 2;
					
				  contorOperanzi++;
				}
				else if(row & 0x80){
					DATWRT4('/');
					if(contorOperanzi == 2)
					     operatie2 = 3;
					else operatie = 3;
					
				  contorOperanzi++;
				}
				break; //iesi din bucla infinita
			}
			
			
			//coloana 2
			PORTA = PORTA & 0xF0; //sterge configuratia anterioara a coloanelor
			PORTA = PORTA | 0x04; //seteaza coloana 2
			MSDelay(10);
			row = PORTA & 0xF0;
			if(row | 0x00){ //tasta apasata se afla in coloana 2
				if(row & 0x10){
					DATWRT4('3');
					if(contorOperanzi == 1)
					  operand1 = operand1 * 10 + 3;
					else if(contorOperanzi == 2)
					  operand2 = operand2 * 10 + 3;
					else if(contorOperanzi == 3)
					  operand3 = operand3 * 10 + 3;
				}
				else if(row & 0x20){
					DATWRT4('6');
				  if(contorOperanzi == 1)
					  operand1 = operand1 * 10 + 6;
					else if(contorOperanzi == 2)
					  operand2 = operand2 * 10 + 6;
					else if(contorOperanzi == 3)
					  operand3 = operand3 * 10 + 6;
				}
				else if(row & 0x40){
					DATWRT4('9');
				  if(contorOperanzi == 1)
					  operand1 = operand1 * 10 + 9;
					else if(contorOperanzi == 2)
					  operand2 = operand2 * 10 + 9;
					else if(contorOperanzi == 3)
					  operand3 = operand3 * 10 + 9;
				}
				else if(row & 0x80){
					DATWRT4('.');
				}
				break; //iesi din bucla infinita
			}
			
			
			//coloana 1
			PORTA = PORTA & 0xF0; //sterge configuratia anterioara a coloanelor
			PORTA = PORTA | 0x02; //seteaza coloana 1
			MSDelay(10);
			row = PORTA & 0xF0;
			if(row | 0x00){ //tasta apasata se afla in coloana 1
				if(row & 0x10){
					DATWRT4('2');
					if(contorOperanzi == 1)
					  operand1 = operand1 * 10 + 2;
				else if(contorOperanzi == 2)
					  operand2 = operand2 * 10 + 2;
				else if(contorOperanzi == 3)
					  operand3 = operand3 * 10 + 2;
				}
				else if(row & 0x20){
					DATWRT4('5');
				  if(contorOperanzi == 1)
					  operand1 = operand1 * 10 + 5;
					else if(contorOperanzi == 2)
					  operand2 = operand2 * 10 + 5;
					else if(contorOperanzi == 3)
					  operand3 = operand3 * 10 + 5;
				}
				else if(row & 0x40){
					DATWRT4('8');
				  if(contorOperanzi == 1)
					  operand1 = operand1 * 10 + 8;
					else if(contorOperanzi == 2)
					  operand2 = operand2 * 10 + 8;
					else if(contorOperanzi == 3)
					  operand3 = operand3 * 10 + 8;
				}
				else if(row & 0x80){
					DATWRT4('0');
				  if(contorOperanzi == 1)
					  operand1 = 0;
					else if(contorOperanzi == 2)
					  operand2 = 0;
					else if(contorOperanzi == 3)
					  operand3 = 0;
				}
				break; //iesi din bucla infinita
			}
			
			
			//coloana 0
			PORTA = PORTA & 0xF0; //sterge configuratia anterioara a coloanelor
			PORTA = PORTA | 0x01; //seteaza coloana 0
			MSDelay(10);
			row = PORTA & 0xF0;
			if(row | 0x00){ //tasta apasata se afla in coloana 0
				if(row & 0x10){
					DATWRT4('1');
					if(contorOperanzi == 1)
					  operand1 = operand1 * 10 + 1;
					else if(contorOperanzi == 2)
					  operand2 = operand2 * 10 + 1;
					else if(contorOperanzi == 3)
					  operand3 = operand3 * 10 + 1;
				}
				else if(row & 0x20){
					DATWRT4('4');
				  if(contorOperanzi == 1)
					  operand1 = operand1 * 10 + 4;
					else if(contorOperanzi == 2)
					  operand2 = operand2 * 10 + 4;
					else if(contorOperanzi == 3)
					  operand3 = operand3 * 10 + 4;
				}
				else if(row & 0x40){
					DATWRT4('7');
				  if(contorOperanzi == 1)
					  operand1 = operand1 * 10 + 7;
					else if(contorOperanzi == 2)
					  operand2 = operand2 * 10 + 7;
					else if(contorOperanzi == 3)
					  operand3 = operand3 * 10 + 7;
				}
				else if(row & 0x80){
					DATWRT4('=');
				  OPERATIE();
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
 
