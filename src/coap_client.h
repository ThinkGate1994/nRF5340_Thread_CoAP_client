/**
 * @file
 * @defgroup coap_client_utils API for coap_client_* samples
 * @{
 */

/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef __COAP_CLIENT_UTILS_H__
#define __COAP_CLIENT_UTILS_H__

/** @brief Initialize CoAP client utilities.
 */
void coap_client_init();

/** @brief Send data through CoAP
 */
void coap_send_data(uint8_t *payload);

#endif

/**
 * @}
 */
