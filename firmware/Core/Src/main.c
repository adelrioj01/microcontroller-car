
// Bluetooth-controlled and autonomous car project

/* USER CODE BEGIN Header */
/**
 **********
 * @file           : main.c
 * @brief          : Main program body
 **********
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 **********
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
ADC_HandleTypeDef hadc;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
int GIRO = 600;
unsigned char pulsacion = 0;
unsigned short tiempo_inicio = 0;
unsigned short tiempo = 0;
int estado = 0;
unsigned short tiempo_2 = 6000;
unsigned short tiempo_3 = 50000;
int estado_2 = 0;
int estado_3 = 0;
int limite = 0;
int espera = 0;
short salta = 0;
int vel = 0;
int DCmax = 0;
int DCmin = 0;
int DCmed = 0;
int estado_4 = 6;
int XD = 0;
uint8_t num_received = '1';
uint8_t text_STOP[7] = " STOP";
uint8_t text_FORWARD[10] = " FORWARD";
uint8_t text_LEFT[7] = " LEFT";
uint8_t text_RIGHT[7] = " RIGHT";
uint8_t text_BACK[7] = " BACK";
uint8_t text_AUTO[7] = " AUTO";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_TS_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void delante() {
	TIM4->CCR3 = DCmax;
	TIM4->CCR4 = DCmax;
	GPIOA->BSRR = (1 << 12 << 16);
	GPIOA->BSRR = (1 << 11 << 16);
}

void atras() {
	TIM4->CCR3 = DCmin;
	TIM4->CCR4 = DCmin;
	GPIOA->BSRR = (1 << 12);
	GPIOA->BSRR = (1 << 11);
}

void giro_derechaA() {
	GPIOA->BSRR = (1 << 11);
	TIM4->CCR4 = 0;
	TIM4->CCR3 = DCmin;
}

void giro_derechaB() {
	GPIOA->BSRR = (1 << 11 << 16);
	TIM4->CCR4 = 0;
	TIM4->CCR3 = DCmax;
}

void giro_izquierdaA() {
	GPIOA->BSRR = (1 << 12);
	TIM4->CCR4 = DCmin;
	TIM4->CCR3 = 0;
}

void giro_izquierdaB() {
	GPIOA->BSRR = (1 << 12 << 16);
	TIM4->CCR4 = DCmax;
	TIM4->CCR3 = 0;
}

void deceleroA() {
	GPIOA->BSRR = (1 << 12 << 16);
	GPIOA->BSRR = (1 << 11 << 16);
	TIM4->CCR3 = DCmin;
	TIM4->CCR4 = DCmin;
}

void parar() {
	GPIOA->BSRR = (1 << 12 << 16);
	GPIOA->BSRR = (1 << 11 << 16);
	TIM4->CCR3 = 0;
	TIM4->CCR4 = 0;
}

void TIM3_IRQHandler(void)       // TIM3 interrupt handler
{
	if ((TIM3->SR & 0x0004) != 0)   // TIM3 channel 2 interrupt
			{

		if (estado_2 == 1) {
			estado_2 = 0;

			TIM3->CCR2 += tiempo_2;
		} else {
			estado_2 = 1;
			TIM3->CCR2 += 1;
		} // This timer sends a 10 us ultrasonic trigger pulse, then waits
		  // 60 ms before sending the next pulse.
		TIM3->SR = 0x0000;         // Clear the timer flags
	}
	if ((TIM3->SR & 0x0002) != 0) // TIM3 channel 1 event
			{

		if (estado_3 == 1) {
			estado_3 = 0;

			TIM3->CCR1 += tiempo_3;
		} else {
			estado_3 = 1;
			TIM3->CCR1 += tiempo_3;
		}       // Register that stores the next compare value
		// Schedule the next timing event.
		TIM3->SR = 0x0000;         // Clear the timer flags
	}

	if ((TIM3->SR & 0x0008) != 0) { // TIM3 channel 3 event

		TIM3->CCR3 += 256;         // 2.56 ms output-compare interval
		// Used to implement non-blocking delays.
		if (limite != 0) {
			espera++;
		}

		TIM3->SR = 0x0000;         // Clear the timer flags

	}
}

void TIM2_IRQHandler(void) {
	if ((TIM2->SR & 0x0002) != 0) {        //Si ha ocurrido el evento en ch1 TIC
		if (pulsacion == 1) { //
			pulsacion = 0;
			tiempo = TIM2->CCR1 - tiempo_inicio;
			tiempo_inicio = TIM2->CCR1; //
			if (tiempo < 0)
				tiempo += 0x0FFFF; //
			TIM2->CCER = (0x0003);
		} else if (pulsacion == 0) { //
			pulsacion = 1;
			tiempo = TIM2->CCR1 - tiempo_inicio; //
			if (tiempo < 0)
				tiempo += 0x0FFFF; //
			TIM2->CCER = (0x0001); //
		}
		//
		TIM2->SR = 0x0000; //
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
	MX_ADC_Init();
	MX_TS_Init();
	MX_USART1_UART_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_TIM4_Init();
	/* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1, &num_received, 1);
	GPIOB->MODER &= ~(1 << (10 * 2 + 1));
	GPIOB->MODER |= (1 << (10 * 2));
	/*GPIOB -> MODER &= ~(1 << (8*2 + 1));        // input 1
	 GPIOB -> MODER |= (1 << (8*2)); */
	GPIOA->MODER &= ~(1 << (11 * 2 + 1));        // input 2
	GPIOA->MODER |= (1 << (11 * 2));
	/*GPIOB -> MODER &= ~(1 << (9*2 +1));         // input 3
	 GPIOB -> MODER |= (1 << (9*2));*/
	GPIOA->MODER &= ~(1 << (12 * 2 + 1));       // input 4
	GPIOA->MODER |= (1 << (12 * 2));

	GPIOA->MODER &= ~(1 << (1 * 2 + 1));
	GPIOA->MODER |= (1 << (1 * 2));

	GPIOB->MODER |= (1 << (8 * 2 + 1));
	GPIOB->MODER &= ~(1 << (8 * 2));

	//IN3(PB9)->AF
	GPIOB->MODER |= (1 << (9 * 2 + 1));
	GPIOB->MODER &= ~(1 << (9 * 2));

	//AFR for PB8 AND PB9(TIM4 CH3 AND CH4)
	GPIOB->AFR[1] = 0x00000022;

	// Internal clock selection: CR1, CR2, and SMCR
	TIM4->CR1 = 0x0080;      // ARPE configured; CEN = 0; counter disabled
	TIM4->CR2 = 0x0000;      // Channel interrupts disabled
	TIM4->SMCR = 0x0000;      // Siempre "0" en este curso

	// Counter configuration: PSC, CNT, ARR, and CCRx
	TIM4->PSC = 319;      // Prescaler: 32 MHz / 320 = 100,000 counts/s
	TIM4->CNT = 0;        // Initialize the counter to zero
	TIM4->ARR = 99;       // PWM auto-reload value

	// Interrupt selection: DIER
	TIM4->DIER = 0x0000;    // No timer interrupt is required for PWM

	TIM4->CCR3 = 50;        // El Duty cycle se pone a 1 inicialmente
	TIM4->CCR4 = 50;        // El Duty cycle se pone a 1 inicialmente

	// Timer output mode
	TIM4->CCMR2 = 0x6868;
	TIM4->CCER = 0x1100;   // CCyNP:CCyP = 00 (activo a flanco de subida)
						   // CCyE = 1: channel output enabled

	// Enable the counter and clear pending flags
	TIM4->EGR |= 0x0001;   // UG = 1: generate an update event
	TIM4->SR = 0;          // Clear timer flags
	TIM4->CR1 |= 0x0001;   // CEN = 1: start the counter

	TIM3->CR1 = 0x0000;  // Output-compare timer; counter initially disabled
	TIM3->CR2 = 0x0000;
	TIM3->SMCR = 0x0000;

	// Counter configuration: PSC, CNT, ARR, and CCRx
	TIM3->PSC = 319;     // Prescaler: 32 MHz / 320 = 100,000 counts/s
	TIM3->CNT = 0;       // Initialize the counter to zero
	TIM3->ARR = 0xFFFF;  // Full 16-bit timer range
	TIM3->CCR2 = tiempo_2;    // Initial output-compare value
	TIM3->CCR1 = tiempo_3;
	TIM3->CCR3 = 256;

	// Initialize each channel with its first compare target.

	// Enable compare interrupts for channels 1 through 3
	TIM3->DIER = 0x000E;

	// Timer output mode
	TIM3->CCMR1 = 0x0000;  // CCyS = 0 (TOC)
	TIM3->CCMR2 = 0x0000;

	// OCyPE = 0 (sin precarga)
	TIM3->CCER = 0x0000;   // CCyP = 0 (siempre para TOC)
						   // CCyE = 0: hardware output disabled

	// Enable the counter and clear pending flags
	TIM3->EGR |= 0x0001;   // UG = 1: generate an update event
	TIM3->SR = 0;          // Clear timer flags
	TIM3->CR1 |= 0x0001;   // CEN = 1: start the counter

	GPIOA->MODER |= 0x00000001 << (2 * 5 + 1); // MODER = 10 (AF) para el bit 0 del
	// puerto A
	GPIOA->MODER &= ~(0x00000001 << (2 * 5));
	GPIOA->AFR[0] |= (0x01 << (5 * 4));      // AFR para decir que el P0A es AF1
	// (TIM2)
	GPIOA->AFR[0] &= ~(0x0E << (5 * 4));
	// Internal clock selection: CR1, CR2, and SMCR
	TIM2->CR1 = 0x0000;      // Input-capture timer; counter initially disabled
	TIM2->CR2 = 0x0000;      // Channel interrupts configured through DIER
	TIM2->SMCR = 0x0000;

	// Counter configuration: PSC, CNT, ARR, and CCRx
	TIM2->PSC = 319;      // Prescaler: 32 MHz / 320 = 100,000 counts/s
	TIM2->CNT = 0;        // Initialize the counter to zero
	TIM2->ARR = 0xFFFF;   // Full 16-bit timer range

	// Enable the channel 1 capture interrupt
	TIM2->DIER = 0x0002;

	// Input-capture mode
	TIM2->CCMR1 = 0x0001;  // CCyS = 1: channel configured as input
	TIM2->CCER = 0x0001;   // CCyNP:CCyP = 00 (activo a flanco de subida)
						   // CCyE = 1: input capture enabled

	// Enable the counter and clear pending flags
	TIM2->EGR |= 0x0001;   // UG = 1: generate an update event
	TIM2->SR = 0;          // Clear timer flags
	TIM2->CR1 |= 0x0001;   // CEN = 1: start the counter

	TIM4->CCER |= (0x01 << 4);
	TIM4->CR1 |= 0x01;

	GPIOA->MODER |= 0x00000300;        // Configure PA4 as analog input
	ADC1->CR2 &= ~(0x00000001);        // ADON = 0: ADC disabled
	ADC1->CR1 = 0x00000000;            // RES = 00 (resolución = 12 bits)
									   // SCAN = 0: scan mode disabled
	// EOC interrupt disabled
	ADC1->CR2 = 0x00000400;           // EOCS = 1 (activado el bit EOC al acabar
	// each conversion)
	// DELS = 000 (sin retardo en la
	// conversion)
	// CONT = 0: single conversion
	ADC1->SMPR1 = 0;                   // Sin sampling time (4 cycles)
	ADC1->SMPR2 = 0;
	ADC1->SMPR3 = 0;
	ADC1->SQR1 = 0x00000000;           // 1 elemento solo en la secuencia
	ADC1->SQR5 = 0x00000004;           // Select analog input channel 4
	ADC1->CR2 |= 0x00000001;           // ADON = 1 (ADC activado)

	NVIC->ISER[0] |= (1 << 28);  // Enable the TIM2 IRQ in the NVIC
	NVIC->ISER[0] |= (1 << 29);
	NVIC->ISER[0] |= (1 << 30);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		switch (num_received) {  // Process commands received over USART
		case '1': // STOP
			estado_4 = 0;
			estado = 0;
			for (int i = 0; i < 5; i++) {
				HAL_UART_Transmit(&huart1, (&text_STOP[i]), 1, 10000);
			}
			num_received = ' ';

			break;
		case '2': // FORWARD
			estado_4 = 1;
			estado = 0;
			for (int i = 0; i < 8; i++) {
				HAL_UART_Transmit(&huart1, (&text_FORWARD[i]), 1, 10000);
			}
			num_received = ' ';

			break;
		case '3': // BACK
			estado_4 = 5;
			estado = 0;
			for (int i = 0; i < 5; i++) {
				HAL_UART_Transmit(&huart1, (&text_BACK[i]), 1, 10000);
			}
			num_received = ' ';

			break;
		case '4': // LEFT
			estado_4 = 4;
			estado = 0;
			for (int i = 0; i < 5; i++) {
				HAL_UART_Transmit(&huart1, (&text_LEFT[i]), 1, 10000);
			}
			num_received = ' ';
			XD = 0;
			break;
		case '5': // RIGHT
			for (int i = 0; i < 6; i++) {
				HAL_UART_Transmit(&huart1, (&text_RIGHT[i]), 1, 10000);
			}
			num_received = ' ';
			estado_4 = 3;
			estado = 0;
			XD = 0;
			break;
		case '6': // AUTONOMOUS
			estado_4 = 6;
			for (int i = 0; i < 5; i++) {
				HAL_UART_Transmit(&huart1, (&text_AUTO[i]), 1, 10000);
			}
			num_received = ' ';
			break;
		}

		if (vel < 2048) {		//ADC
			DCmax = 80;
			DCmin = 40;
			DCmed = 60;
		} else {
			DCmax = 50;
			DCmin = 60;
			DCmed = 25;
		}

		if ((ADC1->SR & 0x0040) != 0)    // Start a conversion when the ADC is ready

			ADC1->CR2 |= 0x40000000;         // Set SWSTART to begin conversion

		// Wait for the conversion to finish
		if ((ADC1->SR & 0x0002) == 0)
			;    // EOC = 0: conversion is still in progress

		vel = ADC1->DR;

		if (tiempo < 70) {     // Continuous buzzer warning at close range
			GPIOA->BSRR = (1 << 1);

		} else if (tiempo < 150 && tiempo >= 70) {
			if (estado_3 == 0) {
				GPIOA->BSRR = (1 << 1);

			} else {
				GPIOA->BSRR = (1 << 1 << 16);
			}

		} else {
			GPIOA->BSRR = (1 << 1 << 16);
		}

		if (estado_2 == 1) {     // Generate the ultrasonic trigger pulse
			GPIOB->BSRR = (1 << (10));
		} else {
			GPIOB->BSRR = (1 << (10) << 16);
		}

		switch (estado_4) { // Execute the action selected by the USART command
		case 0: //stop
			parar();
			break;
		case 1: // move forward
			if (tiempo < 70) {
				parar();
			} else {
				delante();
			}
			break;
		case 2: // decelerate
			deceleroA();
			break;
		case 3: // turn right
			if (XD == 0) {
				parar();
				XD++;
			}
			giro_derechaB();

			break;
		case 4: // turn left
			if (XD == 0) {
				parar();
				XD++;
			}
			giro_izquierdaB();
			break;
		case 5: // reverse
			atras();
			break;
		default:

			switch (estado) {
			case 0: // move forward while the path is clear
				delante();
				if (tiempo < 150)
					estado = 1;
				break;
			case 1: // decelerate when an obstacle is detected
				deceleroA();
				if (tiempo < 70) {
					estado = 2;
					espera = 0;
				} else if (tiempo > 150)
					estado = 0;

				break;
			case 2: // stop before changing direction
				parar();
				limite = 3 * GIRO;
				if (espera > limite) {
					espera = 0;
					limite = 0;
					estado = 3;
				} else {
					break;
				}

				break;
			case 3: // begin the right-turn avoidance maneuver
				giro_derechaA();
				limite = GIRO;
				if (espera > limite) {
					espera = 0;
					limite = 0;
					parar();
					if (tiempo <= 150)
						estado = 4;
					else
						estado = 0;
				} else {
					break;
				}

			case 4: // continue the right-turn maneuver
				giro_derechaB();
				limite = GIRO;
				if (espera > limite) {
					espera = 0;
					limite = 0;
					estado = 5;
				} else {
					break;
				}

				break;
			case 5: // begin the left-turn avoidance maneuver
				giro_izquierdaA();
				limite = GIRO;
				if (espera > limite) {
					espera = 0;
					limite = 0;
				} else {
					break;
				}
				parar();
				if (tiempo <= 150)
					estado = 6;
				else
					estado = 0;
				break;
			case 6: // continue the left-turn maneuver

				giro_izquierdaB();
				limite = GIRO;
				if (espera > limite) {
					espera = 0;
					limite = 0;
				} else {
					break;
				}
				estado = 7;
				break;
			case 7: // reverse to complete the avoidance sequence
				//GPIOB->BSRR = ((1<<9));
				atras();
				limite = GIRO;
				if (espera > limite) {
					espera = 0;
					limite = 0;
				} else {
					break;
				}
				parar();
				break;

			default: // fail-safe stop
				parar();

			}
			break;

		}
	}
	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */
}
/* USER CODE END 3 */

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief ADC Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC_Init(void) {

	/* USER CODE BEGIN ADC_Init 0 */

	/* USER CODE END ADC_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC_Init 1 */

	/* USER CODE END ADC_Init 1 */

	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc.Instance = ADC1;
	hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc.Init.Resolution = ADC_RESOLUTION_12B;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
	hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
	hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
	hadc.Init.ContinuousConvMode = DISABLE;
	hadc.Init.NbrOfConversion = 1;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_CC3;
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc.Init.DMAContinuousRequests = DISABLE;
	if (HAL_ADC_Init(&hadc) != HAL_OK) {
		Error_Handler();
	}

	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC_Init 2 */

	/* USER CODE END ADC_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 65535;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

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
	htim3.Init.Prescaler = 0;
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
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void) {

	/* USER CODE BEGIN TIM4_Init 0 */

	/* USER CODE END TIM4_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };

	/* USER CODE BEGIN TIM4_Init 1 */

	/* USER CODE END TIM4_Init 1 */
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 0;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 65535;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim4) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim4) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM4_Init 2 */

	/* USER CODE END TIM4_Init 2 */
	HAL_TIM_MspPostInit(&htim4);

}

