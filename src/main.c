#include <zephyr/kernel.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>

// #include "coap_client.h"
#include "ot_coap_client.h"
#include "openthread.h"

LOG_MODULE_REGISTER(coap_client);

int main(void)
{
	int ret;

	LOG_INF("Start CoAP-client sample");

	ret = dk_leds_init();
	if (ret)
	{
		LOG_ERR("Cannot init leds, (error: %d)", ret);
		return 0;
	}

	openthread_init();

	// coap_client_init();

	// uint8_t state = 48;
	// while (1)
	// {
	// 	/* code */
	// 	LOG_INF("Send multicast mesh 'light' request");
	// 	if (state == 49)
	// 	{
	// 		state = 48;
	// 	}
	// 	else
	// 	{
	// 		state = 49;
	// 	}

	// 	printk("state : %d\n", state);

	// 	char *payload = "{\"user\" : \"nrf5340\",\"senor_01\" : 4.56,\"senor_02\" : 100,\"senor_03\" : 220}";

	// 	coap_send_data(payload);

	// 	k_msleep(1000);
	// }

	// Initialize CoAP server settings
	struct ot_coap_client_settings coap_settings = {
		.coap_uri_path = NULL,
		.coap_port = 5683, // #default 5683, You can replace it with your desired port
	};

	ret = ot_coap_client_init(&coap_settings);
	if (ret)
	{
		LOG_ERR("Could not initialize OpenThread CoAP");
		goto end;
	}

	while (1)
	{
		coap_send_data_request("data", "hello world");
		k_msleep(1000);
	}

end:
	return 0;
}
