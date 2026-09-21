#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/init.h>
#include <zmk/events/layer_state_changed.h>

#define BOOT_TONE_DURATION_MS  300
#define LAYER_TONE_DURATION_MS 500

static const struct pwm_dt_spec buzzer = PWM_DT_SPEC_GET(DT_PATH(zephyr_user));

static const uint32_t boot_tones_hz[] = {2000, 2600, 3200, 2000};
static const uint32_t layer_tones_hz[] = {2000, 2600, 3200, 3800};

static void play_tone(uint32_t freq_hz, uint32_t duration_ms) {
    if (!pwm_is_ready_dt(&buzzer)) {
        return;
    }

    uint32_t period = PWM_HZ(freq_hz);

    /* 50% duty cycle for maximum volume */
    pwm_set_dt(&buzzer, period, period / 2U);
    k_sleep(K_MSEC(duration_ms));
    pwm_set_pulse_dt(&buzzer, 0);
}

static void boot_tone_work_handler(struct k_work *work) {
    for (size_t i = 0; i < ARRAY_SIZE(boot_tones_hz); i++) {
        play_tone(boot_tones_hz[i], BOOT_TONE_DURATION_MS);
    }
}

static K_WORK_DEFINE(boot_tone_work, boot_tone_work_handler);

static uint32_t pending_layer_tone_hz;

static void layer_tone_work_handler(struct k_work *work) {
    play_tone(pending_layer_tone_hz, LAYER_TONE_DURATION_MS);
}

static K_WORK_DEFINE(layer_tone_work, layer_tone_work_handler);

static int layer_tone_listener_cb(const zmk_event_t *eh) {
    const struct zmk_layer_state_changed *ev = as_zmk_layer_state_changed(eh);

    if (!ev->state || ev->layer >= ARRAY_SIZE(layer_tones_hz)) {
        return 0;
    }

    pending_layer_tone_hz = layer_tones_hz[ev->layer];
    k_work_submit(&layer_tone_work);

    return 0;
}

ZMK_LISTENER(layer_tone_listener, layer_tone_listener_cb);
ZMK_SUBSCRIPTION(layer_tone_listener, zmk_layer_state_changed);

static int buzzer_init(void) {
    k_work_submit(&boot_tone_work);

    return 0;
}

SYS_INIT(buzzer_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
