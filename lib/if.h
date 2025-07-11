// SPDX-License-Identifier: GPL-2.0-or-later
/* Interface related header.
 * Copyright (C) 1997, 98, 99 Kunihiro Ishiguro
 */

#ifndef _ZEBRA_IF_H
#define _ZEBRA_IF_H

#include "zebra.h"
#include "linklist.h"
#include "memory.h"
#include "qobj.h"
#include "hook.h"
#include "admin_group.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_MTYPE(CONNECTED_LABEL);
DECLARE_MTYPE(LIB_ALTNAME);

/* Interface link-layer type, if known. Derived from:
 *
 * net/if_arp.h on various platforms - Linux especially.
 * http://www.iana.org/assignments/arp-parameters/arp-parameters.xhtml
 *
 * Some of the more obviously defunct technologies left out.
 */
enum zebra_link_type {
	ZEBRA_LLT_UNKNOWN = 0,
	ZEBRA_LLT_ETHER,
	ZEBRA_LLT_EETHER,
	ZEBRA_LLT_AX25,
	ZEBRA_LLT_PRONET,
	ZEBRA_LLT_IEEE802,
	ZEBRA_LLT_ARCNET,
	ZEBRA_LLT_APPLETLK,
	ZEBRA_LLT_DLCI,
	ZEBRA_LLT_ATM,
	ZEBRA_LLT_METRICOM,
	ZEBRA_LLT_IEEE1394,
	ZEBRA_LLT_EUI64,
	ZEBRA_LLT_INFINIBAND,
	ZEBRA_LLT_SLIP,
	ZEBRA_LLT_CSLIP,
	ZEBRA_LLT_SLIP6,
	ZEBRA_LLT_CSLIP6,
	ZEBRA_LLT_RSRVD,
	ZEBRA_LLT_ADAPT,
	ZEBRA_LLT_ROSE,
	ZEBRA_LLT_X25,
	ZEBRA_LLT_PPP,
	ZEBRA_LLT_CHDLC,
	ZEBRA_LLT_LAPB,
	ZEBRA_LLT_RAWHDLC,
	ZEBRA_LLT_IPIP,
	ZEBRA_LLT_IPIP6,
	ZEBRA_LLT_FRAD,
	ZEBRA_LLT_SKIP,
	ZEBRA_LLT_LOOPBACK,
	ZEBRA_LLT_LOCALTLK,
	ZEBRA_LLT_FDDI,
	ZEBRA_LLT_SIT,
	ZEBRA_LLT_IPDDP,
	ZEBRA_LLT_IPGRE,
	ZEBRA_LLT_IP6GRE,
	ZEBRA_LLT_PIMREG,
	ZEBRA_LLT_HIPPI,
	ZEBRA_LLT_ECONET,
	ZEBRA_LLT_IRDA,
	ZEBRA_LLT_FCPP,
	ZEBRA_LLT_FCAL,
	ZEBRA_LLT_FCPL,
	ZEBRA_LLT_FCFABRIC,
	ZEBRA_LLT_IEEE802_TR,
	ZEBRA_LLT_IEEE80211,
	ZEBRA_LLT_IEEE80211_RADIOTAP,
	ZEBRA_LLT_IEEE802154,
	ZEBRA_LLT_IEEE802154_PHY,
};

/*
  Interface name length.

   Linux define value in /usr/include/linux/if.h.
   #define IFNAMSIZ        16

   FreeBSD define value in /usr/include/net/if.h.
   #define IFNAMSIZ        16
*/
#define INTERFACE_HWADDR_MAX  20

typedef signed int ifindex_t;

