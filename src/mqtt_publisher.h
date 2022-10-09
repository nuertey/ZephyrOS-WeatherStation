/*
 * Copyright (c) 2022 Nuertey Odzeyem
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(dht11_and_lcd16x2, LOG_LEVEL_DBG);

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/mqtt.h>
#include <zephyr/random/rand32.h>

#include <string.h>
#include <errno.h>

#include "config.h"

#define SUCCESS_OR_EXIT(rc) { if (rc != 0) { return 1; } }
#define SUCCESS_OR_BREAK(rc) { if (rc != 0) { break; } }

#define RC_STR(rc) ((rc) == 0 ? "OK" : "ERROR")

#define PRINT_RESULT(func, rc) \
    LOG_INF("%s: %d <%s>", (func), rc, RC_STR(rc))

#if defined(CONFIG_USERSPACE)
    #include <zephyr/app_memory/app_memdomain.h>
    K_APPMEM_PARTITION_DEFINE(app_partition);
    struct k_mem_domain app_domain;
    #define APP_BMEM K_APP_BMEM(app_partition)
    #define APP_DMEM K_APP_DMEM(app_partition)
#else
    #define APP_BMEM
    #define APP_DMEM
#endif

/* Buffers for MQTT client. */
APP_BMEM uint8_t rx_buffer[APP_MQTT_BUFFER_SIZE];
APP_BMEM uint8_t tx_buffer[APP_MQTT_BUFFER_SIZE];

#if defined(CONFIG_MQTT_LIB_WEBSOCKET)
    /* Making RX buffer large enough that the full IPv6 packet can fit into it */
    #define MQTT_LIB_WEBSOCKET_RECV_BUF_LEN 1280
    
    /* Websocket needs temporary buffer to store partial packets */
    APP_BMEM uint8_t temp_ws_rx_buf[MQTT_LIB_WEBSOCKET_RECV_BUF_LEN];
#endif

/* The mqtt client struct */
APP_BMEM struct mqtt_client client_ctx;

/* MQTT Broker details. */
APP_BMEM struct sockaddr_storage broker;

#if defined(CONFIG_SOCKS)
    APP_BMEM struct sockaddr socks5_proxy;
#endif

APP_BMEM struct zsock_pollfd fds[1];
APP_BMEM int nfds;

APP_BMEM bool connected;

#if defined(CONFIG_MQTT_LIB_TLS)
    #include "test_certs.h"
    
    #define TLS_SNI_HOSTNAME "localhost"
    #define APP_CA_CERT_TAG 1
    #define APP_PSK_TAG 2
    
    APP_DMEM sec_tag_t m_sec_tags[] =
    {
    #if defined(MBEDTLS_X509_CRT_PARSE_C) || defined(CONFIG_NET_SOCKETS_OFFLOAD)
        APP_CA_CERT_TAG,
    #endif
    #if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
        APP_PSK_TAG,
    #endif
    };
    
    /*************************************************
    * USER can use these TLS APIs that follow below.
    ************************************************/ 
    int tls_init(void);
#endif /* CONFIG_MQTT_LIB_TLS */

#if defined(CONFIG_USERSPACE)
    #define STACK_SIZE 2048
    
    #if IS_ENABLED(CONFIG_NET_TC_THREAD_COOPERATIVE)
        #define THREAD_PRIORITY K_PRIO_COOP(CONFIG_NUM_COOP_PRIORITIES - 1)
    #else
        #define THREAD_PRIORITY K_PRIO_PREEMPT(8)
    #endif
#endif

/******************************************
 * USER can use the APIs that follow below.
 *****************************************/ 
void prepare_fds(struct mqtt_client *client);
void clear_fds(void);
int wait(int timeout);
void mqtt_evt_handler(struct mqtt_client *const client, const struct mqtt_evt *evt);
int publish(struct mqtt_client *client, char * topic, char * payload);
void broker_init(void);
void client_init(struct mqtt_client *client);
int try_to_connect(struct mqtt_client *client);
int process_mqtt_and_sleep(struct mqtt_client *client, int timeout);
