/*******************************************************************************
 * @file    netdev_eth.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/11/14
 * @brief   Ethernet interface abstraction layer. Note: todo
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/11/14 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint +fie                     // Integer model for Enum flag
//lint -e801                    // Use of goto is deprecated
//lint -e818                    // Pointer parameter 'xxx' could be declared to const
//lint -emacro(?*, LWIP_DEBUGF)
//lint -emacro(?*, ip6_addr_set_allnodes_linklocal)
//lint -esym(534, netif::mld_mac_filter, pbuf_free, *::ReadFrame)
//lint -emacro(717, netif_set_hostname, ip6_addr_set_allnodes_linklocal)
//lint -emacro(717, MIB2_INIT_NETIF, MIB2_STATS_NETIF_*)
//lint -emacro(835, ARM_ETH_MAC_*)

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "Net_ETH"


#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/sys.h"           // Used: sys_now()  sys_msleep()

#include <stdio.h>
#include "Driver_ETH_MAC.h"
#include "Driver_ETH_PHY.h"
#include "net_user.h"
#include "net_device.h"
#include "netdev_eth.h"


/*******************************************************************************
 * @addtogroup NetDev_ETH
 * @{
 * @addtogroup              Private_Constants
 * @{
 ******************************************************************************/

#ifndef ETH_FRAME_LEN_MAX
#define ETH_FRAME_LEN_MAX           1514  // 1518   //!< (C) Ref: Win32/pcapif.c
#endif

#ifndef ETH_TIMEOUT_TX
#define ETH_TIMEOUT_TX              3               //!< (C) TimeOut of transmit
#endif

// Describe your network interface.
#define IFNAME0                    'e'
#define IFNAME1                    '0'

#define ETH_DEV_INIT_OK             0x55AA5AA5

#define NETIF_CHECKSUM_GEN_ANY   (  NETIF_CHECKSUM_GEN_IP        \
                                  | NETIF_CHECKSUM_GEN_UDP       \
                                  | NETIF_CHECKSUM_GEN_TCP       \
                                  | NETIF_CHECKSUM_GEN_ICMP      \
                                  | NETIF_CHECKSUM_GEN_ICMP6     \
                                 )
#define NETIF_CHECKSUM_CHECK_ANY (  NETIF_CHECKSUM_CHECK_IP      \
                                  | NETIF_CHECKSUM_CHECK_UDP     \
                                  | NETIF_CHECKSUM_CHECK_TCP     \
                                  | NETIF_CHECKSUM_CHECK_ICMP    \
                                  | NETIF_CHECKSUM_CHECK_ICMP6   \
                                 )
#define NETIF_SERIOUS            (NETIF_DEBUG | LWIP_DBG_LEVEL_SERIOUS)
#define NETIF_SEVERE             (NETIF_DEBUG | LWIP_DBG_LEVEL_SEVERE)


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
/**
 * Initialize the network interface hardware.
 *
 * @param[in]  netif  The lwip network interface (const)
 *
 * @retval  (== ERR_OK) Success
 * @retval  (!= ERR_OK) Failed
 */
