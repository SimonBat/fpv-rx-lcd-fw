/**
  ***************************************************************************************************************************************
  * @file     bsp.c
  * @owner    SimonBat
  * @version  v0.0.1
  * @date     2022.07.30
  * @update   2022.07.30
  * @brief    fpv_rx_fw v1.0
  ***************************************************************************************************************************************
  * @attention
  *
  * (Where to use)
  *
  ***************************************************************************************************************************************
  */

#include "bsp.h"

/**
  ***************************************************************************************************************************************
  * @brief  System Clock Configuration
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void BSP_System_Clock_Config(void)
{
	RCC_ClkInitTypeDef _rccClkInitStruct={0};
	RCC_OscInitTypeDef _rccOscInitStruct={0};

	HAL_Init();

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	_rccOscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_HSE;
	_rccOscInitStruct.HSEState=RCC_HSE_ON;
	_rccOscInitStruct.PLL.PLLState=RCC_PLL_ON;
	_rccOscInitStruct.PLL.PLLSource=RCC_PLLSOURCE_HSE;
	_rccOscInitStruct.PLL.PLLM=25U;
	_rccOscInitStruct.PLL.PLLN=400U;
	_rccOscInitStruct.PLL.PLLP=RCC_PLLP_DIV2;
	_rccOscInitStruct.PLL.PLLQ=9U;
	if(HAL_OK!=HAL_RCC_OscConfig(&_rccOscInitStruct)){BSP_Error_Handler();}

	/* Activate the OverDrive to reach the 200 MHz Frequency */
	if(HAL_OK!=HAL_PWREx_ActivateOverDrive()){BSP_Error_Handler();}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
	_rccClkInitStruct.ClockType=(RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2);
	_rccClkInitStruct.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
	_rccClkInitStruct.AHBCLKDivider=RCC_SYSCLK_DIV1;
	_rccClkInitStruct.APB1CLKDivider=RCC_HCLK_DIV4;
	_rccClkInitStruct.APB2CLKDivider=RCC_HCLK_DIV2;
	if(HAL_OK!=HAL_RCC_ClockConfig(&_rccClkInitStruct,FLASH_LATENCY_6)){BSP_Error_Handler();}

	/* Enable GPIO clocks */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
}

/**
  ***************************************************************************************************************************************
  * @brief  MPU configuration
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void BSP_MPU_Config(void)
{
	MPU_Region_InitTypeDef _mpuInitStruct={0};

	/* Disable the MPU */
	HAL_MPU_Disable();
	/* Configure the MPU attributes for SDRAM */
	_mpuInitStruct.Enable=MPU_REGION_ENABLE;
	_mpuInitStruct.BaseAddress=0xC0000000;
	_mpuInitStruct.Size=MPU_REGION_SIZE_8MB;
	_mpuInitStruct.AccessPermission=MPU_REGION_FULL_ACCESS;
	_mpuInitStruct.IsBufferable=MPU_ACCESS_NOT_BUFFERABLE;
	_mpuInitStruct.IsCacheable=MPU_ACCESS_CACHEABLE;
	_mpuInitStruct.IsShareable=MPU_ACCESS_NOT_SHAREABLE;
	_mpuInitStruct.Number=MPU_REGION_NUMBER4;
	_mpuInitStruct.TypeExtField=MPU_TEX_LEVEL0;
	_mpuInitStruct.SubRegionDisable=0x00;
	_mpuInitStruct.DisableExec=MPU_INSTRUCTION_ACCESS_ENABLE;
	HAL_MPU_ConfigRegion(&_mpuInitStruct);
	/* Enable the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  ***************************************************************************************************************************************
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void BSP_CPU_CACHE_Enable(void)
{
	/* Enable I-Cache */
	SCB_EnableICache();
	/* Enable D-Cache */
	SCB_EnableDCache();
}

/**
  ***************************************************************************************************************************************
  * @brief  This function is executed in case of error occurrence
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void BSP_Error_Handler(void)
{
	NVIC_SystemReset();
}

#ifdef USE_FULL_ASSERT
/**
  ***************************************************************************************************************************************
  * @brief  Reports the name of the source file and the source line number where the assert_param error has occurred
  * @param  File (uint8_t*), line (uint32_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void BSP_Assert_Failed(uint8_t *_file, uint32_t _line)
{
	UNUSED(_file);
	UNUSED(_line);
}
#endif
