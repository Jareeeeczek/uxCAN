#ifndef UX_CAN_TASKS_H
#define UX_CAN_TASKS_H

#include "FreeRTOSApi.h"
#include "uart.h"
void vCanRxTask(void * aParam);
void vCanTxTask(void * aParam);
void vUartRxTask(void *aParam);
void vUartTxTask(void *aParam);

#endif
