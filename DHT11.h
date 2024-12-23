#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

#define DHT11_PIN 6

extern uint8_t I_RH, D_RH, I_Temp, D_Temp, CheckSum;

// Function prototypes
void Request(void);
void Response(void);
uint8_t Receive_data(void);

#endif // DHT11_H
