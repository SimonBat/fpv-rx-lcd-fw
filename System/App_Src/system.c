/**
  ***************************************************************************************************************************************
  * @file     system.c
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

#include "system.h"
#include "tvp5150.h"
#include "fpv.h"

static system_ts H_SYSTEM;

/* Local functions declarations */
static void SYSTEM_Display_Info(void);
static void SYSTEM_RSSI_ADC_Init(system_ts* _system);

/**
  ***************************************************************************************************************************************
  * @brief  The application entry point.
  * @param  None
  * @retval Value (int)
  ***************************************************************************************************************************************
  */
int main(void)
{
	BSP_MPU_Config();
	BSP_CPU_CACHE_Enable();
	BSP_System_Clock_Config();
	BSP_LCD_Init();

	/* Init LCD screen buffer */
	uint32_t *_ptrLcd=(uint32_t*)(FPV_LCD_FRAME_BUFFER);
	for(uint32_t _idx=0U;_idx<(BSP_LCD_GetXSize()*BSP_LCD_GetYSize());_idx++){_ptrLcd[_idx]=0U;}

	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER,FPV_LCD_FRAME_BUFFER);
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_TS_Init(BSP_LCD_GetXSize(),BSP_LCD_GetYSize());
	FPV_Module_Init();
	SYSTEM_RSSI_ADC_Init(&H_SYSTEM);
	SYSTEM_Display_Info();
	HAL_Delay(500U);

	while(1U)
	{
		if(TVP5150_Get_Video_Status()&&(!H_SYSTEM.resetFpvFlag))
		{
			HAL_Delay(500U);
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			FPV_Module_Continuous_Start();
			H_SYSTEM.resetFpvFlag=1U;
		}
		else if((!TVP5150_Get_Video_Status())&&H_SYSTEM.resetFpvFlag)
		{
			H_SYSTEM.resetFpvFlag=0U;
			FPV_Module_Stop();
			SYSTEM_Display_Info();
		}
	};
}

/**
  ***************************************************************************************************************************************
  * @brief  System display inforamtion
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
static void SYSTEM_Display_Info(void)
{
	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(156U,110U,(uint8_t*)"NO SIGNAL...",LEFT_MODE);
	BSP_LCD_SetFont(&Font12);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
	BSP_LCD_DisplayStringAt(260U,260U,(uint8_t*)"5.8 GHz PAL Video Receiver v1.0",LEFT_MODE);
}

/**
  ***************************************************************************************************************************************
  * @brief  System RSSI ADC initialization
  * @param  System handle (system_ts*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void SYSTEM_RSSI_ADC_Init(system_ts* _system)
{
	ADC_ChannelConfTypeDef _sConfig={0};

	/* Configure the ADC peripheral */
	_system->hAdc.Instance=ADC1;
	_system->hAdc.Init.ClockPrescaler=ADC_CLOCKPRESCALER_PCLK_DIV4;
	_system->hAdc.Init.Resolution=ADC_RESOLUTION_12B;
	_system->hAdc.Init.ScanConvMode=DISABLE;
	_system->hAdc.Init.ContinuousConvMode=ENABLE;
	_system->hAdc.Init.DiscontinuousConvMode=DISABLE;
	_system->hAdc.Init.NbrOfDiscConversion=0U;
	_system->hAdc.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;
	_system->hAdc.Init.ExternalTrigConv=ADC_EXTERNALTRIGCONV_T1_CC1;
	_system->hAdc.Init.DataAlign=ADC_DATAALIGN_RIGHT;
	_system->hAdc.Init.NbrOfConversion=1U;
	_system->hAdc.Init.DMAContinuousRequests=DISABLE;
	_system->hAdc.Init.EOCSelection=DISABLE;
	if(HAL_OK!=HAL_ADC_Init(&_system->hAdc)){BSP_Error_Handler();}

	/* Configure ADC regular channel */
	_sConfig.Channel=SYSTEM_RSSI_ADC_CHANNEL;
	_sConfig.Rank=1U;
	_sConfig.SamplingTime=ADC_SAMPLETIME_84CYCLES;
	_sConfig.Offset=0U;
	if(HAL_OK!=HAL_ADC_ConfigChannel(&_system->hAdc,&_sConfig)){BSP_Error_Handler();}
	/* Start the conversion process */
	if(HAL_OK!=HAL_ADC_Start_IT(&_system->hAdc)){BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  System RSSI ADC MSP initialization
  * @param  ADC handle (ADC_HandleTypeDef*)
  * @retval None
  ***************************************************************************************************************************************
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef *_hadc)
{
	GPIO_InitTypeDef _gpioInitStruct={0};

	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* ADC Channel GPIO pin configuration */
	_gpioInitStruct.Pin=SYSTEM_RSSI_ADC_PIN;
	_gpioInitStruct.Mode=GPIO_MODE_ANALOG;
	_gpioInitStruct.Pull=GPIO_NOPULL;
	HAL_GPIO_Init(SYSTEM_RSSI_ADC_PORT,&_gpioInitStruct);

	/* NVIC configuration for ADC interrupt */
	HAL_NVIC_SetPriority(ADC_IRQn,1U,0U);
	HAL_NVIC_EnableIRQ(ADC_IRQn);
}

/**
  ***************************************************************************************************************************************
  * @brief  System RSSI ADC conversion complete callback
  * @param  ADC handle (ADC_HandleTypeDef*)
  * @retval None
  ***************************************************************************************************************************************
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* _adcHandle)
{
	/* Get the converted value of regular channel */
	H_SYSTEM.rssiAdcValue=HAL_ADC_GetValue(_adcHandle);
}

/**
  ***************************************************************************************************************************************
  * @brief  System RSSI ADC IRQ handler
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void SYSTEM_RSSI_ADC_IRQ_Handler(void)
{
	HAL_ADC_IRQHandler(&H_SYSTEM.hAdc);
}
