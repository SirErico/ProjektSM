/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "adc.h"
#include "eth.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pid_controller_config.h"
#include "btn_config.h"
#include "encoder_config.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


// Definiowanie zmiennych //

int rpm = 0;
int rpm_ref_t = 0;
float temp = 0;
int Duty = 50;
int rpm_ref = 2000;
float e = 0;
_Bool mode = 0;
uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
uint8_t Is_First_Captured = 0;
uint32_t frequency = 0;
int prd = 0;
int niwim = 0;
uint8_t rx_buffer[32];
uint16_t msg_len;
int enc_raw = 0;
int enc_mod = 0;


/**
  * @brief  Input Capture callback
  * @param  htim TIM handle
  * @retval None
  */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{


	/********************************************//**
	 *  Pomiar z tachometru
	 ***********************************************/


	if(htim == &htim2)
	{
				HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
				{
					if (Is_First_Captured==0) // if the first rising edge is not captured
					{
						IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
						Is_First_Captured = 1;  // set the first captured as true
					}

					else   // If the first rising edge is captured, now capture the second edge
					{
						IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value

						if (IC_Val2 > IC_Val1)
						{
							Difference = IC_Val2-IC_Val1;
						}

						else if (IC_Val1 > IC_Val2)
						{
							Difference = (0xffffffff - IC_Val1) + IC_Val2;
						}
						frequency = HAL_RCC_GetPCLK1Freq()/Difference;  // read frequency from timer clock
						rpm = frequency*30;
						__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
						Is_First_Captured = 0; // set it back to false
				}
			}
	}
}





/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim4)
  {

	  /********************************************//**
	   *  Sterowanie pwm za pomocą regulatora PID
	   ***********************************************/


		  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint32_t)(Duty));
	  	  Duty = PID_GetOutput(&hpid1, rpm_ref, rpm);
	  	  e = rpm_ref - rpm;


		  /********************************************//**
		   *  Zmiana trybu sterowania wartością ustalaną
		   ***********************************************/


	  	if(BTN_DIO_EdgeDetected(&husrbtn) == BTN_PRESSED_EDGE){
	  		if (mode == 1)
	  			mode = 0;
	  		else if(mode == 0)
	  			mode=1;
	  	}




		  /********************************************//**
		   *  Sterowanie wartością ustalaną za pomocą enkodera
		   ***********************************************/


	  	enc_raw = __HAL_TIM_GET_COUNTER(&htim8);
	  	if(enc_raw<100)
	  		enc_mod=1000+enc_raw;  // if enc value is below the minimum work range put it to the minimum working value and add unmodified amount
	  	else
	  		enc_mod=enc_raw * 10; // if enc value is within the work range multiply it for easier maneuverability
 if(mode==1)
	 rpm_ref = enc_mod;

  }

  /********************************************//**
   *  Wysyłanie informacji o działaniu układu przez port szeregowy
   ***********************************************/

 if(htim == &htim7)
 {
	 uint8_t tx_buffer[64];
	 int resp_len = sprintf((char*)tx_buffer, "{ \"RPM\":%d, \"RPM_REF\":%d, \"Duty\":%d }\r", rpm, rpm_ref, Duty); // Creating a message with RPM, RPM_REF, DUTY to send to termial
	 HAL_UART_Transmit(&huart3, tx_buffer, resp_len, 10);
	 }
 }




/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	  /********************************************//**
	   *  Kontrolowanie wartości ustalanej za pomocą terminalu
	   ***********************************************/
	if(mode == 0){
  if(huart == &huart3)
	  {
      sscanf((char*)&rx_buffer, "%d", &rpm_ref); // Get value from terminal
	  }
  HAL_UART_Receive_IT(&huart3, rx_buffer, msg_len);
}
}




/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */

  // Inicjacja timerów //
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim4);
  HAL_TIM_Base_Start_IT(&htim7);
  msg_len = strlen("000\r");
  HAL_UART_Receive_IT(&huart3, rx_buffer, msg_len);
  HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
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
