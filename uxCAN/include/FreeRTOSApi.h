#ifndef FREE_RTOS_API_H
#define FREE_RTOS_API_H
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "queue.h"
#include "uxCanTasks.h"

#define CAN_RX_QUEUE_SIZE 			15
#define UART_RX_QUEUE_SIZE 			20

#define CAN_RX_QUEUE_ITEM_SIZE 		sizeof(CanRxMsg)
#define UART_RX_QUEUE_ITEM_SIZE 	sizeof(uint8_t)

#define TASK_STACK_SIZE 			200

typedef struct
{
	StaticQueue_t 	mStaticQueue;
	QueueHandle_t 	mHandler;
	uint8_t 	  	mQueueStack[ CAN_RX_QUEUE_SIZE * CAN_RX_QUEUE_ITEM_SIZE ];

}CanRxQueue;

typedef struct
{
	StaticQueue_t 	mStaticQueue;
	QueueHandle_t 	mHandler;
	uint8_t 		mQueueStack[ UART_RX_QUEUE_SIZE * UART_RX_QUEUE_ITEM_SIZE ];

}UartRxQueue;

typedef struct
{
	StaticTask_t 	mStaticTask;
	StackType_t  	mTaskStack[ TASK_STACK_SIZE ];
	TaskHandle_t 	mHandler;
}Task;

CanRxQueue  xCanRxQueue;
UartRxQueue xUartRxQueue;

Task xCanRxTask;
Task xUartRxTask;

void initRTOS(void);

#endif
