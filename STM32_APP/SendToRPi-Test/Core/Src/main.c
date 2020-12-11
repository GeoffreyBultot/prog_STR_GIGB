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
#define C_N_BYTES_TOT		(C_N_BYTES_DATA+1)	// 1 byte start + (4) de data + 1 byte checksum + 1 byte stop = 7

#define C_POS_FIRST_DATA 	(0)
#define C_POS_CHECKSUM		(C_N_BYTES_TOT-1)

//#define C_START_BYTE		(0xF4)
//#define C_STOP_BYTE			(0xAA)

//#define CONVERSION_PULSE_TO_DEGREE 1			// Gearbox ratio 1:30 --> 1 output shaft = 30 little gear rotation (LGR)
												// 2 hall sensors
												// motor of 3 pairs of poles --> 6 states/hall sensor/LGR
												// for 1 output shaft turn : 30 LGR * 2 hall sensor * 6 states/hall sensor/ LGR
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

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

_Bool encoder_on_HATrig[2] = {0, 0};
_Bool encoder_on_HBTrig[2]= {0, 0};

signed int pulse_counter = 0;
signed int nbr_degrees_to_send = 0;
unsigned char checksum = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
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
//  bToSend[C_POS_START_BYTE] = C_START_BYTE;
//  bToSend[C_POS_STOP_BYTE] = C_STOP_BYTE;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  // done in gpio init so config in the .ioc
  //HAL_GPIO_WritePin(TX_GPIO_Port, TX_Pin, GPIO_PIN_SET);
  //HAL_TIM_Base_Start_IT(&htim2);
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
	//--> fait en jouant sur les prio et ignorer les entrées Hall if(is_sending)

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
  htim2.Init.Prescaler = 47999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 50;
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
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|DEBUG_ISR_HALL_B_Pin|DEBUG_ISR_HALL_A_Pin|DEBUG_ISR_COMM_Pin, GPIO_PIN_RESET);

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

  /*Configure GPIO pins : LD2_Pin DEBUG_ISR_HALL_B_Pin DEBUG_ISR_HALL_A_Pin DEBUG_ISR_COMM_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|DEBUG_ISR_HALL_B_Pin|DEBUG_ISR_HALL_A_Pin|DEBUG_ISR_COMM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
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
			HAL_GPIO_WritePin(DEBUG_ISR_COMM_GPIO_Port, DEBUG_ISR_COMM_Pin, GPIO_PIN_SET);

			if(!is_sending){ // was in falling edge detection at first
				/* At this point RPi is waiting for us to reset our TX
				 * So can take the time we need (<timeout) to prep the bytes to send before responding
				 */
				// set is_sending flag
				is_sending = 1;
				//Start interrupt for timeout on communication
				HAL_TIM_Base_Start_IT(&htim2);
			    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

				// set pin on rising edge
				GPIO_InitStruct_RX.Mode = GPIO_MODE_IT_RISING;
				HAL_GPIO_Init(RX_GPIO_Port, &GPIO_InitStruct_RX);

				// Debug LED set : rising edge mode notifier
				//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

				// Reset bit & byte counters
				tx_bit_sending = 0;
				tx_byte_sending = 0;

				// NB cannot move in main because it HAS to be finished before we accept to start communication
				// Optimisation : reduce to reduce time in this ISR : minimize the nbr of packets to send
				// prep package to send
				checksum = 0;//(unsigned char) (C_START_BYTE + C_STOP_BYTE);
				nbr_degrees_to_send = pulse_counter;
				for(int i = 0; i < C_N_BYTES_DATA; i++){
					bToSend[C_POS_FIRST_DATA+i] = ((nbr_degrees_to_send >> (i*8)) & 0xFF); //LSB First
					checksum += (unsigned char) bToSend[C_POS_FIRST_DATA+i];
				}
				bToSend[C_POS_CHECKSUM] = checksum;

				// tx pin reset to tell RPi to start its clock
				HAL_GPIO_WritePin(TX_GPIO_Port, TX_Pin, GPIO_PIN_RESET);
			}
			else{	// is sending data on rising edge
				if(tx_byte_sending == C_N_BYTES_TOT)	// last byte+1 : end communication
				{
					// clear is_sending flag first
					is_sending = 0;
					//Start interrupt for timeout on communication
					HAL_TIM_Base_Stop_IT(&htim2);
					htim2.Instance->CNT = 0;

					// tx pin set for ending communication
					HAL_GPIO_WritePin(TX_GPIO_Port, TX_Pin, GPIO_PIN_SET);

					// set pin on falling edge again
					GPIO_InitStruct_RX.Mode = GPIO_MODE_IT_FALLING;
					HAL_GPIO_Init(RX_GPIO_Port, &GPIO_InitStruct_RX);

					// Debug LED reset : falling edge mode notifier

					//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

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
			HAL_GPIO_WritePin(DEBUG_ISR_COMM_GPIO_Port, DEBUG_ISR_COMM_Pin, GPIO_PIN_RESET);
			break;

		case Hall_A_Pin:
			HAL_GPIO_WritePin(DEBUG_ISR_HALL_A_GPIO_Port, DEBUG_ISR_HALL_A_Pin, GPIO_PIN_SET);
			// first we need to know the state of Hall_B
			encoder_on_HATrig[1] = HAL_GPIO_ReadPin(Hall_B_GPIO_Port, Hall_B_Pin);
			encoder_on_HATrig[0] = HAL_GPIO_ReadPin(Hall_A_GPIO_Port, Hall_A_Pin);

			//TODO move in main
			// increment or decrement nbr of pulse measured according to the direction of rotation
			// if it is rotating clock wise (CW), A always trigs before B, so they're always different
			if(encoder_on_HATrig[0] != encoder_on_HATrig[1]){
				pulse_counter++;	// atomic operation
			}
			else{
				pulse_counter--;
			}
			HAL_GPIO_WritePin(DEBUG_ISR_HALL_A_GPIO_Port, DEBUG_ISR_HALL_A_Pin, GPIO_PIN_RESET);
			break;
		case Hall_B_Pin:
			HAL_GPIO_WritePin(DEBUG_ISR_HALL_B_GPIO_Port, DEBUG_ISR_HALL_B_Pin, GPIO_PIN_SET);
			// first we need to know the state of Hall_B
			encoder_on_HBTrig[1] = HAL_GPIO_ReadPin(Hall_B_GPIO_Port, Hall_B_Pin);
			encoder_on_HBTrig[0] = HAL_GPIO_ReadPin(Hall_A_GPIO_Port, Hall_A_Pin);
			//TODO move in main
			// increment or decrement nbr of pulse measured according to the direction of rotation

			// if it is rotating clock wise (CW), B always trigs after A, so they're always equal
			if(encoder_on_HBTrig[0] == encoder_on_HBTrig[1]){
				pulse_counter++;	// atomic operation
			}
			else{
				pulse_counter--;
			}
			HAL_GPIO_WritePin(DEBUG_ISR_HALL_B_GPIO_Port, DEBUG_ISR_HALL_B_Pin, GPIO_PIN_RESET);
			break;
	}
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    // clear is_sending flag first
    					is_sending = 0;
    					//Start interrupt for timeout on communication
    					HAL_TIM_Base_Stop_IT(&htim2);
    					htim2.Instance->CNT = 0;

    					// tx pin set for ending communication
    					HAL_GPIO_WritePin(TX_GPIO_Port, TX_Pin, GPIO_PIN_SET);

    					// set pin on falling edge again
    					GPIO_InitStruct_RX.Mode = GPIO_MODE_IT_FALLING;
    					HAL_GPIO_Init(RX_GPIO_Port, &GPIO_InitStruct_RX);

    					// Debug LED reset : falling edge mode notifier

    					//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    					// prep for start counting from 0 at the end of the communication
    					//counter_Hall_A = 0;
}


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
