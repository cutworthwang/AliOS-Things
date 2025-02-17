/*
 * V4L2 clock service
 *
 * Copyright (C) 2012-2013, Guennadi Liakhovetski <g.liakhovetski@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * ATTENTION: This is a temporary API and it shall be replaced by the generic
 * clock API, when the latter becomes widely available.
 */

#ifndef MEDIA_V4L2_CLK_H
#define MEDIA_V4L2_CLK_H

#if 0
#include <linux/atomic.h>
#include <linux/list.h>
#include <linux/mutex.h>
#endif
#include "v4l2-osdep.h"


//struct module;
//struct device;

struct v4l2_clk {
	struct LIST_HEADER list;
	const struct v4l2_clk_ops *ops;
	const char *dev_id;
	const char *id;
	int enable;
	//struct mutex lock; /* Protect the enable count */
	_Mutex lock;
	atomic_t use_count;
	void *priv;
};

struct v4l2_clk_ops {
	//struct module	*owner;
	int		(*enable)(struct v4l2_clk *clk);
	void		(*disable)(struct v4l2_clk *clk);
	unsigned long	(*get_rate)(struct v4l2_clk *clk);
	int		(*set_rate)(struct v4l2_clk *clk, unsigned long);
};

struct v4l2_clk *v4l2_clk_register(const struct v4l2_clk_ops *ops,
				   const char *dev_name,
				   const char *name, void *priv);
void v4l2_clk_unregister(struct v4l2_clk *clk);
struct v4l2_clk *v4l2_clk_get(const char *id);
void v4l2_clk_put(struct v4l2_clk *clk);
int v4l2_clk_enable(struct v4l2_clk *clk);
void v4l2_clk_disable(struct v4l2_clk *clk);
unsigned long v4l2_clk_get_rate(struct v4l2_clk *clk);
int v4l2_clk_set_rate(struct v4l2_clk *clk, unsigned long rate);

#endif