static err_t low_level_init(struct netif *netif)
{
    ARM_ETH_MAC_CAPABILITIES  cap;
    netdev_eth_t             *eth;

    LWIP_ERROR( "low_level_init: invalid pointer of \"netif->state\" (ethernet private)"
               , (eth = netif->state) != NULL
               , return( ERR_ARG )
              );
    LWIP_ERROR( "low_level_init: invalid pointer of ethernet MAC or PHY"
               , (eth->mac != NULL) && (eth->phy != NULL)
               , return( ERR_ARG )
              );
    eth->link    = ARM_ETH_LINK_DOWN;
    eth->init_ok = 0;

    LWIP_ERROR( "low_level_init: invalid driver version of ethernet MAC or PHY"
               ,   (((eth->mac->GetVersion) != NULL) && ((eth->mac->GetVersion()).api >= 0x0200))
                && (((eth->phy->GetVersion) != NULL) && ((eth->phy->GetVersion()).api >= 0x0200))
               , return( ERR_IF )
              );
    LWIP_ERROR( "low_level_init: invalid capabilities of ethernet MAC"
               , ((eth->mac->GetCapabilities) != NULL) && ((cap = eth->mac->GetCapabilities()).event_rx_frame != 0)
               , return( ERR_IF )
              );

    /* Initialize MAC Interface */
    if (eth->mac->Initialize(eth->notify) != ARM_DRIVER_OK) {
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->mac->Initialize(eth->notify) == ARM_DRIVER_OK\n"));
        return( ERR_IF );
    }
    if (eth->mac->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK) {
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->mac->PowerControl(ARM_POWER_FULL)\n"));
        return( ERR_IF );
    }
    if (eth->mac->SetMacAddress((ARM_ETH_MAC_ADDR*)(netif->hwaddr)) != ARM_DRIVER_OK) {
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->mac->SetMacAddress((ARM_ETH_MAC_ADDR*)(netif->hwaddr))\n"));
        return( ERR_IF );
    }
    if (eth->mac->Control(ARM_ETH_MAC_CONTROL_TX, 0) != ARM_DRIVER_OK) {
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->mac->Control(ARM_ETH_MAC_CONTROL_TX, 0)\n"));
        return( ERR_IF );
    }
    if (eth->mac->Control(ARM_ETH_MAC_CONTROL_RX, 0) != ARM_DRIVER_OK) {
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->mac->Control(ARM_ETH_MAC_CONTROL_RX, 0)\n"));
        return( ERR_IF );
    }

    /* Initialize Physical Media Interface */
    if (eth->phy->Initialize(eth->mac->PHY_Read, eth->mac->PHY_Write) != ARM_DRIVER_OK) {
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->phy->Initialize(eth->mac->PHY_Read, eth->mac->PHY_Write)\n"));
        return( ERR_IF );
    }
    if (eth->phy->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK) {
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->phy->PowerControl(ARM_POWER_FULL)\n"));
        return( ERR_IF );
    }
    if (eth->phy->SetInterface(cap.media_interface) != ARM_DRIVER_OK) {
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->phy->SetInterface(cap.media_interface)\n"));
        return( ERR_IF );
    }
    if (eth->phy->SetMode(ARM_ETH_PHY_AUTO_NEGOTIATE) != ARM_DRIVER_OK) {
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->phy->SetMode(ARM_ETH_PHY_AUTO_NEGOTIATE)\n"));
        return( ERR_IF );
    }

    NETIF_SET_CHECKSUM_CTRL( netif,
       (  ( (cap.checksum_offload_tx_ip4
          && cap.checksum_offload_tx_ip6 ) ? 0 : (NETIF_CHECKSUM_GEN_IP)    )
        | ( (cap.checksum_offload_tx_icmp) ? 0 : (NETIF_CHECKSUM_GEN_ICMP
                                                | NETIF_CHECKSUM_GEN_ICMP6) )
        | ( (cap.checksum_offload_tx_udp ) ? 0 : (NETIF_CHECKSUM_GEN_UDP)   )
        | ( (cap.checksum_offload_tx_tcp ) ? 0 : (NETIF_CHECKSUM_GEN_TCP)   )

        | ( (cap.checksum_offload_rx_ip4
          && cap.checksum_offload_rx_ip6 ) ? 0 : (NETIF_CHECKSUM_CHECK_IP)    )
        | ( (cap.checksum_offload_rx_icmp) ? 0 : (NETIF_CHECKSUM_CHECK_ICMP
                                                | NETIF_CHECKSUM_CHECK_ICMP6) )
        | ( (cap.checksum_offload_rx_udp ) ? 0 : (NETIF_CHECKSUM_CHECK_UDP)   )
        | ( (cap.checksum_offload_rx_tcp ) ? 0 : (NETIF_CHECKSUM_CHECK_TCP)   )
       )
    );
    eth->init_ok = ETH_DEV_INIT_OK;
    return( ERR_OK );
}


/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf might be chained.
 *
 * @param[in]  netif  The lwip network interface (const)
 * @param[in]  p      The MAC packet to send (e.g. IP packet including MAC addr and type)
 *
 * @retval  (== ERR_OK) the packet could be sent
 * @retval  (!= ERR_OK) the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    int            i;
    uint32_t       now;
    int32_t        result;
    struct pbuf   *q;
    netdev_eth_t  *eth = netif->state;

    if ((eth->init_ok != ETH_DEV_INIT_OK) || (eth->link != ARM_ETH_LINK_UP)) {
        return( ERR_IF );   // No Init or link-up
    }
#if ETH_PAD_SIZE
    pbuf_remove_header(p, ETH_PAD_SIZE);    //lint !e534
