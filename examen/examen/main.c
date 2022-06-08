/*
 * main.c
 *
 * Created: 6/6/2022 4:43:25 PM
 * Author: Guillermo
 */ 

#include <xc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

const uint8_t temperatura PROGMEM = 50;

uint8_t f_read = 1, count = 0, count_int = 0;

uint8_t valores[3] = {0,0,0};

ISR(INT0_vect);								// Switch interrupt (INT0)
ISR(INT1_vect);								// Optical sensor interrupt (INT1)
ISR(TIMER0_COMP_vect);						// RFID read (TIMER0)
ISR(TIMER2_COMP_vect);						// 7 segment display (TIMER2)

void lectura();

int main(void){
	/*------ Port set up ------*/ 
	DDRD |= (0 << PD2);						// Switch pin
	DDRD |= (0 << PD3);						// Optical sensor pin
	DDRA = 0b00000000;						// RFID pin
	
	DDRB |= (1 << PD4);						// Heater port (PWM = 0C1B)
	DDRB |= (1 << PB0);						// Electro valve port
	DDRC |= 0b11111111;						// 7 segments display port
	
	/*------ Control word ------*/
	// External interrupts
	GICR = (1 << INT0) | (1 << INT1);
	MCUCR = (1 << ISC00) | (1 << ISC11);
	
	// Timer interrupt
	TIMSK = (1 << OCIE0);
	
	sei();
		
    while(1);
}

ISR(INT0_vect){
	if(f_read == 1){
		OCR0 = 196;
		TCCR0 = (1 << WGM01) | (1 << CS02) | (1 << CS00);						//Initialize 25ms Timer

		OCR1A = 0;
		OCR1B = 0;
		TCCR1A = (1 << COM1B1) | (1 << WGM11) |(1 << WGM10);					//Initialize PWM
		TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);						//Initialize PWM
		f_read = 0;
	} else{
		TCCR0 = 0;								//Turn down 100ms Timer
		
		TCCR1A = 0;								//Stop PWM
		TCCR1B = 0;								//Stop PWM
		f_read = 1;
	}	
}

ISR(TIMER0_COMP_vect){
	if(count_int == 3){
		count_int = 0;		
		valores[count] = PINA;
		if(count == 0){
			if(valores[count] < 3){
				PORTB |= (1 << PB0);
			} else{
				PORTB &= ~(1 << PB0);
			}
			count++;
		} else if(count == 1){
			OCR1A = 781;
			if(valores[count] < temperatura){		//75% PWM
				OCR1B = 586;
			} else{										//25% PWM
				OCR1B = 195;
			}
			count++;
		} else if(count == 2){
			count = 0;
		}
	} else{
		count_int++;
	}	 
}

ISR(INT1_vect){
	TCCR0 = 0;										//Turn down 100ms Timer
	TCCR1A = 0;										//Stop PWM
	TCCR1B = 0;
}
		
		
// 		if(valores[count] < temperatura[0]){
// 			OCR1A = 781;
// 			OCR1B = 586;
// 			TCCR1A = (1 << COM1B1) | (1 << WGM11) |(1 << WGM10);
// 			TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
// 		} else{
// 			OCR1A = 781;
// 			OCR1B = 195;
// 			TCCR1A = (1 << COM1B1) | (1 << WGM11) |(1 << WGM10);
// 			TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);			
// 		}