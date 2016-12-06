/*
 * dma.h
 *
 *  Created on: 5. 12. 2016
 *      Author: Patrik Bakyta
 */

#ifndef DMA_H_
#define DMA_H_

#define BUFFER_SIZE 5
#define TIMEOUT_MAX 10000

void initLED(void);
void initDMA(uint32_t *SRC_Const_Buffer, uint32_t *DST_Buffer);
extern "C" void DMA2_Stream0_IRQHandler(void);

#endif /* DMA_H_ */
