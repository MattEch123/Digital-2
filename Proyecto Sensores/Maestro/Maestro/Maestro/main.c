/*
 * Maestro.c
 *
 * Created: 2/9/2026 10:31:08 PM
 * Author : admin
 */ 

#define F_CPU 16000000UL
// Librerias
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h> 
#include "I2C/I2C.h"
#include "UART/UART.h"
#include "LCD/LCD.h"

#define slave1 0x30
#define slave2 0x40
#define AHT10  0x38

#define slave1R (0x30 << 1) | 0x01
#define slave1W (0x30 << 1) & 0b11111110

#define slave2R (0x40 << 1) | 0x01
#define slave2W (0x40 << 1) & 0b11111110

#define  AHT10R (0x38 << 1) | 0x01
#define  AHT10W (0x38 << 1) & 0b11111110

// Prototypes functions
void setup();
void leds(uint8_t valor);



// Variables
uint8_t adress;
uint8_t temp;
uint8_t bufferI2C = 0;

char value1_LCD[16];

// Sensor de peso
uint8_t pesoMSB = 0;
uint8_t pesoLSB = 0;
float peso = 0;

#define PESO_MAX 500
#define STEPS_MAX 512

// Sensor AHT10
uint8_t AHT10_buf[6];
uint32_t hum_raw;
float humedad;
uint8_t pwm = 0;

// Sensor temperatura
uint8_t temperature = 0;


