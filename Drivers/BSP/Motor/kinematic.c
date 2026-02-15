#include "BSP/Motor/kinematic.h"

#include <math.h>
#include <stdint.h>

#define pi 3.1415926535


void calculate_L(uint8_t R, float theta[], float phi[], float deltaL[]) {
    deltaL[1] = R * theta[0] * cos(phi[0]);
	deltaL[3] = R * theta[0] * cos(phi[0] + 2.0 / 3.0 * pi);
	deltaL[5] = R * theta[0] * cos(phi[0] + 4.0 / 3.0 * pi);
	deltaL[2] = R * theta[0] * cos(phi[0] + pi / 3.0) + R * theta[1] * cos(phi[1] + pi / 3.0);
	deltaL[4] = R * theta[0] * cos(phi[0] + pi) + R * theta[1] * cos(phi[1] + pi);
	deltaL[6] = R * theta[0] * cos(phi[0] + 5.0 / 3.0 * pi) + R * theta[1] * cos(phi[1] + 5.0 / 3.0 * pi);
}
