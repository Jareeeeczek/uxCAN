#include <stdio.h>
#include <stdint.h>
#include "stm32f10x.h"
#include "gpio.h"
#include "FreeRTOSApi.h"

#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))
#define UART_DMA_CHANNEL		DMA1_Channel1

uint8_t rxUartDmaBuffer[100];

void uartSetup();
void uartDmaInit();
void uartInit();
void uartSendData(uint8_t aData);
void uartSendCANMsg(CanRxMsg aCanRxMsg);
void usartCheck(void);
