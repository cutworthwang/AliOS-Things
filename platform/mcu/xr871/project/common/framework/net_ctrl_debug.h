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

#ifndef _NET_CTRL_DEBUG_H_
#define _NET_CTRL_DEBUG_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NET_DBG_ON      0
#define NET_INF_ON      1
#define NET_WRN_ON      1
#define NET_ERR_ON      1
#define NET_ABORT_ON    0

#define NET_SYSLOG      printf
#define NET_ABORT()     do { } while (0)

#define NET_LOG(flags, fmt, arg...) \
    do {                            \
        if (flags)                  \
            NET_SYSLOG(fmt, ##arg); \
    } while (0)

#define NET_DBG(fmt, arg...)    \
    NET_LOG(NET_DBG_ON, "[net DBG] "fmt, ##arg)

#define NET_INF(fmt, arg...)    \
    NET_LOG(NET_INF_ON, "[net INF] "fmt, ##arg)

#define NET_WRN(fmt, arg...)    \
    NET_LOG(NET_WRN_ON, "[net WRN] "fmt, ##arg)

#define NET_ERR(fmt, arg...)                            \
    do {                                                \
        NET_LOG(NET_ERR_ON, "[net ERR] %s():%d, "fmt,   \
               __func__, __LINE__, ##arg);              \
        if (NET_ABORT_ON)                               \
            NET_ABORT();                                \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* _NET_CTRL_DEBUG_H_ */
