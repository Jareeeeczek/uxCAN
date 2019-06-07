/*
 * uart.c
 *
 *  Created on: 5 cze 2019
 *      Author: Jareeeeczek
 */
#include "uart.h"

void USART1_IRQHandler()
{
	  if(USART1->SR & USART_SR_IDLE)
	  {
	    //idle stan
		  USART_ClearFlag(USART1, USART_SR_IDLE);
		  usartCheck();
	  }
}
void DMA1_Channel1_IRQHandler()
{
    if(DMA1->ISR & DMA_ISR_HTIF1) {
    	DMA_ClearFlag(DMA1_FLAG_HT1);
       // usart_rx_check();                       /* Check for data to process */
    }

    /* Check transfer-complete interrupt */
    if (DMA1->ISR & DMA_ISR_TCIF1) {
    	DMA_ClearFlag(DMA1_FLAG_TC1);             /* Clear half-transfer complete flag */
        //usart_rx_check();                       /* Check for data to process */
    }
}

void uartSetup()
{
	GPIO_InitTypeDef gpio;
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
	USART_DeInit(USART1);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = UART_TX_PIN;//TX
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &gpio);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = UART_RX_PIN;//RX
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &gpio);
}

void uartDmaInit()
{

	DMA_InitTypeDef   dma;
	NVIC_InitTypeDef  nvicInitStructure;

	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	dma.DMA_MemoryBaseAddr = (uint32_t)rxUartDmaBuffer;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_M2M = DMA_M2M_Disable;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_BufferSize = ARRAY_LEN(rxUartDmaBuffer);

	DMA_Init(DMA1_Channel1, &dma);
	DMA_ITConfig(UART_DMA_CHANNEL, DMA_IT_TC, ENABLE);
	DMA_ITConfig(UART_DMA_CHANNEL, DMA_IT_HT, ENABLE);

	nvicInitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	nvicInitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	nvicInitStructure.NVIC_IRQChannelSubPriority = 0x0;
	nvicInitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInitStructure);
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void uartInit()
{
	uartSetup();
	uartDmaInit();

	USART_InitTypeDef uart;

	USART_Cmd(USART1, ENABLE);

	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;
	uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_WordLength = 8;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &uart);
}

void uartSendData(uint8_t aData)
{
 while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
 	 USART_SendData(USART1, aData);
}

void uartSendCANMsg(CanRxMsg aCanRxMsg)
{
	int i = 0;
	int numberToSend = 0;
	char buffer[200];
	numberToSend = sprintf(buffer, "CAN msg ID : %d, DLC: %d, \n", aCanRxMsg.StdId, aCanRxMsg.DLC);
	for(i = 0; i < numberToSend; i++)
	{
		uartSendData(buffer[i]);
	}

	for(i = 0; i < aCanRxMsg.DLC; i++)
	{
		USART_SendData(USART1, aCanRxMsg.Data[i]);
		USART_SendData(USART1, 0x20);//send space
	}

	numberToSend = sprintf(buffer, "\n\n\n");

	for(i = 0; i < numberToSend; i++)
	{
		uartSendData(buffer[i]);
	}
}
void usartCheck(void) {
    static size_t old_pos;
    size_t pos;

    pos = ARRAY_LEN(rxUartDmaBuffer) - DMA_GetCurrDataCounter(UART_DMA_CHANNEL);
    if(pos > old_pos)
    {
    	//tutaj mamy do przetworzenia dane od old_pos do (pos-old_pos)
    }

}

/*
 * For now a UART frame looks like:
 * 0xFF - Byte 0
 * DLC  - Byte 1
 * StdId - low - Byte 2
 * stdId - high - Byte 3
 * Data[0] ... Data[DLC -1]
 * 0xFF
 */

void uartProcessData(uint8_t *aData)
{
	if((aData[0] == 0xFF) && (aData[5+aData[1]] == 0xFF))
	{
		CanTxMsg canTxMsg;
		uint8_t i;
		canTxMsg.DLC = aData[1];
		canTxMsg.RTR = CAN_RTR_Data;
		canTxMsg.IDE = CAN_Id_Standard;
		canTxMsg.StdId = (((uint32_t)aData[3])<<0xFE) & aData[2];

		for(i = 0; i<canTxMsg.DLC; i++)
		{
			canTxMsg.Data[i] = aData[4 + canTxMsg.DLC];
		}

		xQueueSend(xUartRxQueue.mHandler, &canTxMsg, (TickType_t)100);

	}
	else
	{
		//TODO it is not CAN msg
	}
}
