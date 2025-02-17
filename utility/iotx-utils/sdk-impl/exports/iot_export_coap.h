/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __IOTX_COAP_API_H__
#define __IOTX_COAP_API_H__

#define IOTX_PRODUCT_KEY_LEN     (11)   /* IoTx product key length  */
#define IOTX_DEVICE_NAME_LEN     (20)   /* IoTx device name length */
#define IOTX_DEVICE_ID_LEN       (64)   /* IoTx device ID length */
#define IOTX_DEVICE_SECRET_LEN   (32)   /* IoTx device secret length */
#define IOTX_URI_MAX_LEN         (135)  /* IoTx CoAP uri maximal length */


/*iotx return code definition*/
typedef enum
{
    IOTX_ERR_MSG_TOO_LOOG    =  -7,   /* The payload too loog */
    IOTX_ERR_URI_TOO_LOOG    =  -6,   /* URI length too long */
    IOTX_ERR_NOT_AUTHED      =  -5,   /* Client isn't authed */
    IOTX_ERR_AUTH_FAILED     =  -4,   /* Client authed failed */
    IOTX_ERR_BUFF_TOO_SHORT  =  -3,   /* Buffer too short */
    IOTX_ERR_NO_MEM          =  -2,   /* Malloc failed */
    IOTX_ERR_INVALID_PARAM   =  -1,   /* Invalid parameter */
    IOTX_SUCCESS             =   0,   /* Success */
}iotx_ret_code_t;

/* The message payload encode format */
typedef enum
{
    IOTX_CONTENT_TYPE_JSON,
    IOTX_CONTENT_TYPE_CBOR,
}iotx_content_type_t;

/* The message type */
typedef enum
{
    IOTX_MESSAGE_CON  = 0,       /* confirmable message */
    IOTX_MESSAGE_NON  = 1,       /* non-confirmable message */
} iotx_msg_type_t;

/* IoTx events to notify application */
typedef enum
{
    IOTX_COAP_EVENT_SEND_FAILED = 0,
    IOTX_COAP_EVENT_RECV_FAILED = 1,
    IOTX_COAP_EVENT_AUTH_FAILED = 2,
}iotx_coap_event_t;

typedef enum
{
    IOXT_COAP_RESP_CODE_CONTENT        = 0x45,       /* Mapping to 2.05, Content*/
    IOXT_COAP_RESP_CODE_BAD_REQUEST    = 0x80,       /* Mapping to 4.00, Bad Request*/
    IOXT_COAP_RESP_CODE_UNAUTHORIZED   = 0x81,       /* Mapping to 4.01, Token is invalid or expire*/
    IOXT_COAP_RESP_CODE_NOT_FOUND      = 0x84,       /* Mapping to 4.04, Path or uri is not found*/
    IOXT_COAP_RESP_CODE_URL_TOO_LONG   = 0x8E,       /* Mapping to 4.14, The request url is too long*/
    IOXT_COAP_RESP_CODE_INTERNAL_SERVER_ERROR = 0xA0,/* Mapping to 5.00, Internal server error*/

}iotx_coap_resp_code_t;

/* Callback function to notify the application events.*/
typedef void (*iotx_event_handle_t)(void *context, iotx_coap_event_t event, void *p_data);

/*IoTx device*/
typedef struct
{
    char     product_key[IOTX_PRODUCT_KEY_LEN + 1];
    char     device_name[IOTX_DEVICE_NAME_LEN + 1];
    char     device_id[IOTX_DEVICE_ID_LEN + 1];
    char     device_secret[IOTX_DEVICE_SECRET_LEN + 1];
} iotx_deviceinfo_t;


/* IoTx initializa parameters */
typedef struct
{
    char                 *p_url;
    iotx_deviceinfo_t    *p_devinfo;
    iotx_event_handle_t   event_handle;
}iotx_coap_config_t;

/* Callback function to handle the response message.*/
typedef void (*iotx_response_callback_t)(void *p_arg, void *p_message);

/* IoTx message definition */
typedef struct
{
    unsigned char           *p_payload;
    unsigned short           payload_len;
    iotx_content_type_t      content_type;
    iotx_msg_type_t          msg_type;
    void                    *user_data;
    iotx_response_callback_t resp_callback;
}iotx_message_t;


/*iotx coap context definition*/
typedef void iotx_coap_context_t;

iotx_coap_context_t *IOT_CoAP_Init(iotx_coap_config_t *p_config);

void IOT_CoAP_Deinit(iotx_coap_context_t **p_context);

int  IOT_CoAP_DeviceNameAuth(iotx_coap_context_t *p_context);

int  IOT_CoAP_Yield(iotx_coap_context_t *p_context);

int  IOT_CoAP_SendMessage(iotx_coap_context_t *p_context,   char *p_path, iotx_message_t *p_message);

int  IOT_CoAP_SendMessage_block(iotx_coap_context_t *p_context, char *p_path, iotx_message_t *p_message,
                               unsigned int block_type, unsigned int num, unsigned int more, unsigned int size);

int  IOT_CoAP_ParseOption_block(void *p_message, int type, unsigned int *num, 
                                unsigned int *more, unsigned int *size);

int  IOT_CoAP_GetMessagePayload(void *p_message, unsigned char **pp_payload, int *p_len);

int  IOT_CoAP_GetMessageCode(void *p_message, iotx_coap_resp_code_t *p_resp_code);

#endif
