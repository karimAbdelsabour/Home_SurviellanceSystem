#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include "ssd1306.h"
#include <util/delay.h>
#include "FlyingFish.h"
#include "delay.h"
#include "adc.h"
#include <avr/interrupt.h>

#define PIR_SENSOR 3
#define FLAME_SENSOR 2
#define VIBRATION_SENSOR 1
#define ALERT_LED 5

volatile uint8_t PIR_triggered = 0;          // Flag for PIR sensor
volatile uint8_t FLAME_triggered = 0;        // Flag for flame sensor
volatile uint8_t VIBRATION_triggered = 0;    // Flag for vibration sensor
volatile uint8_t GAS_triggered = 0;          // Flag for gas leakage

char data[16]; // Buffer for storing formatted strings

float lpg_ratio;
int lpg_ppm;
float smoke_ratio;
int smoke_ppm;
float Ro;
#define DHT11_PIN 4
uint8_t c = 0, I_RH, D_RH, I_Temp, D_Temp, CheckSum;

volatile char Start_Stop_Flag = 0;

#define SET_BIT(VAR, BIT) (VAR |= (1 << BIT))
#define CLR_BIT(VAR, BIT) (VAR &= (~(1 << BIT)))
#define GET_BIT(VAR, BIT) (((VAR >> BIT)) & 1)

void Display_Readings();

void Request() {
	DDRB |= (1 << DHT11_PIN);
	PORTB &= ~(1 << DHT11_PIN);
	_delay_ms(20);
	PORTB |= (1 << DHT11_PIN);
}

void Response() {
	DDRB &= ~(1 << DHT11_PIN);
	while (PINB & (1 << DHT11_PIN));
	while ((PINB & (1 << DHT11_PIN)) == 0);
	while (PINB & (1 << DHT11_PIN));
}

uint8_t Receive_data() {
	uint8_t c = 0;
	for (int q = 0; q < 8; q++) {
		while ((PINB & (1 << DHT11_PIN)) == 0);
		_delay_us(30);
		if (PINB & (1 << DHT11_PIN)) {
			c = (c << 1) | (0x01);
			} else {
			c = (c << 1);
		}
		while (PINB & (1 << DHT11_PIN));
	}
	return c;
}

void Display_Readings() {
	lpg_ratio = ReadSensor() / Ro;
	lpg_ppm = GetGasPercentage(lpg_ratio, LPG);

	smoke_ratio = ReadSensor() / Ro;
	smoke_ppm = GetGasPercentage(smoke_ratio, SMOKE);

	Request();
	Response();
	I_RH = Receive_data();
	D_RH = Receive_data();
	I_Temp = Receive_data();
	D_Temp = Receive_data();
	CheckSum = Receive_data();

	if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum) {
		SSD1306_ClearScreen();
		SSD1306_SetPosition(0, 1);
		SSD1306_DrawString("ERROR");
		SSD1306_UpdateScreen(SSD1306_ADDR);
		} else {
		SSD1306_ClearScreen();

		SSD1306_SetPosition(0, 0);
		SSD1306_DrawString("LPG: ");
		sprintf(data, "%d ppm", lpg_ppm);
		SSD1306_DrawString(data);

		SSD1306_SetPosition(0, 1);
		SSD1306_DrawString("SMOKE: ");
		sprintf(data, "%d ppm", smoke_ppm);
		SSD1306_DrawString(data);

		SSD1306_SetPosition(0, 2);
		SSD1306_DrawString("Humidity: ");
		sprintf(data, "%d.%d%%", I_RH, D_RH);
		SSD1306_DrawString(data);

		SSD1306_SetPosition(0, 3);
		SSD1306_DrawString("Temp: ");
		sprintf(data, "%d.%dC", I_Temp, D_Temp);
		SSD1306_DrawString(data);

		SSD1306_UpdateScreen(SSD1306_ADDR);
	}
}

