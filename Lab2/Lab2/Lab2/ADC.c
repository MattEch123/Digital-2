/*
 * ADC.c
 *
 * Created: 4/30/2025 11:25:45 AM
 *  Author: admin
 */ 
#include "ADC.h"

void initADC(){
	// Incializar pines del PORTC como entrada PC0, PC1
	
	DDRC &= ~((PORTC0 << 1) | (PORTC1 << 1));
	
	 
	ADMUX = 0; // LIMPIAR EL REGISTRO ADMUX
	ADMUX |= (1 << REFS0); // REFERENCIA = AVCC
	ADMUX |= (1 << ADLAR); // JUSTIFICACION A LA IZQUIERDA
	ADMUX = (ADMUX & 0xF0) | 1;  // ADC1

	
	ADCSRA = 0;
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0); // PRESCALER DE 8 (125 kHz con frecuencia principal de 1 MHz)
	ADCSRA |= (1 << ADEN) | (1 << ADIE); // HABILITAR INTERRUPCIONES

	ADCSRA |= (1 << ADSC); // EMPIEZA A HACER LA CONVERSION DEL PIN
}