#ifdef HAVE_PROC_NET_DEV
struct if_stats {
	unsigned long rx_packets;   /* total packets received       */
	unsigned long tx_packets;   /* total packets transmitted    */
	unsigned long rx_bytes;     /* total bytes received         */
	unsigned long tx_bytes;     /* total bytes transmitted      */
	unsigned long rx_errors;    /* bad packets received         */
	unsigned long tx_errors;    /* packet transmit problems     */
	unsigned long rx_dropped;   /* no space in linux buffers    */
	unsigned long tx_dropped;   /* no space available in linux  */
	unsigned long rx_multicast; /* multicast packets received   */
	unsigned long rx_compressed;
	unsigned long tx_compressed;
	unsigned long collisions;

	/* detailed rx_errors: */
	unsigned long rx_length_errors;
	unsigned long rx_over_errors;   /* receiver ring buff overflow  */
	unsigned long rx_crc_errors;    /* recved pkt with crc error    */
	unsigned long rx_frame_errors;  /* recv'd frame alignment error */
	unsigned long rx_fifo_errors;   /* recv'r fifo overrun          */
	unsigned long rx_missed_errors; /* receiver missed packet     */
	/* detailed tx_errors */
	unsigned long tx_aborted_errors;
	unsigned long tx_carrier_errors;
	unsigned long tx_fifo_errors;
	unsigned long tx_heartbeat_errors;
	unsigned long tx_window_errors;
};
#endif /* HAVE_PROC_NET_DEV */

/* Here are "non-official" architectural constants. */
#define TE_EXT_MASK             0x00FFFFFF
#define TE_EXT_ANORMAL          0x80000000
#define LOSS_PRECISION          0.000003
/* TE_MEGA_BIT and TE_BYTE are utilized to convert TE bandwidth */
#define TE_MEGA_BIT             1000000
#define TE_BYTE                 8
/* Default TE bandwidth when no value in config.
 * The value is in Mbps (will be multiplied by TE_BYTE)
 */
#define DEFAULT_BANDWIDTH 10
#define MAX_CLASS_TYPE          8
#define MAX_PKT_LOSS            50.331642

enum affinity_mode {
	/* RFC7308 Extended Administrative group */
	AFFINITY_MODE_EXTENDED = 0,
	/* RFC3630/RFC5305/RFC5329 Administrative group */
	AFFINITY_MODE_STANDARD = 1,
	/* Standard and Extended Administrative group */
	AFFINITY_MODE_BOTH = 2,
};

/*
 * Link Parameters Status:
 *  equal to 0: unset
 *  different from 0: set
 */
#define LP_UNSET                0x0000
#define LP_TE_METRIC            0x0001
#define LP_MAX_BW               0x0002
#define LP_MAX_RSV_BW           0x0004
#define LP_UNRSV_BW             0x0008
#define LP_ADM_GRP              0x0010
#define LP_RMT_AS               0x0020
#define LP_DELAY                0x0040
#define LP_MM_DELAY             0x0080
#define LP_DELAY_VAR            0x0100
#define LP_PKT_LOSS             0x0200
#define LP_RES_BW               0x0400
#define LP_AVA_BW               0x0800
#define LP_USE_BW               0x1000
#define LP_EXTEND_ADM_GRP 0x2000

#define IS_PARAM_UNSET(lp, st) !(lp->lp_status & st)
#define IS_PARAM_SET(lp, st) (lp->lp_status & st)
#define IS_LINK_PARAMS_SET(lp) (lp->lp_status != LP_UNSET)

#define SET_PARAM(lp, st) (lp->lp_status) |= (st)
#define UNSET_PARAM(lp, st) (lp->lp_status) &= ~(st)
#define RESET_LINK_PARAM(lp) (lp->lp_status = LP_UNSET)

/* Link Parameters for Traffic Engineering
 * Do not forget to update if_link_params_copy()
 * and if_link_params_cmp() when updating the structure
 */
