#ifndef DHT22_H
#define DHT22_H

#include "main.h"
#include <stdint.h>

typedef struct {
    float temperature;
    float humidity;
    uint8_t status;  // 0 = OK, 1 = Error
} DHT22_Data;

DHT22_Data DHT22_Read(void);

#endif /* DHT22_H */
