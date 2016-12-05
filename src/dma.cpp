/*
 * dma.cpp
 *
 *  Created on: 5. 12. 2016
 *      Author: Patrik Bakyta
 */

#include "stm32f4xx.h"
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <misc.h>
#include <dma.h>

void initLED(void) {

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

	GPIO_InitTypeDef initStruct;
	initStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	initStruct.GPIO_Mode = GPIO_Mode_OUT;
	initStruct.GPIO_OType = GPIO_OType_PP;
	initStruct.GPIO_PuPd = GPIO_PuPd_UP;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&initStruct);

	return;
}

void initDMA(void) {

	const uint32_t SRC_Const_Buffer[BUFFER_SIZE]= {
	                                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
	                                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
	                                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
	                                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
	                                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
	                                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
	                                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
	                                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};
	uint32_t DST_Buffer[BUFFER_SIZE];

	uint32_t Timeout;

	//Enable DMA1 clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	//Create DMA structure
	DMA_InitTypeDef  DMA_InitStructure;
	//Reset DMA2 Stream0 to default values;
	DMA_DeInit(DMA2_Stream0);

	/* Check if the DMA Stream is disabled before enabling it.
	   Note that this step is useful when the same Stream is used multiple times:
	   enabled, then disabled then re-enabled... In this case, the DMA Stream disable
	   will be effective only at the end of the ongoing data transfer and it will
	   not be possible to re-configure it before making sure that the Enable bit
	   has been cleared by hardware. If the Stream is used only once, this step might
	   be bypassed. */
	while (DMA_GetCmdStatus(DMA2_Stream0)!=DISABLE) {}

	//Configure DMA Stream
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SRC_Const_Buffer;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DST_Buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
	DMA_InitStructure.DMA_BufferSize = (uint32_t)BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);

	//Enable DMA Stream Transfer Complete interrupt
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

	/* Check if the DMA Stream has been effectively enabled.
	   The DMA Stream Enable bit is cleared immediately by hardware if there is an
	   error in the configuration parameters and the transfer is no started (ie. when
	   wrong FIFO threshold is configured ...) */
	Timeout = TIMEOUT_MAX;
	while ((DMA_GetCmdStatus(DMA2_Stream0)!=ENABLE)&&(Timeout-->0)) {}

	//Check if a timeout condition occurred
	if (Timeout == 0) {

		//Manage the error: to simplify the code enter an infinite loop
		while (1) {}
	}

	//Enable the DMA Stream IRQ Channel
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	return;
}


extern "C" void DMA2_Stream0_IRQHandler(void) {

	//Test on DMA2 Channel0 Transfer Complete Interrupt
	if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0)) {

		//LED on after transfer (PIN13)
		GPIO_ToggleBits(GPIOD,GPIO_Pin_13);
		//Clear DMA2 Channel0 Transfer Complete Interrupt pending bit
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
  }
}
