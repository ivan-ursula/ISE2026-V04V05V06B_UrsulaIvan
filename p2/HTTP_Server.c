/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "main.h"
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "SPI.h"
//#include "Board_LED.h"                  // ::Board Support:LED
//#include "Board_Buttons.h"              // ::Board Support:Buttons
//#include "Board_ADC.h"                  // ::Board Support:A/D Converter
//#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
//#include "GLCD_Config.h"                // Keil.MCBSTM32F400::Board Support:Graphic LCD

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

//extern GLCD_FONT GLCD_Font_6x8;
//extern GLCD_FONT GLCD_Font_16x24;

//extern uint16_t AD_in          (uint32_t ch);
//extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

//extern bool LEDrun;
//extern char lcd_text[2][20+1];

//extern osThreadId_t TID_Display;
//extern osThreadId_t TID_Led;

//bool LEDrun;
//char lcd_text[2][20+1] = { "LCD line 1",
//                           "LCD line 2" };

/* Thread IDs */
//osThreadId_t TID_Display;

osThreadId_t thLed;
static uint8_t flag;

/* Thread declarations */
//static void BlinkLed (void *arg);
//static void Display  (void *arg);
static void blink (void *arg);

__NO_RETURN void app_main (void *arg);

/* Read analog inputs */
//uint16_t AD_in (uint32_t ch) {
//  int32_t val = 0;

//  if (ch == 0) {
//    ADC_StartConversion();
//    while (ADC_ConversionDone () < 0);
//    val = ADC_GetValue();
//  }
//  return ((uint16_t)val);
//}

/* Read digital inputs */
//uint8_t get_button (void) {
//  return ((uint8_t)Buttons_GetState ());
//}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
//    osThreadFlagsSet (TID_Display, 0x01);
  }
}

/*----------------------------------------------------------------------------
  Inicializar LED
 *---------------------------------------------------------------------------*/
static void initi_gpio_Led(void){
  GPIO_InitTypeDef timgpio;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  timgpio.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14;
  timgpio.Mode = GPIO_MODE_OUTPUT_PP;
  timgpio.Pull = GPIO_NOPULL;
  timgpio.Speed = GPIO_SPEED_FREQ_LOW;
	
  HAL_GPIO_Init(GPIOB, &timgpio);
}

/*----------------------------------------------------------------------------
  Thread LED INIT
 *---------------------------------------------------------------------------*/
int init_thLed (void){
  thLed = osThreadNew(blink, NULL, NULL);
  if (thLed == NULL){
    return(-1);
  }
  return(0);
}

static void blink (void *arg){
  initi_gpio_Led();
  while(1){
    flag = osThreadFlagsWait(0x0f, osFlagsWaitAny, osWaitForever);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,flag&0x01);
    
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,flag&0x02);
    
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,flag&0x04);
   if(flag==0x08){
          HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14,0);
    }
    
    osThreadYield();
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;
  init_thLCD();
  init_thLed();
  init_thadc();
//	Init_Thread_LCD();
//  LED_Initialize();
//  Buttons_Initialize();
//  ADC_Initialize();

  netInitialize ();

//  TID_Led  = osThreadNew (BlinkLed, NULL, NULL);
//  TID_Display = osThreadNew (Display,  NULL, NULL);

  osThreadExit();
}

