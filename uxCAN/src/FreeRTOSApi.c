/*
 * FreeRTOSApi.c
 *
 *  Created on: 6 cze 2019
 *      Author: Jareeeeczek
 */
#include "FreeRTOSApi.h"

void initRTOS(void)
{

	xCanRxQueue.mHandler = xQueueCreateStatic(CAN_RX_QUEUE_SIZE, CAN_RX_QUEUE_ITEM_SIZE, xCanRxQueue.mQueueStack, &xCanRxQueue.mStaticQueue);
	xUartRxQueue.mHandler = xQueueCreateStatic(UART_RX_QUEUE_SIZE, UART_RX_QUEUE_ITEM_SIZE, xUartRxQueue.mQueueStack, &xUartRxQueue.mStaticQueue);

	xCanRxTask.mHandler  =  xTaskCreateStatic(vCanRxTask, "CanRxTask", TASK_STACK_SIZE,( void * ) NULL, 2,xCanRxTask.mTaskStack , &xCanRxTask.mStaticTask);
	xUartRxTask.mHandler = xTaskCreateStatic(vUartRxTask, "UartRxTask", TASK_STACK_SIZE,( void * ) NULL, 1,xUartRxTask.mTaskStack , &xUartRxTask.mStaticTask);

	configASSERT( xCanRxQueue.mHandler );
	configASSERT( xUartRxQueue.mHandler );
}