struct if_link_params {
	uint32_t lp_status; /* Status of Link Parameters: */
	uint32_t te_metric; /* Traffic Engineering metric */
	float default_bw;
	float max_bw;			/* Maximum Bandwidth */
	float max_rsv_bw;		/* Maximum Reservable Bandwidth */
	float unrsv_bw[MAX_CLASS_TYPE]; /* Unreserved Bandwidth per Class Type
					   (8) */
	uint32_t admin_grp; /* RFC5305/RFC5329 Administrative group */
	struct admin_group ext_admin_grp; /* RFC7308 Extended Admin group */
	uint32_t rmt_as;		/* Remote AS number */
	struct in_addr rmt_ip;		/* Remote IP address */
	uint32_t av_delay;		/* Link Average Delay */
	uint32_t min_delay;		/* Link Min Delay */
	uint32_t max_delay;		/* Link Max Delay */
	uint32_t delay_var;		/* Link Delay Variation */
	uint32_t pkt_loss;		/* Link Packet Loss */
	float res_bw;			/* Residual Bandwidth */
	float ava_bw;			/* Available Bandwidth */
	float use_bw;			/* Utilized Bandwidth */
};

#define INTERFACE_LINK_PARAMS_SIZE   sizeof(struct if_link_params)
#define HAS_LINK_PARAMS(ifp)  ((ifp)->link_params != NULL)

PREDECL_DLIST(if_connected);

/* Length of interface altname.*/
#define IF_ALTNAMESIZE 128

struct altname {
	RB_ENTRY(altname) entry;

	char name[IF_ALTNAMESIZE];
};

extern int altname_cmp_func(const struct altname *alt1, const struct altname *alt2);

RB_HEAD(altnames_head, altname);
RB_PROTOTYPE(altnames_head, altname, entry, altname_cmp_func)
DECLARE_QOBJ_TYPE(altname);

/* Interface structure */
struct interface {
	RB_ENTRY(interface) name_entry, index_entry;

	/* Interface name.  This should probably never be changed after the
	   interface is created, because the configuration info for this
	   interface
	   is associated with this structure.  For that reason, the interface
	   should also never be deleted (to avoid losing configuration info).
	   To delete, just set ifindex to IFINDEX_INTERNAL to indicate that the
	   interface does not exist in the kernel.
	 */
	char name[IFNAMSIZ];

	/* Interface index (should be IFINDEX_INTERNAL for non-kernel or
	   deleted interfaces).
	   WARNING: the ifindex needs to be changed using the if_set_index()
	   function. Failure to respect this will cause corruption in the data
	   structure used to store the interfaces and if_lookup_by_index() will
	   not work as expected.
	 */
	ifindex_t ifindex;
	ifindex_t oldifindex;

	/*
	 * ifindex of parent interface, if any
	 */
	ifindex_t link_ifindex;
#define IFINDEX_INTERNAL	0

	/* Zebra internal interface status */
	uint8_t status;
#define ZEBRA_INTERFACE_ACTIVE     (1 << 0)
#define ZEBRA_INTERFACE_SUB        (1 << 1)
#define ZEBRA_INTERFACE_LINKDETECTION (1 << 2)
#define ZEBRA_INTERFACE_VRF_LOOPBACK (1 << 3)
#define ZEBRA_INTERFACE_DUMMY (1 << 4)

	/* Interface flags. */
	uint64_t flags;

	/* Interface metric */
	uint32_t metric;

	/* Interface Speed in Mb/s */
	uint32_t speed;

	/* TX queue len */
	uint32_t txqlen;

	/* Interface MTU. */
	unsigned int mtu; /* IPv4 MTU */
	unsigned int
		mtu6; /* IPv6 MTU - probably, but not necessarily same as mtu
		       */

	/* Link-layer information and hardware address */
	enum zebra_link_type ll_type;
	uint8_t hw_addr[INTERFACE_HWADDR_MAX];
	int hw_addr_len;

	/* interface bandwidth, kbits */
	unsigned int bandwidth;

	/* Link parameters for Traffic Engineering */
	struct if_link_params *link_params;

	/* description of the interface. */
	char *desc;

	/* Altnames for this interface */
	struct altnames_head altnames;

