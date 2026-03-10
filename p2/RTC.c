#include "RTC.h"
#include <stdio.h>
#include <string.h>

RTC_HandleTypeDef RtcHandle;
extern osThreadId_t thLed;


void Error_Handler(void)
{
  /* Manda mensaje  */
  printf("Hubo un error al inicializar el RTC");
  while (1)
  {
  }
}

void RTC_CalendarConfig(void)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Lunes 2 de Marzo de 2026 */
  sdatestructure.Year = 0x26;
  sdatestructure.Month = RTC_MONTH_MARCH;
  sdatestructure.Date = 0x02;
  sdatestructure.WeekDay = RTC_WEEKDAY_MONDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 14:00:00 */
  stimestructure.Hours = 0x14;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;
  stimestructure.TimeFormat = RTC_HOURFORMAT_24;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  
  
  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

void Init_RTC(void)
 {
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct; 
  
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();
  
  /*##-2- Configure LSE as RTC clock source ###################################*/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    Error_Handler();
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    Error_Handler();
  }
   
   
   
    __HAL_RCC_RTC_ENABLE();
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  /* Inicializar la alarma*/
  
  RTC_AlarmTypeDef alarma;
  alarma.AlarmTime.Hours=0;
  alarma.AlarmTime.Minutes=0;
  alarma.AlarmTime.Seconds=5;
  alarma.AlarmTime.SubSeconds=0;
  alarma.AlarmTime.TimeFormat=RTC_HOURFORMAT_24;
  
  alarma.AlarmTime.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
  alarma.AlarmTime.StoreOperation=RTC_STOREOPERATION_RESET;
  
  alarma.AlarmMask=RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
  
  alarma.AlarmSubSecondMask=RTC_ALARMSUBSECONDMASK_ALL;
  alarma.AlarmDateWeekDaySel=RTC_ALARMDATEWEEKDAYSEL_DATE;
  
  alarma.AlarmDateWeekDay=0x01;
  alarma.Alarm=RTC_ALARM_A;
  
  
  HAL_RTC_SetAlarm_IT(&RtcHandle,&alarma,RTC_FORMAT_BCD);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
  
  RTC_CalendarConfig();
  
  
}
void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf((char *)showtime, "%2d:%2d:%2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  /* Display date Format : mm-dd-yy */
  sprintf((char *)showdate, "%2d-%2d-%2d", sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);
}
void RTC_Alarm_IRQHandler(void){
  HAL_RTC_AlarmIRQHandler(&RtcHandle);
  
}
//void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef* RtcHandle){
//  

//}
