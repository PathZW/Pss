/*******************************************************************************
 * @file    net_device.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/4/21
 * @brief   Network device abstraction
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/4/21 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint +fie                     // Integer model for Enum flag
//lint -esym(?*, rtcfg_?*_?*)
//lint -emacro(717, netif_set_hostname)

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "Net_Dev"

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/err.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/timeouts.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/dns.h"
#include "lwip/inet.h"
#include "lwip/netif.h"

#include <stdio.h>      // sscanf()
#include <string.h>     // strncpy()
#include "defines.h"    // Used: ATPASTEx(), TO_STRING(), UNUSED_VAR()
#include "emb_queue.h"
#include "emb_rtcfg.h"
#include "net_user.h"
#include "net_device.h"
#include "netdev_eth.h"


/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup              Network
 * @{
 ******************************************************************************/

#ifndef NET_DEV_DFT
#define NET_DEV_DFT   NET_DEVICE(eth, 0, IPv4)  /*!< (C) 默认网络设备 */
#endif
#ifdef  __DOXYGEN__
#define NET_DEV_TBL   NET_DEVICE(eth, 1, IPv4)  /*!< (C) 其他网络设备 */ \
                      NET_DEVICE(eth, 2, IPv4)
#endif

#ifndef RTCFG_ITEM_eth_IPv4                     // 用于 NET_CFG_DEV_IPADDR() 宏
#define RTCFG_ITEM_eth_IPv4(drv)                /*!< (C)  以太网 IPv4 配置 */  \
        {  {"Net:ETH" TO_STRING(drv) "_DHCP",   0                           } \
         , {"Net:ETH" TO_STRING(drv) "_IP"  ,  "192.168.5.254"              } \
         , {"Net:ETH" TO_STRING(drv) "_Mask",  "255.255.255.0"              } \
         , {"Net:ETH" TO_STRING(drv) "_GW"  ,  "192.168.5.1"                } \
         , {"Net:ETH" TO_STRING(drv) "_DNS1",  "114.114.114.114"            } \
         , {"Net:ETH" TO_STRING(drv) "_DNS2",  "8.8.8.8"                    } \
        }
#endif//RTCFG_ITEM_eth_IPv4

#ifndef RTCFG_ITEM_eth_IPv6                     // 用于 NET_CFG_DEV_IPADDR() 宏
#define RTCFG_ITEM_eth_IPv6(drv)                /*!< (C)  以太网 IPv6 配置 */
#endif//RTCFG_ITEM_eth_IPv6

#ifndef RTCFG_ITEM_eth_PRIV                     // 用于 NET_DEV_ETH_INFO_DEFS() 宏
#define RTCFG_ITEM_eth_PRIV(drv)                /*!< (C) 以太网 私有 配置 */       \
        {  {"Net:ETH" TO_STRING(drv) "_MAC" ,  "1E-30-53-47-28-E" TO_STRING(drv)} \
         , {"Net:ETH" TO_STRING(drv) "_vLan",  -1                               } \
        }
#endif//RTCFG_ITEM_eth_PRIV

#ifndef RTCFG_ITEM_eth_MTU                      // 用于 NET_DEV_ETH_INFO_DEFS() 宏
#define RTCFG_ITEM_eth_MTU(drv)                 /*!< (C) 以太网 MTU 配置 */   \
        {   "Net:ETH" TO_STRING(drv) "_MTU" ,  1500 }
#endif//RTCFG_ITEM_eth_MTU


/*******************************************************************************
 * @}
 * @}
 * @addtogroup Net_Device
 * @{
 * @addtogroup              Private_Macros
 * @{
 ******************************************************************************/