	/* Connected address list. */
	struct if_connected_head connected[1];

	/* Neighbor connected address list. */
	struct list *nbr_connected;

	/* Daemon specific interface data pointer. */
	void *info;

	char ptm_enable; /* Should we look at ptm_status ? */
	char ptm_status;

/* Statistics fileds. */
#ifdef HAVE_PROC_NET_DEV
	struct if_stats stats;
#endif /* HAVE_PROC_NET_DEV */
#ifdef HAVE_NET_RT_IFLIST
	struct if_data stats;
#endif /* HAVE_NET_RT_IFLIST */

	struct vrf *vrf;

	struct lyd_node *state;

	/*
	 * Has the end users entered `interface XXXX` from the cli in some
	 * fashion?
	 */
	bool configured;

	QOBJ_FIELDS;
};

extern int if_cmp_func(const struct interface *a, const struct interface *b);

RB_HEAD(if_name_head, interface);
RB_PROTOTYPE(if_name_head, interface, name_entry, if_cmp_func)
RB_HEAD(if_index_head, interface);
RB_PROTOTYPE(if_index_head, interface, index_entry, if_cmp_index_func)
DECLARE_QOBJ_TYPE(interface);

#define IFNAME_RB_INSERT(v, ifp)                                                      \
	({                                                                            \
		struct interface *_iz =                                               \
			RB_INSERT(if_name_head, &v->ifaces_by_name, (ifp));           \
		if (_iz)                                                              \
			flog_err(                                                     \
				EC_LIB_INTERFACE,                                     \
				"%s(%s): corruption detected -- interface with this " \
				"name exists already in VRF %s!",                     \
				__func__, (ifp)->name, (ifp)->vrf->name);             \
		_iz;                                                                  \
	})

#define IFNAME_RB_REMOVE(v, ifp)                                                      \
	({                                                                            \
		struct interface *_iz =                                               \
			RB_REMOVE(if_name_head, &v->ifaces_by_name, (ifp));           \
		if (_iz == NULL)                                                      \
			flog_err(                                                     \
				EC_LIB_INTERFACE,                                     \
				"%s(%s): corruption detected -- interface with this " \
				"name doesn't exist in VRF %s!",                      \
				__func__, (ifp)->name, (ifp)->vrf->name);             \
		_iz;                                                                  \
	})


#define IFINDEX_RB_INSERT(v, ifp)                                                     \
	({                                                                            \
		struct interface *_iz =                                               \
			RB_INSERT(if_index_head, &v->ifaces_by_index, (ifp));         \
		if (_iz)                                                              \
			flog_err(                                                     \
				EC_LIB_INTERFACE,                                     \
				"%s(%u): corruption detected -- interface with this " \
				"ifindex exists already in VRF %s!",                  \
				__func__, (ifp)->ifindex, (ifp)->vrf->name);          \
		_iz;                                                                  \
	})

#define IFINDEX_RB_REMOVE(v, ifp)                                                     \
	({                                                                            \
		struct interface *_iz =                                               \
			RB_REMOVE(if_index_head, &v->ifaces_by_index, (ifp));         \
		if (_iz == NULL)                                                      \
			flog_err(                                                     \
				EC_LIB_INTERFACE,                                     \
				"%s(%u): corruption detected -- interface with this " \
				"ifindex doesn't exist in VRF %s!",                   \
				__func__, (ifp)->ifindex, (ifp)->vrf->name);          \
		_iz;                                                                  \
	})

#define FOR_ALL_INTERFACES(vrf, ifp)                                           \
	if (vrf)                                                               \
		RB_FOREACH (ifp, if_name_head, &vrf->ifaces_by_name)

/* called from the library code whenever interfaces are created/deleted
 * note: interfaces may not be fully realized at that point; also they
 * may not exist in the system (ifindex = IFINDEX_INTERNAL)
 *
 * priority values are important here, daemons should be at 0 while modules
 * can use 1000+ so they run after the daemon has initialised daemon-specific
 * interface data
 */
