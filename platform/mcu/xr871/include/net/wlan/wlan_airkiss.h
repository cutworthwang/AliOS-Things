/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _NET_WLAN_WLAN_AIRKISS_H_
#define _NET_WLAN_WLAN_AIRKISS_H_

#if (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_APP_CORE))

#include <stdint.h>
#include "net/wlan/wlan_defs.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WLAN_AIRKISS_SUCCESS = 0,  /* success */
    WLAN_AIRKISS_FAIL    = -1, /* general error */
    WLAN_AIRKISS_TIMEOUT = -2, /* wait timeout */
    WLAN_AIRKISS_INVALID = -3, /* invalid argument */
} wlan_airkiss_status_t;

typedef struct wlan_airkiss_result {
    uint8_t ssid[WLAN_SSID_MAX_LEN];
    uint8_t ssid_len;
    uint8_t passphrase[WLAN_PASSPHRASE_MAX_LEN + 1]; /* ASCII string ending with '\0' */
    uint8_t random_num;
} wlan_airkiss_result_t;

#define WLAN_AIRKISS_KEY_LEN	16

int wlan_airkiss_set_key(const char *key, uint32_t len);

wlan_airkiss_status_t wlan_airkiss_start(struct netif *nif, uint32_t timeout_ms,
                                         wlan_airkiss_result_t *result);
int wlan_airkiss_stop(void);

int wlan_airkiss_ack_start(struct netif *nif, uint32_t random_num, uint32_t timeout_ms);
int wlan_airkiss_ack_stop(void);

/*
 * The wechat_public_id and devic_id should be global variables.
 * In this mode, the driver will be send online data by cycle
 */
int wlan_airkiss_online_cycle_ack_start(char *app_id, char *drv_id, uint32_t period_ms);
int wlan_airkiss_online_cycle_ack_stop(void);

/*
 * The wechat_public_id and devic_id should be global variables.
 * In this mode,  the drivers will be listen to server's request and send ack for server, then the driver will
 * be send online data by cycle
 */
int wlan_airkiss_online_dialog_mode_start(char *app_id, char *drv_id, uint32_t period_ms);
int wlan_airkiss_online_dialog_mode_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_APP_CORE)) */

#endif /* _NET_WLAN_WLAN_AIRKISS_H_ */
