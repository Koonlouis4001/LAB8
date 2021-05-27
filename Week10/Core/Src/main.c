/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include <stdio.h>
#include <string.h>
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
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char TxDataBuffer[32] =
{ 0 };
char RxDataBuffer[32] =
{ 0 };
char ButtonStatus[32] =
{ 0 };

char button[] = " x. Back \r\n Button is pressed\r\n\r\n";
char unbutton[] = " x. Back \r\n Button is unpressed\r\n\r\n";
char off[] = " LED is stop working\r\n\r\n";
char on[] = " LED is start working\r\n\r\n";
char state = '0';

char error[] = "Wrong Input\r\n\r\n";
int Input = 0;
int firsttime = 0;
int Working = 1;
int frequency = 10;

int16_t inputchar = -1;
int16_t lastinputchar = -1;

uint64_t timestamp = 0;
uint64_t _micros = 0;

GPIO_PinState User_Button[2];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM5_Init(void);
/* USER CODE BEGIN PFP */
//void UARTRecieveAndResponsePolling();
int16_t UARTRecieveIT();
uint64_t micros();
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim5);
  {
	  char temp[]="NOW LOADING... \r\nLAB9 is now operating \r\n\r\n";
	  HAL_UART_Transmit(&huart2, (uint8_t*)temp, strlen(temp),1000);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(micros()-timestamp > (1000000/(2*frequency)) && Working)
	  {
		  timestamp = micros();
		  if(frequency != 0)
		  {
			  HAL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
		  }
	  }
	  if(Working == 0)
	  {
		  HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin, GPIO_PIN_RESET);
	  }
	  /*Method 1 Polling Mode*/

	  //UARTRecieveAndResponsePolling();

	  		/*Method 2 Interrupt Mode*/
	  char Frequencyline[] = "Frequency is";
	  HAL_UART_Receive_IT(&huart2,  (uint8_t*)RxDataBuffer, 32);

	  		/*Method 2 W/ 1 Char Received*/
	  int16_t inputchar = UARTRecieveIT();
	  if(inputchar!= -1)
	  {

	  	sprintf(TxDataBuffer, "ReceivedChar:[%c]\r\n\r\n", inputchar);
	  	HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer), 1000);
	  	Input = 1;
	  }
	  else
	  {
		  Input = 0;
	  }

	  		/*This section just simmulate Work Load*/
	  //HAL_Delay(100);
	  //HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

	  switch(state)
	  {
	  	  case '0': //0 start
	  	  {
	  		char temp[] = " 0. LED Control \r\n 1. Button Status\r\n\r\n";
	  		HAL_UART_Transmit(&huart2, (uint8_t*)temp, strlen(temp),1000);
	  		state = '1'; //1 selectMode
	  	  }
	  	  break;
	  	  case '1': //1 selectMode
	  	  {
	  		  switch(inputchar)
	  		  {
	  		  	  case '0'://LED Control
	  		  	  {
	  		  		  char temp[] = " a. Speed up \r\n s. Speed down\r\n d. On/Off\r\n x. Back\r\n\r\n";
	  		  		  HAL_UART_Transmit(&huart2, (uint8_t*)temp, strlen(temp),1000);
	  		  		  state = '2';//LED Control state
	  		  		  break;
	  		  	  }
	  		  	  case '1'://Button Status
	  		  	  {
	  		  		  state = '3';//Button Status state
	  		  		  firsttime = 0;
	  		  		  break;
	  		  	  }
	  		  	  default:
	  		  		  if(Input != 0)
	  		  		  {
	  		  			  HAL_UART_Transmit(&huart2, (uint8_t*)error, strlen(error),1000);
	  		  			  state = '0';//start
	  		  		  }
	  		  		  break;
	  		  }
	  	  }
	  	  break;
	  	  case '2'://LED Control state
	  	  {
	  		  switch(inputchar)
	  		  {
	  		  	  case 'a': //speed up
	  		  	  case 'A':
	  		  	  {
	  		  		  if(frequency < 20)
	  		  		  {
	  		  			  frequency += 1;
	  		  		  }
	  		  		  sprintf(Frequencyline, "Frequency is [%d] Hz\r\n\r\n", frequency);
	  		  		  HAL_UART_Transmit(&huart2, (uint8_t*)Frequencyline, strlen(Frequencyline),1000);
	  		  		  char Frequencyline[32] = { 0 };
	  		  		  break;
	  		  	  }
	  		  	  break;
	  		  	  case 's': //speed down
	  		  	  case 'S':
	  		  	  {
	  		  		  if(frequency > 0)
	  		  		  {
		  		  		  frequency += -1;
	  		  		  }
	  		  		  sprintf(Frequencyline, "Frequency is [%d] Hz\r\n\r\n", frequency);
	  		  		  HAL_UART_Transmit(&huart2, (uint8_t*)Frequencyline, strlen(Frequencyline),1000);
	  		  		  break;
	  		  	  }
	  		  	  break;
	  		  	  case 'd': //on off
	  		  	  case 'D':
	  		  	  {
	  		  		  if(Working)
	  		  		  {
	  		  			  Working = 0;
	  		  			  HAL_UART_Transmit(&huart2, (uint8_t*)off, strlen(off),1000);
	  		  		  }
	  		  		  else
	  		  		  {
	  		  			  Working = 1;
	  		  			  HAL_UART_Transmit(&huart2, (uint8_t*)on, strlen(on),1000);
	  		  		  }
	  		  		  break;
	   		  	  }
	  		  	  break;
	  		  	  case 'x': //back
	  		  	  case 'X':
	  		  	  {
	  		  		  state = '0';//start
	  		  		  break;
	  		  	  }
	  		  	  default:
	  		  		  if(Input != 0)
	  		  		  {
	  		  			  char temp[] = " a. Speed up \r\n s. Speed down\r\n d. On/Off\r\n x. Back\r\n\r\n";
	  		  			  HAL_UART_Transmit(&huart2, (uint8_t*)error, strlen(error),1000);
	  		  			  HAL_UART_Transmit(&huart2, (uint8_t*)temp, strlen(temp),1000);
	  		  		  }
	  		  		  break;
	  		  }
	  	  }
	  	  break;
	  	  case '3'://Button Status state
	  	  {
	  		  User_Button[0] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
	  		  if(User_Button[1] != User_Button[0])
	  		  {
	  			  firsttime = 0;
	  		  }
	  		  User_Button[1] = User_Button[0];
	  		  if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET && firsttime == 0)
	  		  {
	  			  HAL_UART_Transmit(&huart2, (uint8_t*)button, strlen(button),1000);
	  			  firsttime += 1;
	  		  }
	  		  else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET && firsttime == 0)
	  		  {
	  			  HAL_UART_Transmit(&huart2, (uint8_t*)unbutton, strlen(unbutton),1000);
	  			  firsttime += 1;
	  		  }
	  		  switch(inputchar)
	  		  {
				case 'x': //back
				case 'X':
	  			{
	  				state = '0';//start
	  				break;
	  			}
	  			default:
	  				if(Input != 0)
	  				{
	  					HAL_UART_Transmit(&huart2, (uint8_t*)error, strlen(error),1000);
	  					firsttime = 0;
	  				}
	  				break;
	  		  }
	  	  }
	  	  break;