/**
 * @brief TS Initialization Function
 * @param None
 * @retval None
 */
static void MX_TS_Init(void) {

	/* USER CODE BEGIN TS_Init 0 */

	/* USER CODE END TS_Init 0 */

	/* USER CODE BEGIN TS_Init 1 */

	/* USER CODE END TS_Init 1 */
	/* USER CODE BEGIN TS_Init 2 */

	/* USER CODE END TS_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 9600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
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
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pins : SEG14_Pin SEG15_Pin SEG16_Pin SEG17_Pin
	 SEG18_Pin SEG19_Pin SEG20_Pin SEG21_Pin
	 SEG22_Pin SEG23_Pin */
	GPIO_InitStruct.Pin = SEG14_Pin | SEG15_Pin | SEG16_Pin | SEG17_Pin
			| SEG18_Pin | SEG19_Pin | SEG20_Pin | SEG21_Pin | SEG22_Pin
			| SEG23_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : SEG0_Pin SEG1_Pin SEG2_Pin COM0_Pin
	 COM1_Pin COM2_Pin SEG12_Pin */
	GPIO_InitStruct.Pin = SEG0_Pin | SEG1_Pin | SEG2_Pin | COM0_Pin | COM1_Pin
			| COM2_Pin | SEG12_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : SEG6_Pin SEG7_Pin SEG8_Pin SEG9_Pin
	 SEG10_Pin SEG11_Pin SEG3_Pin SEG4_Pin
	 SEG5_Pin */
	GPIO_InitStruct.Pin = SEG6_Pin | SEG7_Pin | SEG8_Pin | SEG9_Pin | SEG10_Pin
			| SEG11_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	HAL_UART_Receive_IT(huart, &num_received, 1); // Re-enable reception after the byte is processed

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
