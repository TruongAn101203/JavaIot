/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

// Global variables:
uint8_t rx_buff = '\0';
uint8_t tx_buff = 0xAA;

// debug
uint8_t pwmA, pwmB, V1, V2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);

uint8_t decode(uint8_t Vin, _Bool Mode);

// return percent of value
uint8_t map_(uint8_t value);

void Stop(void);

void Forward(void);
void Back(void);
void L_Spin(void);
void R_Spin(void);

void pwm(uint8_t pwmA, uint8_t pwmB);
unsigned char map_pwm_bitwise(unsigned char V1, unsigned char V2);

// void pwm_(uint8_t V2, uint8_t V1);

void controll(uint8_t value);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Call this function after receive data from uart
// Active after receive data
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  //  __NOP();
  controll(rx_buff);
  /* Prevent unused argument(s) compilation warning */
  //  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
}

// Call this function after transmit data
// Active after transmit
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  //	__NOP();
  /* Prevent unused argument(s) compilation warning */
  //  UNUSED(huart);
  HAL_UART_Receive_IT(&huart2, &rx_buff, 1);
  //	controll(rx_buff);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
}

uint8_t decode(uint8_t Vin, _Bool Mode)
{
  return (Mode == 1) ? (Vin >> 4) : (Vin & 15);
}

uint8_t map_(uint8_t value)
{
  uint8_t mp[] = {100, 85, 71, 56, 42, 28, 14, 0, 14, 28, 42, 56, 71, 85, 100};
  return mp[value];
}

// void pwm_(uint8_t V2, uint8_t V1)
//{
//
//   if (V1 == 7)
//   {
//		pwmA = pwmB = map_(V2);
//
//
//     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwmA);
//		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwmA);
//
//		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pwmB);
//		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pwmB);
//   }
//	else
//	{
//		if (V2 > 7)
//		{
//			pwmA = map_(V1);
//			pwmB = pwmA * map_(V2) / 100;
//		}
//		if (V2 < 7)
//		{
//			pwmB = map_(V1);
//			pwmA = pwmB * map_(V2) / 100;
//		}
//		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwmA);
//		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pwmA);
//
//		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwmB);
//		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pwmB);
//	}
////  HAL_Delay(50);
//}

void Stop()
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
}

void Forward()
{
  // Motor L up
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_14, GPIO_PIN_SET);
  // Motor R up
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13 | GPIO_PIN_15, GPIO_PIN_RESET);
}
void Back()
{
  // Motor L down
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_14, GPIO_PIN_RESET);
  // Motor R down
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13 | GPIO_PIN_15, GPIO_PIN_SET);
}

void L_Spin()
{
  // Motor L down
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_15, GPIO_PIN_RESET);
  // Motor R up
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_SET);
}
void R_Spin()
{
  // Motor L up
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_15, GPIO_PIN_SET);
  // Motor R down
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);
}

void pwm(uint8_t pwmA, uint8_t pwmB)
{
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwmA);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pwmA);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwmB);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pwmB);
}

unsigned char map_pwm_bitwise(unsigned char V1, unsigned char V2)
{
    unsigned char map[8][8] = {
      {0, 40, 55, 60, 65, 75, 80, 80},
//			{15, 11, 8, 6, 4, 2, 0, 0},      // 1
      {30, 21, 17, 12, 8, 8, 8, 8},    // 2
      {45, 32, 25, 18, 11, 11, 11, 11},   // 3
      {60, 42, 33, 24, 15, 15, 15, 15},   // 4
      {75, 53, 41, 30, 19, 19, 19, 19},   // 5
      {90, 63, 50, 36, 23, 23, 23, 23},   // 6
			{100, 70, 55, 40, 25, 25, 25, 25},  // 7
      {100, 70, 55, 40, 25, 25, 25, 25}  // 7
    };
  // Use for bug joystick V2 can't send 100% (14)
//  unsigned char map[8][8] = {
//      {0, 25, 40, 55, 70, 85, 100, 100}, // 0
// //     {15, 11, 8, 6, 4, 2, 0, 0},      // 1
//      {30, 21, 17, 12, 8, 3, 0, 0},    // 2
//      {45, 32, 25, 18, 11, 5, 0, 0},   // 3
//      {60, 42, 33, 24, 15, 6, 0, 0},   // 4
//      {75, 53, 41, 30, 19, 8, 0, 0},   // 5
//      {90, 63, 50, 36, 23, 9, 0, 0},   // 6
//			{100, 70, 55, 40, 25, 10, 0, 0},  // 7
//      {100, 70, 55, 40, 25, 10, 0, 0}  // 7
//  };
  V1 = (V1 < 7) ? (7 - V1) : (V1 - 7);
  V2 = (V2 < 7) ? (7 - V2) : (V2 - 7);
  return map[V1][V2];
}

void controll(uint8_t value)
{
  //  uint8_t V1, V2;

  V1 = decode(value, 1);
  V2 = decode(value, 0);

  if (V2 == 7)
  {
    if (V1 == 7)
      Stop();
    else
    {
      // pwmA = pwmB = map_(V1);
      pwmA = pwmB = map_pwm_bitwise(V1, V2);
      pwm(pwmA, pwmB);
      //    pwm(map_(V1),map_(V1));
      if (V1 < 7)
        Back();
      else
        Forward();
    }
  }
  if (V2 < 7)
  {
    if (V1 == 7)
    {
      // pwmA = pwmB = map_(V2);
      pwmA = pwmB = map_pwm_bitwise(V1, V2);
      L_Spin();
      pwm(pwmA, pwmB);
      //      pwm(map_(V2),map_(V2));
    }
    else
    {
      // pwmA = map_(V1)-((~(V2-7)+1)<<1);
      // pwmB = map_(V1);
      pwmB = map_pwm_bitwise(V1, 7);
      pwmA = map_pwm_bitwise(V1, V2);
      pwm(pwmA, pwmB);
      //      pwm(map_(V1)-((~(V2-7)+1)<<1), map_(V1));
      if (V1 > 7)
        Forward();
      else
        Back();
    }
  }
  if (V2 > 7)
  {
    if (V1 == 7)
    {
      // pwmA = pwmB = map_(V2);
      pwmA = pwmB = map_pwm_bitwise(V1, V2);
      pwm(pwmA, pwmB);
      //      pwm(map_(V2),map_(V2));
      R_Spin();
    }
    else
    {
      // pwmA = map_(V1);
      // pwmB = map_(V1) - ((V2-7)<<1);
      pwmA = map_pwm_bitwise(V1, 7);
      pwmB = map_pwm_bitwise(V1, V2);
      pwm(pwmA, pwmB);
      //      pwm(map_(V1), map_(V1) - ((V2-7)<<1));
      if (V1 > 7)
        Forward();
      else
        Back();
    }
  }
}
// pwm
//  pwm_(V2, V1);

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
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_UART_Transmit_IT(&huart2, &tx_buff, 1);
    HAL_Delay(100);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 83;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 99;
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
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB12 PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
