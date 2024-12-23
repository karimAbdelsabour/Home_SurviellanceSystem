#include "FlyingFish.h"
#include "adc.h"
#include "delay.h"

// Curve data for LPG and Smoke gases
float LPGCurve[3] = {2.3, 0.20, -0.45};
float SmokeCurve[3] = {2.3, 0.53, -0.43};

// Function to calculate resistance based on ADC value
float ResistanceCalculation(int raw_adc) {
	// Prevent division by zero
	if (raw_adc == 0) raw_adc = 1;
	return ((float)RL_VALUE * (1023 - raw_adc) / raw_adc);
}

// Function to calibrate the sensor and determine Ro
float SensorCalibration() {
	int i;
	float val = 0;

	for (i = 0; i < 50; i++) {
		int raw_adc = ADC_GetAdcValue(0);
		float rs = ResistanceCalculation(raw_adc);
		val += rs;
	}

	val = val / 50; // Average Rs
	float ro = val / RO_CLEAN_AIR_FACTOR;
	return ro;
}

// Function to read the sensor value and calculate resistance
float ReadSensor() {
	int i;
	float rs = 0;

	for (i = 0; i < 5; i++) {
		int raw_adc = ADC_GetAdcValue(0);
		rs += ResistanceCalculation(raw_adc);
		DELAY_ms(50);
	}

	rs = rs / 5; // Average Rs
	return rs;
}

// Function to calculate the gas percentage from the Rs/Ro ratio
int GetGasPercentage(float rs_ro_ratio, int gas_id) {
	if (gas_id == LPG) {
		return GetPercentage(rs_ro_ratio, LPGCurve);
		} else if (gas_id == SMOKE) {
		return GetPercentage(rs_ro_ratio, SmokeCurve);
	}

	return 0;
}

// Function to calculate percentage from Rs/Ro ratio using the sensor curve
int GetPercentage(float rs_ro_ratio, float *curve) {
	return (pow(10, ((log10(rs_ro_ratio) - curve[1]) / curve[2]) + curve[0]));
}
