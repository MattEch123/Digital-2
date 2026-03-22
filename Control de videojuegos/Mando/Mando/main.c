/*
 * Mando.c
 *
 * Created: 3/17/2026 5:50:43 PM
 * Author : admin
 */ 


// Librerias
#include <avr/io.h>
#include "UART/UART.h"
#include <string.h>
#include <avr/interrupt.h>



// Variables


// Prototype functions
void setup();

int main(void)
{
    setup();
    while (1) 
    {
    }
}

// NON-interrups subroutines
void setup(){
	cli();
	// Confiurar pines PD2-PD7 como entrada con pull ups
	DDRD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5) | (1 << PORTD6) | (1 << PORTD7));
	
	PORTD |= (1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5) | (1 << PORTD6) | (1 << PORTD7);

	
	
	// Habiliar interrupciones de PCINT
	PCICR |= (1 << PCIE2); // Habilita interrupciones para PORTD
	PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);
	
	// Inicializar UART
	initUART();
	
	sei();
}

// Interrups
ISR(PCINT2_vect)
{
	if (!(PIND & (1 << PORTD2)))
	{
		writeString("U\n");
	}

	if (!(PIND & (1 << PORTD3)))
	{
		writeString("D\n");
	}

	if (!(PIND & (1 << PORTD4)))
	{
		writeString("R\n");
	}

	if (!(PIND & (1 << PORTD5)))
	{
		writeString("L\n");
	}

	if (!(PIND & (1 << PORTD6)))
	{
		writeString("A\n");
	}

	if (!(PIND & (1 << PORTD7)))
	{
		writeString("B\n");
	}
}


