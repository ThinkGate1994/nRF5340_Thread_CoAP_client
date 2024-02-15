#ifndef __OT_COAP_CLIENT_H__
#define __OT_COAP_CLIENT_H__

#include <stdint.h>

#define PAYLOAD_SIZE 256

struct ot_coap_client_get_payload
{
    char buffer[PAYLOAD_SIZE];
    uint16_t length;
};

struct ot_coap_client_settings
{
    const char *coap_uri_path;
    uint16_t coap_port;
};

int ot_coap_client_init(struct ot_coap_client_settings *settings);
void coap_send_data_request(char *uri_path, char *payload);
#endif