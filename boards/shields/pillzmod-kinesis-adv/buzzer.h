#pragma once

#include <zephyr/kernel.h>

void buzzer_beep(uint32_t freq_hz, k_timeout_t duration);
