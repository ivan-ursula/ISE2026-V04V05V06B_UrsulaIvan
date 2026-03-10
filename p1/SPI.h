#ifndef __SPI_H
#define __SPI_H

#include "stm32f4xx_hal.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"

#define SPIfclk 20000000

typedef enum {
   CS =GPIO_PIN_4,
   RESET_spi = GPIO_PIN_2,
   A0 = GPIO_PIN_3
  
}SPI_pin;

typedef struct{
  uint8_t val,length;
	char msg[255];
  uint8_t linea;
  
  
}cola_LCD;

//Funciones init
void init_spi(void);
void init_timer(void);

//Funcion manejo del lcd
void LCD_init(void);// cambiar freq
void LCD_reset(void);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);
void LCD_update(void);
void LCD_symbolToLocalBuffer_L1(uint8_t symbol);
void LCD_symbolToLocalBuffer_L2(uint8_t symbol);
void LCD_symbolToLocalBuffer(uint8_t line, uint8_t symbol);
void LCD_StringWrite(uint8_t line ,uint8_t* buff, uint16_t length);
void LCD_StringWrite_Negative(uint8_t line ,uint8_t* buff, uint16_t length);
void LCD_clear(void);
void LCD_clear_L1(void);
void LCD_clear_L2(void);
//Funcion delay
void delay(uint32_t us);
//interrupciones y callbacks
void spi_callback(uint32_t event);

#endif
