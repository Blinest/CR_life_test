#ifndef VOL_SIGN_TYPE_H
#define VOL_SIGN_TYPE_H

#include <stdint.h>

typedef struct {
    uint32_t rawAdcValue;
    uint32_t timestamp;
} VolSignTypeDef;

#endif // VOL_SIGN_TYPE_H
