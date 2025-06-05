/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "cmsis_os.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
// Defines
#define MAX_LEN  64

// Variables
char command[MAX_LEN] = {0};
uint8_t received_data;
uint8_t rxIndex = 0;

// Queue Handles
QueueHandle_t xCommandQueue;

//Function Prototypes
int __io_putchar(int ch);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void LowPriorityTask(void *parameters);
void HighPriorityTask(void *parameters);

// Clock Initialization
void SystemClock_Config(void);

// MAIN FUNCTION
int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_UART4_Init();

  xCommandQueue = xQueueCreate(5, MAX_LEN);
  if (xCommandQueue == NULL)
  {
    for(;;); // infinite loop
  }

  printf("System Initialized...\r\n");
  printf("UART CLI Ready...\r\n");
  printf(">> "); // for CLI experience

  HAL_UART_Receive_IT(&huart4, &received_data, 1);

  xTaskCreate(LowPriorityTask, "low prio", 128, NULL, 1, NULL);
  xTaskCreate(HighPriorityTask, "high prio", 128, NULL, 5, NULL);
  vTaskStartScheduler();

  while (1)
  {

  }
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
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/*
 ----------------  TASKS -------------------
 */
void LowPriorityTask(void *parameters)
{
  while (1)
  {
    while (1)
    {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
  }
  
}

void HighPriorityTask(void *parameters)
{
  char data[MAX_LEN];
  while (1)
  {
    if (xQueueReceive(xCommandQueue, data, portMAX_DELAY) == pdPASS)
    {
      if (strcmp(data, "COMMANDS") == 0)
      { 
        printf("=====COMMANDS=====\r\n");
        printf("1. LED ON  - Turn on Orange LED\r\n");
        printf("2. LED OFF - Turn off Orange LED\r\n");
      }
      else if (strcmp(data, "LED ON") == 0)
      {
        HAL_GPIO_WritePin(LED_ORANGE_GPIO_Port, LED_ORANGE_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
        printf("Debug: Orange LED Turned ON\r\n");
      }
      else if(strcmp(data, "LED OFF") == 0)
      {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
        printf("Debug: Orange LED Turned OFF\r\n");
      }
      else
      {
        printf("Error: Command Not Found\r\n");
      }

      printf(">> "); // for the shell like feel :)
    }
    
  }
  
}


/*
 ----------------  Callback Functions -------------------
 */

 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  if (huart->Instance == UART4)
  {
    if (received_data == '\r')
    {
      command[rxIndex] = '\0';
      rxIndex = 0;
      xQueueSendFromISR(xCommandQueue, command, &xHigherPriorityTaskWoken);
      HAL_UART_Transmit(&huart4, (uint8_t*)"\r\n", 2, portMAX_DELAY);
    }
    else
    {
      HAL_UART_Transmit(&huart4, &received_data, 1, HAL_MAX_DELAY);
      command[rxIndex++] = received_data;  
    }

    HAL_UART_Receive_IT(&huart4, &received_data, 1);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}


// Printf Routed to UART4
int usart_send(int ch)
{
	while(!(UART4->SR & 0x80));
	UART4->DR = (ch & 0xFF);
	return ch;
}

int __io_putchar(int ch)
{
	usart_send(ch);
	return ch;
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
