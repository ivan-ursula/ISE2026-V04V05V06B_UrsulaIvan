#ifndef __STOPMODE_H
#define __STOPMODE_H
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h" 
#include "rl_net.h"
#include "stm32f4xx_hal_eth.h"
#include "adc.h"

void SleepMode_Measure(void);
void ETH_PhyExitFromPowerDownMode(void);
void initi_gpio_Led(void);



#endif
