/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <net/coap_utils.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/openthread.h>
#include <zephyr/net/socket.h>
#include <openthread/thread.h>

#include "coap_client.h"

#define COAP_PORT 5683

/**@brief Enumeration describing light commands. */
enum light_command
{
	THREAD_COAP_UTILS_LIGHT_CMD_OFF = '0',
	THREAD_COAP_UTILS_LIGHT_CMD_ON = '1',
	THREAD_COAP_UTILS_LIGHT_CMD_TOGGLE = '2'
};

#define PROVISIONING_URI_PATH "provisioning"
#define LIGHT_URI_PATH "data"

LOG_MODULE_REGISTER(coap_client_utils, CONFIG_COAP_CLIENT_UTILS_LOG_LEVEL);

/* Options supported by the server */
static const char *const light_option[] = {LIGHT_URI_PATH, NULL};

/* Thread multicast mesh local address */
static struct sockaddr_in6 multicast_local_addr = {
	.sin6_family = AF_INET6,
	.sin6_port = htons(COAP_PORT),
	.sin6_addr.s6_addr = {0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
	.sin6_scope_id = 0U};

void coap_client_init()
{
	coap_init(AF_INET6, NULL);
}

void coap_send_data(uint8_t *payload)
{
	printk("Payload: %s , %zu\n", payload, strlen(payload));

	coap_send_request(COAP_METHOD_PUT,
					  (const struct sockaddr *)&multicast_local_addr,
					  light_option, payload, strlen(payload), NULL);
}
