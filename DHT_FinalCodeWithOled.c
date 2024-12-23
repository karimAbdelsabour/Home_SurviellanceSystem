#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include "ssd1306.h"
#include <util/delay.h>

#define DHT11_PIN 4
uint8_t c = 0, I_RH, D_RH, I_Temp, D_Temp, CheckSum;

void Request() {   /* Microcontroller send start pulse/request */
	DDRB |= (1<<DHT11_PIN);
	PORTB &= ~(1<<DHT11_PIN);    /* set to low pin */
	_delay_ms(20);            /* wait for 20ms */
	PORTB |= (1<<DHT11_PIN);    /* set to high pin */
}

void Response() {   /* receive response from DHT11 */
	DDRB &= ~(1<<DHT11_PIN);
	while(PINB & (1<<DHT11_PIN));
	while((PINB & (1<<DHT11_PIN)) == 0);
	while(PINB & (1<<DHT11_PIN));
}

uint8_t Receive_data() {   /* receive data */
	uint8_t c = 0;
	for (int q = 0; q < 8; q++) {
		while((PINB & (1<<DHT11_PIN)) == 0);  /* check received bit 0 or 1 */
		_delay_us(30);
		if(PINB & (1<<DHT11_PIN)) {    /* if high pulse is greater than 30ms */
			c = (c<<1) | (0x01);    /* then it's logic HIGH */
			} else {            /* otherwise it's logic LOW */
			c = (c<<1);
		}
		while(PINB & (1<<DHT11_PIN));
	}
	return c;
}

int main(void) {
	char data[10];  // Ensure array is large enough to store the formatted strings
	SSD1306_Init(SSD1306_ADDR);  // 0x3C
	SSD1306_ClearScreen();
	SSD1306_SetPosition(7, 1);  // set position for "WELCOME"
	SSD1306_DrawString("WELCOME");
	SSD1306_UpdateScreen(SSD1306_ADDR);  // update

	while (1) {
		Request();        /* send start pulse */
		Response();        /* receive response */
		I_RH = Receive_data();    /* store first eight bits in I_RH */
		D_RH = Receive_data();    /* store next eight bits in D_RH */
		I_Temp = Receive_data();    /* store next eight bits in I_Temp */
		D_Temp = Receive_data();    /* store next eight bits in D_Temp */
		CheckSum = Receive_data();/* store next eight bits in CheckSum */

		if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum) {
			SSD1306_ClearScreen();
			SSD1306_SetPosition(0, 1);  // Set position for the "ERROR" message
			SSD1306_DrawString("ERROR");
			SSD1306_UpdateScreen(SSD1306_ADDR);
			} else {
			SSD1306_ClearScreen();

			// Display Humidity
			SSD1306_SetPosition(0, 0);  // Position for "Humidity: "
			SSD1306_DrawString("Humidity:");
			sprintf(data, "%d", I_RH);  // Use %d to print the integer part
			SSD1306_DrawString(data);
			SSD1306_DrawString(".");
			sprintf(data, "%d", D_RH);  // Use %d to print the decimal part
			SSD1306_SetPosition(75, 0);  // Position (x = 80, y = 0) for RH decimal
			SSD1306_DrawString(data);
			SSD1306_DrawString("%");

			// Display Temperature
			SSD1306_SetPosition(0, 1);  // Position for "Temp: "
			SSD1306_DrawString("Temperature:");
			sprintf(data, "%d", I_Temp);  // Use %d for integer part of temperature
			SSD1306_DrawString(data);
			SSD1306_DrawString(".");
			sprintf(data, "%d", D_Temp);  // Use %d for decimal part
			SSD1306_SetPosition(89, 1);  // Position (x = 80, y = 1) for Temperature decimal
			SSD1306_DrawString(data);
			SSD1306_DrawString("C");

			// Display Checksum
			SSD1306_SetPosition(0, 2);  // Position for "Checksum: "
			SSD1306_DrawString("Checksum: ");
			sprintf(data, "%d", CheckSum);  // Use %d to print the checksum
			SSD1306_DrawString(data);
			SSD1306_DrawString(" ");

			SSD1306_UpdateScreen(SSD1306_ADDR);
		}

		_delay_ms(1000);  // Delay to allow for screen update and prevent flicker
	}
}
