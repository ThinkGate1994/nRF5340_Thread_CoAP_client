#include <zephyr/logging/log.h>
#include <zephyr/net/net_pkt.h>
#include <zephyr/net/net_l2.h>
#include <zephyr/net/openthread.h>
#include <openthread/coap.h>
#include <openthread/ip6.h>
#include <openthread/message.h>
#include <openthread/thread.h>

#include "ot_coap_client.h"

LOG_MODULE_REGISTER(ot_coap_client);

struct client_context
{
	struct otInstance *ot;
};

static struct client_context clt_context = {
	.ot = NULL};

static otCoapResource coap_resource = {
	.mUriPath = NULL,
	.mHandler = NULL,
	.mContext = NULL,
	.mNext = NULL,
};

static struct ot_coap_client_get_payload payload = {
	.buffer = "",
	.length = 0,
};

void coap_send_data_request(char *uri_path, char *payload)
{
	otError error = OT_ERROR_NONE;
	otMessage *myMessage;
	otMessageInfo myMessageInfo;
	otInstance *myInstance = openthread_get_default_instance();

	const otMeshLocalPrefix *ml_prefix = otThreadGetMeshLocalPrefix(myInstance);
	uint8_t serverInterfaceID[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

	do
	{
		myMessage = otCoapNewMessage(myInstance, NULL);
		if (myMessage == NULL)
		{
			printk("Failed to allocate message for CoAP Request\n");
			return;
		}
		// otCoapMessageInit(myMessage, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_PUT);
		otCoapMessageInit(myMessage, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_PUT);

		error = otCoapMessageAppendUriPathOptions(myMessage, uri_path);
		if (error != OT_ERROR_NONE)
		{
			printk("Failed - otCoapMessageAppendUriPathOptions: %d\n", error);
			break;
		}

		error = otCoapMessageAppendContentFormatOption(myMessage, OT_COAP_OPTION_CONTENT_FORMAT_JSON);
		if (error != OT_ERROR_NONE)
		{
			printk("Failed - otCoapMessageAppendContentFormatOption: %d\n", error);
			break;
		}

		error = otCoapMessageSetPayloadMarker(myMessage);
		if (error != OT_ERROR_NONE)
		{
			printk("Failed - otCoapMessageSetPayloadMarker: %d\n", error);
			break;
		}

		error = otMessageAppend(myMessage, payload, strlen(payload));
		if (error != OT_ERROR_NONE)
		{
			printk("Failed - otMessageAppend: %d\n", error);
			break;
		}

		memset(&myMessageInfo, 0, sizeof(myMessageInfo));
		memcpy(&myMessageInfo.mPeerAddr.mFields.m8[0], ml_prefix, 8);
		memcpy(&myMessageInfo.mPeerAddr.mFields.m8[8], serverInterfaceID, 8);
		myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

		// error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo, coap_send_data_response_cb, NULL);
		error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo, NULL, NULL);

	} while (false);

	if (error != OT_ERROR_NONE)
	{
		printk("Failed to send CoAP Request: %d\n", error);
		otMessageFree(myMessage);
	}
	else
	{
		printk("Coap data send.\n");
	}
}

static void CoAP_request_handler(void *context, otMessage *message, const otMessageInfo *message_info)
{

	ARG_UNUSED(context);

	if ((otCoapMessageGetType(message) != OT_COAP_TYPE_CONFIRMABLE) && (otCoapMessageGetType(message) != OT_COAP_TYPE_NON_CONFIRMABLE))
	{
		LOG_ERR("CoAP handler - Unexpected type of message");
		goto end;
	}

	if ((otCoapMessageGetCode(message) != OT_COAP_CODE_PUT) && (otCoapMessageGetCode(message) != OT_COAP_CODE_POST))
	{
		LOG_ERR("CoAP handler - Unexpected CoAP code");
		goto end;
	}

	payload.length = otMessageRead(message, otMessageGetOffset(message), payload.buffer, PAYLOAD_SIZE - 1);

	if (payload.length > 0)
	{
		payload.buffer[payload.length] = '\0';
		LOG_INF("CoAP message: %s , %d", payload.buffer, payload.length);
	}
	else
	{
		LOG_ERR("Error reading payload");
	}

	if (otCoapMessageGetType(message) == OT_COAP_TYPE_CONFIRMABLE)
	{
		// storedata_response_send(message, message_info);
	}
end:
	return;
}

static void CoAP_default_handler(void *context, otMessage *message, const otMessageInfo *message_info)
{
	ARG_UNUSED(context);
	ARG_UNUSED(message);
	ARG_UNUSED(message_info);

	LOG_INF("Received CoAP message that does not match any request or resource");
}

int ot_coap_client_init(struct ot_coap_client_settings *settings)
{
	otError error;

	clt_context.ot = openthread_get_default_instance();
	if (!clt_context.ot)
	{
		LOG_ERR("There is no valid OpenThread instance");
		error = OT_ERROR_FAILED;
		goto end;
	}

	coap_resource.mUriPath = settings->coap_uri_path;
	coap_resource.mHandler = CoAP_request_handler;
	coap_resource.mContext = clt_context.ot;

	otCoapSetDefaultHandler(clt_context.ot, CoAP_default_handler, NULL);
	otCoapAddResource(clt_context.ot, &coap_resource);

	error = otCoapStart(clt_context.ot, settings->coap_port);
	if (error != OT_ERROR_NONE)
	{
		LOG_ERR("Failed to start OT CoAP. Error: %d", error);
		goto end;
	}

end:
	return error == OT_ERROR_NONE ? 0 : 1;
}