/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//#include "iot_import_dtls.h"
#ifdef COAP_DTLS_SUPPORT
#include "mbedtls/ssl.h"
#include "mbedtls/platform.h"
#include "mbedtls/sha256.h"
#include "mbedtls/debug.h"
#include "mbedtls/timing.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ssl_cookie.h"
#include "mbedtls/net_sockets.h"

#if 0

typedef struct {
    mbedtls_ssl_context          context;
    mbedtls_ssl_config           conf;
    mbedtls_ctr_drbg_context     ctr_drbg;
    mbedtls_entropy_context      entropy;
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt             cacert;
#endif
    mbedtls_net_context          fd;
    mbedtls_timing_delay_context timer;
    mbedtls_ssl_cookie_ctx       cookie_ctx;
} dtls_session_t;


static  void *_DTLSCalloc_wrapper(size_t n, size_t s)
{
    void *ptr = NULL;
    size_t len = n * s;
    ptr = coap_malloc(len);
    if (NULL != ptr) {
        memset(ptr, 0x00, len);
    }
    return ptr;
}

static  void _DTLSFree_wrapper(void *ptr)
{
    if (NULL != ptr) {
        coap_free(ptr);
        ptr = NULL;
    }
}

static unsigned int _DTLSVerifyOptions_set(dtls_session_t *p_dtls_session,
        unsigned char    *p_ca_cert_pem)
{
    int result;
    unsigned int err_code = DTLS_SUCCESS;

#ifdef MBEDTLS_X509_CRT_PARSE_C
    if (p_ca_cert_pem != NULL) {
#ifndef TEST_COAP_DAILY
        mbedtls_ssl_conf_authmode(&p_dtls_session->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
#else
        mbedtls_ssl_conf_authmode(&p_dtls_session->conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
#endif
        DTLS_TRC("Call mbedtls_ssl_conf_authmode\r\n");

        DTLS_TRC("x509 ca cert pem len %d\r\n%s\r\n", (int)strlen((char *)p_ca_cert_pem) + 1, p_ca_cert_pem);
        result = mbedtls_x509_crt_parse(&p_dtls_session->cacert,
                                        p_ca_cert_pem,
                                        strlen((const char *)p_ca_cert_pem) + 1);

        DTLS_TRC("mbedtls_x509_crt_parse result 0x%04x\r\n", result);
        if (0 != result) {
            err_code = DTLS_INVALID_CA_CERTIFICATE;
        } else {
            mbedtls_ssl_conf_ca_chain(&p_dtls_session->conf, &p_dtls_session->cacert, NULL);
        }
    } else
#endif
    {
        mbedtls_ssl_conf_authmode(&p_dtls_session->conf, MBEDTLS_SSL_VERIFY_NONE);
    }

    return err_code;
}

static void _DTLSLog_wrapper(void        *p_ctx, int level,
                             const char *p_file, int line,   const char *p_str)
{
    DTLS_INFO("[mbedTLS]:[%s]:[%d]: %s\r\n", p_file, line, p_str);
}

static unsigned int _DTLSContext_setup(dtls_session_t *p_dtls_session, coap_dtls_options_t  *p_options)
{
    int   result = 0;

    mbedtls_ssl_init(&p_dtls_session->context);

    result = mbedtls_ssl_setup(&p_dtls_session->context, &p_dtls_session->conf);
    DTLS_TRC("mbedtls_ssl_setup result 0x%04x\r\n", result);

    if (result == 0) {
        if (p_dtls_session->conf.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
            mbedtls_ssl_set_timer_cb(&p_dtls_session->context,
                                     (void *)&p_dtls_session->timer,
                                     mbedtls_timing_set_delay,
                                     mbedtls_timing_get_delay);
        }

#ifdef MBEDTLS_X509_CRT_PARSE_C
        DTLS_TRC("mbedtls_ssl_set_hostname %s\r\n", p_options->p_host);
        mbedtls_ssl_set_hostname(&p_dtls_session->context, p_options->p_host);
#endif
        mbedtls_ssl_set_bio(&p_dtls_session->context,
                            (void *)&p_dtls_session->fd,
                            mbedtls_net_send,
                            mbedtls_net_recv,
                            mbedtls_net_recv_timeout);
        DTLS_TRC("mbedtls_ssl_set_bio result 0x%04x\r\n", result);

        do {
            result = mbedtls_ssl_handshake(&p_dtls_session->context);
        } while (result == MBEDTLS_ERR_SSL_WANT_READ ||
                 result == MBEDTLS_ERR_SSL_WANT_WRITE);
        DTLS_TRC("mbedtls_ssl_handshake result 0x%04x\r\n", result);
    }

    return (result ? DTLS_HANDSHAKE_FAILED : DTLS_SUCCESS);
}

dtls_session_t *_DTLSSession_init()
{
    dtls_session_t *p_dtls_session = NULL;
    p_dtls_session = coap_malloc(sizeof(dtls_session_t));

    mbedtls_debug_set_threshold(0);
    mbedtls_platform_set_calloc_free(_DTLSCalloc_wrapper, _DTLSFree_wrapper);
    if (NULL != p_dtls_session) {
        mbedtls_net_init(&p_dtls_session->fd);
        mbedtls_ssl_init(&p_dtls_session->context);
        mbedtls_ssl_config_init(&p_dtls_session->conf);
        mbedtls_net_init(&p_dtls_session->fd);

        mbedtls_ssl_cookie_init(&p_dtls_session->cookie_ctx);

#ifdef MBEDTLS_X509_CRT_PARSE_C
        mbedtls_x509_crt_init(&p_dtls_session->cacert);
#endif
        mbedtls_ctr_drbg_init(&p_dtls_session->ctr_drbg);
        mbedtls_entropy_init(&p_dtls_session->entropy);
        DTLS_INFO("HAL_DTLSSession_init success\r\n");

    }

    return p_dtls_session;
}

unsigned int _DTLSSession_deinit(dtls_session_t *p_dtls_session)
{
    int ret;
    if (p_dtls_session != NULL) {
        do {
            ret = mbedtls_ssl_close_notify(&p_dtls_session->context);
        } while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);

        mbedtls_net_free(&p_dtls_session->fd);
#ifdef MBEDTLS_X509_CRT_PARSE_C
        mbedtls_x509_crt_free(&p_dtls_session->cacert);
#endif
        mbedtls_ssl_cookie_free(&p_dtls_session->cookie_ctx);

        mbedtls_ssl_config_free(&p_dtls_session->conf);
        mbedtls_ssl_free(&p_dtls_session->context);

        mbedtls_ctr_drbg_free(&p_dtls_session->ctr_drbg);
        mbedtls_entropy_free(&p_dtls_session->entropy);
        coap_free(p_dtls_session);
    }

    return DTLS_SUCCESS;
}

DTLSContext *HAL_DTLSSession_create(coap_dtls_options_t            *p_options)
{
    char port[6] = {0};
    int result = 0;
    dtls_session_t *p_dtls_session = NULL;

    p_dtls_session = _DTLSSession_init();
    if (NULL != p_dtls_session) {
        mbedtls_ssl_config_init(&p_dtls_session->conf);
        result = mbedtls_ctr_drbg_seed(&p_dtls_session->ctr_drbg, mbedtls_entropy_func, &p_dtls_session->entropy,
                                       (const unsigned char *)"IoTx",
                                       strlen("IoTx"));
        if (0 !=  result) {
            DTLS_ERR("mbedtls_ctr_drbg_seed result 0x%04x\r\n", result);
            goto error;
        }
        result = mbedtls_ssl_config_defaults(&p_dtls_session->conf,
                                             MBEDTLS_SSL_IS_CLIENT,
                                             MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                             MBEDTLS_SSL_PRESET_DEFAULT);
        if (0 != result) {
            DTLS_ERR("mbedtls_ssl_config_defaults result 0x%04x\r\n", result);
            goto error;
        }
        mbedtls_ssl_conf_rng(&p_dtls_session->conf, mbedtls_ctr_drbg_random, &p_dtls_session->ctr_drbg);
        mbedtls_ssl_conf_dbg(&p_dtls_session->conf, _DTLSLog_wrapper, NULL);

        result = mbedtls_ssl_cookie_setup(&p_dtls_session->cookie_ctx,
                                          mbedtls_ctr_drbg_random, &p_dtls_session->ctr_drbg);
        if (0 != result) {
            DTLS_ERR("mbedtls_ssl_cookie_setup result 0x%04x\r\n", result);
            goto error;
        }
#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY) && defined(MBEDTLS_SSL_SRV_C)
        mbedtls_ssl_conf_dtls_cookies(&p_dtls_session->conf, mbedtls_ssl_cookie_write,
                                      mbedtls_ssl_cookie_check, &p_dtls_session->cookie_ctx);
#endif

        result = _DTLSVerifyOptions_set(p_dtls_session, p_options->p_ca_cert_pem);

        if (DTLS_SUCCESS != result) {
            DTLS_ERR("DTLSVerifyOptions_set result 0x%04x\r\n", result);
            goto error;
        }
        sprintf(port, "%u", p_options->port);
        result = mbedtls_net_connect(&p_dtls_session->fd, p_options->p_host,
                                     port, MBEDTLS_NET_PROTO_UDP);
        if (0 != result) {
            DTLS_ERR("mbedtls_net_connect result 0x%04x\r\n", result);
            goto error;
        }

#ifdef MBEDTLS_SSL_PROTO_DTLS
        if (p_dtls_session->conf.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
            mbedtls_ssl_conf_min_version(&p_dtls_session->conf,
                                         MBEDTLS_SSL_MAJOR_VERSION_3,
                                         MBEDTLS_SSL_MINOR_VERSION_3);

            mbedtls_ssl_conf_max_version(&p_dtls_session->conf,
                                         MBEDTLS_SSL_MAJOR_VERSION_3,
                                         MBEDTLS_SSL_MINOR_VERSION_3);

            mbedtls_ssl_conf_handshake_timeout(&p_dtls_session->conf,
                                               (MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN * 2),
                                               (MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN * 2 * 4));
        }
#endif
        result = _DTLSContext_setup(p_dtls_session, p_options);
        if (DTLS_SUCCESS != result) {
            DTLS_ERR("DTLSVerifyOptions_set result 0x%04x\r\n", result);
            goto error;
        }

        return (DTLSContext *)p_dtls_session;
    }

error:
    if (NULL != p_dtls_session) {
        _DTLSSession_deinit(p_dtls_session);
    }
    return NULL;
}

unsigned int HAL_DTLSSession_write(DTLSContext *context,
                                   const unsigned char   *p_data,
                                   unsigned int    *p_datalen)
{
    int len  = 0;
    unsigned int err_code = DTLS_SUCCESS;
    dtls_session_t *p_dtls_session = (dtls_session_t *)context;

    if (NULL != p_dtls_session && NULL != p_data && p_datalen != NULL) {
        len = (*p_datalen);
        len = mbedtls_ssl_write(&p_dtls_session->context, p_data, len);

        if (len < 0) {
            if (len == MBEDTLS_ERR_SSL_CONN_EOF) {
                if (p_dtls_session->context.state < MBEDTLS_SSL_HANDSHAKE_OVER) {
                    err_code = DTLS_HANDSHAKE_IN_PROGRESS;
                }
            }
        } else {
            (*p_datalen) = len;
            err_code      = DTLS_SUCCESS;
        }
    }

    return err_code;
}

unsigned int HAL_DTLSSession_read(DTLSContext *context,
                                  unsigned char   *p_data,
                                  unsigned int    *p_datalen,
                                  unsigned int     timeout)
{
    int len = 0;
    unsigned int err_code = DTLS_READ_DATA_FAILED;
    dtls_session_t *p_dtls_session = (dtls_session_t *)context;

    if (NULL != p_dtls_session && NULL != p_data && p_datalen != NULL) {
        mbedtls_ssl_conf_read_timeout(&(p_dtls_session->conf), timeout);
        len = mbedtls_ssl_read(&p_dtls_session->context, p_data, *p_datalen);

        if (0  <  len) {
            *p_datalen = len;
            err_code = DTLS_SUCCESS;
            DTLS_TRC("mbedtls_ssl_read len %d bytes\r\n", len);
        } else {
            *p_datalen = 0;
            if (MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE == len) {
                err_code = DTLS_FATAL_ALERT_MESSAGE;
                DTLS_INFO("Recv peer fatal alert message\r\n");
            } else if (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY == len) {
                err_code = DTLS_PEER_CLOSE_NOTIFY;
                DTLS_INFO("The DTLS session was closed by peer\r\n");
            } else if (MBEDTLS_ERR_SSL_TIMEOUT == len) {
                err_code = DTLS_SUCCESS;
                DTLS_TRC("DTLS recv timeout\r\n");
            } else {
                DTLS_TRC("mbedtls_ssl_read error result (-0x%04x)\r\n", len);
            }
        }
    }
    return err_code;
}

unsigned int HAL_DTLSSession_free(DTLSContext *context)
{
    dtls_session_t *p_dtls_session = NULL;
    if (NULL != context) {
        p_dtls_session = (dtls_session_t *)context;
        return _DTLSSession_deinit(p_dtls_session);
    }

    return DTLS_INVALID_PARAM;
}
#endif

#endif
