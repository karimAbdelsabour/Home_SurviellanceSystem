#ifndef FLYINGFISH_H
#define FLYINGFISH_H

#include <math.h>

// Constants
#define RL_VALUE (10)                 // Load resistance in kilo-ohms
#define RO_CLEAN_AIR_FACTOR (9.83)    // Clean air factor (sensor resistance in clean air / Ro)
#define LPG (0)                       // Gas identity number for LPG
#define SMOKE (1)                     // Gas identity number for Smoke

// Curve data for LPG and Smoke gases
extern float LPGCurve[3];
extern float SmokeCurve[3];

// Function prototypes
float ResistanceCalculation(int raw_adc);
float SensorCalibration();
float ReadSensor();
int GetGasPercentage(float rs_ro_ratio, int gas_id);
int GetPercentage(float rs_ro_ratio, float *curve);

#endif // FLYINGFISH_H