//	  	  case '4'://Press
//	  	  {
//	  		  HAL_UART_Transmit(&huart2, (uint8_t*)error, strlen(error),1000);
//	  		  char button[] = " x. Back \r\n Button is pressed\r\n\r\n";
//	  		  HAL_UART_Transmit(&huart2, (uint8_t*)button, strlen(button),1000);
//	  		  state = '3';
//	  	  }
//	  	  break;
//	  	  case '5'://Unpress
//	  	  {
//	  		  HAL_UART_Transmit(&huart2, (uint8_t*)error, strlen(error),1000);
//	  		  char button[] = " x. Back \r\n Button is unpressed\r\n\r\n";
//	  		  HAL_UART_Transmit(&huart2, (uint8_t*)button, strlen(button),1000);
//	  		  state = '3';
//	  	  }
//	  	  break;

	  }

	  lastinputchar = inputchar;
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 99;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

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
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim5)
	{
		_micros += 4294967295;
	}
}

uint64_t micros()
{
	return (_micros + htim5.Instance->CNT);//counter of Timer 5
}

//void UARTRecieveAndResponsePolling()
//{
//	char Recieve[32]={0};
//
//	HAL_UART_Receive(&huart2, (uint8_t*)Recieve, 32, 1000);
//
//	sprintf(TxDataBuffer, "Received:[%s]\r\n", Recieve);
//	HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer), 1000);
//
//}

int16_t UARTRecieveIT()
{
	static uint32_t dataPos =0;
	int16_t data=-1;
	if(huart2.RxXferSize - huart2.RxXferCount!=dataPos)
	{
		data = RxDataBuffer[dataPos];
		dataPos= (dataPos+1)%huart2.RxXferSize;
	}
	return data;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	sprintf(TxDataBuffer, "Received:[%s]\r\n", RxDataBuffer);
	HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer), 1000);
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