DECLARE_HOOK(if_add, (struct interface *ifp), (ifp));
DECLARE_KOOH(if_del, (struct interface *ifp), (ifp));

/* called (in daemons) when ZAPI tells us the interface actually exists
 * (ifindex != IFINDEX_INTERNAL)
 *
 * WARNING: these 2 hooks NEVER CALLED inside zebra!
 */
DECLARE_HOOK(if_real, (struct interface *ifp), (ifp));
DECLARE_KOOH(if_unreal, (struct interface *ifp), (ifp));

/* called (in daemons) on state changes on interfaces.  Whether this is admin
 * state (= pure config) or carrier state (= hardware link plugged) depends on
 * zebra's "link-detect" configuration.  By default, it's carrier state, so
 * this won't happen until the interface actually has a link.
 *
 * WARNING: these 2 hooks NEVER CALLED inside zebra!
 */
DECLARE_HOOK(if_up, (struct interface *ifp), (ifp));
DECLARE_KOOH(if_down, (struct interface *ifp), (ifp));


#define METRIC_MAX (~0)

/* Connected address structure. */
struct connected {
	struct if_connected_item item;

	/* Attached interface. */
	struct interface *ifp;

	/* Flags for configuration. */
	uint8_t conf;
#define ZEBRA_IFC_REAL         (1 << 0)
#define ZEBRA_IFC_CONFIGURED   (1 << 1)
#define ZEBRA_IFC_QUEUED       (1 << 2)
#define ZEBRA_IFC_DOWN         (1 << 3)
	/*
	   The ZEBRA_IFC_REAL flag should be set if and only if this address
	   exists in the kernel and is actually usable. (A case where it exists
	   but is not yet usable would be IPv6 with DAD)
	   The ZEBRA_IFC_CONFIGURED flag should be set if and only if this
	   address was configured by the user from inside frr.
	   The ZEBRA_IFC_QUEUED flag should be set if and only if the address
	   exists in the kernel. It may and should be set although the
	   address might not be usable yet. (compare with ZEBRA_IFC_REAL)
	   The ZEBRA_IFC_DOWN flag is used to record that an address is
	   present, but down/unavailable.
	 */

	/* Flags for connected address. */
	uint8_t flags;
#define ZEBRA_IFA_SECONDARY    (1 << 0)
#define ZEBRA_IFA_PEER         (1 << 1)
#define ZEBRA_IFA_UNNUMBERED   (1 << 2)
#define ZEBRA_IFA_NOPREFIXROUTE (1 << 3)

	/* N.B. the ZEBRA_IFA_PEER flag should be set if and only if
	   a peer address has been configured.  If this flag is set,
	   the destination field must contain the peer address.
	 */

	/* Address of connected network. */
	struct prefix *address;

	/* Peer address, if ZEBRA_IFA_PEER is set, otherwise NULL */
	struct prefix *destination;

	/* Label for Linux 2.2.X and upper. */
	char *label;

	/*
	 * Used for setting the connected route's cost. If the metric
	 * here is set to METRIC_MAX the connected route falls back to
	 * "struct interface"
	 */
	uint32_t metric;
};

DECLARE_DLIST(if_connected, struct connected, item);

/* Nbr Connected address structure. */
struct nbr_connected {
	/* Attached interface. */
	struct interface *ifp;

	/* Address of connected network. */
	struct prefix *address;
};

/* Does the destination field contain a peer address? */
#define CONNECTED_PEER(C) CHECK_FLAG((C)->flags, ZEBRA_IFA_PEER)

/* Prefix to insert into the RIB */
#define CONNECTED_PREFIX(C)                                                    \
	(CONNECTED_PEER(C) ? (C)->destination : (C)->address)

/* Identifying address.  We guess that if there's a peer address, but the
   local address is in the same prefix, then the local address may be unique. */
