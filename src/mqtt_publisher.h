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

/******************************************
 * USER can use the APIs that follow below.
 *****************************************/ 
int tls_init(void);
void prepare_fds(struct mqtt_client *client);
