/**
  ***************************************************************************************************************************************
  * @file     fpv.c
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

#include "fpv.h"
#include "tvp5150.h"

static fpv_ts H_FPV;
static uint8_t FPV_BUFFER_RGB[BSP_DISPLAY_SIZE_X*3U];
static uint16_t FPV_BUFFER_YCBCR[BSP_DISPLAY_SIZE_X*BSP_DISPLAY_SIZE_Y];

/* Local functions declarations */
static void FPV_Module_MSP_Init(fpv_ts *_hFpv);

/**
  ***************************************************************************************************************************************
  * @brief  FPV module initialization
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void FPV_Module_Init(void)
{
	/* DCMI configuration */
	H_FPV.hDcmi.Instance=DCMI;
	H_FPV.hDcmi.Init.CaptureRate=DCMI_CR_ALL_FRAME;
	H_FPV.hDcmi.Init.HSPolarity=DCMI_HSPOLARITY_HIGH;
	H_FPV.hDcmi.Init.SynchroMode=DCMI_SYNCHRO_HARDWARE;
	H_FPV.hDcmi.Init.VSPolarity=DCMI_VSPOLARITY_HIGH;
	H_FPV.hDcmi.Init.ExtendedDataMode=DCMI_EXTEND_DATA_8B;
	H_FPV.hDcmi.Init.PCKPolarity=DCMI_PCKPOLARITY_RISING;
	H_FPV.hDcmi.Init.JPEGMode=DCMI_JPEG_DISABLE;
	H_FPV.hDcmi.Init.ByteSelectMode=DCMI_BSM_ALL;
	H_FPV.hDcmi.Init.ByteSelectStart=DCMI_OEBS_ODD;
	H_FPV.hDcmi.Init.SyncroCode.LineStartCode=0x80;
	H_FPV.hDcmi.Init.SyncroCode.LineEndCode=0x9D;
	H_FPV.hDcmi.Init.SyncroCode.FrameStartCode=0xFF;
	H_FPV.hDcmi.Init.SyncroCode.FrameEndCode=0xFF;

	if(TVP5150_Check_ID_Status())
	{
		HAL_Delay(100U);
		TVP5150_Init();
		HAL_Delay(100U);
	    FPV_Module_MSP_Init(&H_FPV);
	    HAL_DCMI_Init(&H_FPV.hDcmi);
	    HAL_DCMI_ConfigCROP(&H_FPV.hDcmi,(FPV_PAL_RESOLUTION_X-BSP_DISPLAY_SIZE_X), \
	    				    ((FPV_PAL_RESOLUTION_Y-BSP_DISPLAY_SIZE_Y)/2U),(BSP_DISPLAY_SIZE_X*2U-1U),(BSP_DISPLAY_SIZE_Y-1U));
	    HAL_DCMI_EnableCROP(&H_FPV.hDcmi);
	    H_FPV.initFlag=1U;
	}else{BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  FPV module MSP initialization
  * @param  FPV handle (fpv_ts*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void FPV_Module_MSP_Init(fpv_ts *_hFpv)
{
	GPIO_InitTypeDef _gpioInitStructure={0};

	/* Enable DCMI clock */
	__HAL_RCC_DCMI_CLK_ENABLE();
	/* Enable DMA2 clock */
	__HAL_RCC_DMA2_CLK_ENABLE();
	/* Enable DMA2D clock */
	__HAL_RCC_DMA2D_CLK_ENABLE();

	/* Configure DCMI GPIO as alternate function */
	_gpioInitStructure.Pin=FPV_H_SYNC_PIN_A|FPV_H_PCLK_PIN_A;
	_gpioInitStructure.Mode=GPIO_MODE_AF_PP;
	_gpioInitStructure.Pull=GPIO_NOPULL;
	_gpioInitStructure.Speed=GPIO_SPEED_HIGH;
	_gpioInitStructure.Alternate=GPIO_AF13_DCMI;
	HAL_GPIO_Init(FPV_PORT_A,&_gpioInitStructure);

	_gpioInitStructure.Pin=FPV_D5_PIN_D;
	_gpioInitStructure.Mode=GPIO_MODE_AF_PP;
	_gpioInitStructure.Pull=GPIO_NOPULL;
	_gpioInitStructure.Speed=GPIO_SPEED_HIGH;
	_gpioInitStructure.Alternate=GPIO_AF13_DCMI;
	HAL_GPIO_Init(FPV_PORT_D,&_gpioInitStructure);

	_gpioInitStructure.Pin=FPV_D6_PIN_E|FPV_D7_PIN_E;
	_gpioInitStructure.Mode=GPIO_MODE_AF_PP;
	_gpioInitStructure.Pull=GPIO_NOPULL;
	_gpioInitStructure.Speed=GPIO_SPEED_HIGH;
	_gpioInitStructure.Alternate=GPIO_AF13_DCMI;
	HAL_GPIO_Init(FPV_PORT_E,&_gpioInitStructure);

	_gpioInitStructure.Pin=FPV_V_SYNC_PIN_G;
	_gpioInitStructure.Mode=GPIO_MODE_AF_PP;
	_gpioInitStructure.Pull=GPIO_NOPULL;
	_gpioInitStructure.Speed=GPIO_SPEED_HIGH;
	_gpioInitStructure.Alternate=GPIO_AF13_DCMI;
	HAL_GPIO_Init(FPV_PORT_G,&_gpioInitStructure);

	_gpioInitStructure.Pin=FPV_D0_PIN_H|FPV_D1_PIN_H|FPV_D2_PIN_H|FPV_D3_PIN_H|FPV_D4_PIN_H;
	_gpioInitStructure.Mode=GPIO_MODE_AF_PP;
	_gpioInitStructure.Pull=GPIO_NOPULL;
	_gpioInitStructure.Speed=GPIO_SPEED_HIGH;
	_gpioInitStructure.Alternate=GPIO_AF13_DCMI;
	HAL_GPIO_Init(FPV_PORT_H,&_gpioInitStructure);

	/* Configure the DMA */
	_hFpv->hDma.Init.Channel=DMA_CHANNEL_1;
	_hFpv->hDma.Init.Direction=DMA_PERIPH_TO_MEMORY;
	_hFpv->hDma.Init.PeriphInc=DMA_PINC_DISABLE;
	_hFpv->hDma.Init.MemInc=DMA_MINC_ENABLE;
	_hFpv->hDma.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;
	_hFpv->hDma.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;
	_hFpv->hDma.Init.Mode=DMA_CIRCULAR;
	_hFpv->hDma.Init.Priority=DMA_PRIORITY_VERY_HIGH;
	_hFpv->hDma.Init.FIFOMode=DMA_FIFOMODE_ENABLE;
	_hFpv->hDma.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_1QUARTERFULL;
	_hFpv->hDma.Init.MemBurst=DMA_MBURST_SINGLE;
	_hFpv->hDma.Init.PeriphBurst=DMA_PBURST_SINGLE;
	_hFpv->hDma.Instance=DMA2_Stream1;
	/* Associate the initialized DMA handle to the DCMI handle */
	__HAL_LINKDMA(&_hFpv->hDcmi,DMA_Handle,_hFpv->hDma);

	/* NVIC configuration for DCMI transfer complete interrupt */
	HAL_NVIC_SetPriority(DCMI_IRQn,0U,0U);
	HAL_NVIC_EnableIRQ(DCMI_IRQn);
	/* NVIC configuration for DMA2D transfer complete interrupt */
	HAL_NVIC_SetPriority(DMA2_Stream1_IRQn,0U,0U);
	HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

	/* Configure the DMA stream */
	HAL_DMA_Init(_hFpv->hDcmi.DMA_Handle);
}

/**
  ***************************************************************************************************************************************
  * @brief  Starts the camera capture in continuous mode
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void FPV_Module_Continuous_Start(void)
{
	/* Start the camera capture */
	if(HAL_OK!=HAL_DCMI_Start_DMA(&H_FPV.hDcmi,DCMI_MODE_CONTINUOUS,(uint32_t)FPV_BUFFER_YCBCR, \
								  (BSP_DISPLAY_SIZE_X*BSP_DISPLAY_SIZE_Y/2U))){BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  Suspend the FPV capture
  * @param  Node
  * @retval None
  ***************************************************************************************************************************************
  */
void FPV_Module_Suspend(void)
{
	/* Suspend the Camera Capture */
	if(HAL_OK!=HAL_DCMI_Suspend(&H_FPV.hDcmi)){BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  Resume the FPV capture
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void FPV_Module_Resume(void)
{
	/* Start the Camera Capture */
	if(HAL_OK!=HAL_DCMI_Resume(&H_FPV.hDcmi)){BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  Stop the FPV capture
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void FPV_Module_Stop(void)
{
	if(HAL_OK!=HAL_DCMI_Stop(&H_FPV.hDcmi)){BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  Handles DCMI interrupt request.
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void FPV_Module_DCMI_IRQ_Handler(void)
{
	uint32_t _isrValue=H_FPV.hDcmi.Instance->MISR;

	/* Synchronization error interrupt management */
	if(_isrValue&DCMI_FLAG_ERRRI)
	{
		/* Clear the Synchronization error flag */
	    __HAL_DCMI_CLEAR_FLAG(&H_FPV.hDcmi,DCMI_FLAG_ERRRI);
	    /* Abort the DMA Transfer */
	    (void)HAL_DMA_Abort_IT(H_FPV.hDcmi.DMA_Handle);
	}

	/* Overflow interrupt management */
	if(_isrValue&DCMI_FLAG_OVRRI)
	{
		/* Clear the Overflow flag */
	    __HAL_DCMI_CLEAR_FLAG(&H_FPV.hDcmi,DCMI_FLAG_OVRRI);
	    (void)HAL_DMA_Abort_IT(H_FPV.hDcmi.DMA_Handle);
	}

	/* Line Interrupt management */
	if(_isrValue&DCMI_FLAG_LINERI)
	{
		/* Clear the Line interrupt flag */
	    __HAL_DCMI_CLEAR_FLAG(&H_FPV.hDcmi,DCMI_FLAG_LINERI);

	    for(uint16_t _idx=0U;_idx<BSP_DISPLAY_SIZE_X;_idx+=2U)
	    {
	    	int32_t _y1=(int32_t)((FPV_BUFFER_YCBCR[H_FPV.camBufferOffset]>>8));
	    	int32_t _y2=(int32_t)((FPV_BUFFER_YCBCR[H_FPV.camBufferOffset+1]>>8));
	    	int32_t _cr=(int32_t)((FPV_BUFFER_YCBCR[H_FPV.camBufferOffset]&0xFF)-128);
	    	int32_t _cb=(int32_t)((FPV_BUFFER_YCBCR[H_FPV.camBufferOffset+1]&0xFF)-128);
	    	int32_t _rTemp=(45*_cr)>>5;
	    	int32_t _gTemp=(11*_cb+23*_cr)>>5;
	    	int32_t _bTemp=(113*_cb)>>6;

	    	int32_t _r=_y1+_rTemp;
	    	int32_t _g=_y1-_gTemp;
	    	int32_t _b=_y1+_bTemp;
	    	FPV_BUFFER_RGB[_idx*3]=(_r>255)?255:(_r<0)?0:_r;
	    	FPV_BUFFER_RGB[_idx*3+1]=(_g>255)?255:(_g<0)?0:_g;
	    	FPV_BUFFER_RGB[_idx*3+2]=(_b>255)?255:(_b<0)?0:_b;

	    	_r=_y2+_rTemp;
	    	_g=_y2-_gTemp;
	    	_b=_y2+_bTemp;
	    	FPV_BUFFER_RGB[_idx*3+3]=(_r>255)?255:(_r<0)?0:_r;
	    	FPV_BUFFER_RGB[_idx*3+4]=(_g>255)?255:(_g<0)?0:_g;
	    	FPV_BUFFER_RGB[_idx*3+5]=(_b>255)?255:(_b<0)?0:_b;

	    	H_FPV.camBufferOffset+=2U;
	    	if(H_FPV.camBufferOffset>=BSP_DISPLAY_SIZE_XY){H_FPV.camBufferOffset=0U;}
	    }

	    /* DMA2D CR register configuration */
	    MODIFY_REG(DMA2D->CR,DMA2D_CR_MODE,DMA2D_M2M_PFC);
	    /* DMA2D OPFCCR register configuration */
	    MODIFY_REG(DMA2D->OPFCCR,DMA2D_OPFCCR_CM,DMA2D_OUTPUT_ARGB8888);
	    /* DMA2D OOR register configuration */
	    MODIFY_REG(DMA2D->OOR,DMA2D_OOR_LO,0U);
	    /* Write DMA2D FGPFCCR register */
	    MODIFY_REG(DMA2D->FGPFCCR,(DMA2D_BGPFCCR_CM|DMA2D_BGPFCCR_AM|DMA2D_BGPFCCR_ALPHA), \
	    		   (DMA2D_INPUT_RGB888|(DMA2D_NO_MODIF_ALPHA<<DMA2D_BGPFCCR_AM_Pos)|(0xFF<<DMA2D_BGPFCCR_ALPHA_Pos)));
	    /* DMA2D FGOR register configuration */
	    WRITE_REG(DMA2D->FGOR,0U);
	    /* Configure DMA2D data size */
	    MODIFY_REG(DMA2D->NLR,(DMA2D_NLR_NL|DMA2D_NLR_PL),(1U|(BSP_DISPLAY_SIZE_X<<DMA2D_NLR_PL_Pos)));
	    /* Configure DMA2D destination address */
	    WRITE_REG(DMA2D->OMAR,(uint32_t)(FPV_LCD_FRAME_BUFFER+H_FPV.lcdBufferOffset));
	    WRITE_REG(DMA2D->FGMAR,(uint32_t)FPV_BUFFER_RGB);
	    DMA2D->CR|=DMA2D_CR_START;

	    /* Polling For DMA transfer */
	    while(!(DMA2D->ISR&DMA2D_FLAG_TC))
	    {
	    	if(DMA2D->ISR&(DMA2D_FLAG_CE|DMA2D_FLAG_TE))
	    	{
	    		DMA2D->IFCR=(DMA2D_FLAG_CE|DMA2D_FLAG_TE);
	    		break;
	    	}
	    }

	    if(DMA2D->FGPFCCR&DMA2D_FGPFCCR_START)
	    {
	    	while(!(DMA2D->ISR&DMA2D_FLAG_CTC))
	    	{
	    		if(DMA2D->ISR&(DMA2D_FLAG_CAE|DMA2D_FLAG_CE|DMA2D_FLAG_TE))
	    		{
	    			DMA2D->IFCR=(DMA2D_FLAG_CAE|DMA2D_FLAG_CE|DMA2D_FLAG_TE);
	    			break;
	    		}
	    	}
	    }

	    DMA2D->IFCR=(DMA2D_FLAG_TC|DMA2D_FLAG_CTC);
	    H_FPV.lcdBufferOffset=H_FPV.lcdBufferOffset+BSP_DISPLAY_SIZE_X*sizeof(uint32_t);
	    if(++H_FPV.lineIdx>(BSP_DISPLAY_SIZE_Y-1U)){H_FPV.lineIdx=0U; H_FPV.lcdBufferOffset=0U;}
	}

	/* VSYNC interrupt management */
	if(_isrValue&DCMI_FLAG_VSYNCRI)
	{
		/* Clear the VSYNC flag */
	    __HAL_DCMI_CLEAR_FLAG(&H_FPV.hDcmi,DCMI_FLAG_VSYNCRI);
	}

	/* FRAME interrupt management */
	if(_isrValue&DCMI_FLAG_FRAMERI)
	{
		/* When snapshot mode, disable Vsync, Error and Overrun interrupts */
	    if(DCMI_MODE_SNAPSHOT==(H_FPV.hDcmi.Instance->CR&DCMI_CR_CM))
	    {
	      /* Disable the Line, Vsync, Error and Overrun interrupts */
	      __HAL_DCMI_DISABLE_IT(&H_FPV.hDcmi,(DCMI_IT_LINE|DCMI_IT_VSYNC|DCMI_IT_ERR|DCMI_IT_OVR));
	    }

	    /* Disable the Frame interrupt */
	    __HAL_DCMI_DISABLE_IT(&H_FPV.hDcmi,DCMI_IT_FRAME);
	    /* Clear the End of Frame flag */
	    __HAL_DCMI_CLEAR_FLAG(&H_FPV.hDcmi,DCMI_FLAG_FRAMERI);
	}
}

/**
  ***************************************************************************************************************************************
  * @brief  Handles DMA interrupt request.
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void FPV_Module_DMA_IRQ_Handler(void)
{
	HAL_DMA_IRQHandler(H_FPV.hDcmi.DMA_Handle);
}
