/*
 * main.c
 *
 * Created: 6/6/2022 4:43:25 PM
 * Author: Guillermo
 */ 

#include <xc.h>
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t f_read = 1, count = 0;

uint8_t valores[3] = {0,0,0};

ISR(INT0_vect);								// Interrupción switch
ISR(INT1_vect);								// Interrupción sensor óptico
ISR(TIMER0_COMP_vect);						// Interrupción Timer 0 (para RFID)

void lectura();

int main(void){
	/*------ Seteo de puertos ------*/ 
	DDRD |= (0 << PD2);						// Switch
	DDRD |= (0 << PD3);						// Sensor óptico
	DDRA = 0b00000000;						// RFID
	
	DDRB |= (1 << PB3);						// Calefactor (PWM)
	DDRB |= (1 << PB0);						// Electroválvula
	DDRC |= 0b11111111;					// Display 7 segmentos
	
	sei();
	
	/*------ Palabras de control ------*/
	//Interrupciones externas 
	GICR = (1 << INT1) | (1 << INT2);
	MCUCR = (1 << ISC00) | (1 << ISC11);
	
	//Interrupcion Timer
	TIMSK = (1 << OCIE2);
		
    while(1);
}

ISR(INT0_vect){
	if(f_read == 1){
		OCR2 = 7;
		TCCR2 = (1 << WGM21) | (1 << CS22) | (1 << CS21) | (1 << CS20);
		f_read--;
	} else{
		TCCR2 = 0;
		f_read++;
	}	
}

ISR(TIMER2_COMP_vect){
	valores[count] = PORTA;
	count++;
	if(count == 2) count = 0;	
}

