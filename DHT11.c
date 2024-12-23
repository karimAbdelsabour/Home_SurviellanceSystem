#include "DHT11.h"
#include <avr/io.h>
#include <util/delay.h>

uint8_t I_RH, D_RH, I_Temp, D_Temp, CheckSum;

// Function to send a start pulse/request to DHT11
void Request(void) {
	DDRD |= (1<<DHT11_PIN);        // Set DHT11_PIN as output
	PORTD &= ~(1<<DHT11_PIN);      // Set the pin low
	_delay_ms(20);                 // Wait for 20ms
	PORTD |= (1<<DHT11_PIN);       // Set the pin high
}

// Function to receive response from DHT11
void Response(void) {
	DDRD &= ~(1<<DHT11_PIN);       // Set DHT11_PIN as input
	while(PIND & (1<<DHT11_PIN));  // Wait for the pin to go low
	while((PIND & (1<<DHT11_PIN)) == 0); // Wait for the pin to go high
	while(PIND & (1<<DHT11_PIN));  // Wait for the pin to go low again
}

// Function to receive data from DHT11
uint8_t Receive_data(void) {
	uint8_t c = 0;
	for (int q = 0; q < 8; q++) {
		while((PIND & (1<<DHT11_PIN)) == 0);  // Wait for the pin to go high
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN)) {
			c = (c<<1) | (0x01);    // If high pulse is greater than 30ms, it's logic HIGH
			} else {
			c = (c<<1);             // Otherwise, it's logic LOW
		}
		while(PIND & (1<<DHT11_PIN));  // Wait for the pin to go low
	}
	return c;
}