/**
 * 网络设备定义, 用于 NET_DEV_DFT / NET_DEV_TBL
 *
 * @param[in]  type   eth: 以太网
 * @param[in]  drv    设备编号
 * @param[in]  ip46   IPv4: 仅使用IPv4; IPv6: 仅使用IPv6; IP46: 同时使用IPv4和IPv6;
 *
 * @note  必须定义 RTCFG_ITEM_##type##_IPv4(drv) 配置宏, 例如 RTCFG_ITEM_eth_IPv4(drv)
 * @note  必须定义 RTCFG_ITEM_##type##_IPv6(drv) 配置宏, 例如 RTCFG_ITEM_eth_IPv6(drv)
 * @note  必须定义 RTCFG_ITEM_##type##_PRIV(drv) 配置宏, 例如 RTCFG_ITEM_eth_PRIV(drv)
 * @note  必须定义 RTCFG_ITEM_##type##_MTU(drv)  配置宏, 例如 RTCFG_ITEM_eth_MTU(drv)
 */
#define NET_DEVICE(type, drv, ip46)        _NET_DEV_INFO(type, drv, ip46)
/**
 * @param[in]  opt    DEFS: 定义; REFC: 引用;
 */
#define NET_DEV_INFO(type, drv, ip46, opt)  ATPASTE4(NET_DEV_INFO_, type, _, opt)(drv, ip46)


/*----------------------------------------------------------------------------*/
#ifndef NET_DEV_INFO_eth_DEFS
/**
 * 以太网设备(netdev_info_t) "定义" 与 "引用"
 *
 * @param[in]  drv    设备编号
 * @param[in]  ip46   IPv4: 仅IPv4; IPv6: 仅IPv6; IP46: IPv4和IPv6;
 */
#define NET_DEV_INFO_eth_DEFS(drv, ip46)                                     \
  /*static  const netcfg_ipx_t*/   NET_CFG_DEV_IPADDR(eth, drv, ip46, DEFS); \
    static  const netcfg_eth_t    _NET_CFG_PRIV_NAME (eth, drv)              \
                                     = RTCFG_ITEM_eth_PRIV(drv) ;            \
  /*static        netdev_eth_t*/   NET_DEV_eth_STATE_DEFS (drv) ;            \
    static  struct  netif         _NET_DEV_NETIF_NAME(eth, drv) ;            \
  /*static*/const netdev_info_t   _NET_DEV_INFO_NAME (eth, drv) =            \
    {                                                                        \
        TO_STRING(ATPASTE2(e, drv)),                   /* ifname  */         \
        RTCFG_ITEM_eth_MTU(drv),                       /* cfgmtu  */         \
        NET_CFG_DEV_IPADDR(eth, drv, ip46, REFC),      /* cfgip4  */         \
                                                       /* cfgip6  */         \
        (const void *)&_NET_CFG_PRIV_NAME (eth, drv),  /* cfgpriv */         \
        (      void *)&_NET_DEV_NETIF_NAME(eth, drv),  /* netif   */         \
        (      void *)&_NET_DEV_STATE_NAME(eth, drv),  /* state   */         \
        (int  (*)(void *, void *))(NETDEV_INPKT),      /* inpkt   */         \
        (int  (*)(void *        ))(netdev_eth_init),   /* init    */         \
        (void (*)(void *        ))(NULL) ,             /* poll    */         \
        (int  (*)(void *        ))(netdev_eth_link),   /* link    */         \
        (unsigned                )(500)                /* invl    */         \
    }
#define NET_DEV_INFO_eth_REFC(drv, ip46)   _NET_DEV_INFO_NAME(eth, drv)
#endif//NET_DEV_INFO_eth_DEFS


#ifndef NET_CFG_DEV_IPADDR
/**
 * IPv4(netcfg_ip4_t) / IPv6(netcfg_ip6_t) 配置 "定义" 与 "引用"
 *
 * @param[in]  type   eth: 以太网
 * @param[in]  drv    设备编号
 * @param[in]  ip46   IPv4: 仅IPv4; IPv6: 仅IPv6; IP46: IPv4和IPv6;
 * @param[in]  opt    DEFS: 定义; REFC: 引用;
 */
#define NET_CFG_DEV_IPADDR(type, drv, ip46, opt)   ATPASTE4(_NET_CFG_, ip46, _, opt)(type, drv)

