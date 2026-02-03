/*
 * Lab2.c
 *
 * Created: 1/27/2026 10:23:38 AM
 * Author : admin
 */ 

// Librerias
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "LCD.h"
#include "ADC.h"
#include "UART.h"
//#include <stdio.h> Esta libreria no fue util
#include <stdlib.h> // Esta es la liberia que reemplaza a la anterior 

// Protype functions
void setup();


// Varibles
float adc_value1;
float adc_value2;


float value1;
float value2;

char value1_LCD[4];
char value2_LCD[4];
char cont_LCD[4];
uint8_t mux_adc = 0;

uint8_t cont = 0;


int main(void)
{
	
	setup();
	LCD_Set_Cursor8(1,1);
	LCD_Write_String8("S1:   S2:   S3:");
	LCD_Set_Cursor8(5,2);
	LCD_Write_String8("V     V");
	
	while (1) 
    {
		value1 = adc_value1*5/255; // Convertir de 0-255 a 0-5
		value2 = adc_value2*5/1023;
		
		// Con ayuda de intelegencia artifical se aprendio a usar esta función
		dtostrf(value1, 1, 2, value1_LCD); 
		dtostrf(value2, 1, 2, value2_LCD); 
		dtostrf(cont, 1, 0, cont_LCD);
		
		// Se escribe en la pantalla el valor del pot1
		LCD_Set_Cursor8(1,2);
		LCD_Write_String8(value1_LCD);
		LCD_Set_Cursor8(7,2);
		LCD_Write_String8(value2_LCD);
		LCD_Set_Cursor8(13,2);
		LCD_Write_String8(cont_LCD);
		
		writeString("S1: ");
		writeString(value1_LCD);
		writeString(" S2: ");
		writeString(value2_LCD);
		writeString("\n");
		
    }
}

// NON-INTERRUPT subroutines
void setup(){
	cli();
	init_LCD8();
	initADC();
	initUART();
	
	// PRESCALER PRINCIPLAR EN 16, 1MHZ
	CLKPR |= (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);
	
	
	sei();
}

// INTERRUPCIONES
ISR(ADC_vect)
{
	if (mux_adc == 0) {
		adc_value1 = ADCH;

		ADMUX = (ADMUX & 0xF0) | 1;  // ADC1
		mux_adc = 1;
		
		} else {
		// ADC1 ? 10 bits
		uint8_t low  = ADCL;   // SIEMPRE primero
		uint8_t high = ADCH;

		adc_value2 = ((uint16_t)high << 2) | (low >> 6);

		ADMUX = (ADMUX & 0xF0);  // ADC0
		mux_adc = 0;
		
	}

	ADCSRA |= (1 << ADSC); // siguiente conversión
}

ISR(USART_RX_vect){
	char recibido = UDR0;
	
	switch(recibido){
		case ('+'):
			cont++;
			break;
		case ('-'):
			if (cont != 0) cont--;
			break;
		default:
			break;
	}
}


