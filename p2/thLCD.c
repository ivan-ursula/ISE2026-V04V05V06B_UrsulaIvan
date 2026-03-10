#include "SPI.h"
#include "cmsis_os2.h"                      
#include "stm32f4xx_hal.h"

osThreadId_t thLCD;                        // thread id
osMessageQueueId_t msglcd;
void th_lcd (void );                   // thread function

cola_LCD colaLCD;

int init_thLCD (void) {
  init_spi();
  init_timer();
	delay(1000);
  LCD_init();
	delay(1000);
	 
	msglcd=osMessageQueueNew(10,sizeof(cola_LCD),NULL) ;
  thLCD = osThreadNew(th_lcd, NULL, NULL);
  if (thLCD == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void th_lcd (void) {
	osStatus_t stat;

  while (1) {
    stat=	osMessageQueueGet(msglcd,&colaLCD,NULL,osWaitForever); 

			LCD_StringWrite(colaLCD.linea,colaLCD.msg,colaLCD.length);

		osThreadYield();
  }
}
