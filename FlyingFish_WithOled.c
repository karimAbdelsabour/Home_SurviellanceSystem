#include <avr/io.h>
#include <stdio.h>
#include "FlyingFish.h"
#include "ssd1306.h"
#include "delay.h"
#include "adc.h"
char data[16]; // Buffer for storing formatted strings

int main(void) {
	// Initialize modules
	ADC_Init();
	SSD1306_Init(SSD1306_ADDR); // OLED initialization
	
	// Display calibration status
	SSD1306_SetPosition(0, 0);  // Correct position for "Calibrating..."
	SSD1306_DrawString("Calibrating...");
	SSD1306_UpdateScreen(SSD1306_ADDR);

	// Perform sensor calibration
	float Ro = SensorCalibration();
	DELAY_ms(2000); // Wait for a while before starting gas detection

	while (1) {
		// Read LPG concentration
		SSD1306_ClearScreen();  // Clear screen before updating the display

		// Display LPG
		SSD1306_SetPosition(0, 0);  // Position for "LPG: "
		SSD1306_DrawString("LPG:");
		float lpg_ratio = ReadSensor() / Ro;
		sprintf(data, "%3d ppm", GetGasPercentage(lpg_ratio, LPG));
		SSD1306_SetPosition(40, 0);  // Position (x = 80, y = 0) for RH decimal
		SSD1306_DrawString(data);
		
		// Read Smoke concentration
		SSD1306_SetPosition(0, 1); // Set position for smoke data
		SSD1306_DrawString("SMOKE: ");
		float smoke_ratio = ReadSensor() / Ro;
		sprintf(data, "%3d ppm", GetGasPercentage(smoke_ratio, SMOKE));
		SSD1306_DrawString(data);

		SSD1306_UpdateScreen(SSD1306_ADDR); // Update the screen with new values
		DELAY_ms(500); // Update every 500ms
	}
}
