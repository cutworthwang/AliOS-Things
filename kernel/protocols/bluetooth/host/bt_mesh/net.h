/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define BT_MESH_NET_FLAG_KR       BIT(0)
#define BT_MESH_NET_FLAG_IVU      BIT(1)

#define BT_MESH_KR_NORMAL         0x00
#define BT_MESH_KR_PHASE_1        0x01
#define BT_MESH_KR_PHASE_2        0x02
#define BT_MESH_KR_PHASE_3        0x03

#define BT_MESH_IV_UPDATE(flags)   ((flags >> 1) & 0x01)
#define BT_MESH_KEY_REFRESH(flags) (flags & 0x01)

struct bt_mesh_app_key {
	u16_t net_idx;
	u16_t app_idx;
	bool  updated;
	struct bt_mesh_app_keys {
		u8_t id;
		u8_t val[16];
	} keys[2];
};

struct bt_mesh_subnet {
	u32_t beacon_sent;        /* Timestamp of last sent beacon */
	u8_t  beacons_last;       /* Number of beacons during last
				   * observation window
				   */
	u8_t  beacons_cur;        /* Number of beaconds observed during
				   * currently ongoing window.
				   */

	u8_t  beacon_cache[21];   /* Cached last authenticated beacon */

	u16_t net_idx;            /* NetKeyIndex */

	bool  kr_flag;            /* Key Refresh Flag */
	u8_t  kr_phase;           /* Key Refresh Phase */

	u8_t  node_id;            /* Node Identity State */
	u32_t node_id_start;      /* Node Identity started timestamp */

	u8_t  auth[8];            /* Beacon Authentication Value */

	struct bt_mesh_subnet_keys {
		u8_t net[16];       /* NetKey */
		u8_t nid;           /* NID */
		u8_t enc[16];       /* EncKey */
		u8_t net_id[8];     /* Network ID */
#if defined(CONFIG_BT_MESH_GATT_PROXY)
		u8_t identity[16];  /* IdentityKey */
#endif
		u8_t privacy[16];   /* PrivacyKey */
		u8_t beacon[16];    /* BeaconKey */
	} keys[2];
};

struct bt_mesh_rpl {
	u16_t src;
	bool  old_iv;
	u32_t seq;
};

#if defined(CONFIG_BT_MESH_FRIEND)
#define FRIEND_SEG_RX CONFIG_BT_MESH_FRIEND_SEG_RX
#define FRIEND_SUB_LIST_SIZE CONFIG_BT_MESH_FRIEND_SUB_LIST_SIZE
#else
#define FRIEND_SEG_RX 0
#define FRIEND_SUB_LIST_SIZE 0
#endif

struct bt_mesh_friend {
	u16_t lpn;
	u8_t  recv_delay;
	u8_t  fsn:1,
	      send_last:1,
	      pending_req:1,
	      sec_update:1,
	      pending_buf:1,
	      valid:1,
	      established:1;
	s32_t poll_to;
	u16_t lpn_counter;
	u16_t counter;

	u16_t net_idx;

	u16_t sub_list[FRIEND_SUB_LIST_SIZE];

	struct k_delayed_work timer;

	struct bt_mesh_friend_seg {
		sys_slist_t queue;
	} seg[FRIEND_SEG_RX];

	struct net_buf *last;

	sys_slist_t queue;
	u32_t queue_size;

	/* Friend Clear Procedure */
	struct {
		u32_t start;                  /* Clear Procedure start */
		u16_t frnd;                   /* Previous Friend's address */
		u16_t repeat_sec;             /* Repeat timeout in seconds */
		struct k_delayed_work timer;  /* Repeat timer */
	} clear;
};

#if defined(CONFIG_BT_MESH_LOW_POWER)
#define LPN_GROUPS CONFIG_BT_MESH_LOW_POWER
#else
#define LPN_GROUPS 0
#endif