#endif

    for(result = ARM_DRIVER_ERROR, q = p;  q != NULL;  q = q->next)
    {
        for(i = 0;  i < ETH_TIMEOUT_TX;  i++)
        {
            result = eth->mac->SendFrame(   q->payload
                                         ,  q->len
                                         ,((q->next) ? ARM_ETH_MAC_TX_FRAME_FRAGMENT : 0)
                                        );
            if (result != ARM_DRIVER_ERROR_BUSY)  break;
            for(now = sys_now();  now == sys_now();  sys_msleep(1)) ;
        }
        if (result != ARM_DRIVER_OK) {
            LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->mac->SendFrame(q->payload, q->len, flags)\n"));
            goto ERROR_OUTPUT;
        }
    }
    {
        MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);
        if(((u8_t*)p->payload)[0] & 1) {    // broadcast or multicast packet
            MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
        } else {                            // unicast packet
            MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
        }
        LINK_STATS_INC(link.xmit);          // todo: thread-safe
    }
#if ETH_PAD_SIZE
    pbuf_add_header(p, ETH_PAD_SIZE);       //lint !e534
#endif
    return( ERR_OK );

    ERROR_OUTPUT :
    {
        MIB2_STATS_NETIF_INC(netif, ifoutdiscards);
        LINK_STATS_INC(link.drop);          // todo: thread-safe
    }
#if ETH_PAD_SIZE
    pbuf_add_header(p, ETH_PAD_SIZE);       //lint !e534
#endif
    return( ERR_IF );
}


/**
 * Allocate and transfer to the pbuf for incoming packet from the interface.
 *
 * @param[in]  netif  The lwip network interface (const)
 *
 * @retval  (!= NULL) A pbuf filled with the received packet (including MAC header)
 * @retval  (== NULL) Failed
 */
static struct pbuf* low_level_input(struct netif *netif)
{
    u32_t         len;
    struct pbuf   *p;
    netdev_eth_t  *eth = netif->state;

    if ((eth->init_ok != ETH_DEV_INIT_OK) || (eth->link != ARM_ETH_LINK_UP)) {
        return( NULL );     // No Init or link-up
    }

    /* Obtain the size of the packet and put it into the "len" variable. */
    if((len = eth->mac->GetRxFrameSize()) == 0) {  //lint !e650 !e737
        return( NULL );                     // No packet available
    }
    if (len > ETH_FRAME_LEN_MAX) {
        eth->mac->ReadFrame(NULL, 0);       // Drop packet
        if((s32_t)len >= 0) {
            LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: low_level_input() oversized packet! size: %u\n", len));
        }
        goto ERROR_INPUT;
    }
#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE;
#endif

    /* allocate a pbuf chain of pbufs from the pool. */
    if((p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL)) == NULL) { //lint !e734
        eth->mac->ReadFrame(NULL, 0);       // Drop packet
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: pbuf_alloc(PBUF_RAW, len, PBUF_POOL)\n"));
        LINK_STATS_INC(link.memerr);        // todo: thread-safe
        goto ERROR_INPUT;
    }
    if (p->tot_len != p->len) {
        eth->mac->ReadFrame(NULL, 0);       // Drop packet
        pbuf_free(p);
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: Can't alloc one single payload for pbuf!\n"));
        goto ERROR_INPUT;
    }
#if ETH_PAD_SIZE
    pbuf_remove_header(p, ETH_PAD_SIZE);    //lint !e534
#endif

    /* Read packet to fill this pbuf */
    if (eth->mac->ReadFrame(p->payload, MIN(len, p->len)) < 0) {
        pbuf_free(p);
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->mac->ReadFrame(p->payload, len)\n"));
        goto ERROR_INPUT;
    }
    {
        MIB2_STATS_NETIF_ADD(netif, ifinoctets, p->tot_len);
        if(((u8_t*)p->payload)[0] & 1) {    // broadcast or multicast packet
            MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
        } else {                            // unicast packet
            MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
        }
        LINK_STATS_INC(link.recv);          // todo: thread-safe
    }
#if ETH_PAD_SIZE
    pbuf_add_header(p, ETH_PAD_SIZE);       //lint !e534
#endif
    return( p );

    ERROR_INPUT :
    {
        MIB2_STATS_NETIF_INC(netif, ifindiscards);
        LINK_STATS_INC(link.drop);          // todo: thread-safe
    }
    return( NULL );
}


