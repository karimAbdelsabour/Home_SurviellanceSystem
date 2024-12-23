#include "adc.h"
#include "delay.h"

/***************************************************************************************************
                         void ADC_Init()
****************************************************************************************************
 * I/P Arguments: none.
 * Return value  : none

 * Description   : This function initializes the ADC module.
***************************************************************************************************/
void ADC_Init() {
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC, prescaler = 128
    ADMUX = (1 << REFS0);  // Reference voltage = AVcc, select channel 0
}

/***************************************************************************************************
                         uint16_t ADC_GetAdcValue(uint8_t v_adcChannel_u8)
****************************************************************************************************
 * I/P Arguments: uint8_t(channel number).
 * Return value  : uint16_t(10-bit ADC result)

 * Description   : This function performs ADC conversion for the selected channel
 *                 and returns the converted 10-bit result.
***************************************************************************************************/
uint16_t ADC_GetAdcValue(uint8_t v_adcChannel_u8) {
    ADMUX = (ADMUX & 0xF0) | (v_adcChannel_u8 & 0x0F); // Set channel, preserve reference voltage
    DELAY_us(10); // Wait for the channel to stabilize
    util_BitSet(ADCSRA, ADSC); // Start the ADC conversion
    while (util_IsBitCleared(ADCSRA, ADIF)); // Wait until the conversion is complete
    util_BitSet(ADCSRA, ADIF); // Clear the ADIF flag
    return ADCW; // Return the 10-bit ADC result
}
