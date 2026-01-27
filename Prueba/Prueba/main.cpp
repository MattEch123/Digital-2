/*
 * Prueba.c
 *
 * Created: 20/01/2026 4:40 PM
 * Author: Pablo Echeverria
 * Description: Programación para hacer un juego de carrera.
 */
/****************************************/

// Librerias
#include <avr/io.h>
#include <avr/interrupt.h>


// Function prototypes
void setup(void);

void display(uint8_t cont);

// Variables
uint8_t timer0_ini = 100;
uint8_t flag_inicio = 0;
uint8_t flag_race = 0;
uint8_t contador1 = 0;
uint8_t player1 = 0;
uint8_t player2 = 0;
uint8_t led1 = 0;
uint8_t led2 = 0;
uint8_t cont_d = 5;
uint8_t mux = 0;

uint8_t tabla7seg[] = {
	0x81, 0xCF, 0x92, 0x86,
	0xCC, 0xA4, 0xA0, 0x8F,
	0x80, 0x84, 0x88, 0xE0,
	0xB1, 0xC2, 0xB0, 0xB8
};
uint8_t lastPINC;




uint8_t carrera[] = {
	0x00, 0x08, 0x04, 0x02, 0x01
};

// Main function
int main(void)
{
	setup();
	while (1)
	{
		if ((player1 == 4) || (player2 == 4)) {
			flag_race = 0;
			if (player1 > player2){
				cont_d = 1;
			}
			if (player2 > player1){
				cont_d = 2;
			}
			
		}
		
		
		
		
		
		
		
	}
}

// NON-Interrupt subroutines
void setup(void)
{
	// Configurar PORTB y PORTD como salida
	DDRB = 0xFF;
	DDRD = 0xFF;
	
	PORTB = 0b0000100;
	
	// Configurar PORTC como entrada
	DDRC = 0x00;
	PORTC = 0xFF; //ACTICAR PULL-UPS EN PUERTO C
	
	// Habilitar interrupciones de PIN-change en el puerto C
	PCICR |= (1 << PCIE1);
	// Habilitar interrupciones en pines especificos del puerto C
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10); 
	
	 // PRESCALER PRINCIPLAR EN 16, 1MHZ
	CLKPR |= (1 << CLKPCE);
	CLKPR = (1 << CLKPS2); 
	
	// Interrupcion de timers
	TCCR0B |= (1 << CS00) | (1 << CS01); // Prescaler de 64
	TCNT0 = timer0_ini;
	TIMSK0 = (1 << TOIE0);				// Habilitar la interrupcion
	 
	//display(5);
	sei();
}

void display(uint8_t cont){
	PORTD = tabla7seg[cont];
}

// Interrupciones 
ISR(PCINT1_vect)
{
	uint8_t currentPINC = PINC;
	uint8_t changed = currentPINC ^ lastPINC;  // Verificar si el boton se quedo apachado

	// ---- BOTÓN (inicio) ----
	if (changed & (1 << PORTC0)) {
		if (!(currentPINC & (1 << PORTC0))) { // flanco de bajada
			if ((flag_inicio == 0) && (flag_race == 0)) {
				player1 = 0;
				player2 = 0;
				cont_d = 5;
				flag_inicio = 1;
			}
		}
	}

	// ---- BOTONES DE JUEGO ----
	if (flag_race) {
		if (changed & (1 << PORTC1)) {
			if (!(currentPINC & (1 << PORTC1))) {
				player1++;
			}
		}

		if (changed & (1 << PORTC2)) {
			if (!(currentPINC & (1 << PORTC2))) {
				player2++;
			}
		}
	}

	lastPINC = currentPINC;
}


ISR(TIMER0_OVF_vect){
	TCNT0 = timer0_ini;
	if (mux == 0){
		PORTD = carrera[player1];
		PORTB = 0x01;
		mux++;
	}else if(mux == 1){
		PORTD = carrera[player2];
		PORTB = 0x02;
		mux++;
	}else{
		display(cont_d);
		PORTB = 0x04;
		mux = 0;
	}
	
	
	if ((flag_inicio == 1) && (flag_race == 0)){
		contador1++;
	}
	if (contador1 == 100){
		cont_d--;
		contador1 = 0;
		if (cont_d == 0){
			flag_inicio = 0;
			flag_race = 1;
		}
	}
}