/**
 * Check MAC link status, then Start or Stop EMAC DMA
 *
 * @param[in]  netif   The lwip network interface (const) 
 *
 * @retval  ( > 0) MAC link status changed to link-up
 * @retval  (== 0) MAC link status not changed
 * @retval  ( < 0) MAC link status changed to link-down
 */
static int low_level_link(struct netif *netif)
{
    u32_t               arg;
    ARM_ETH_LINK_INFO   info;
    ARM_ETH_LINK_STATE  link;
    netdev_eth_t       *eth = netif->state;

    if (eth->init_ok != ETH_DEV_INIT_OK) {
        return( 0 );    // not initialize
    }
    if((link = eth->phy->GetLinkState()) == eth->link) {
        return( 0 );    // link status not changed
    }
    eth->link = link;

    if (link == ARM_ETH_LINK_UP)
    {
        info = eth->phy->GetLinkInfo();

        arg  = (  (info.speed  << ARM_ETH_MAC_SPEED_Pos)    //lint !e835
                | (info.duplex << ARM_ETH_MAC_DUPLEX_Pos)
            //  |  ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX
            //  |  ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX
                |  ARM_ETH_MAC_ADDRESS_BROADCAST
            //  |  ARM_ETH_MAC_ADDRESS_MULTICAST
               );
#if LWIP_CHECKSUM_CTRL_PER_NETIF
        if((netif->chksum_flags & NETIF_CHECKSUM_GEN_ANY) != NETIF_CHECKSUM_GEN_ANY) {
            arg |= ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX;
        }
        if((netif->chksum_flags & NETIF_CHECKSUM_CHECK_ANY) != NETIF_CHECKSUM_CHECK_ANY) {
            arg |= ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX;
        }
#endif
        if (eth->mac->Control(ARM_ETH_MAC_CONFIGURE, arg) != ARM_DRIVER_OK) {
            LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->mac->Control(ARM_ETH_MAC_CONFIGURE, arg)\n"));
            return( 0 );
        }
        if (eth->mac->Control(ARM_ETH_MAC_CONTROL_TX, 1) != ARM_DRIVER_OK) {
            LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->mac->Control(ARM_ETH_MAC_CONTROL_TX, 1)\n"));
            return( 0 );
        }
        if (eth->mac->Control(ARM_ETH_MAC_CONTROL_RX, 1) != ARM_DRIVER_OK) {
            LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->mac->Control(ARM_ETH_MAC_CONTROL_RX, 1)\n"));
            return( 0 );
        }
        arg = (   (info.speed == 2) ?  (1000 * 1000 * 1000)
               :  (info.speed == 1) ?  (1000 * 1000 * 100 )
               :/*(info.speed == 0) ?*/(1000 * 1000 * 10  )
              );
        LWIP_UNUSED_ARG(arg);       // todo: thread-safe ¡ý¡ý¡ý¡ý
        MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, arg);
    //  netif_set_link_up(netif);
        return( 1 );
    }
    else
    {
        if (eth->mac->Control(ARM_ETH_MAC_CONTROL_TX, 0) != ARM_DRIVER_OK) {
            LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->mac->Control(ARM_ETH_MAC_CONTROL_TX, 0)\n"));
            return( 0 );
        }
        if (eth->mac->Control(ARM_ETH_MAC_CONTROL_RX, 0) != ARM_DRIVER_OK) {
            LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: eth->mac->Control(ARM_ETH_MAC_CONTROL_RX, 0)\n"));
            return( 0 );
        }
        while (eth->mac->GetRxFrameSize() > 0)
        {   // Drop all received packet
            eth->mac->ReadFrame(NULL, 0);
        }
    //  netif_set_link_down(netif);
        return( -1 );
    }
}


/*----------------------------------------------------------------------------*/
/**
 * Polls the serial device and feeds the IP layer with incoming packets.
 *
 * @param[in]  netif   The network device
 */
void netdev_eth_poll(void *netif)
{
    struct pbuf    *p;
    struct netif   *netifc = netif;

    while(1)  //lint !e716
    {
        if((p = low_level_input(netifc)) == NULL) {     // Receive packet into new pbuf
            break;
        }
        if (netifc->input != NULL) {
            if (netifc->input(p, netifc) == ERR_OK) {   // Pass packet to ethernet_input()
                continue;
            }
        }
        pbuf_free(p);   // p = NULL;
        LWIP_DEBUGF(NETIF_SEVERE, ("ERROR: netdev_eth_poll() call to netif->input()\n"));
    }
}


/**
 * Callback function for the network device notify event.
 *
 * @param[in]  netif   The network device interface
 * @param[in]  event   event notification mask
 */
