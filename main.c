/*
    Examen microcontroladores
	propuesta poncho
    main.c

    * Falta la implementación del display de 7 segmentos
*/

#define F_CPU 8000000UL

#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define RFID    PORTA
#define V_BIT   0

uint8_t F_READ = 1;
uint8_t cont = 0;
uint8_t cont_delay = 0;
uint8_t arr[3] = {0x00, 0x00, 0x00};

const uint8_t umbral PROGMEM = 5;

void deact(){
    // Desactivar TIMER2
    TCCR2 = 0;
    OCR2 = 0;

    // Desactivar PWM
    TCCR1A = 0;
    TCCR1B = 0;
    OCR1B = 0;
    OCR1A = 0;
}

ISR(TIMER0_COMP_vect){
	if (cont_delay != 3) cont_delay++;
	else{
		arr[cont] = RFID;
		cont_delay = 0;

		if (cont == 0){
			if (arr[0] < 3) PORTB |= 1<<V_BIT;
			else PORTB &= ~(1<<V_BIT);
			cont++;
		} else if (cont == 1){
			if (arr[1] < umbral) OCR1B = 586;   // 75% duty cycle
			else OCR1B = 195;                   // 25% duty cycle
			cont++;
		} else if (cont == 2){
			cont = 0;
		}
	}
    // reti();
}

ISR(INT0_vect){
    if (F_READ){
        // Activación del TIMER2
        TCCR0 = (1<<WGM01) | (1<<CS02) | (1<<CS00);
        OCR0 = 195;

        // Inicialización del PWM
        /*
            Clock select: clk/1024
            Operation mode: FastPWM top OCR1A
            COM: Toggle on compare
        */
        OCR1A = 781;
        OCR1B = 0;
        TCCR1A |= (1<<WGM10) | (1<<WGM11) | (1<<COM1B0);
        TCCR1B |= (1<<WGM12) | (1<<WGM13) | (1<<CS10) | (1<<CS12);

        // Cambio del estado de la bandera
        F_READ = 0;

    } else{
        deact();

        // Cambio del estado de la bandera
        F_READ = 1;
    }
    // reti();
}

ISR(INT1_vect){
    deact();
}

int main(void){

	// Interrupción externa
	GICR |= (1<<INT0) | (1<<INT1);
	MCUCR |= (1<<ISC00) | (1<<ISC11);
    // Interrupción por TIMER2
    TIMSK = (1<<OCIE0);

    sei();

	while(1);
    return 0;
}
