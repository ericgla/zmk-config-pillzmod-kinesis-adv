#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/init.h>

#include "buzzer.h"

#define BOOT_TONE_HZ       2000
#define BOOT_TONE_DURATION K_MSEC(120)

static const struct pwm_dt_spec buzzer = PWM_DT_SPEC_GET(DT_PATH(zephyr_user));

static uint32_t beep_freq_hz;
static k_timeout_t beep_duration;

static void beep_work_handler(struct k_work *work) {
    if (!pwm_is_ready_dt(&buzzer)) {
        return;
    }

    uint32_t period = PWM_HZ(beep_freq_hz);

    /* 50% duty cycle for maximum volume */
    pwm_set_dt(&buzzer, period, period / 2U);
    k_sleep(beep_duration);
    pwm_set_pulse_dt(&buzzer, 0);
}

static K_WORK_DEFINE(beep_work, beep_work_handler);

void buzzer_beep(uint32_t freq_hz, k_timeout_t duration) {
    beep_freq_hz = freq_hz;
    beep_duration = duration;
    k_work_submit(&beep_work);
}

static int buzzer_init(void) {
    buzzer_beep(BOOT_TONE_HZ, BOOT_TONE_DURATION);

    return 0;
}

SYS_INIT(buzzer_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
