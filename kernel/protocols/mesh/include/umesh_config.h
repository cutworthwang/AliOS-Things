/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_CONFIG_H
#define UMESH_CONFIG_H

#define MAX_NEIGHBORS_NUM 32

#define ATTACH_REQUEST_RETRY_TIMES 2
#define ATTACH_REQUEST_INTERVAL 1000

#ifdef CONFIG_AOS_MESH_LOWPOWER
#define SCHEDULE_SLOTS_SIZE 3
#define SCHEDULE_SLOT_INTERVAL 6000
#endif

#ifdef CONFIG_AOS_MESH_AUTH
#define AUTH_REQUEST_RETRY_TIMES 2
#define AUTH_RELAY_RETRY_TIMES 2

#define ID2_LISTEN_PORT 1234
#define ID2_SERVER_PORT 2345
#define ID2_SERVER_ADDR "192.168.16.188" // testbed sp server ip
#endif

#endif
