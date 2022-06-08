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

const char temperatura[] PROGMEM = {0x55};

uint8_t f_read = 0x00, count = 0;

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
	
	DDRB |= (1 << PD4);						// Calefactor (PWM)
	DDRB |= (1 << PB0);						// Electroválvula
	DDRC |= 0b11111111;						// Display 7 segmentos
	
	sei();
	
	/*------ Palabras de control ------*/
	//Interrupciones externas 
	GICR = (1 << INT0) | (1 << INT1);
	MCUCR = (1 << ISC00) | (1 << ISC11);
	
	//Interrupcion Timer
	TIMSK = (1 << OCIE2);
		
    while(1);
}

ISR(INT0_vect){
	f_read *= (-1);																	//Negate flag
	if(f_read == 0xFF){
		OCR2 = 7;
		TCCR2 = (1 << WGM21) | (1 << CS22) | (1 << CS21) | (1 << CS20);				//Initialize 100ms Timer
		TCCR1A = (1 << COM1B1) | (1 << WGM11) |(1 << WGM10);						//Initialize PWM
		TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);							//Initialize PWM
	} else{
		TCCR2 = 0;																	//Turn down 100ms Timer
		TCCR1A = 0;																	//Stop PWM
		TCCR1B = 0;																	//Stop PWM
	}	
}

ISR(TIMER2_COMP_vect){
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
		if(valores[count] < temperatura[0]){						//75% PWM
			OCR1B = 586;
		} else{														//25% PWM
			OCR1B = 195;
		}
		count++;
	} else if(count == 2){
		
		count = 0;
	} 
}

ISR(INT1_vect){
	TCCR2 = 0;																	//Turn down 100ms Timer
	TCCR1A = 0;																	//Stop PWM
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


