#ifndef __FPV_H
#define __FPV_H

#include "bsp.h"

#define FPV_PAL_RESOLUTION_X		800U
#define FPV_PAL_RESOLUTION_Y		306U
#define FPV_LCD_FRAME_BUFFER		0xC0000000

#define FPV_D0_PIN_H				GPIO_PIN_9
#define FPV_D1_PIN_H				GPIO_PIN_10
#define FPV_D2_PIN_H				GPIO_PIN_11
#define FPV_D3_PIN_H				GPIO_PIN_12
#define FPV_D4_PIN_H				GPIO_PIN_14
#define FPV_PORT_H					GPIOH
#define FPV_D5_PIN_D				GPIO_PIN_3
#define FPV_PORT_D					GPIOD
#define FPV_D6_PIN_E				GPIO_PIN_5
#define FPV_D7_PIN_E				GPIO_PIN_6
#define FPV_PORT_E					GPIOE
#define FPV_V_SYNC_PIN_G			GPIO_PIN_9
#define FPV_PORT_G					GPIOG
#define FPV_H_SYNC_PIN_A			GPIO_PIN_4
#define FPV_H_PCLK_PIN_A			GPIO_PIN_6
#define FPV_PORT_A					GPIOA

typedef struct{
	DCMI_HandleTypeDef hDcmi;
	DMA_HandleTypeDef hDma;
	__IO uint16_t lineIdx;
	__IO uint32_t camBufferOffset;
	__IO uint32_t lcdBufferOffset;
	uint8_t initFlag;
}fpv_ts;

/* Global functions declarations */
void FPV_Module_Init(void);
void FPV_Module_Continuous_Start(void);
void FPV_Module_Suspend(void);
void FPV_Module_Resume(void);
void FPV_Module_Stop(void);
void FPV_Module_DCMI_IRQ_Handler(void);
void FPV_Module_DMA_IRQ_Handler(void);

#endif
