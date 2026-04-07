/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include "dht22.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
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
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
volatile uint8_t state = 0;
uint8_t nim_state = 0;
volatile uint32_t gauge_val = 0;
volatile uint16_t adc_val = 0;

volatile uint32_t last_shift_time = 0;
volatile uint8_t shift_idx = 0;
volatile uint8_t temperature = 0;
volatile uint8_t humidity = 0;

uint8_t virtual_btn = 0;

volatile uint8_t button_interrupted = 0;
volatile uint8_t virtual_interrupt = 0;
uint32_t interrupt_start_time = 0;

GPIO_TypeDef *LED_PORT[8] = { LED1_GPIO_Port, LED2_GPIO_Port, LED3_GPIO_Port,
LED4_GPIO_Port, LED5_GPIO_Port, LED6_GPIO_Port, LED7_GPIO_Port,
LED8_GPIO_Port };
uint16_t LED_PIN[8] = { LED1_Pin, LED2_Pin, LED3_Pin, LED4_Pin, LED5_Pin,
LED6_Pin, LED7_Pin, LED8_Pin };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Set_LED_State(uint8_t mask) {
	for (int i = 0; i < 8; i++) {
		// Cek apakah bit ke-i bernilai 1
		if (mask & (1 << i)) {
			HAL_GPIO_WritePin(LED_PORT[i], LED_PIN[i], GPIO_PIN_SET); // Nyalakan LED
		} else {
			HAL_GPIO_WritePin(LED_PORT[i], LED_PIN[i], GPIO_PIN_RESET); // Matikan LED
		}
	}
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

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
	MX_ADC1_Init();
	MX_TIM3_Init();
	MX_I2C2_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start(&htim3);
	HAL_Delay(3000);

	ssd1306_Init();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	char str_state[20];
	while (1) {
		uint8_t btn_fisik = (HAL_GPIO_ReadPin(Button1_GPIO_Port, Button1_Pin)
				== GPIO_PIN_RESET);
		if (btn_fisik == 1 || virtual_btn == 1) {
			HAL_Delay(50);
			btn_fisik = (HAL_GPIO_ReadPin(Button1_GPIO_Port, Button1_Pin)
					== GPIO_PIN_RESET);
			if (btn_fisik == 1 || virtual_btn == 1) {
				state++;
				if (state > 3)
					state = 0;

				Set_LED_State(0x00);
				shift_idx = 0;
				last_shift_time = 0;
				virtual_btn = 0;

				if (btn_fisik == 1) {
					while (HAL_GPIO_ReadPin(Button1_GPIO_Port, Button1_Pin)
							== GPIO_PIN_RESET)
						;
				}
			}
		}
		if (button_interrupted == 1 || virtual_interrupt == 1) {
			if (virtual_interrupt == 1) {
				button_interrupted = 1;
				virtual_interrupt = 0;
				interrupt_start_time = HAL_GetTick();
			}

			Set_LED_State(0xFF);

			if (HAL_GetTick() - interrupt_start_time >= 5000) {
				button_interrupted = 0;
				Set_LED_State(0x00);
			}
		}

		switch (state) {
		case 0:
			ssd1306_Fill(Black);
			if (HAL_GetTick() - last_shift_time >= 200) {
				last_shift_time = HAL_GetTick();
				Set_LED_State(1 << shift_idx);
				shift_idx++;
				if (shift_idx >= 8)
					shift_idx = 0;
			}
			sprintf(str_state, "State : %d", state);
			ssd1306_SetCursor(15, 25);
			ssd1306_WriteString(str_state, Font_11x18, White);
			ssd1306_UpdateScreen();
			break;

		case 1:
			ssd1306_Fill(Black);
			char str_gauge[20];
			gauge_val++;
			if (nim_state == 0) {
				if (gauge_val <= 3) {
					Set_LED_State(0x01);
					HAL_Delay(250);
				} else if (gauge_val <= 7) {
					Set_LED_State(0x03);
					HAL_Delay(250);
				} else if (gauge_val <= 10) {
					Set_LED_State(0x07);
					HAL_Delay(250);
				} else if (gauge_val <= 14) {
					Set_LED_State(0x0F);
					HAL_Delay(250);
				} else if (gauge_val <= 17) {
					Set_LED_State(0x1F);
					HAL_Delay(250);
				} else if (gauge_val <= 21) {
					Set_LED_State(0x3F);
					HAL_Delay(250);
				} else if (gauge_val <= 24) {
					Set_LED_State(0x7F);
					HAL_Delay(250);
				} else if (gauge_val <= 27) {
					Set_LED_State(0xFF);
					HAL_Delay(250);
				} else {
					Set_LED_State(0xFF);
					HAL_Delay(200);
					Set_LED_State(0x00);
					HAL_Delay(200);
					Set_LED_State(0xFF);
					HAL_Delay(200);
					Set_LED_State(0x00);
					HAL_Delay(200);
					Set_LED_State(0xFF);
					gauge_val = 0;
					nim_state = 1;
				}
				sprintf(str_gauge, "Value: %lu", gauge_val);
				ssd1306_SetCursor(15, 5);
				ssd1306_WriteString(str_gauge, Font_11x18, White);

				sprintf(str_state, "State: %d", state);
				ssd1306_SetCursor(15, 35);
				ssd1306_WriteString(str_state, Font_11x18, White);
				ssd1306_UpdateScreen();
			} else {
				if (gauge_val <= 9) {
					Set_LED_State(0x01);
					HAL_Delay(250);
				} else if (gauge_val <= 18) {
					Set_LED_State(0x03);
					HAL_Delay(250);
				} else if (gauge_val <= 28) {
					Set_LED_State(0x07);
					HAL_Delay(250);
				} else if (gauge_val <= 37) {
					Set_LED_State(0x0F);
					HAL_Delay(250);
				} else if (gauge_val <= 47) {
					Set_LED_State(0x1F);
					HAL_Delay(250);
				} else if (gauge_val <= 56) {
					Set_LED_State(0x3F);
					HAL_Delay(250);
				} else if (gauge_val <= 66) {
					Set_LED_State(0x7F);
					HAL_Delay(250);
				} else if (gauge_val <= 76) {
					Set_LED_State(0xFF);
					HAL_Delay(250);
				} else {
					Set_LED_State(0xFF);
					HAL_Delay(200);
					Set_LED_State(0x00);
					HAL_Delay(200);
					Set_LED_State(0xFF);
					HAL_Delay(200);
					Set_LED_State(0x00);
					HAL_Delay(200);
					Set_LED_State(0xFF);
					gauge_val = 0;
					nim_state = 0;
				}
				sprintf(str_gauge, "Value: %lu", gauge_val);
				ssd1306_SetCursor(15, 5);
				ssd1306_WriteString(str_gauge, Font_11x18, White);

				sprintf(str_state, "State: %d", state);
				ssd1306_SetCursor(15, 35);
				ssd1306_WriteString(str_state, Font_11x18, White);
				ssd1306_UpdateScreen();
			}
			break;

		case 2:
			ssd1306_Fill(Black);
			char str_adc[20];
			HAL_ADC_Start(&hadc1);
			if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
				adc_val = HAL_ADC_GetValue(&hadc1);
				int leds_to_turn_on = (adc_val * 8) / 4095;
				Set_LED_State((1 << leds_to_turn_on) - 1);
			}
			HAL_ADC_Stop(&hadc1);
			sprintf(str_adc, "Value: %u", adc_val);
			ssd1306_SetCursor(5, 5);
			ssd1306_WriteString(str_adc, Font_11x18, White);

			sprintf(str_state, "State: %d", state);
			ssd1306_SetCursor(15, 35);
			ssd1306_WriteString(str_state, Font_11x18, White);
			ssd1306_UpdateScreen();
			break;

		case 3: { // ← kurung kurawal wajib untuk deklarasi variabel di dalam case
			DHT22_Data sensor = DHT22_Read();
			temperature = sensor.temperature;
			humidity = sensor.humidity;

			char str_temp[20];
			char str_hum[20];

			ssd1306_Fill(Black);

			if (sensor.status == 0) {
				uint8_t temp_mask = 0x00;
				if (sensor.temperature > 30.0f)
					temp_mask = 0x0F;
				else if (sensor.temperature > 28.0f)
					temp_mask = 0x07;
				else if (sensor.temperature > 24.0f)
					temp_mask = 0x03;
				else if (sensor.temperature > 22.0f)
					temp_mask = 0x01;

				uint8_t hum_mask = 0x00;
				if (sensor.humidity > 90.0f)
					hum_mask = 0xF0;
				else if (sensor.humidity > 76.0f)
					hum_mask = 0xE0;
				else if (sensor.humidity > 74.0f)
					hum_mask = 0xC0;
				else if (sensor.humidity > 70.0f)
					hum_mask = 0x80;

				Set_LED_State(temp_mask | hum_mask);

				// Jika pembacaan BERHASIL
				// Format: "Temp: 25.4 C" (%.1f artinya 1 angka di belakang koma)
				sprintf(str_temp, "Temp: %.1f C", sensor.temperature);
				sprintf(str_hum, "Humid: %.1f %%", sensor.humidity);
				sprintf(str_state, "State: %d", state);

				// Tampilkan Suhu (Baris 1)
				ssd1306_SetCursor(15, 10);
				ssd1306_WriteString(str_temp, Font_7x10, White);

				// Tampilkan Kelembapan (Baris 2)
				ssd1306_SetCursor(15, 25);
				ssd1306_WriteString(str_hum, Font_7x10, White);

				ssd1306_SetCursor(20, 40);
				ssd1306_WriteString(str_state, Font_7x10, White);
			} else {
				// Jika pembacaan GAGAL
				ssd1306_SetCursor(0, 20);
				ssd1306_WriteString("SENSOR ERROR", Font_11x18, White);
			}

			ssd1306_UpdateScreen();

			HAL_Delay(200);
			break;
		}
		}
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 192;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */

	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(&hadc1) != HAL_OK) {
		Error_Handler();
	}

	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief I2C2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C2_Init(void) {

	/* USER CODE BEGIN I2C2_Init 0 */

	/* USER CODE END I2C2_Init 0 */

	/* USER CODE BEGIN I2C2_Init 1 */

	/* USER CODE END I2C2_Init 1 */
	hi2c2.Instance = I2C2;
	hi2c2.Init.ClockSpeed = 100000;
	hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c2.Init.OwnAddress1 = 0;
	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c2.Init.OwnAddress2 = 0;
	hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C2_Init 2 */

	/* USER CODE END I2C2_Init 2 */

}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 47;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 65535;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, DHT22_Pin | LED6_Pin | LED5_Pin | LED4_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB,
	LED8_Pin | LED7_Pin | LED3_Pin | LED2_Pin | LED1_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : Button1_Pin */
	GPIO_InitStruct.Pin = Button1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(Button1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : Button2_Pin */
	GPIO_InitStruct.Pin = Button2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(Button2_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : DHT22_Pin LED6_Pin LED5_Pin LED4_Pin */
	GPIO_InitStruct.Pin = DHT22_Pin | LED6_Pin | LED5_Pin | LED4_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : LED8_Pin LED7_Pin LED3_Pin LED2_Pin
	 LED1_Pin */
	GPIO_InitStruct.Pin = LED8_Pin | LED7_Pin | LED3_Pin | LED2_Pin | LED1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == Button2_Pin) {
		button_interrupted = 1;
		interrupt_start_time = HAL_GetTick();
	}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