#define _NET_CFG_IPv4_REFC(type, drv)  &_NET_CFG_IPv4_NAME(type, drv), NULL
#define _NET_CFG_IPv4_DEFS(type, drv)   static const netcfg_ip4_t           \
                                        _NET_CFG_IPv4_NAME(type, drv) = ATPASTE3(RTCFG_ITEM_, type, _IPv4)(drv)
#define _NET_CFG_IPv6_REFC(type, drv)   NULL, &_NET_CFG_IPv6_NAME(type, drv)
#define _NET_CFG_IPv6_DEFS(type, drv)   static const netcfg_ip6_t           \
                                        _NET_CFG_IPv6_NAME(type, drv) = ATPASTE3(RTCFG_ITEM_, type, _IPv6)(drv)
#define _NET_CFG_IP46_DEFS(type, drv)  _NET_CFG_IPv4_DEFS(type, drv); _NET_CFG_IPv6_DEFS(type, drv)
#define _NET_CFG_IP46_REFC(type, drv)  &_NET_CFG_IPv4_NAME(type, drv), &_NET_CFG_IPv6_NAME(type, drv)
#endif//NET_CFG_DEV_IPADDR


/**
 * 网络设备相关变量名, 便于统一命名与使用
 *
 * @param[in]  type   eth: 以太网
 * @param[in]  drv    设备编号
 */
#define _NET_CFG_IPv4_NAME( type, drv)   ATPASTE4(_netcfg_, type, drv, _ipv4)
#define _NET_CFG_IPv6_NAME( type, drv)   ATPASTE4(_netcfg_, type, drv, _ipv6)
#define _NET_CFG_PRIV_NAME( type, drv)   ATPASTE4(_netcfg_, type, drv, _priv)
#define _NET_DEV_INFO_NAME( type, drv)   ATPASTE4(_netdev_, type, drv, _info)
#define _NET_DEV_NETIF_NAME(type, drv)   ATPASTE4(_netdev_, type, drv, _netif)
#define _NET_DEV_STATE_NAME(type, drv)   ATPASTE4(_netdev_, type, drv, _state)


/*******************************************************************************
 * @}
 * @addtogroup              Private_Defines
 * @{
 ******************************************************************************/

#if NO_SYS
/**
 * 以太网进站包传递到协议栈函数
 */
#define NETDEV_INPKY    netif_input
#else
#define NETDEV_INPKT    tcpip_input
#endif

/*----------------------------------------------------------------------------*/
#define _NET_DEV_INFO(type, drv, ip46)    NET_DEV_INFO(type, drv, ip46, DEFS);
/**
 * 定义网络设备
 */
/*static const netdev_info_t  */ NET_DEV_DFT
#ifdef NET_DEV_TBL
/*static const netdev_info_t[]*/ NET_DEV_TBL
#endif
#undef  _NET_DEV_INFO

#define _NET_DEV_INFO(type, drv, ip46)  &(NET_DEV_INFO(type, drv, ip46, REFC)), 
/**
 * 定义网络设备索引表
 */
static const netdev_info_t * const _netdev_info_tbl[] =
{
    NET_DEV_DFT
#ifdef NET_DEV_TBL
    NET_DEV_TBL
#endif
    NULL
};
#undef  _NET_DEV_INFO


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
/**
 * Loop for polling network device to handle packet reception.
 */
static void netdev_poll_loop(uint32_t dummy1, void *dummy2, void *device)
{
    netdev_info_t  *dev =  device;
    LWIP_UNUSED_ARG(dummy1);
    LWIP_UNUSED_ARG(dummy2);

    // Polling device to handle packet reception
    dev->poll(dev->netif);

    net_callback_thd(netdev_poll_loop, 0, NULL, dev);
}


/**
 * Loop for Check network device link status.
 */
static void netdev_link_loop(void *device)
{
    int             link_status;
    netdev_info_t  *dev =  device;

    // Check device link status
    link_status = dev->link(dev->netif);

    if (link_status > 0) {
        netif_set_link_up(dev->netif);
    } else
    if (link_status < 0) {
        netif_set_link_down(dev->netif);
    }
    sys_timeout(dev->invl, netdev_link_loop, dev);
}


