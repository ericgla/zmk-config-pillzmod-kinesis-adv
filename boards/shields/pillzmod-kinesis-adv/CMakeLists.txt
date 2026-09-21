target_sources(app PRIVATE leds.c)
target_sources_ifdef(CONFIG_PWM app PRIVATE buzzer.c)
