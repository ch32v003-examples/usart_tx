/*
 * File:   ringbuffer.c
 * Author: shanai
 *
 * Created on 2022/12/31, 10:03
 */

#include "ringbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_buffer(volatile RingBuffer* pBuf) {
  pBuf->start_idx = 0;
  pBuf->len = 0;
}

bool add_to_buffer(volatile RingBuffer* pBuf, uint8_t c) {
  if (RING_BUFFER_SIZE <= pBuf->len) {
    return false;
  }
  
  uint8_t idx = (pBuf->start_idx + pBuf->len) % RING_BUFFER_SIZE;
  pBuf->buf[idx] = c;
  ++pBuf->len;

  return true;
}

bool remove_from_buffer(volatile RingBuffer* pBuf, uint8_t *pc) {
  if (pBuf->len <= 0) {
    return false;
  }

  uint8_t idx = pBuf->start_idx;
  *pc = pBuf->buf[idx++];
  pBuf->start_idx = idx % RING_BUFFER_SIZE;
  --pBuf->len;

  return true;
}

#ifdef __cplusplus
}
#endif