// Main loop
int main(void)
{
	setup();
	
	// Configurar la pantalla LCD
	// Temperatura
	LCD_Set_Cursor8(1,1);
	LCD_Write_String8("T1:");
	LCD_Set_Cursor8(3,2);
	LCD_Write_Char8(0xDF);
	LCD_Write_Char8('C');
	
	// Peso
	LCD_Set_Cursor8(6,1);
	LCD_Write_String8("P1:");
	LCD_Set_Cursor8(10,2);
	LCD_Write_Char8('g');
	
	// Humedad
	LCD_Set_Cursor8(12,1);
	LCD_Write_String8("H1:");
	
	
	
	
	// Incializar sensor AHT10
	I2C_Master_Start();
	I2C_Master_Write(AHT10W);
	I2C_Master_Write(0xE1);
	I2C_Master_Write(0x08);
	I2C_Master_Write(0x00);
	I2C_Master_stop();
	
    while (1) 
    {
		// =======================================================
		// ====  Inicio del la comunicacion con el eslcavo 1 =====
		// =======================================================
		if(!I2C_Master_Start()) return;
		
		
		if(!I2C_Master_Write(slave1W)){
			I2C_Master_stop();
			return;
		}
		
		
		I2C_Master_Write('R');
		
		
		if (!I2C_Master_RepeatedStart()){
			I2C_Master_stop();
			return;
		}
		
		if(!I2C_Master_Write(slave1R)){
			I2C_Master_stop();
			return;
		}
		
		I2C_Master_Read(&bufferI2C, 0); // NACK
		
		temperature = bufferI2C;
		
		dtostrf(bufferI2C, 1, 0, value1_LCD);
		LCD_Set_Cursor8(1,2);
		LCD_Write_String8(value1_LCD);
		writeString("T");
		writeString(value1_LCD);
		writeString("\n");
		
		if (!I2C_Master_RepeatedStart()){
			I2C_Master_stop();
			return;
		}
		
		if(!I2C_Master_Write(slave1W)){
			I2C_Master_stop();
			return;
		}
		
		I2C_Master_Write('V');
		I2C_Master_Write(pwm);
		
		I2C_Master_stop();
		
		
		//leds(0);
		
		// =======================================================
		// ====  Fin del la comunicacion con el eslcavo 1 ========
		// =======================================================
		
		// =======================================================
		// ====  Inicio del la comunicacion con el eslcavo 2 =====
		// =======================================================
		if(!I2C_Master_Start()) return;
		
		
		if(!I2C_Master_Write(slave2W)){
			I2C_Master_stop();
			return;
		}
		
		
		I2C_Master_Write('R');
		
		
		if (!I2C_Master_RepeatedStart()){
			I2C_Master_stop();
			return;
		}
		
		if(!I2C_Master_Write(slave2R)){
			I2C_Master_stop();
			return;
		}
		
		I2C_Master_Read(&bufferI2C, 1); //ACK
		pesoMSB =  bufferI2C;
		I2C_Master_Read(&bufferI2C, 0); //NACK
		pesoLSB =  bufferI2C;
		
		peso = ((uint16_t)pesoMSB << 8) | pesoLSB;
		
		// Lógica de que hacer con el peso
		
		uint32_t pasos = ((uint32_t)peso * STEPS_MAX) / PESO_MAX;
		if (pasos >= STEPS_MAX) pasos = STEPS_MAX;
		
		uint16_t pasos16 = (uint16_t)pasos;
		
		if (!I2C_Master_RepeatedStart()){
			I2C_Master_stop();
			return;
		}
		
		if(!I2C_Master_Write(slave2W)){
			I2C_Master_stop();
			return;
		}
		
		I2C_Master_Write('S');
		I2C_Master_Write((pasos16 >> 8) & 0xFF);  // MSB
		I2C_Master_Write(pasos16 & 0xFF);         // LSB
		
		// Mandar que hacer con la temperatura
		I2C_Master_Write('P');
		if (temperature <=30){
			I2C_Master_Write(0x01);
		}else{
			I2C_Master_Write(0x00);
		}
		
		
		I2C_Master_stop();
		
		dtostrf(peso, 3, 0, value1_LCD);
		LCD_Set_Cursor8(6,2);
		LCD_Write_String8(value1_LCD);
		
		writeString("P");
		writeString(value1_LCD);
		writeString("\n");
		
		// =======================================================
		// ====  Fin del la comunicacion con el eslcavo 2 ========
		// =======================================================
		
		// =======================================================
		// ====  Inicion de comunicacion con AHT10 ===============
		// =======================================================
		
		if(!I2C_Master_Start()) return;
		
		
		if(!I2C_Master_Write(AHT10W)){
			I2C_Master_stop();
			return;
		}
		
		// Comandos para comunicarse con AHT10
		I2C_Master_Write(0xAC);
		I2C_Master_Write(0x33);
		I2C_Master_Write(0x00); 
		
		I2C_Master_stop();
		
		_delay_ms(100);
		
		if (!I2C_Master_Start()){
			I2C_Master_stop();
			return;
		}
		
		if(!I2C_Master_Write(AHT10R)){
			I2C_Master_stop();
			return;
		}
		
		for (uint8_t i = 0; i < 5; i++){
			I2C_Master_Read(&AHT10_buf[i],1); // ACK
		}
		
		I2C_Master_Read(&AHT10_buf[5],0); // NACK
		I2C_Master_stop();
		
		// Obtener la humedad de la informacion que medio el sensor
		
		hum_raw = ((uint32_t)AHT10_buf[1] << 12) | ((uint32_t)AHT10_buf[2] << 4) | (AHT10_buf[3] >> 4);
		humedad = (hum_raw * 100.0) / 1048576.0;
		
		
		if(humedad <= 55.0)
		{
			pwm = 0;
		}
		else if(humedad >= 85.0)
		{
			pwm = 255;
		}
		else
		{
			pwm = 195 + (uint8_t)(((humedad - 55.0) * 60.0) / 30.0);
		}

		
		dtostrf(humedad, 5, 1, value1_LCD);
		
		LCD_Set_Cursor8(11,2);
		LCD_Write_String8(value1_LCD);
		
		writeString("H");
		writeString(value1_LCD);
		writeString("\n");
		// =======================================================
		// ====  Fin de comunicacion con AHT10 ===============
		// =======================================================
		//_delay_ms(250);
		
    }
}

// NON-Interruputs sub-routines
void setup(){
	cli();
	
	
	
	DDRD |= (1<<PORTD2) | (1<<PORTD3) | (1<<PORTD4) | (1<<PORTD5) | (1<<PORTD6) | (1<<PORTD7);
	PORTD &= ~((1<<PORTD2) | (1<<PORTD3) | (1<<PORTD4) | (1<<PORTD5) | (1<<PORTD6) | (1<<PORTD7));
	// PORTB 0-1
	DDRB |= (1<<PORTB0) | (1 << PORTB1);
	PORTB &= ~((1<<PORTB0) | (1 << PORTB1));
	
	I2C_Master_Init(100000, 1);
	initUART();
	init_LCD8();
	
	
	sei();
	
}


void leds(uint8_t a){
	uint8_t puerto_d = (a & 0b00111111) << 2;
	uint8_t puerto_b = (a & 0b11000000) >> 6; // Máscara para conservar únicamente los últimos 2 bits
	
	PORTD = (PORTD & 0b00000011) | puerto_d;
	PORTB = (PORTB & 0b11111100) | puerto_b;
}

// Interrupts
