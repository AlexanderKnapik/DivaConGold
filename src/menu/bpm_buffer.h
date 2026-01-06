#ifndef BPM_BUFFER_H_
#define BPM_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void bpm_buffer_write(uint32_t val);
void bpm_buffer_clear(void);
uint32_t bpm_buffer_read_average(void);

#ifdef __cplusplus
}
#endif

#endif // BPM_BUFFER_H_