#define CONNECTED_ID(C)                                                        \
	((CONNECTED_PEER(C) && !prefix_match((C)->destination, (C)->address))  \
		 ? (C)->destination                                            \
		 : (C)->address)

/* There are some interface flags which are only supported by some
   operating system. */

#ifndef IFF_NOTRAILERS
#define IFF_NOTRAILERS 0x0
#endif /* IFF_NOTRAILERS */
#ifndef IFF_OACTIVE
#define IFF_OACTIVE 0x0
#endif /* IFF_OACTIVE */
#ifndef IFF_SIMPLEX
#define IFF_SIMPLEX 0x0
#endif /* IFF_SIMPLEX */
#ifndef IFF_LINK0
#define IFF_LINK0 0x0
#endif /* IFF_LINK0 */
#ifndef IFF_LINK1
#define IFF_LINK1 0x0
#endif /* IFF_LINK1 */
#ifndef IFF_LINK2
#define IFF_LINK2 0x0
#endif /* IFF_LINK2 */
#ifndef IFF_NOXMIT
#define IFF_NOXMIT 0x0
#endif /* IFF_NOXMIT */
#ifndef IFF_NORTEXCH
#define IFF_NORTEXCH 0x0
#endif /* IFF_NORTEXCH */
#ifndef IFF_IPV4
#define IFF_IPV4 0x0
#endif /* IFF_IPV4 */
#ifndef IFF_IPV6
#define IFF_IPV6 0x0
#endif /* IFF_IPV6 */
#ifndef IFF_VIRTUAL
#define IFF_VIRTUAL 0x0
#endif /* IFF_VIRTUAL */

/* Prototypes. */
extern int if_cmp_name_func(const char *p1, const char *p2);

/*
 * Passing in VRF_UNKNOWN is a valid thing to do, unless we
 * are creating a new interface.
 *
 * This is useful for vrf route-leaking.  So more than anything
 * else think before you use VRF_UNKNOWN
 */
extern void if_update_to_new_vrf(struct interface *ifp, vrf_id_t vrf_id);

extern struct interface *if_lookup_by_index(ifindex_t ifindex, vrf_id_t vrf_id);
extern struct interface *if_vrf_lookup_by_index_next(ifindex_t ifindex,
						     vrf_id_t vrf_id);
extern struct interface *if_lookup_address_local(const void *matchaddr,
						 int family, vrf_id_t vrf_id);
extern struct connected *if_lookup_address(const void *matchaddr, int family,
					   vrf_id_t vrf_id);
extern struct interface *if_lookup_prefix(const struct prefix *prefix,
					  vrf_id_t vrf_id);
size_t if_lookup_by_hwaddr(const uint8_t *hw_addr, size_t addrsz,
			   struct interface ***result, vrf_id_t vrf_id);

static inline bool if_address_is_local(const void *matchaddr, int family,
				       vrf_id_t vrf_id)
{
	return if_lookup_address_local(matchaddr, family, vrf_id) != NULL;
}

struct vrf;
extern struct interface *if_lookup_by_name_vrf(const char *name, struct vrf *vrf);
extern struct interface *if_lookup_by_name(const char *ifname, vrf_id_t vrf_id);
extern struct interface *if_get_vrf_loopback(vrf_id_t vrf_id);
extern struct interface *if_get_by_name(const char *ifname, vrf_id_t vrf_id,
					const char *vrf_name);

/* Sets the index and adds to index list */
extern int if_set_index(struct interface *ifp, ifindex_t ifindex);

/* Delete the interface, but do not free the structure, and leave it in the
   interface list.  It is often advisable to leave the pseudo interface
   structure because there may be configuration information attached. */
extern void if_delete_retain(struct interface *ifp);

/* Delete and free the interface structure: calls if_delete_retain and then
   deletes it from the interface list and frees the structure. */
extern void if_delete(struct interface **ifp);

