#ifndef __TVP5150_H
#define __TVP5150_H

#include "stm32f7xx_hal.h"
#include "bsp.h"

#define TVP5150_PATCH_SIZE						6051U

#define TVP5150_WRITE_ADDRESS 					0xB8
#define	TVP5150_READ_ADDRESS 					0xB9

#define TVP5150_ADDR_INPUT_SOURCE               0x00
#define TVP5150_ADDR_MISCELLANEOUS_CONTROLS     0x03
#define TVP5150_ADDR_BRIGHTNESS_REG				0x09
#define TVP5150_ADDR_SATURATION_REG				0x0A
#define TVP5150_ADDR_CONTRAST_REG				0x0C
#define TVP5150_ADDR_STATUS_1_REG				0x88
#define TVP5150_ADDR_INDIRECT_REG_DATA_1		0x21
#define TVP5150_ADDR_INDIRECT_REG_DATA_2		0x22
#define TVP5150_ADDR_INDIRECT_REG				0x23
#define TVP5150_ADDR_INDIRECT_REG_RW			0x24
#define TVP5150_ADDR_PATCH_CODE_WRITE			0x7E
#define TVP5150_ADDR_PATCH_CODE_EXECUTE			0x7F

#define TVP5150_ADDR_DEVICE_MSB                 0x80
#define TVP5150_ADDR_DEVICE_LSB                 0x81
#define TVP5150_DEVICE_ID						0x5150

/* Global functions declarations */
void TVP5150_Init(void);
uint8_t TVP5150_Check_ID_Status(void);
uint8_t TVP5150_Get_Video_Status(void);

#endif
