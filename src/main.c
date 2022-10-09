/*
 * Copyright (c) 2022 Nuertey Odzeyem
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "lcd16x2.h"
#include "mqtt_publisher.h"
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

LOG_MODULE_DECLARE(dht11_and_lcd16x2, LOG_LEVEL_DBG);

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

    snprintf(buf, sizeof(buf), "%u:%02u:%02u.%03u", h, min, s, ms);
    return buf;
}

void main(void)
{
    const struct device *const dht22 = DEVICE_DT_GET_ONE(aosong_dht);

    if (!device_is_ready(dht22))
    {
        printf("Device %s is not ready\n", dht22->name);
        printf("Exiting application...\n");
        return;
    }

    const struct device *const gpio_dev = DEVICE_DT_GET(GPIO_NODE);

    if (!device_is_ready(gpio_dev))
    {
        printk("Device %s not ready!\n", gpio_dev->name);
        printf("Exiting application...\n");
        return;
    }

    /* Setup GPIO output to the LCD16x2 */
    GPIO_PIN_CFG(gpio_dev, GPIO_PIN_E,  GPIO_OUTPUT);
    GPIO_PIN_CFG(gpio_dev, GPIO_PIN_RS, GPIO_OUTPUT);
    GPIO_PIN_CFG(gpio_dev, GPIO_PIN_D4, GPIO_OUTPUT);
    GPIO_PIN_CFG(gpio_dev, GPIO_PIN_D5, GPIO_OUTPUT);
    GPIO_PIN_CFG(gpio_dev, GPIO_PIN_D6, GPIO_OUTPUT);
    GPIO_PIN_CFG(gpio_dev, GPIO_PIN_D7, GPIO_OUTPUT);
    
    int result = -1;

    LOG_INF("Attempting to connect: ");
    result = try_to_connect(&client_ctx);
    PRINT_RESULT("try_to_connect", result);
    SUCCESS_OR_EXIT(result);

    printk("LCD Init\n");
    pi_lcd_init(gpio_dev, 16, 2, LCD_5x8_DOTS);

    printk("Outputting initial LCD16x2 welcome message...\n");

    /* Clear display */
    pi_lcd_clear(gpio_dev);
    pi_lcd_set_cursor(gpio_dev, 0, 0);
    pi_lcd_string(gpio_dev, "Nuertey Odzeyem");
    pi_lcd_set_cursor(gpio_dev, 0, 1);
    pi_lcd_string(gpio_dev, "NUCLEO F767ZI");
    k_msleep(MSEC_PER_SEC * 5U);

    while (true)
    {
        int rc = sensor_sample_fetch(dht22);

        if (rc != 0)
        {
            printf("DHT11 Sensor fetch failed: %d\n", rc);
            printf("Exiting application...\n");
            break;
        }

        struct sensor_value temperature;
        struct sensor_value humidity;

        rc = sensor_channel_get(dht22, SENSOR_CHAN_AMBIENT_TEMP, &temperature);
        if (rc == 0)
        {
            rc = sensor_channel_get(dht22, SENSOR_CHAN_HUMIDITY, &humidity);
            if (rc != 0)
            {
                printf("Sensor humidity retrieval failed: %d\n", rc);
                printf("Exiting application...\n");
                break;              
            }
        }
        else 
        {
            printf("Sensor ambient temperature retrieval failed: %d\n", rc);
            printf("Exiting application...\n");
            break;
        }

        printf("[%s]: %.2f °C ; %.2f %%RH\n", now_str(), sensor_value_to_double(&temperature),
            sensor_value_to_double(&humidity));
            
        char tempBuffer[16];
        char humiBuffer[16];
        char tempBuffer2[100];
        char humiBuffer2[100];
        snprintf(tempBuffer, sizeof(tempBuffer), "%.2f °C", sensor_value_to_double(&temperature));
        snprintf(humiBuffer, sizeof(humiBuffer), "%.2f %%RH", sensor_value_to_double(&humidity));
        snprintf(tempBuffer2, sizeof(tempBuffer2), "%f", sensor_value_to_double(&temperature));
        snprintf(humiBuffer2, sizeof(humiBuffer2), "%f", sensor_value_to_double(&humidity));

        pi_lcd_clear(gpio_dev);
        pi_lcd_set_cursor(gpio_dev, 0, 0);
        pi_lcd_string(gpio_dev, tempBuffer);
        pi_lcd_set_cursor(gpio_dev, 0, 1);
        pi_lcd_string(gpio_dev, humiBuffer);
        
        int result2 = -1;
        
        result2 = publish(&client_ctx, NUCLEO_F767ZI_DHT11_IOT_MQTT_TOPIC1, tempBuffer2);
        PRINT_RESULT("mqtt_publish temperature", result2);
        SUCCESS_OR_BREAK(result2);

        result2 = process_mqtt_and_sleep(&client_ctx, APP_SLEEP_MSECS);
        SUCCESS_OR_BREAK(result2);

        result2 = publish(&client_ctx, NUCLEO_F767ZI_DHT11_IOT_MQTT_TOPIC2, humiBuffer2);
        PRINT_RESULT("mqtt_publish humidity", result2);
        SUCCESS_OR_BREAK(result2);

        result2 = process_mqtt_and_sleep(&client_ctx, APP_SLEEP_MSECS);
        SUCCESS_OR_BREAK(result2);
            
        k_sleep(K_SECONDS(300)); // 5 minutes is reasonable of any discernable change.
    }
    
    result = mqtt_disconnect(&client_ctx);
    PRINT_RESULT("mqtt_disconnect", result);

    LOG_INF("Bye!");
}
