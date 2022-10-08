/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef CONFIG_NET_CONFIG_SETTINGS
#ifdef CONFIG_NET_IPV6
#define ZEPHYR_ADDR     CONFIG_NET_CONFIG_MY_IPV6_ADDR
#define SERVER_ADDR     CONFIG_NET_CONFIG_PEER_IPV6_ADDR
#else
#define ZEPHYR_ADDR     CONFIG_NET_CONFIG_MY_IPV4_ADDR
#define SERVER_ADDR     CONFIG_NET_CONFIG_PEER_IPV4_ADDR
#endif
#else
#ifdef CONFIG_NET_IPV6
#define ZEPHYR_ADDR     "2001:db8::1"
#define SERVER_ADDR     "2001:db8::2"
#else
#define ZEPHYR_ADDR     "10.42.0.2"
#define SERVER_ADDR     "10.42.0.1"
#endif
#endif

#if defined(CONFIG_SOCKS)
#define SOCKS5_PROXY_ADDR   SERVER_ADDR
#define SOCKS5_PROXY_PORT   1080
#endif

#ifdef CONFIG_MQTT_LIB_TLS
#ifdef CONFIG_MQTT_LIB_WEBSOCKET
#define SERVER_PORT     9001
#else
#define SERVER_PORT     8883
#endif /* CONFIG_MQTT_LIB_WEBSOCKET */
#else
#ifdef CONFIG_MQTT_LIB_WEBSOCKET
#define SERVER_PORT     9001
#else
#define SERVER_PORT     1883
#endif /* CONFIG_MQTT_LIB_WEBSOCKET */
#endif

#define APP_CONNECT_TIMEOUT_MS  2000
#define APP_SLEEP_MSECS     500

#define APP_CONNECT_TRIES   10

#define APP_MQTT_BUFFER_SIZE    128

#define MQTT_CLIENTID       "zephyr_publisher"

#define NUCLEO_F767ZI_DHT11_IOT_MQTT_TOPIC1   "/Nuertey/Nucleo/F767ZI/Temperature"
#define NUCLEO_F767ZI_DHT11_IOT_MQTT_TOPIC2   "/Nuertey/Nucleo/F767ZI/Humidity"
