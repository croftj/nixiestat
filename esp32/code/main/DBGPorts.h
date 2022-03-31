#ifndef DEBUG_PORTS_H
#define DEBUG_PORTS_H

#define DBG_PIN_1 ((gpio_num_t)25)
#define DBG_PIN_2 ((gpio_num_t)23)
#define DBG_PIN_3 ((gpio_num_t)12)
#define DBG_PIN_4 ((gpio_num_t)13)
//#define DBG_PIN_MSK ((1ULL<<DBG_PIN_1) | (1ULL<<DBG_PIN_2) | (1ULL<<DBG_PIN_3) | (1ULL<<DBG_PIN_4))
#define DBG_PIN_MSK ((1ULL<<DBG_PIN_3) | (1ULL<<DBG_PIN_4))

void init_debug();

#endif
