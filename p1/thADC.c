
#include "adc.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t thADC;                        // thread id
 ADC_HandleTypeDef adchandle;
osMessageQueueId_t msgadc;
 
void th_adc(void *argument);                   // thread function
 
int init_thadc(void) {
  ADC1_pins_F429ZI_config();
  ADC_Init_Single_Conversion(&adchandle,ADC1);
  thADC = osThreadNew(th_adc, NULL, NULL);
  
  msgadc=osMessageQueueNew(5,sizeof(uint32_t),NULL) ;
  if (thADC == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void th_adc (void *argument) {
  uint32_t value;
  while (1) {
    
  value=ADC_getVoltage(&adchandle,10);
   osMessageQueuePut(msgadc, &value,0,0);
   osDelay(1000);   
   //osThreadYield();
  }
}
