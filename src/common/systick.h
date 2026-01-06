#ifndef SYSTICK_H_
#define SYSTICK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint32_t systick_now_ms(void);
uint32_t systick_interval_ms(uint32_t current_ms, uint32_t previous_ms);
uint32_t systick_time_since_ms(uint32_t prev_tick_ms);

#ifdef __cplusplus
}
#endif

#endif // SYSTICK_H_
