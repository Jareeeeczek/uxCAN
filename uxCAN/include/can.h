#ifndef CAN_API_H_
#define CAN_API_H_
#include "stm32f10x.h"
#include "gpio.h"
#include "FreeRTOSApi.h"

void canSetup();
void canInit();
uint8_t canSendMsg(CanTxMsg aMsg);
#endif
