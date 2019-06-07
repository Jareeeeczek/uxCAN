/*
 * can.c
 *
 *  Created on: 5 cze 2019
 *      Author: Jareeeeczek
 */

#include "can.h"
#include "diag/Trace.h"

void USB_LP_CAN1_RX0_IRQHandler(void) {
	if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET) {
		CanRxMsg canRxMsg;
		CAN_Receive(CAN1, CAN_FIFO0, &canRxMsg);
		xQueueSendFromISR(xCanRxQueue.mHandler, &canRxMsg, NULL);
	}
}

void canSetup() {

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	/* Configure CAN pin: RX */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(CAN_PORT, &GPIO_InitStructure);

	/* Configure CAN pin: TX */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = CAN_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(CAN_PORT, &GPIO_InitStructure);
}

void canInit() {

	canSetup();

	CAN_InitTypeDef canInit;
	CAN_FilterInitTypeDef canFilter;
	NVIC_InitTypeDef nvicInitStructure;

	nvicInitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	nvicInitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	nvicInitStructure.NVIC_IRQChannelSubPriority = 0x0;
	nvicInitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInitStructure);
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

	CAN_DeInit(CAN1);
	CAN_StructInit(&canInit);
	canInit.CAN_TTCM = DISABLE;
	canInit.CAN_ABOM = DISABLE;
	canInit.CAN_AWUM = DISABLE;
	canInit.CAN_NART = DISABLE;
	canInit.CAN_RFLM = DISABLE;
	canInit.CAN_TXFP = DISABLE;
	canInit.CAN_Mode = CAN_Mode_Normal;
	canInit.CAN_SJW = CAN_SJW_1tq;
	canInit.CAN_BS1 = CAN_BS1_15tq;
	canInit.CAN_BS2 = CAN_BS2_2tq;
	canInit.CAN_Prescaler = 10;
	CAN_Init(CAN1, &canInit);

	canFilter.CAN_FilterNumber = 0;
	canFilter.CAN_FilterMode = CAN_FilterMode_IdMask;
	canFilter.CAN_FilterScale = CAN_FilterScale_32bit;
	canFilter.CAN_FilterIdHigh =  0;
	canFilter.CAN_FilterIdLow = 0;
	canFilter.CAN_FilterMaskIdHigh = 0;
	canFilter.CAN_FilterMaskIdLow = 0;
	canFilter.CAN_FilterFIFOAssignment = 0;
	canFilter.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&canFilter);
}

uint8_t canSendMsg(CanTxMsg aMsg)
{
	uint8_t meilbox = 0;
	meilbox = CAN_Transmit(CAN1, &aMsg);

	if (meilbox != CAN_TxStatus_NoMailBox)
	{
		return CAN_TransmitStatus(CAN1, meilbox);
	}
	else
	{
		return 0;
	}
}
