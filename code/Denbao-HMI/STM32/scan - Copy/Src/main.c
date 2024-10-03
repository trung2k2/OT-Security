/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "enc28j60.h"
#include "net.h"
#include "uart.h"
#include "arp_fake.h"
#include "arp_reborn.h"
#include "scan.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
extern int ARP_table_index;
extern const  uint8_t macaddr[6];
extern  struct
{
   uint8_t ip[4];
   uint8_t mac[6];
}ARP_table[5];
extern  struct
{
   uint8_t ip[4];
   uint8_t mac[6];
}ARP_table_PLC[5];
extern void ARP_send_request_fake(uint8_t *ip_dest, uint8_t *ip_source, uint8_t * MAC_target);
extern void ARP_send_request_reborn(uint8_t *ip_dest, uint8_t *ip_source, uint8_t * MAC_source);
extern void ARP_table_setIP(uint8_t *ip_set, uint8_t *mac_set);
extern int8_t ARP_table_checkIP(uint8_t *ip_check);
uint8_t ip_server[4]={192,168,0,0};
uint8_t ip_ref[4]={192,168,0,0};
int count =0;
int led =1;
uint8_t mac_siemens[6]={0xE0,0xDC,0xA0};
int state=0;
uint8_t zero_ip[4]={0,0,0,0};
extern char debug_string[60];
int m =0,n=1;
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int t =0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	 if(htim->Instance == TIM2)
		 
