/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCHED_CLUSTER_H
#define _LINUX_SCHED_CLUSTER_H

#include <linux/static_key.h>

#ifdef CONFIG_SCHED_CLUSTER
extern struct static_key_false sched_cluster_present;

static __always_inline bool sched_cluster_active(void)
{
	return static_branch_likely(&sched_cluster_present);
}
#else
static inline bool sched_cluster_active(void) { return false; }

#endif

#endif