/* Low Power Node state */
struct bt_mesh_lpn {
	enum __packed {
		BT_MESH_LPN_DISABLED,     /* LPN feature is disabled */
		BT_MESH_LPN_CLEAR,        /* Clear in progress */
		BT_MESH_LPN_TIMER,        /* Waiting for auto timer expiry */
		BT_MESH_LPN_ENABLED,      /* LPN enabled, but no Friend */
		BT_MESH_LPN_REQ_WAIT,     /* Wait before scanning for offers */
		BT_MESH_LPN_WAIT_OFFER,   /* Friend Req sent */
		BT_MESH_LPN_ESTABLISHED,  /* Friendship established */
		BT_MESH_LPN_RECV_DELAY,   /* Poll sent, waiting ReceiveDelay */
		BT_MESH_LPN_WAIT_UPDATE,  /* Waiting for Update or message */
	} state;

	/* Transaction Number (used for subscription list) */
	u8_t xact_next;
	u8_t xact_pending;
	u8_t sent_req;

	/* Address of our Friend when we're a LPN. Unassigned if we don't
	 * have a friend yet.
	 */
	u16_t frnd;

	/* Value from the friend offer */
	u8_t  recv_win;

	u8_t  req_attempts;     /* Number of Request attempts */

	s32_t poll_timeout;

	u8_t  groups_changed:1, /* Friend Subscription List needs updating */
	      pending_poll:1,   /* Poll to be sent after subscription */
	      disable:1,        /* Disable LPN after clearing */
	      fsn:1,            /* Friend Sequence Number */
	      established:1,    /* Friendship established */
	      clear_success:1;  /* Friend Clear Confirm received */

	/* Friend Queue Size */
	u8_t  queue_size;

	/* LPNCounter */
	u16_t counter;

	/* Previous Friend of this LPN */
	u16_t old_friend;

	/* Duration reported for last advertising packet */
	u16_t adv_duration;

	/* Next LPN related action timer */
	struct k_delayed_work timer;

	/* Subscribed groups */
	u16_t groups[LPN_GROUPS];

	/* Bit fields for tracking which groups the Friend knows about */
	ATOMIC_DEFINE(added, LPN_GROUPS);
	ATOMIC_DEFINE(pending, LPN_GROUPS);
	ATOMIC_DEFINE(to_remove, LPN_GROUPS);
};

struct bt_mesh_net {
	u32_t iv_index;          /* Current IV Index */
	u32_t seq:24,            /* Next outgoing sequence number */
	      iv_update:1,       /* 1 if IV Update in Progress */
	      ivu_initiator:1,   /* IV Update initiated by us */
	      ivu_test:1,        /* IV Update test mode */
	      pending_update:1,  /* Update blocked by SDU in progress */
	      valid:1;           /* 0 if unused */

	s64_t last_update;       /* Time since last IV Update change */

	/* Local network interface */
	struct k_work local_work;
	sys_slist_t local_queue;

#if defined(CONFIG_BT_MESH_FRIEND)
	/* Friend state, unique for each LPN that we're Friends for */
	struct bt_mesh_friend frnd[CONFIG_BT_MESH_FRIEND_LPN_COUNT];
#endif

#if defined(CONFIG_BT_MESH_LOW_POWER)
	struct bt_mesh_lpn lpn;  /* Low Power Node state */
#endif

	/* Timer to transition IV Update in Progress state */
	struct k_delayed_work ivu_complete;

	u8_t dev_key[16];

	struct bt_mesh_app_key app_keys[CONFIG_BT_MESH_APP_KEY_COUNT];

	struct bt_mesh_subnet sub[CONFIG_BT_MESH_SUBNET_COUNT];

	struct bt_mesh_rpl rpl[CONFIG_BT_MESH_CRPL];
};

/* Network interface */
enum bt_mesh_net_if {
	BT_MESH_NET_IF_ADV,
	BT_MESH_NET_IF_LOCAL,
	BT_MESH_NET_IF_PROXY,
	BT_MESH_NET_IF_PROXY_CFG,
};

