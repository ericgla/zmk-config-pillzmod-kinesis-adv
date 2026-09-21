#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/init.h>

#define BOOT_TONE_HZ       2000
#define BOOT_TONE_DURATION K_MSEC(120)

static const struct pwm_dt_spec buzzer = PWM_DT_SPEC_GET(DT_PATH(zephyr_user));

static void boot_beep_work_handler(struct k_work *work) {
    if (!pwm_is_ready_dt(&buzzer)) {
        return;
    }

    uint32_t period = PWM_HZ(BOOT_TONE_HZ);

    /* 50% duty cycle for maximum volume */
    pwm_set_dt(&buzzer, period, period / 2U);
    k_sleep(BOOT_TONE_DURATION);
    pwm_set_pulse_dt(&buzzer, 0);
}

static K_WORK_DEFINE(boot_beep_work, boot_beep_work_handler);

static int buzzer_init(void) {
    k_work_submit(&boot_beep_work);

    return 0;
}

SYS_INIT(buzzer_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
