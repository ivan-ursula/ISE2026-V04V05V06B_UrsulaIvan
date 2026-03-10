                    
#include "RTC.h"
#include "SPI.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "rl_net.h"

osThreadId_t thRTC;                        // thread id
osMessageQueueId_t msgrtc;
extern osMessageQueueId_t msglcd;

extern osThreadId_t thLed;

rtc_t fechas;
cola_LCD lcd;
void th_rtc (void *argument);                   // thread function
struct tm ts;
 
osTimerId_t tim_idRTC;

osTimerId_t tim_BLINK;
uint8_t auxcont=0;
uint8_t fblink=1;

uint32_t exec2;
void TimerRTC_Callback (void const *arg);
void TimerBLINK_Callback (void const *arg);

//client SNTP
const NET_ADDR4 ntp_server = { NET_ADDR_IP4, 123, 216, 239, 35, 4 };

void time_callback(uint32_t seconds, uint32_t seconds_fraction);



int init_thRTC (void) {
 
  thRTC = osThreadNew(th_rtc, NULL, NULL);
  
  exec2 = 1U;
  tim_idRTC = osTimerNew((osTimerFunc_t)&TimerRTC_Callback, osTimerPeriodic, &exec2, NULL);
  
  tim_BLINK=osTimerNew((osTimerFunc_t)&TimerBLINK_Callback, osTimerPeriodic, &exec2, NULL);
  osTimerStart(tim_idRTC, 30000U); 
  
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
  // netSNTPc_GetTimeX ("time1.google.com", &f, NULL);
//  if(  netSNTPc_GetTime ((NET_ADDR *)&ntp_server, time_callback)==netOK){
//    printf ("SNTP request sent.\n");
//  }
    
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
 /*----------------------------------------------------------------------------
*     CALLBACK
 *---------------------------------------------------------------------------*/
void time_callback(uint32_t seconds, uint32_t seconds_fraction){
 
 struct tm* ptr_ts;
  time_t sys_time=(time_t)seconds +3600;
  ptr_ts=localtime(&sys_time);
  ts=*ptr_ts;
  RTC_CalendarConfig(ts);
  //ts=*localtime(&sys_time);


}

void TimerRTC_Callback (void const *arg) {
  netSNTPc_GetTime ((NET_ADDR *)&ntp_server, time_callback);
  osTimerStart(tim_BLINK,200);
}

void TimerBLINK_Callback (void const *arg){
  if(auxcont>=20){
    
    osThreadFlagsSet(thLed, 0x08);
    fblink=0;
    auxcont=0;
    osTimerStop(tim_BLINK);
  }
  
  if(fblink==1){
    osThreadFlagsSet(thLed, 0x04);
    fblink=0;
  }else{
  
  osThreadFlagsSet(thLed, 0x08);
    fblink=1;
  }
  auxcont++;
  
  
}
