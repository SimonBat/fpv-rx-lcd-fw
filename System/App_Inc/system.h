#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "bsp.h"
#include "stm32f7508_discovery.h"
#include "stm32f7508_discovery_lcd.h"
#include "stm32f7508_discovery_sdram.h"
#include "stm32f7508_discovery_ts.h"

#define SYSTEM_RSSI_ADC_PIN				GPIO_PIN_3
#define SYSTEM_RSSI_ADC_PORT			GPIOA
#define SYSTEM_RSSI_ADC_CHANNEL			ADC_CHANNEL_3

typedef struct{
	ADC_HandleTypeDef hAdc;
	__IO uint16_t rssiAdcValue;
	uint8_t resetFpvFlag;
}system_ts;

/* Global functions declarations */
void SYSTEM_RSSI_ADC_IRQ_Handler(void);

#endif
