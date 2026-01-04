#ifndef VOL_DATA_TYPE_H
#define VOL_DATA_TYPE_H

#include <stdint.h>

typedef struct {
    float voltage;
    float current;
    uint8_t status;
} VolDataTypeDef;

#endif // VOL_DATA_TYPE_H
