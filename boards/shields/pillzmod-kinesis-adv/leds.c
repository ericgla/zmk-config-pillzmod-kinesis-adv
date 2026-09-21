#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zmk/events/layer_state_changed.h>

#define LED_GPIO_NODE_ID DT_COMPAT_GET_ANY_STATUS_OKAY(gpio_leds)

// GPIO-based LED device
static const struct device *led_dev = DEVICE_DT_GET(LED_GPIO_NODE_ID);

// Light one LED per active layer: layer 1 -> led-layer1, layer 2 -> led-layer2
static int led_layer_listener_cb(const zmk_event_t *eh) {
    const struct zmk_layer_state_changed *ev = as_zmk_layer_state_changed(eh);

    switch (ev->layer) {
    case 1:
        if (ev->state) {
            led_on(led_dev, DT_NODE_CHILD_IDX(DT_ALIAS(led_layer1)));
        } else {
            led_off(led_dev, DT_NODE_CHILD_IDX(DT_ALIAS(led_layer1)));
        }
        break;
    case 2:
        if (ev->state) {
            led_on(led_dev, DT_NODE_CHILD_IDX(DT_ALIAS(led_layer2)));
        } else {
            led_off(led_dev, DT_NODE_CHILD_IDX(DT_ALIAS(led_layer2)));
        }
        break;
    }

    return 0;
}

ZMK_LISTENER(layer_led_listener, led_layer_listener_cb);
ZMK_SUBSCRIPTION(layer_led_listener, zmk_layer_state_changed);

static int leds_init(const struct device *device) {
    if (!device_is_ready(led_dev)) {
        return -ENODEV;
    }

    return 0;
}

// run leds_init on boot
SYS_INIT(leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
