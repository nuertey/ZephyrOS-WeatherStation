/*
 * Copyright (c) 2022 Nuertey Odzeyem
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
#include "lcd16x2.h"
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

static const char *now_str(void)
{
    static char buf[16]; /* ...HH:MM:SS.MMM */
    uint32_t now = k_uptime_get_32();
    unsigned int ms = now % MSEC_PER_SEC;
    unsigned int s;
    unsigned int min;
    unsigned int h;

    now /= MSEC_PER_SEC;
    s = now % 60U;
    now /= 60U;
    min = now % 60U;
    now /= 60U;
    h = now;

    snprintf(buf, sizeof(buf), "%u:%02u:%02u.%03u",
         h, min, s, ms);
    return buf;
}

void main(void)
{
    const struct device *const dht22 = DEVICE_DT_GET_ONE(aosong_dht);

    if (!device_is_ready(dht22)) {
        printf("Device %s is not ready\n", dht22->name);
        return;
    }

    while (true) {
        int rc = sensor_sample_fetch(dht22);

        if (rc != 0) {
            printf("Sensor fetch failed: %d\n", rc);
            break;
        }

        struct sensor_value temperature;
        struct sensor_value humidity;

        rc = sensor_channel_get(dht22, SENSOR_CHAN_AMBIENT_TEMP,
                    &temperature);
        if (rc == 0) {
            rc = sensor_channel_get(dht22, SENSOR_CHAN_HUMIDITY,
                        &humidity);
        }
        if (rc != 0) {
            printf("get failed: %d\n", rc);
            break;
        }

        printf("[%s]: %.1f °C ; %.1f %%RH\n",
               now_str(),
               sensor_value_to_double(&temperature),
               sensor_value_to_double(&humidity));
        k_sleep(K_SECONDS(2));
    }
}

void main(void)
{
    const struct device *const gpio_dev = DEVICE_DT_GET(GPIO_NODE);

    if (!device_is_ready(gpio_dev))
    {
        printk("Device %s not ready!\n", gpio_dev->name);
        return;
    }

    /* Setup GPIO output */
    GPIO_PIN_CFG(gpio_dev, GPIO_PIN_E,  GPIO_OUTPUT);
    GPIO_PIN_CFG(gpio_dev, GPIO_PIN_RS, GPIO_OUTPUT);
    GPIO_PIN_CFG(gpio_dev, GPIO_PIN_D4, GPIO_OUTPUT);
    GPIO_PIN_CFG(gpio_dev, GPIO_PIN_D5, GPIO_OUTPUT);
    GPIO_PIN_CFG(gpio_dev, GPIO_PIN_D6, GPIO_OUTPUT);
    GPIO_PIN_CFG(gpio_dev, GPIO_PIN_D7, GPIO_OUTPUT);

    printk("LCD Init\n");
    pi_lcd_init(gpio_dev, 16, 2, LCD_5x8_DOTS);

    printk("Outputting initial LCD16x2 Welcome message...\n");

    while (1)
    {
        /* Clear display */
        pi_lcd_clear(gpio_dev);
        pi_lcd_set_cursor(gpio_dev, 0, 0);
        pi_lcd_string(gpio_dev, "Nuertey Odzeyem");
        pi_lcd_set_cursor(gpio_dev, 0, 1);
        pi_lcd_string(gpio_dev, "NUCLEO F767ZI");
        k_msleep(MSEC_PER_SEC * 5U);
    }
}