/* Decoding context for Network/Transport data */
struct bt_mesh_net_rx {
	struct bt_mesh_subnet *sub;
	struct bt_mesh_msg_ctx ctx;
	u32_t  seq;            /* Sequence Number */
	u16_t  dst;            /* Destination address */
	u8_t   old_iv:1,       /* iv_index - 1 was used */
	       new_key:1,      /* Data was encrypted with updated key */
	       friend_cred:1,  /* Data was encrypted with friend cred */
	       ctl:1,          /* Network Control */
	       net_if:2,       /* Network interface */
	       local_match:1,  /* Matched a local element */
	       friend_match:1; /* Matched an LPN we're friends for */
	s8_t   rssi;
};

/* Encoding context for Network/Transport data */
struct bt_mesh_net_tx {
	struct bt_mesh_subnet *sub;
	struct bt_mesh_msg_ctx *ctx;
	u16_t src;
	u8_t  xmit;
	u8_t  friend_cred:1,
	      aszmic:1,
	      aid:6;
};

extern struct bt_mesh_net bt_mesh;

#define BT_MESH_NET_IVI_TX (bt_mesh.iv_index - bt_mesh.iv_update)
#define BT_MESH_NET_IVI_RX(rx) (bt_mesh.iv_index - (rx)->old_iv)

#define BT_MESH_NET_HDR_LEN 9

int bt_mesh_net_keys_create(struct bt_mesh_subnet_keys *keys,
			    const u8_t key[16]);

int bt_mesh_net_create(u16_t idx, u8_t flags, const u8_t key[16],
		       u32_t iv_index);

u8_t bt_mesh_net_flags(struct bt_mesh_subnet *sub);

bool bt_mesh_kr_update(struct bt_mesh_subnet *sub, u8_t new_kr, bool new_key);

void bt_mesh_net_revoke_keys(struct bt_mesh_subnet *sub);

int bt_mesh_net_beacon_update(struct bt_mesh_subnet *sub);

void bt_mesh_rpl_reset(void);

bool bt_mesh_net_iv_update(u32_t iv_index, bool iv_update);

void bt_mesh_net_sec_update(struct bt_mesh_subnet *sub);

struct bt_mesh_subnet *bt_mesh_subnet_get(u16_t net_idx);

struct bt_mesh_subnet *bt_mesh_subnet_find(const u8_t net_id[8], u8_t flags,
					   u32_t iv_index, const u8_t auth[8],
					   bool *new_key);

int bt_mesh_net_encode(struct bt_mesh_net_tx *tx, struct net_buf_simple *buf,
		       bool proxy);

int bt_mesh_net_send(struct bt_mesh_net_tx *tx, struct net_buf *buf,
		     const struct bt_mesh_send_cb *cb, void *cb_data);

int bt_mesh_net_resend(struct bt_mesh_subnet *sub, struct net_buf *buf,
		       bool new_key, const struct bt_mesh_send_cb *cb,
		       void *cb_data);

int bt_mesh_net_decode(struct net_buf_simple *data, enum bt_mesh_net_if net_if,
		       struct bt_mesh_net_rx *rx, struct net_buf_simple *buf);

void bt_mesh_net_recv(struct net_buf_simple *data, s8_t rssi,
		      enum bt_mesh_net_if net_if);

void bt_mesh_net_init(void);

/* Friendship Credential Management */
struct friend_cred {
	u16_t net_idx;
	u16_t addr;

	u16_t lpn_counter;
	u16_t frnd_counter;

	struct {
		u8_t nid;         /* NID */
		u8_t enc[16];     /* EncKey */
		u8_t privacy[16]; /* PrivacyKey */
	} cred[2];
};

int friend_cred_get(struct bt_mesh_subnet *sub, u16_t addr, u8_t *nid,
			    const u8_t **enc, const u8_t **priv);
int friend_cred_set(struct friend_cred *cred, u8_t idx, const u8_t net_key[16]);
void friend_cred_refresh(u16_t net_idx);
int friend_cred_update(struct bt_mesh_subnet *sub);
struct friend_cred *friend_cred_create(struct bt_mesh_subnet *sub, u16_t addr,
				       u16_t lpn_counter, u16_t frnd_counter);
void friend_cred_clear(struct friend_cred *cred);
int friend_cred_del(u16_t net_idx, u16_t addr);