void netdev_eth_notify(void *netif, uint32_t event)
{
    // frame is received
    if (event & ARM_ETH_MAC_EVENT_RX_FRAME) {
        net_callpoll_isr(netdev_eth_poll, netif);
    }

    // frame is transmitted
    if (event & ARM_ETH_MAC_EVENT_TX_FRAME) {
    }

    // Wake-up (on Magic Packet)
    if (event & ARM_ETH_MAC_EVENT_WAKEUP) {
    }

    // Timer Alarm
    if (event & ARM_ETH_MAC_EVENT_TIMER_ALARM) {
    }
}


/**
 * Check network device interface link status.
 *
 * @param[in]  netif   The network device interface
 *
 * @retval  ( > 0) MAC link status changed to link-up
 * @retval  (== 0) MAC link status not changed
 * @retval  ( < 0) MAC link status changed to link-down
 */
int netdev_eth_link(void *netif)
{
    return( low_level_link(netif) );
}


/**
 * Initialize the actual hardware of the network device interface.
 *
 * @param[in]  netif   The network device interface
 *
 * @retval  (== ERR_OK) Success
 * @retval  (!= ERR_OK) Failed
 *
 * @note    This function should be passed as a parameter to netif_add().
 */
int netdev_eth_init(void *netif)
{   //lint --e{613}
    struct netif   *netifc = netif;

    LWIP_ERROR( "netdev_eth_init: invalid pointer of \"netif\" (network interfaces)"
               , netifc != NULL
               , return( ERR_ARG )
              );
    LWIP_ERROR( "netdev_eth_init: invalid pointer of \"netif->state\" (ethernet device)"
               , netifc->state != NULL
               , return( ERR_ARG )
              );

#if LWIP_NETIF_HOSTNAME
    netif_set_hostname(netifc, "lwip");     // Initialize interface hostname
#endif
    netifc->name[0] = IFNAME0;
    netifc->name[1] = IFNAME1;              // Ethernet interface Name

    netifc->hwaddr_len = netdev_ethmac_get( ((netdev_eth_t *)(netifc->state))->device
                                           , netifc->hwaddr
                                          );       //lint !e734
    if (netifc->hwaddr_len != ETHARP_HWADDR_LEN) { //lint !e734   // 31 bits to 8 bits
        netifc->hwaddr_len  = ETHARP_HWADDR_LEN;
        netifc->hwaddr[0]   = 0;            // MAC address
        netifc->hwaddr[1]   = 0;
        netifc->hwaddr[2]   = 0;
        netifc->hwaddr[3]   = 0;
        netifc->hwaddr[4]   = 0;
        netifc->hwaddr[5]   = 0;
    }
    netifc->mtu = 1500;                     // Maximum Transfer Unit

    /* Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units of bits per second.
     */
    MIB2_INIT_NETIF(netifc, snmp_ifType_ethernet_csmacd, 0);

    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output() from it
     * if you have to do some checks before sending (e.g. if link* is available...)
     */
#if LWIP_IPV4
    netifc->output     = etharp_output;     // Called by the IPv4 module to send a packet
#endif
#if LWIP_IPV6
    netifc->output_ip6 = ethip6_output;     // Called by the IPv6 module to send a packet
#endif
    netifc->linkoutput = low_level_output;  // Called by ethernet_output() to output a pbuf

    /* device capabilities. don't set NETIF_FLAG_ETHARP if device is not an ethernet */
    netifc->flags  = NETIF_FLAG_ETHERNET | NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
#if LWIP_IPV4 && LWIP_IGMP
    /* device capabilities for IGMP */
    netifc->flags |=  NETIF_FLAG_IGMP;
#endif

#if LWIP_IPV6 && LWIP_IPV6_MLD
    /* device capabilities for MLD6 */
    netifc->flags |=  NETIF_FLAG_MLD6;

    /* For hardware/netifs that implement MAC filtering.
     * All-nodes link-local is handled by default, so we must let the hardware know
     * to allow multicast packets in.   Should set mld_mac_filter previously.
     */
    if (netifc->mld_mac_filter != NULL) {
        ip6_addr_t  ip6_allnodes_ll;
        ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
        netifc->mld_mac_filter(netifc, &ip6_allnodes_ll, NETIF_ADD_MAC_FILTER);
    }
#endif

    return( low_level_init(netifc) );        // Initialize the network interface hardware
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

