/**
  ***************************************************************************************************************************************
  * @file     tvp5150.c
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

#include <stdio.h>
#include "tvp5150.h"
#include "stm32f7508_discovery.h"

extern const uint8_t TVP5150_PATCH[TVP5150_PATCH_SIZE];

/* Local functions declarations */
static void TVP5150_I2C_Init(void);
static void TVP5150_I2C_Write(uint8_t _reg, uint8_t _value);
static void TVP5150_I2C_Write_Multiple(uint8_t _reg, const uint8_t* _data, uint16_t _size);
static uint8_t TVP5150_I2C_Read(uint8_t _reg);
void CAMERA_IO_Init(void);
void CAMERA_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
void CAMERA_IO_Write_Multiple(uint8_t Addr, uint8_t Reg, uint8_t* Data, uint16_t Size);
uint8_t CAMERA_IO_Read(uint8_t Addr, uint8_t Reg);

/**
  ***************************************************************************************************************************************
  * @brief  TVP5150 initialization
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void TVP5150_Init(void)
{
	TVP5150_I2C_Write(TVP5150_ADDR_PATCH_CODE_EXECUTE,0x00); /* Restart TVP5150AM1 */
	TVP5150_I2C_Write(TVP5150_ADDR_MISCELLANEOUS_CONTROLS,0x0D);

	/* Unlock password for patch code download & register write */
	TVP5150_I2C_Write(TVP5150_ADDR_INDIRECT_REG_DATA_1,0x51);
	TVP5150_I2C_Write(TVP5150_ADDR_INDIRECT_REG_DATA_2,0x50);
	TVP5150_I2C_Write(TVP5150_ADDR_INDIRECT_REG,0xFF);
	TVP5150_I2C_Write(TVP5150_ADDR_INDIRECT_REG_RW,0x04);

	TVP5150_I2C_Write_Multiple(TVP5150_ADDR_PATCH_CODE_WRITE,TVP5150_PATCH,TVP5150_PATCH_SIZE);
	TVP5150_I2C_Write(TVP5150_ADDR_PATCH_CODE_EXECUTE,0x00); /* Restart TVP5150AM1 */

	/* Lock password for patch code download & register write */
	TVP5150_I2C_Write(TVP5150_ADDR_INDIRECT_REG_DATA_1,0x00);
	TVP5150_I2C_Write(TVP5150_ADDR_INDIRECT_REG_DATA_2,0x00);
	TVP5150_I2C_Write(TVP5150_ADDR_INDIRECT_REG,0xFF);
	TVP5150_I2C_Write(TVP5150_ADDR_INDIRECT_REG_RW,0x04);

	TVP5150_I2C_Write(TVP5150_ADDR_INPUT_SOURCE,0x02); /* AIP1B */
	TVP5150_I2C_Write(TVP5150_ADDR_MISCELLANEOUS_CONTROLS,0x0D);
	TVP5150_I2C_Write(TVP5150_ADDR_BRIGHTNESS_REG,0x82);
	TVP5150_I2C_Write(TVP5150_ADDR_SATURATION_REG,0xB0);
	TVP5150_I2C_Write(TVP5150_ADDR_CONTRAST_REG,0x87);
}

/**
  ***************************************************************************************************************************************
  * @brief  TVP5150 check ID status
  * @param  None
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t TVP5150_Check_ID_Status(void)
{
	uint16_t _id;
	uint8_t _status;

	TVP5150_I2C_Init();

	_id=(TVP5150_I2C_Read(TVP5150_ADDR_DEVICE_MSB)<<8)|TVP5150_I2C_Read(TVP5150_ADDR_DEVICE_LSB);

	if(_id==TVP5150_DEVICE_ID){_status=1U;}
	else{_status=0U;}

	return _status;
}

/**
  ***************************************************************************************************************************************
  * @brief  TVP5150 I2C init
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
static void TVP5150_I2C_Init(void)
{
	CAMERA_IO_Init();
}

/**
  ***************************************************************************************************************************************
  * @brief  TVP5150 I2C write
  * @param  Register (uint8_t), value (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
static void TVP5150_I2C_Write(uint8_t _reg, uint8_t _value)
{
	CAMERA_IO_Write(TVP5150_WRITE_ADDRESS,_reg,_value);
}

/**
  ***************************************************************************************************************************************
  * @brief  TVP5150 I2C write multiple
  * @param  Register (uint8_t), data (uint8_t*), size (uint16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
static void TVP5150_I2C_Write_Multiple(uint8_t _reg, const uint8_t* _data, uint16_t _size)
{
	CAMERA_IO_Write_Multiple(TVP5150_WRITE_ADDRESS,_reg,(uint8_t*)_data,_size);
}

/**
  ***************************************************************************************************************************************
  * @brief  TVP5150 I2C read
  * @param  Register (uint8_t)
  * @retval Value (uint8_t)
  ***************************************************************************************************************************************
  */
static uint8_t TVP5150_I2C_Read(uint8_t _reg)
{
	return CAMERA_IO_Read(TVP5150_READ_ADDRESS,_reg);
}

/**
  ***************************************************************************************************************************************
  * @brief  TVP5150 get video status
  * @param  None
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t TVP5150_Get_Video_Status(void)
{
	return (TVP5150_I2C_Read(TVP5150_ADDR_STATUS_1_REG)&0x0E)? 1U:0U;
}
