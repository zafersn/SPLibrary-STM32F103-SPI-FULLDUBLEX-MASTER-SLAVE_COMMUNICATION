/**
  ******************************************************************************
  STM32F103RB NUCLEO KITI UZERINDE SLAVE CIHAZ OLARAK ÇALISTIRILMISTIR.
  
  STM32F103C8 ISE MASTER CIHAZ OLARAK AYARLANDI.

  Interrupt  Kullanilarak Veri okuma ve veri yazma islemleri YAPILDI.

  SPI1 Uzerinde denendi

  SPI_data_direction   FullDuplex  olarak ayarlandi.

 Interruptlarin handler lari IT.c dosyasinda tanimlandi.

  Test edildi Hatasiz çalisiyor.

  MOSI          PA7
  MISO          PA6
   SCK          PA5     
 
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "platform_config.h"


/* Private define ------------------------------------------------------------*/
#define BufferSize 32
#define txBufferSize 5

/* Private variables ---------------------------------------------------------*/
SPI_InitTypeDef SPI_InitStructure;
uint8_t SPI_MASTER_Buffer_Tx[txBufferSize] = {'S','e','l','a','m'};
uint8_t SPI_SLAVE_Buffer_Rx[BufferSize];
__IO uint8_t TxIdx = 0, RxIdx = 0;

/* Private functions ---------------------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  
       
  /* System clocks configuration ---------------------------------------------*/
  RCC_Configuration();

  /* NVIC configuration ------------------------------------------------------*/
  NVIC_Configuration();

  /* GPIO configuration ------------------------------------------------------*/
  GPIO_Configuration();


  /* SPI_SLAVE configuration -------------------------------------------------*/
  
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI_SLAVE, &SPI_InitStructure);

  /* Enable SPI_SLAVE TXE interrupt */
  SPI_I2S_ITConfig(SPI_SLAVE, SPI_I2S_IT_TXE, ENABLE);
  /* Enable SPI_SLAVE RXNE interrupt */
  SPI_I2S_ITConfig(SPI_SLAVE, SPI_I2S_IT_RXNE, ENABLE);

  /* Enable SPI_SLAVE */
  SPI_Cmd(SPI_SLAVE, ENABLE);
 
 

  while (1)
  {}
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* PCLK2 = HCLK/2 */
  RCC_PCLK2Config(RCC_HCLK_Div2); 

/* Enable peripheral clocks --------------------------------------------------*/

  /* Enable SPI_SLAVE clock and GPIO clock for SPI_SLAVE */
  RCC_APB2PeriphClockCmd(SPI_SLAVE_GPIO_CLK  | SPI_SLAVE_CLK, ENABLE);
  /* Enable SPI_SLAVE Periph clock */
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;


  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  /* Configure SPI_SLAVE pins: SCK and MISO ---------------------------------*/
  /* Configure SCK and MOSI pins as Input Floating */
  GPIO_InitStructure.GPIO_Pin = SPI_SLAVE_PIN_SCK | SPI_SLAVE_PIN_MOSI;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SPI_SLAVE_GPIO, &GPIO_InitStructure);
  /* Configure MISO pin as Alternate Function Push Pull */
  GPIO_InitStructure.GPIO_Pin = SPI_SLAVE_PIN_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_SLAVE_GPIO, &GPIO_InitStructure);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* 1 bit for pre-emption priority, 3 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 

  /* Configure and enable SPI_SLAVE interrupt --------------------------------*/
  NVIC_InitStructure.NVIC_IRQChannel = SPI_SLAVE_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/