static err_t netdev_add_do(struct netif *netif)
{   //lint --e{613}
    int32_t               mtu;
    err_t                 err;
    const netdev_info_t  *device = netif->state;

    LWIP_ERROR( "netdev_add_do: invalid network device"
               , device != NULL
               , return( ERR_ARG )
              );
    LOG_INFO("Loading TCP/IP stack network interface: %s", device->ifname);

    netif->state = device->state;   // netif->state = netif->state->state

    //------------------ Prev: device->init() ----------------------------------
    //
    // todo:
    //

    //------------- device->init(): netif_inif --> init() ----------------------
    if((err = device->init(netif)) != ERR_OK) {     // init()
        return( err );
    }

    //------------------ Post: device->init() ----------------------------------
#if LWIP_NETIF_HOSTNAME
    netif_set_hostname(netif, net_hostname_get());  // hostname
#endif
    if (device->ifname != NULL) {
        netif->name[0] = device->ifname[0];         // ifname
        netif->name[1] = device->ifname[1];
    }
    if((mtu = rtcfg_int32_read(&(device->cfgmtu))) > 0) {
        netif->mtu = mtu; //lint !e734              // mtu
    }
    if (device->poll != NULL) {                     // poll()
        net_callback_thd(netdev_poll_loop, 0, NULL, (void *)device);
    }
    if (device->link != NULL) {                     // link()
        sys_timeout(device->invl, netdev_link_loop, (void *)device);
    }

    return( ERR_OK );
}


/**
 * Init and Add all network device to system
 */
void netdev_sysyem_init(void)
{
    unsigned  i;

    if (netdev_device_add(_netdev_info_tbl[0]) == 0) {
        netif_set_default(_netdev_info_tbl[0]->netif);  // default network device
    }
    for(i = 1;  i < (ARRAY_SIZE(_netdev_info_tbl) - 1);  i++) //lint !e681
    {
        netdev_device_add(_netdev_info_tbl[i]);               //lint !e534
    }
}


/**
 * Get the network device from network device table
 */
const netdev_info_t *netdev_device_get(unsigned idx)
{
    if (idx < ARRAY_SIZE(_netdev_info_tbl)) {
        return( _netdev_info_tbl[idx] );
    }
    return( NULL );
}


/**
 * Add a network device to the system.
 */
int netdev_device_add(const netdev_info_t *device)
{
    struct netif        *netif;

    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    LWIP_ERROR( "netdev_device_add: invalid network device"
               , device->init != NULL
               , return( -1 )
              );

    netif = netif_add_noaddr(  (struct netif *)(device->netif)
                             , (void *        )(device)
                             , (netif_init_fn )(netdev_add_do)
                             , (netif_input_fn)(device->inpkt)
                            );          // network device Add
    LWIP_ERROR( "netdev_device_add: failed to called netdev_add()"
               , netif != NULL
               , return( -1 )
              );
    netif_set_up(netif);                // network device UP

#if LWIP_NETIF_STATUS_CALLBACK
    netif_set_status_callback(netif, (netif_status_callback_fn)net_status_callback);
#endif
#if LWIP_NETIF_LINK_CALLBACK
    netif_set_link_callback  (netif, (netif_status_callback_fn)net_link_callback  );
#endif

    return( netdev_ipaddr_updata(device) );
}


/**
 * Remove a network device from the system.
 */
int netdev_device_remove(const netdev_info_t *device)
{
    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    LWIP_ERROR( "netdev_device_remove: invalid network device"
               , (device != NULL) && (device->netif != NULL)
               , return( -1 )
              );

    netif_remove(device->netif);

    return( 0 );
}


/**
 * Update the IP and Mask and Gateway from config item
 */