//	 { if(memcmp(ARP_table[n].mac,mac_siemens,3)==0 && memcmp(ARP_table[m].mac,mac_siemens,3)==0)
	 {
//		 	if(m==ARP_table_index-1)
//		 {
//	  sprintf(debug_string,"So ARP trong bang la: %u\r\n",ARP_table_index); //in ra do dai goi tin
//		UART_putString(debug_string);
//			led++;
//			HAL_TIM_Base_Stop_IT(&htim2);
//		 }
		 if(n == ARP_table_index && m<ARP_table_index-2)
		 {
			 m++;
			 n=m+1;
		 }
		 if(m==ARP_table_index-2 && n == ARP_table_index)
		 {
		UART_putString("\nIn ra dia chi cac thiet bi giao tiep Modbus TCP\r\n");
		sprintf(debug_string,"Dia chi thiet bi 1 Ip %i.%i.%i.%i = Mac %02X:%02X:%02X:%02X:%02X:%02X\r\n",ARP_table_PLC[0].ip[0],ARP_table_PLC[0].ip[1],
		ARP_table_PLC[0].ip[2],	ARP_table_PLC[0].ip[3],ARP_table_PLC[0].mac[0],ARP_table_PLC[0].mac[1],ARP_table_PLC[0].mac[2],ARP_table_PLC[0].mac[3],
		ARP_table_PLC[0].mac[4],ARP_table_PLC[0].mac[5]);
		UART_putString(debug_string);
		sprintf(debug_string,"Dia chi thiet bi 2 Ip %i.%i.%i.%i = Mac %02X:%02X:%02X:%02X:%02X:%02X\r\n",ARP_table_PLC[1].ip[0],ARP_table_PLC[1].ip[1],
		ARP_table_PLC[1].ip[2],	ARP_table_PLC[1].ip[3],ARP_table_PLC[1].mac[0],ARP_table_PLC[1].mac[1],ARP_table_PLC[1].mac[2],ARP_table_PLC[1].mac[3],
		ARP_table_PLC[1].mac[4],ARP_table_PLC[1].mac[5]);
		UART_putString(debug_string);
		led=1;

			 HAL_TIM_Base_Stop_IT(&htim2);
		 }
		 if(state==0 && n < ARP_table_index){
			ARP_send_request_fake(ARP_table[m].ip, ARP_table[n].ip, ARP_table[m].mac);
			ARP_send_request_fake(ARP_table[n].ip, ARP_table[m].ip, ARP_table[n].mac);	
			NET_loop();
			state++;}
		 
		 else if(state ==1 )
		 {
			ARP_send_request_reborn(ARP_table[m].ip, ARP_table[n].ip, ARP_table[n].mac);
			ARP_send_request_reborn(ARP_table[n].ip, ARP_table[m].ip, ARP_table[m].mac);
			NET_loop();
			 n++;
			 state=0;
		 }
//	 }
//	 if(htim->Instance == TIM3)
//	 {
//		UART_putString("Bat timer 3");
//	 }
}}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
	{
		if(GPIO_Pin == GPIO_PIN_0 && count ==0)
	{
		t = 1;
//		UART_putString("In ra dia chi cac thiet bi trong mang\r\n\n");
//		for(int j =0;j<ARP_table_index;j++)
//		{
//		sprintf(debug_string,"Thiet bi %i Ip %i.%i.%i.%i = Mac %02X:%02X:%02X:%02X:%02X:%02X\r\n",j+1,ARP_table[j].ip[0],ARP_table[j].ip[1],
//		ARP_table[j].ip[2],	ARP_table[j].ip[3],ARP_table[j].mac[0],ARP_table[j].mac[1],ARP_table[j].mac[2],ARP_table[j].mac[3],
//		ARP_table[j].mac[4],ARP_table[j].mac[5]);
//		UART_putString(debug_string);
//		}
		
		HAL_TIM_Base_Start_IT(&htim2);
		led=0;
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);
		count++;
	 GPIO_Pin = GPIO_PIN_0 -1;}
	if (GPIO_Pin == GPIO_PIN_0 && count ==1)
	{
		t=2;
		HAL_TIM_Base_Stop_IT(&htim2);
//		HAL_TIM_Base_Start_IT(&htim3);
		ARP_send_request_fake(ARP_table_PLC[0].ip, ARP_table_PLC[1].ip, ARP_table_PLC[0].mac);
		ARP_send_request_fake(ARP_table_PLC[1].ip, ARP_table_PLC[0].ip, ARP_table_PLC[1].mac);
		NET_loop();
		led=0;
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);
		
		count++;
		GPIO_Pin = GPIO_PIN_0 -1;
	}
	if (GPIO_Pin == GPIO_PIN_0 && count == 2)
	{
		ARP_send_request_reborn(ARP_table_PLC[0].ip, ARP_table_PLC[1].ip, ARP_table_PLC[1].mac);
		ARP_send_request_reborn(ARP_table_PLC[1].ip, ARP_table_PLC[0].ip, ARP_table_PLC[0].mac);
		NET_loop();
		led=1;
		count=1;
		GPIO_Pin = GPIO_PIN_0 -1;
	}
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
			uint8_t zero_ip[4]={0,0,0,0};
	memcpy(ARP_table[1].ip, zero_ip, 4);
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
	MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	UART_init(&huart1);
  ENC29J600_ini(&hspi1);
	UART_putString("Khoi tao thanh cong\r\n\nThuc hien quet....\r\n\n");
	ARP_clear_table();
	
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	//ARP_table_checkIP(ip_fake);
	//*ARP_table_setIP(ip_fake,mac_fake);
	//ARP_send_request(ip_server);
	ip_server[3]=2;
			ARP_send_request(ip_server);
      HAL_Delay(50);
		for (int i=1;i<=215;i++)
		{

			ip_server[3]=i;
			ARP_send_request(ip_server);
      HAL_Delay(50);

		}
		UART_putString("Quet hoan tat in ra dia chi cac thiet bi trong mang\r\n\n");
		

  while (1)
  {
    /* USER CODE END WHILE */
			NET_loop();
			HAL_Delay(20);
		if((memcmp(ARP_table[1].ip, zero_ip, 4) != 0 && led == 1)||m==ARP_table_index-1)
		{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);
			HAL_Delay(200);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_SET);
			HAL_Delay(200);
		}
    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7200-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7200-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 9999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
