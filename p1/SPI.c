#include "SPI.h"
#include "Arial12x12.h"
extern ARM_DRIVER_SPI Driver_SPI1 ;
ARM_DRIVER_SPI * SPIsrv=&Driver_SPI1;
TIM_HandleTypeDef timer7;
unsigned char buffer[512];

uint16_t positionL1, positionL2;
//Funciones init
void init_spi(void){
  
  GPIO_InitTypeDef gpio;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  gpio.Pin=RESET_spi|A0|CS;
  gpio.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Mode=GPIO_MODE_OUTPUT_PP;
  
  HAL_GPIO_Init(GPIOB,&gpio);
  HAL_GPIO_WritePin(GPIOB,RESET_spi,SET);// reset a nivel alto aun por decidir
  HAL_GPIO_WritePin(GPIOB,A0|CS,RESET); // cs y A0 aun por decidir temporal
  
  SPIsrv->Initialize(spi_callback);
  SPIsrv->PowerControl(ARM_POWER_FULL);
  SPIsrv->Control(ARM_SPI_MODE_MASTER| ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8),SPIfclk);
  
  //reset del principio
  
  HAL_GPIO_WritePin(GPIOB,RESET_spi,RESET);
  delay(5);// esperar un ms
  HAL_GPIO_WritePin(GPIOB,RESET_spi,SET);
  delay(1000);
  
}

void init_timer(void){
  //el timer tiene que saltar cada 1 us
  __HAL_RCC_TIM7_CLK_ENABLE();
  timer7.Instance=TIM7;
  
  timer7.Init.Prescaler= 48-1 ; // sin configurar
  timer7.Init.Period=4-1; // sin configurar
  timer7.Init.CounterMode=TIM_COUNTERMODE_UP;
  
  HAL_TIM_Base_Init(&timer7);
  
}
//Funciones del lcd
void LCD_init(void){
  
  LCD_wr_cmd(0xae);
  LCD_wr_cmd(0xa2);
  LCD_wr_cmd(0xa0);
  LCD_wr_cmd(0xc8);
  LCD_wr_cmd(0x22);
  LCD_wr_cmd(0x2f);
  LCD_wr_cmd(0x40);
  LCD_wr_cmd(0xaf);
  LCD_wr_cmd(0x81);
  LCD_wr_cmd(0x17);
  LCD_wr_cmd(0xa4);
  LCD_wr_cmd(0xa6);
}

void LCD_reset(){
  
  init_spi();

}

void LCD_wr_data(unsigned char data){
  ARM_SPI_STATUS stat;
  HAL_GPIO_WritePin(GPIOB,CS,RESET);
  HAL_GPIO_WritePin(GPIOB,A0,SET);
  
  SPIsrv->Send(&data,1);
  // esperar a que termine incluir
  do{
    stat=SPIsrv->GetStatus();
      
  }while(stat.busy);
  HAL_GPIO_WritePin(GPIOB,CS,SET);
  
}
void LCD_wr_cmd(unsigned char cmd){
	unsigned char cmf;
	cmf=cmd;
  ARM_SPI_STATUS stat;
  HAL_GPIO_WritePin(GPIOB,CS,RESET);
  HAL_GPIO_WritePin(GPIOB,A0,RESET);
  
  SPIsrv->Send(&cmd,1);
  // esperar a que termine incluir
  do{
    stat=SPIsrv->GetStatus();
      
  }while(stat.busy);
  HAL_GPIO_WritePin(GPIOB,CS,SET);  
  
  
}

void LCD_update(void){
  int i;
  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd(0xB0); // Página 0
  for(i=0;i<128;i++){
    LCD_wr_data(buffer[i]);
  }
  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd(0xB1); // Página 1
  for(i=128;i<256;i++){
    LCD_wr_data(buffer[i]);
  }
  LCD_wr_cmd(0x00);
  LCD_wr_cmd(0x10);
  LCD_wr_cmd(0xB2); //Página 2
  for(i=256;i<384;i++){
    LCD_wr_data(buffer[i]);
  }
  LCD_wr_cmd(0x00);
  LCD_wr_cmd(0x10);
  LCD_wr_cmd(0xB3); // Pagina 3
  for(i=384;i<512;i++){
    LCD_wr_data(buffer[i]);
  }
  
}

//Funcion delay en us
void delay(uint32_t us){
  uint8_t flag=0;
  //maximo de 
  timer7.Init.Period=us-1;
  HAL_TIM_Base_Init(&timer7);
  HAL_TIM_Base_Start(&timer7);
  
  while(!flag){
    flag=__HAL_TIM_GET_FLAG(&timer7,TIM_FLAG_UPDATE);
 
  }
  __HAL_TIM_CLEAR_FLAG(&timer7,RESET);
  
  
   HAL_TIM_Base_Stop(&timer7);
  //poner a cero el taimer
  timer7.Instance->CNT=0;  
  
  
}


//interrupciones y callbacks
void spi_callback(uint32_t event){
  
  
}
void LCD_symbolToLocalBuffer_L1(uint8_t symbol){
  uint16_t offset=0;
  uint8_t value1,value2,i;
  offset=25*(symbol - ' ');
  for(i=0;i<12;i++){
    value1=Arial12x12[offset+i*2+1];
    value2=Arial12x12[offset+i*2+2];
    buffer[i+positionL1]=value1;
    buffer[i+positionL1+128]=value2;
    
  }
  positionL1=positionL1 +Arial12x12[offset];
}
void LCD_symbolToLocalBuffer_L2(uint8_t symbol){
  uint16_t offset=0;
  uint8_t value1,value2,i;
  offset=25*(symbol - ' ');
  for(i=0;i<12;i++){
    value1=Arial12x12[offset+i*2+1];
    value2=Arial12x12[offset+i*2+2];
    buffer[i+positionL2+256]=value1;
    buffer[i+positionL2+384]=value2;
    
  }
  positionL2=positionL2 +Arial12x12[offset];
}
void LCD_symbolToLocalBuffer(uint8_t line, uint8_t symbol){
  
  (line==1)? LCD_symbolToLocalBuffer_L1( symbol):LCD_symbolToLocalBuffer_L2( symbol);
  
}
void LCD_StringWrite(uint8_t line ,uint8_t* buff, uint16_t length){
	positionL1=0;
	positionL2=0;
	
//  LCD_clear();
//	LCD_update();
  if(line==1){
      LCD_clear_L1();
  }else{
      LCD_clear_L2();
  }
  
  for(int i=0; i<length;i++){
    LCD_symbolToLocalBuffer(line, buff[i]);
    
  }
	LCD_update();

}

void LCD_StringWrite_Negative(uint8_t line ,uint8_t* buff, uint16_t length){
	positionL1=0;
	positionL2=0;
	
  LCD_clear();
	LCD_update();
	
  for(int i=0; i<length;i++){
    LCD_symbolToLocalBuffer(line, buff[i]);
    
  }
	
  for(int i=0; i<512;i++){
		buffer[i]=~buffer[i];	
		
  }		
	LCD_update();

}

void LCD_clear(void){
  for(int i=0; i<512;i++){
		buffer[i]=0;	
		
  }	
}
void LCD_clear_L1(void){
  for(int i=0; i<256;i++){
    buffer[i]=0;
  }
}

void LCD_clear_L2(void){
  for(int i=256; i<512;i++){
    buffer[i]=0;
  }
  
}
	

