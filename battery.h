#ifndef BATTERY_H
#define BATTERY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"

// battery configuration
#define BATTERY_FULL 4100
#define BATTERY_LOW 3500
#define BATTERY_SHD 3350

    void checkBattery();

#ifdef __cplusplus
}
#endif

#endif /* BATTERY_H */
