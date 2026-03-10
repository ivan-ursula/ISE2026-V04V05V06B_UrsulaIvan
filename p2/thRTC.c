#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "RTC.h"
#include "stm32f4xx_hal.h"
#include "SPI.h"
#include <stdio.h>
#include <string.h>
osThreadId_t thRTC;                        // thread id
osMessageQueueId_t msgrtc;
extern osMessageQueueId_t msglcd;
rtc_t fechas;
cola_LCD lcd;
void th_rtc (void *argument);                   // thread function

int init_thRTC (void) {
 
  thRTC = osThreadNew(th_rtc, NULL, NULL);
  Init_RTC();
  //RTC_CalendarConfig();
  msgrtc=osMessageQueueNew(1,sizeof(rtc_t),NULL);
  if (thRTC == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void th_rtc (void *argument) {
 
  while (1) {
   RTC_CalendarShow(fechas.hora, fechas.fecha) ;
    osMessageQueuePut(msgrtc,&fechas,NULL,osWaitForever);
    
//   lcd.length=strlen((char*)fechas.hora);      
//   for(int i=0; i<lcd.length;i++){
//    lcd.msg[i]=fechas.hora[i];
//   }
//   lcd.linea=1;
//   osMessageQueuePut(msglcd,&lcd,NULL,0);

//   lcd.length=strlen((char*)fechas.fecha); 
//   for(int i=0; i<lcd.length;i++){
//    lcd.msg[i]=fechas.fecha[i];
//   }
//   lcd.linea=0;
//   osMessageQueuePut(msglcd,&lcd,NULL,0);   
   
   osDelay(1000);                           // suspend thread
  }
}