int netdev_ipaddr_updata(const netdev_info_t *device)
{
    struct in_addr  ipaddr[3];

    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    LWIP_ERROR( "netdev_ipaddr_updata: invalid network device"
               , device->netif != NULL
               , return( -1 )
              );

#if LWIP_IPV4
    if (device->cfgip4 != NULL)
    {
    #if LWIP_DHCP
        if (rtcfg_int32_read(&(device->cfgip4->dhcp)) > 0)
        {   //---------- DHCPv4 ------------------------------------------------
            if (netif_is_link_up((struct netif *)(device->netif))) {
                netif_set_link_down(device->netif);
                dhcp_start(device->netif);  //lint !e534
                netif_set_link_up(device->netif);
            } else {
                dhcp_start(device->netif);  //lint !e534
            }
        }
        else
    #endif

        if (netdev_ipaddr_get(device, ipaddr) == 3)
        {   //---------- Static IPv4 address -----------------------------------
            if (netif_is_link_up((struct netif *)(device->netif))) {
                netif_set_link_down(device->netif);
    #if LWIP_DHCP
                dhcp_stop(device->netif);
    #endif
                netif_set_addr(device->netif, (void *)(ipaddr), (void *)(ipaddr + 1), (void *)(ipaddr + 2));
                netif_set_link_up(device->netif);
            } else {
    #if LWIP_DHCP
                dhcp_stop(device->netif);
    #endif
                netif_set_addr(device->netif, (void *)(ipaddr), (void *)(ipaddr + 1), (void *)(ipaddr + 2));
            }
        }

    #if LWIP_AUTOIP
        else
        {   //---------- AutoIPv4 ----------------------------------------------
            if (netif_is_link_up((struct netif *)(device->netif))) {
                netif_set_link_down(device->netif);
                autoip_start(device->netif)
                netif_set_link_up(device->netif);
            } else {
                autoip_start(device->netif)
            }
        }
    #endif
    }
#endif

#if LWIP_IPV6
    if (device->cfgip6 != NULL)
    {
//  #if LWIP_IPV6_AUTOCONFIG
//      netif_set_ip6_autoconfig_enabled(netif, 1);
//  #endif
//      netif_create_ip6_linklocal_address(netif, 1);
    }
#endif

    return( 0 );
}


/*----------------------------------------------------------------------------*/
/**
 * Get binary IP and Mask and Gateway from config item
 */
int netdev_ipaddr_get(const netdev_info_t *device, struct in_addr addr[3])
{
    char         buf[16 + 4];
    const char  *str;
    UNUSED_VAR(device);  UNUSED_VAR(addr);

    if (addr == NULL) {
        return( -1 );
    }
#if LWIP_IPV4
    str = netdev_ip_read(device, buf, sizeof(buf));
    if((str == NULL) || (inet_aton(str, (addr + 0)) == 0)) { //lint !e740
        return( -1 );
    }
    str = netdev_mask_read(device, buf, sizeof(buf));
    if((str == NULL) || (inet_aton(str, (addr + 1)) == 0)) { //lint !e740
        return( -1 );
    }
    str = netdev_gw_read(device, buf, sizeof(buf));
    if((str == NULL) || (inet_aton(str, (addr + 2)) == 0)) { //lint !e740
        return( -1 );
    }
    return( 3 );
#endif
}


/**
 * Read IP string from config item
 */
const char *netdev_ip_read(const netdev_info_t *device, char *buf, unsigned len)
{
    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    if (device->cfgip4 == NULL) {
        return( NULL );
    }
    return( rtcfg_string_read(&(device->cfgip4->ip), buf, len) );
}


/**
 * Write IP string to config item
 */
int netdev_ip_write(const netdev_info_t *device, const char *str)
{
#if LWIP_IPV4
    char  addr[sizeof(struct in_addr)];
#endif

    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    if (device->cfgip4 == NULL) {
        return( -1 );
    }
#if LWIP_IPV4
    if((str == NULL) || (inet_aton(str, addr) == 0)) {
        return( -1 );
    }
#endif
    return( rtcfg_string_write(&(device->cfgip4->ip), str) );
}


/**
 * Read Subnet-Mask string from config item
 */
