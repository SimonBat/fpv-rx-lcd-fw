#ifndef __BSP_H
#define __BSP_H

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_conf.h"

#define BSP_DISPLAY_SIZE_X		480U
#define BSP_DISPLAY_SIZE_Y		272U
#define BSP_DISPLAY_SIZE_XY		130560U

/* Global functions definitions */
void BSP_System_Clock_Config(void);
void BSP_MPU_Config(void);
void BSP_CPU_CACHE_Enable(void);
void BSP_Error_Handler(void);

#endif
