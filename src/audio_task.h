#ifndef AUDIO_TASK_H
#define AUDIO_TASK_H

#include <stdbool.h>
#include <stdint.h>

bool audio_task_init();

extern volatile uint32_t voice_type;

#endif
