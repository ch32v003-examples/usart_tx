#include <stdint.h>
#include <stdbool.h> 

#define RING_BUFFER_SIZE 32

typedef struct {
  uint8_t start_idx;
  uint8_t len;
  uint8_t buf[RING_BUFFER_SIZE];
} RingBuffer;

#ifdef __cplusplus
extern "C" {
#endif

  extern void init_buffer(volatile RingBuffer* pBuf);
  extern bool add_to_buffer(volatile RingBuffer* pBuf, uint8_t c);
  extern bool remove_from_buffer(volatile RingBuffer* pBuf, uint8_t *pc);

#ifdef __cplusplus
}
#endif
