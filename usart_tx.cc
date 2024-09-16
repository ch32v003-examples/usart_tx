#include <stdarg.h>
#include "ch32v003fun.h"
#include <stdio.h>
#include "ringbuffer.h"

extern "C" {
  int mini_vpprintf(int (*puts)(char* s, int len, void* buf), void* buf, const char *fmt, va_list va);
}

static volatile RingBuffer uartBuf;

void uartSetup(int uartBRR) {
  // Enable GPIOD and UART. Need to enable AFIO to remap TX=D6.
  RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO;
  // Push-Pull, 10MHz Output, GPIO D5, with AutoFunction
  //  GPIOD->CFGLR &= ~(0xf<<(4*5));
  //  GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*5);
  
  AFIO->PCFR1 = 0x200000; // Remap TX=D6

  // Push-Pull, 10MHz Output, GPIO D6, with AutoFunction
  GPIOD->CFGLR &= ~(0xf<<(4*6));
  GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*6);
  
  // Enable transmit completion interrupt.
  USART1->CTLR1 = USART_WordLength_8b | USART_Parity_No | USART_Mode_Tx | USART_CTLR1_TCIE;
  USART1->CTLR2 = USART_StopBits_1;
  USART1->CTLR3 = USART_HardwareFlowControl_None;
  
  // 115200bps
  USART1->BRR = uartBRR;

  // Enable USART1 interrupt
  NVIC_EnableIRQ(USART1_IRQn);

  // Enable UART, 8bit
  USART1->CTLR1 |= CTLR1_UE_Set;
}

static volatile uint32_t itrCounter = 0;

extern "C" {
  void USART1_IRQHandler(void) __attribute__((interrupt));
  void USART1_IRQHandler(void) {
    static uint8_t chBuf;
    ++itrCounter;
    remove_from_buffer(&uartBuf, &chBuf);
    USART1->STATR &= (~USART_FLAG_TC);
    USART1->DATAR = chBuf;
    if (uartBuf.len == 0) {
      USART1->CTLR1 &= ~USART_CTLR1_TCIE;
    }
  }
}

static void txUart(uint8_t ch) {
  USART1->CTLR1 |= USART_CTLR1_TCIE;
  add_to_buffer(&uartBuf, ch);
}

static int uartWrite(const char *buf, int size) {
  for (int i = 0; i < size; i++) {
    txUart(*buf++);
  }
  return size;
}

static int uartPuts(char *s, int len, void *buf) {
  uartWrite(s, len);
  return len;
}

static int uartPrintf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  int ret_status = mini_vpprintf(uartPuts, 0, format, args);
  va_end(args);
  return ret_status;
}

int main() {
  uint32_t count = 0;
  init_buffer(&uartBuf);
	
  SystemInit();
  uartSetup(UART_BRR);

  while (1) {
    Delay_Ms(500);
    printf("Count: %lu, Interrupted %lu\r\n", count, itrCounter);
    uartPrintf("%lu\r\n", count);
    ++count;
  }
}
