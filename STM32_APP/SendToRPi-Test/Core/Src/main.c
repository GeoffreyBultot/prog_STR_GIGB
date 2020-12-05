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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define C_N_BITS 			(8)
#define C_N_BYTES_DATA 		(sizeof(int))
#define C_N_BYTES_TOT		(C_N_BYTES_DATA+4)	// 1 byte start + (4) de data + 1 byte d'erreur + 1 byte checksum + 1 byte stop = 8

#define C_POS_START_BYTE 	(0)
#define C_POS_FIRST_DATA 	(1)
#define C_POS_CHECKSUM		(C_N_BYTES_TOT-2)
#define C_POS_STOP_BYTE 	(C_N_BYTES_TOT-1)

#define C_START_BYTE		(0xF4)
#define C_STOP_BYTE			(0xAA)

#define CONVERSION_PULSE_TO_RPM 83.33	// (60/(360*0.002)) \approx 83.33
#define CONVERSION_TICKS_TO_NS	83.33	// 1/12MEG \approx 83.33ns
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
unsigned char bToSend[C_N_BYTES_TOT];
int tx_byte_sending;
int tx_bit_sending;
_Bool is_sending = 0;
GPIO_InitTypeDef GPIO_InitStruct_RX =	{
												.Pin = RX_Pin,
												.Mode = GPIO_MODE_IT_FALLING,
												.Pull = GPIO_NOPULL
										};
int counter_Hall_A = 0;
int counter_Hall_A_to_send = 0;
int delta_time_between_HA_HB_ns = 0;
unsigned int ticks_counter = 0;
int speed_RPM = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  // init bytes to send
  for(int i=0; i<C_N_BYTES_TOT; i++){
	  bToSend[i] = 0x00;
  }
  bToSend[C_POS_START_BYTE] = C_START_BYTE;
  bToSend[C_POS_STOP_BYTE] = C_STOP_BYTE;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  // init timer
  HAL_TIM_Base_Start_IT(&htim3);
  // done in gpio init so config in the .ioc
  //HAL_GPIO_WritePin(TX_GPIO_Port, TX_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	// Lecture des capteurs à effet hall
	/*
	if(delta_time_Hall_A > delta_time_Hall_B){
		direction_of_rotation = HORAIRE;
	}
	*/

	// Stockage dans des variables différentes de celles pour la comm
	// pour éviter les accès concurrents

	// timer

    /* USER CODE END WHILE */

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
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
  htim3.Init.Prescaler = 4;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
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

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DEBUG_INT_GPIO_Port, DEBUG_INT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TX_GPIO_Port, TX_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DEBUG_INT_Pin */
  GPIO_InitStruct.Pin = DEBUG_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DEBUG_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TX_Pin */
  GPIO_InitStruct.Pin = TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(TX_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RX_Pin */
  GPIO_InitStruct.Pin = RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RX_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Hall_A_Pin Hall_B_Pin */
  GPIO_InitStruct.Pin = Hall_A_Pin|Hall_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	switch(GPIO_Pin){
		case RX_Pin:
			// Debug Interrupt Pin SET to see when we enter in the callback function
			HAL_GPIO_WritePin(DEBUG_INT_GPIO_Port, DEBUG_INT_Pin, GPIO_PIN_SET);

			if(!is_sending){ // was in falling edge detection at first
				/* At this point RPi is waiting for us to reset our TX
				 * So can take the time we need (<timeout) to prep the bytes to send before responding
				 */
				//bToSend[C_POS_CHECKSUM] = checksum;
				// set is_sending flag
				is_sending = 1; // NB : hall sensor counting has stopped, since is_sending is true


				// set pin on rising edge
				GPIO_InitStruct_RX.Mode = GPIO_MODE_IT_RISING;
				HAL_GPIO_Init(RX_GPIO_Port, &GPIO_InitStruct_RX);

				// Debug LED set : rising edge mode notifier
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

				// Reset bit & byte counters
				tx_bit_sending = 0;
				tx_byte_sending = 0;

				// compute RPM speed
				speed_RPM = delta_time_between_HA_HB_ns/counter_Hall_A*1;//CONVERSION_PULSE_TO_RPM;

				// prep package to send
				for(int i = 0; i < C_N_BYTES_DATA; i++){
					bToSend[C_POS_FIRST_DATA+i] = ((speed_RPM >> (i*8)) & 0xFF); //LSB First
					//checksum += bToSend[C_POS_FIRST_DATA+i];
					//bToSend[C_POS_FIRST_DATA+i] = ((counter_Hall_A >> (i*8)) & 0xFF); //LSB First
				}

				// tx pin reset to tell RPi to start its clock
				HAL_GPIO_WritePin(TX_GPIO_Port, TX_Pin, GPIO_PIN_RESET);
			}
			else{	// is sending data on rising edge
				if(tx_byte_sending == C_N_BYTES_TOT)	// last byte+1 : end communication
				{
					// clear is_sending flag first
					is_sending = 0;

					// tx pin set for ending communication
					HAL_GPIO_WritePin(TX_GPIO_Port, TX_Pin, GPIO_PIN_SET);

					// set pin on falling edge again
					GPIO_InitStruct_RX.Mode = GPIO_MODE_IT_FALLING;
					HAL_GPIO_Init(RX_GPIO_Port, &GPIO_InitStruct_RX);

					// Debug LED reset : falling edge mode notifier
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

					// prep for start counting from 0 at the end of the communication
					//counter_Hall_A = 0;
				}
				else{
					HAL_GPIO_WritePin(TX_GPIO_Port, TX_Pin, (bToSend[tx_byte_sending] & (1<<tx_bit_sending)));
					tx_bit_sending++;
					if(tx_bit_sending == 8){
						tx_bit_sending = 0;
						tx_byte_sending++;
					}
				}
			}
			HAL_GPIO_WritePin(DEBUG_INT_GPIO_Port, DEBUG_INT_Pin, GPIO_PIN_RESET);
			break;

		case Hall_A_Pin:
			if(!is_sending){
				// to get speed
				counter_Hall_A++;
				HAL_TIM_Base_Start(&htim3);
				// to get direction of rotation
				//time_Hall_A = time(NULL);
				//delta_time_Hall_A = difftime(time_Hall_A, time_Hall_B);
			}
			break;

		case Hall_B_Pin:
			if(!is_sending){
				HAL_TIM_Base_Stop(&htim3);
				ticks_counter = htim3.Instance->CNT;
				delta_time_between_HA_HB_ns += ticks_counter;// * CONVERSION_TICKS_TO_NS;
				// just to get the direction of rotation
				//time_Hall_B = time(NULL);
				//delta_time_Hall_B = difftime(time_Hall_B, time_Hall_A);
			}
			break;

	}
}
/*
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	// 2 ms period
	if(htim == &htim3){
		counter_Hall_A_to_send = counter_Hall_A;
		counter_Hall_A = 0;
	}
}
*/
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
