#ifndef FIRMWARE_H
#define FIRMWARE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal.h"
#include "types.h"

    void startup(setupInstructions setup);

    void loop();

#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_H */
