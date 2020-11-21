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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

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

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /*	TIMMER 2 CONFIG		*/
   LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
   LL_TIM_EnableIT_UPDATE(TIM2);
   LL_TIM_EnableCounter(TIM2);

   /*	DMA CONFIG		*/
   LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_6,
     						 	LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_RECEIVE),
     							(uint32_t)USART2_DMA_Buffer,
     							LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_6));

     LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_6, USART2_DMA_BUFFER_SIZE);
     LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_6);
     LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_6);
     LL_USART_EnableDMAReq_RX(USART2);

   /*	USART CONFIG	*/
     LL_USART_Disable(USART2);
     LL_USART_EnableIT_IDLE(USART2);
     LL_USART_Enable(USART2);

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
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }
  LL_Init1msTick(8000000);
  LL_SetSystemCoreClock(8000000);
}

/* USER CODE BEGIN 4 */
void setDutyCycle(uint8_t D){
	if(D>99)
		D=99;
	if(D<0)
		D = 0;

	TIM2->CCR1 = D;

}
void MAIN_TIM2_Callback(){

	static uint8_t cnt_brightnes = 0;

	if(MAIN_OperationalMode == MAIN_MODE_AUTO){

		if(!(MAIN_ManualBrightnes == 99 || MAIN_ManualBrightnes == 0 ))
			MAIN_ManualBrightnes = 99;
		if(cnt_brightnes == 99)
			MAIN_ManualBrightnes = 0;
		if(cnt_brightnes == 0)
					MAIN_ManualBrightnes = 99;

		cnt_brightnes += (MAIN_ManualBrightnes > cnt_brightnes)? +1: -1;

		setDutyCycle(cnt_brightnes);

	}else{

		cnt_brightnes += (MAIN_ManualBrightnes < cnt_brightnes)? -1: +1;
		// viem bude to oscilovat okolo zelanej hodnoty.

		setDutyCycle(cnt_brightnes);
	}

}

int8_t FindStrInStr(char* ref, char* cmp){
// strings must end with '\0' == 0
// return  start positin of str
// fcn is not defined for empty imputs

	uint8_t i_ref = 0;


	for(uint8_t i = 0 ; i<255 ; i++){

		// check end of string
		if(ref[i_ref] == '\0')
			return i-i_ref; // ref string was found
		if(cmp[i] == '\0')
			return -1; // could not find string

		if(cmp[i] == ref[i_ref])
			i_ref++; // find next char
		else if(cmp[i] == ref[0])
			i_ref = 1;	// it already find start character
		else
			i_ref = 0; // start finding string from beginning
	}
return -1; //error

}

uint8_t MAIN_ProceessData(){
	// retrun 1 if all data was processed and buffer can be reseted

	const char strMan[] = "$manual$";
	const char strAuto[] = "$automatic$";
	const char strPwm[] = "$PWM"; //XX$"

	int8_t idMan = 0;
	int8_t idPwm = 0;
	int8_t idAuto = 0;

	int8_t idManEnd = 0;
	int8_t idPwmEnd = 0;
	int8_t idAutoEnd = 0;


	// get mode from data
	for(uint8_t i=0; i < USART2_DMA_BUFFER_SIZE ;i++){

		if(idMan >= 0 ){
			const int8_t id = FindStrInStr( (char*)strMan, (char*)&USART2_DMA_Buffer[i]);
			idMan = (id>=0)? i+id: -1;
		}
		if(idAuto >= 0 ){
			const int8_t id = FindStrInStr( (char*)strAuto, (char*)&USART2_DMA_Buffer[i]);
			idAuto = (id>=0)? i+id: -1;
		}

		if(idMan>idAuto && idMan >= 0 ){
			MAIN_OperationalMode = MAIN_MODE_MAN;
			i = idMan + 7;
			idManEnd = i+1;
			idMan = 0;

		}
		if(idAuto>idMan  && idAuto >= 0){
			MAIN_OperationalMode = MAIN_MODE_AUTO;
			i = idAuto + 10;
			idAutoEnd = i+1;
			idAuto = 0;
		}

		if(idAuto == -1 && idMan==-1)
			break;

	}


	// GET manual pwm from data
	if(MAIN_OperationalMode == MAIN_MODE_MAN){

		for(uint8_t i = idManEnd; i<USART2_DMA_BUFFER_SIZE ; i++){
			if(idPwmEnd >= 0 ){
				idPwm = i + FindStrInStr((char*)strPwm, (char*)&USART2_DMA_Buffer[i]);
				if(idPwm >= 0){
					const char ch1	 = USART2_DMA_Buffer[idPwm+4]; // "PWMx--" 0-9
					const char ch0 	 = USART2_DMA_Buffer[idPwm+5]; // "PWM-x-" 0-9
				    const char chEnd = USART2_DMA_Buffer[idPwm+6]; // "PWM--x" $
					if( chEnd == '$' &&
						ch0 >= '0' && ch0 <= '9' &&
						ch1 >= '0' && ch1 <= '9' ){
							MAIN_ManualBrightnes = 10*(ch1-'0') + (ch0-'0');
							idPwmEnd = idPwm+7;
						}
				}
			}
		}
	} // end of if mode == manual

	//	Is there some unfinished packet?

	//find highest
	int8_t lastValidId = 0;
	if(idAutoEnd > idManEnd)
		lastValidId = idAutoEnd;
	if(idManEnd > idAutoEnd)
		lastValidId = idManEnd;
	if(idPwmEnd > idManEnd)
		lastValidId = idPwmEnd;

	uint8_t idSpecialCh = 0;
	uint8_t idEmptyCh = 0;
	for(uint8_t i = lastValidId; i<USART2_DMA_BUFFER_SIZE ; i++){

		if( USART2_DMA_Buffer[i] == '$')
			idSpecialCh = i;
		if( USART2_DMA_Buffer[i] == 0){
			idEmptyCh = i;
			break;
		}

	}

	if((idSpecialCh - idEmptyCh) > MAIN_MAX_PACKET_SIZE)
		return 1; // remaining data are not valid, reset buffer
	else
		return 0;

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