int main(void) {
	DDRB |= (1 << ALERT_LED);
	DDRD &= ~(1 << 2);
	PORTD |= (1 << 2);
    PORTB |=(1<<1)|(1<<2)|(1<<3);
	sei();
	EICRA |= (1 << ISC01);
	EICRA &= ~(1 << ISC00);
	EIMSK |= (1 << INT0);

	ADC_Init();
	SSD1306_Init(SSD1306_ADDR);
	SSD1306_ClearScreen();

	SSD1306_SetPosition(7, 1);
	SSD1306_DrawString("WELCOME");
	SSD1306_UpdateScreen(SSD1306_ADDR);
	_delay_ms(3000);
     Ro=SensorCalibration(); 
	while (1) {
		Display_Readings();
		_delay_ms(1000);

		if (Start_Stop_Flag) {
			SSD1306_ClearScreen();
			SSD1306_SetPosition(0, 1);
			SSD1306_DrawString("Surveillance Alarm Activated");
			SSD1306_UpdateScreen(SSD1306_ADDR);
			_delay_ms(3000);

			if (GET_BIT(PINB, FLAME_SENSOR) == 0 && FLAME_triggered == 0) {
				FLAME_triggered = 1;
				SET_BIT(PORTB, ALERT_LED);
				SSD1306_ClearScreen();
				SSD1306_SetPosition(0, 0);
				SSD1306_DrawString("ALERT: FIRE DETECTED!");
				SSD1306_UpdateScreen(SSD1306_ADDR);
				_delay_ms(5000);
				CLR_BIT(PORTB, ALERT_LED);
				} else if (GET_BIT(PINB, FLAME_SENSOR) != 0) {
				FLAME_triggered = 0;
			}

			if (GET_BIT(PINB, VIBRATION_SENSOR) == 0 && VIBRATION_triggered == 0) {
				VIBRATION_triggered = 1;
				SET_BIT(PORTB, ALERT_LED);
				SSD1306_ClearScreen();
				SSD1306_SetPosition(0, 0);
				SSD1306_DrawString("ALERT: EARTHQUAKE DETECTED!");
				SSD1306_UpdateScreen(SSD1306_ADDR);
				_delay_ms(5000);
				CLR_BIT(PORTB, ALERT_LED);
				} else if (GET_BIT(PINB, VIBRATION_SENSOR) != 0) {
				VIBRATION_triggered = 0;
			}

			if (GET_BIT(PINB, PIR_SENSOR) == 0 && PIR_triggered == 0) {
				PIR_triggered = 1;
				SET_BIT(PORTB, ALERT_LED);
				SSD1306_ClearScreen();
				SSD1306_SetPosition(0, 0);
				SSD1306_DrawString("ALERT: THIEF DETECTED!");
				SSD1306_UpdateScreen(SSD1306_ADDR);
				_delay_ms(5000);
				CLR_BIT(PORTB, ALERT_LED);
				} else if (GET_BIT(PINB, PIR_SENSOR) != 0) {
				PIR_triggered = 0;
			}

			if ((lpg_ppm >= 70 || smoke_ppm >= 70) && GAS_triggered == 0) {
				GAS_triggered = 1;
				SET_BIT(PORTB, ALERT_LED);
				SSD1306_ClearScreen();
				SSD1306_SetPosition(0, 0);
				SSD1306_DrawString("ALERT: GAS LEAKAGE!");
				SSD1306_UpdateScreen(SSD1306_ADDR);
				_delay_ms(5000);
				CLR_BIT(PORTB, ALERT_LED);
				} else if (lpg_ppm < 70 && smoke_ppm < 70) {
				GAS_triggered = 0;
			}
		}
	}
}

ISR(INT0_vect) {
	// Debounce the button
	while ((PIND & (1 << 2)) == 0);  // Wait until the button is released

	// Toggle the system state
	if (Start_Stop_Flag) {
		Start_Stop_Flag = 0;  // Turn off the system
		} else {
		Start_Stop_Flag = 1;  // Turn on the system

		// Display the "Surveillance Alarm Activated" message
		SSD1306_ClearScreen();
		SSD1306_SetPosition(0, 1);  // Set position for the message
		SSD1306_DrawString("Surveillance Alarm Activated");
		SSD1306_UpdateScreen(SSD1306_ADDR);
		_delay_ms(3000);  // Display the message for 3 seconds
	}
}
