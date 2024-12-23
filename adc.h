#ifndef _ADC_H
#define _ADC_H

#include <avr/io.h>
#include "stdutils.h"

// Function prototypes
void ADC_Init();
uint16_t ADC_GetAdcValue(uint8_t v_adcChannel_u8);

#endif
