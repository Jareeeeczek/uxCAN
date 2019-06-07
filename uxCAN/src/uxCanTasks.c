/*
 * uxCanTasks.c
 *
 *  Created on: 6 cze 2019
 *      Author: Jareeeeczek
 */

#include "uxCanTasks.h"

void vCanRxTask(void * aParam) {
	CanRxMsg canRxMsg;
	for (;;)
	{
		if (xQueueReceive(xCanRxQueue.mHandler, &(canRxMsg), (TickType_t) 10))
		{
			uartSendCANMsg(canRxMsg);
		}
	}
}

void vUartRxTask(void * aParam)
{
	CanRxMsg canTxMsg;
	for (;;)
	{
		if (xQueueReceive(xUartRxQueue.mHandler, &(canTxMsg), (TickType_t) 10))
		{
			canSendMsg(canTxMsg);
		}
	}

}
void vUartTxTask(void * aParam) {

}