const char *netdev_mask_read(const netdev_info_t *device, char *buf, unsigned len)
{
    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    if (device->cfgip4 == NULL) {
        return( NULL );
    }
    return( rtcfg_string_read(&(device->cfgip4->mask), buf, len) );
}


/**
 * Write Subnet-Mask address string to config item
 */
int netdev_mask_write(const netdev_info_t *device, const char *str)
{
#if LWIP_IPV4
    char  addr[sizeof(struct in_addr)];
#endif

    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    if (device->cfgip4 == NULL) {
        return( -1 );
    }
#if LWIP_IPV4
    if((str == NULL) || (inet_aton(str, addr) == 0)) {
        return( -1 );
    }
#endif
    return( rtcfg_string_write(&(device->cfgip4->mask), str) );
}


/**
 * Read Default-Gateway address string from config item
 */
const char *netdev_gw_read(const netdev_info_t *device, char *buf, unsigned len)
{
    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    if (device->cfgip4 == NULL) {
        return( NULL );
    }
    return( rtcfg_string_read(&(device->cfgip4->gw), buf, len) );
}


/**
 * Write Default-Gateway address string to config item
 */
int netdev_gw_write(const netdev_info_t *device, const char *str)
{
#if LWIP_IPV4
    char  addr[sizeof(struct in_addr)];
#endif

    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    if (device->cfgip4 == NULL) {
        return( -1 );
    }
#if LWIP_IPV4
    if((str == NULL) || (inet_aton(str, addr) == 0)) {
        return( -1 );
    }
#endif
    return( rtcfg_string_write(&(device->cfgip4->gw), str) );
}


/**
 * Read DHCP status value from config item
 */
int netdev_dhcp_read(const netdev_info_t *device)
{
    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    if (device->cfgip4 == NULL) {
        return( INT32_MIN );
    }
    return( rtcfg_int32_read(&(device->cfgip4->dhcp)) );
}


/**
 * Write DHCP status value to config item
 */
int netdev_dhcp_write(const netdev_info_t *device, int dhcp)
{
    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    if((device->cfgip4 == NULL) || (dhcp < 0)) {
        return( -1 );
    }
    return( rtcfg_int32_write(&(device->cfgip4->dhcp), dhcp) );
}


/*----------------------------------------------------------------------------*/
/**
 * Convert from text address to a MAC address
 */
int netdev_ethmac_aton(const char *str, uint8_t addr[6])
{
    if((str != NULL) && (addr != NULL))
    {
        if (sscanf(str, "%2hhx-%2hhx-%2hhx-%2hhx-%2hhx-%2hhx",addr+0,addr+1,addr+2,addr+3,addr+4,addr+5) == 6) {
            return( 6 );
        }
        if (sscanf(str, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",addr+0,addr+1,addr+2,addr+3,addr+4,addr+5) == 6) {
            return( 6 );
        }
    }
    return( -1 );
}


/**
 * Get ethernet binary MAC address from config item
 */
int netdev_ethmac_get(const netdev_info_t *device, uint8_t addr[6])
{
    char        buf[20];

    return( netdev_ethmac_aton(netdev_ethmac_read(device, buf, sizeof(buf)), addr) );
}


/**
 * Read MAC address string from config item
 */
const char *netdev_ethmac_read(const netdev_info_t *device, char *buf, unsigned len)
{
    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    if (device->cfgpriv == NULL) {
        return( NULL );
    }
    return( rtcfg_string_read(&(((const netcfg_eth_t*)(device->cfgpriv))->hwaddr), buf, len) );
}


/**
 * Write MAC address string to config item
 */
int netdev_ethmac_write(const netdev_info_t *device, const char *str)
{
    uint8_t  addr[6];

    if (device == NULL) {
        device = _netdev_info_tbl[0];   // default network device
    }
    if (device->cfgpriv == NULL) {
        return( -1 );
    }
    if (netdev_ethmac_aton(str, addr) < 0) {
        return( -1 );                   // error for formatting
    }
    return( rtcfg_string_write(&(((const netcfg_eth_t*)(device->cfgpriv))->hwaddr), str) );
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