extern int if_is_up(const struct interface *ifp);
extern int if_is_running(const struct interface *ifp);
extern int if_is_operative(const struct interface *ifp);
extern int if_is_no_ptm_operative(const struct interface *ifp);
extern int if_is_loopback_exact(const struct interface *ifp);
extern int if_is_vrf(const struct interface *ifp);
extern bool if_is_loopback(const struct interface *ifp);
extern int if_is_broadcast(const struct interface *ifp);
extern int if_is_pointopoint(const struct interface *ifp);
extern int if_is_multicast(const struct interface *ifp);
extern void if_terminate(struct vrf *vrf);
extern void if_dump_all(void);
extern const char *if_flag_dump(unsigned long flags);
extern const char *if_link_type_str(enum zebra_link_type zlt);

/* Please use ifindex2ifname instead of if_indextoname where possible;
   ifindex2ifname uses internal interface info, whereas if_indextoname must
   make a system call. */
extern const char *ifindex2ifname(ifindex_t ifindex, vrf_id_t vrf_id);

/* Please use ifname2ifindex instead of if_nametoindex where possible;
   ifname2ifindex uses internal interface info, whereas if_nametoindex must
   make a system call. */
extern ifindex_t ifname2ifindex(const char *ifname, vrf_id_t vrf_id);

/* Connected address functions. */
extern struct connected *connected_new(void);
extern void connected_free(struct connected **connected);
extern struct connected *connected_add_by_prefix(struct interface *ifp,
						 struct prefix *p,
						 struct prefix *dest);
extern struct connected *connected_delete_by_prefix(struct interface *ifp,
						    struct prefix *p);
extern struct connected *connected_lookup_prefix(struct interface *ifp,
						 const struct prefix *p);
extern struct connected *connected_lookup_prefix_exact(struct interface *ifp,
						       const struct prefix *p);
extern unsigned int connected_count_by_family(struct interface *ifp, int family);
extern struct nbr_connected *nbr_connected_new(void);
extern void nbr_connected_free(struct nbr_connected *connected);
struct nbr_connected *nbr_connected_check(struct interface *ifp,
					  struct prefix *p);
struct connected *connected_get_linklocal(struct interface *ifp);

/* link parameters */
bool if_link_params_cmp(struct if_link_params *iflp1,
			struct if_link_params *iflp2);
void if_link_params_copy(struct if_link_params *dst,
			 struct if_link_params *src);
struct if_link_params *if_link_params_get(struct interface *ifp);
struct if_link_params *if_link_params_enable(struct interface *ifp);
struct if_link_params *if_link_params_init(struct interface *ifp);
void if_link_params_free(struct interface *ifp);

/* Northbound. */
struct vty;
extern void if_vty_config_start(struct vty *vty, struct interface *ifp);
extern void if_vty_config_end(struct vty *vty);
extern void if_cmd_init(int (*config_write)(struct vty *));
extern void if_cmd_init_default(void);

extern void if_new_via_zapi(struct interface *ifp);
extern void if_up_via_zapi(struct interface *ifp);
extern void if_down_via_zapi(struct interface *ifp);
extern void if_destroy_via_zapi(struct interface *ifp);

extern void if_update_state(struct interface *ifp);
extern void if_update_state_metric(struct interface *ifp, uint32_t metric);
extern void if_update_state_mtu(struct interface *ifp, uint mtu);
extern void if_update_state_mtu6(struct interface *ifp, uint mtu);
extern void if_update_state_hw_addr(struct interface *ifp, const uint8_t *hw_addr, uint len);
extern void if_update_state_speed(struct interface *ifp, uint32_t speed);

extern bool if_notify_oper_changes;
extern const struct frr_yang_module_info frr_interface_info;
extern const struct frr_yang_module_info frr_interface_cli_info;

#ifdef __cplusplus
}
#endif

#endif /* _ZEBRA_IF_H */
