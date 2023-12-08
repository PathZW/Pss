/*******************************************************************************
 * @file    net_user.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/11/16
 * @brief   User define for network.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/11/16 -- Linghu -- the first version
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

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "Net_User"
#include "emb_log.h"

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/err.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/tcpip.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/dns.h"
#include "lwip/api.h"

#include <stdint.h>
#include <stdio.h>      // Used: sscanf()
#include "defines.h"    // Used: ATPASTEx(), TO_STRING(), UNUSED_VAR()
#include "emb_queue.h"
#include "emb_rtcfg.h"
#include "shell/cli_shell.h"
#include "net_device.h"
#include "net_user.h"


/******************************************************************************/
/**
 * @addtogroup __Config
 * @{
 * @addtogroup              Network
 * @{
 ******************************************************************************/

#ifndef NET_POLL_THREAD_NAME
#define NET_POLL_THREAD_NAME       "net_poll_thread"     //!< (C) 线程名: 网络驱动轮询
#endif
#ifndef NET_POLL_THREAD_STKB
#define NET_POLL_THREAD_STKB               0             //!< (C) 线程栈: 网络驱动轮询
#endif
#ifndef NET_POLL_THREAD_PRIO
#define NET_POLL_THREAD_PRIO               1             //!< (C) 优先级: 网络驱动轮询
#endif
#ifndef NET_POLL_QUEUE_SIZE
#define NET_POLL_QUEUE_SIZE                16            //!< (C) 队列长: 网络驱动轮询
#endif

#ifndef NET_CUST_PSS_INS
#define NET_CUST_PSS_INS                   1             //!< (C) 普塞斯仪表 定制是(1)否(0)
#endif

#ifndef RTCFG_ITEM_HOST_NAME
#define RTCFG_ITEM_HOST_NAME {"Sys:HostName", HOST_NAME} //!< (C) hostname 配置项
#endif

#ifndef RTCFG_ITME_SRV_PORT
#define RTCFG_ITME_SRV_PORT     {"Net:SrvPort", 5025}    //!< (C) TCP/UDP 服务端口
#endif

#ifndef ENDL
#define ENDL               "\r\n"           //!< (C) End of line, start the new line
#endif


/*******************************************************************************
 * @}
 * @}
 * @addtogroup Net_User
 * @{
 * @addtogroup              Private_Macros
 * @{
 ******************************************************************************/

#ifndef HOST_NAME
#define HOST_NAME             COMPANY_NAME "_" DEVICE_NAME
#endif
#ifndef COMPANY_NAME
#define COMPANY_NAME            "PSS"
#endif
#ifndef DEVICE_NAME
#define DEVICE_NAME             "NAME"
#endif


/*******************************************************************************
 * @}
 * @addtogroup              Private_Types
 * @{
 ******************************************************************************/

typedef struct net_event_s {
    net_event_fn    func;
    uint32_t        argi;
    void           *argf;
    void           *argp;
} net_event_t;

typedef QUEUE_TYPE (
    net_event_t,
    NET_POLL_QUEUE_SIZE
) net_queue_t;


/*******************************************************************************
 * @}
 * @addtogroup              Private_Variables
 * @{
 ******************************************************************************/

#if !NO_SYS
static sys_sem_t    _net_MsgSem;    // the network poll semaphore
#endif
static net_queue_t  _net_MsgQue;    // the network poll cycle-queue

/**
 * config item of hostname
 */
static const rtcfg_string_t  _netcfg_hostname  = RTCFG_ITEM_HOST_NAME ;
static char                  _net_hostname[16] = HOST_NAME; //lint !e784

/**
 * config item of server port
 */
static const rtcfg_int32_t   _netcfg_srvport = RTCFG_ITME_SRV_PORT ;


/*******************************************************************************
 * @}
 * @addtogroup              Private_Prototypes
 * @{
 ******************************************************************************/

static void net_poll_thread(void *arg);
static void net_init_done  (void *arg);
static void net_init_apps  (void);

static void net_hostname_init(void);


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
///
static void net_callpoll_do(uint32_t dummy, net_poll_fn func, void *netif)
{   //lint --e{613}  // func: Possible use of null pointer
    LWIP_UNUSED_ARG(dummy);
    LWIP_ASSERT( "func != NULL",  func != NULL);
    LWIP_ASSERT("netif != NULL", netif != NULL);

    func(netif);
}


/**
 * Callback a network function in the thread context.
 */
void net_callpoll_thd(net_poll_fn func, void *netif)
{
    net_callback_thd((net_event_fn)net_callpoll_do, 0, func, netif);
}


/**
 * Callback a network function in the ISR context.
 */
void net_callpoll_isr(net_poll_fn func, void *netif)
{
    net_callback_isr((net_event_fn)net_callpoll_do, 0, func, netif);
}


/**
 * Callback a specific function in the thread context.
 */
void net_callback_thd(net_event_fn func, uint32_t argi, void *argf, void *argp)
{
    net_event_t    *ptr;

    SYS_ARCH_DECL_PROTECT(lev);
    SYS_ARCH_PROTECT(lev);

    if (QUEUE_FREE((_net_MsgQue)) != 0)
    {   // Push the handler to queue
        QUEUE_I_BUF(_net_MsgQue, &ptr);
        ptr->func = func;
        ptr->argi = argi;
        ptr->argf = argf;
        ptr->argp = argp;
        QUEUE_I_ADJ(_net_MsgQue, 1);

        SYS_ARCH_UNPROTECT(lev);
        sys_sem_signal(&_net_MsgSem);
    }
    else
    {
        SYS_ARCH_UNPROTECT(lev);

        LWIP_ERROR( "Out of net poll queue, NET_POLL_QUEUE_SIZE is too small"
                   , LWIP_DBG_MIN_LEVEL >= LWIP_DBG_LEVEL_WARNING
                   , ((void)0)
                  );
    }
}


/**
 * Callback a specific function in the ISR context.
 */
void net_callback_isr(net_event_fn func, uint32_t argi, void *argf, void *argp)
{
    net_event_t    *ptr;

    SYS_ARCH_DECL_PROTECT(lev);
    SYS_ARCH_PROTECT(lev);

    if (QUEUE_FREE((_net_MsgQue)) != 0)
    {   // Push the handler to queue
        QUEUE_I_BUF(_net_MsgQue, &ptr);
        ptr->func = func;
        ptr->argi = argi;
        ptr->argf = argf;
        ptr->argp = argp;
        QUEUE_I_ADJ(_net_MsgQue, 1);

        SYS_ARCH_UNPROTECT(lev);
        sys_sem_signal_fromisr(&_net_MsgSem);
    }
    else
    {
        SYS_ARCH_UNPROTECT(lev);

        LWIP_ERROR( "Out of net poll queue, NET_POLL_QUEUE_SIZE is too small"
                   , LWIP_DBG_MIN_LEVEL >= LWIP_DBG_LEVEL_WARNING
                   , ((void)0)
                  );
    }
}


/*----------------------------------------------------------------------------*/
/**
 * The Cyclic polling thread of network protocol stack
 */
static void net_poll_thread(void *arg)
{
    LWIP_UNUSED_ARG(arg);

    netconn_thread_init();
    while (net_main() >= 0) ;
    netconn_thread_cleanup();
}


/**
 * The main function of network protocol stack. It handle protocol timeouts
 * and polling network device event.
 */
int net_main(void)
{
    net_event_t  *ptr;

    /* Pop from queue and Call the handler */
    for(sys_sem_wait(&_net_MsgSem);  !QUEUE_IS_EMPTY(_net_MsgQue);)
    {
        QUEUE_O_BUF(_net_MsgQue, &ptr);
        if (ptr->func == NULL) {
            QUEUE_O_ADJ(_net_MsgQue, 1);
            return( -1 );   // End of network protocol stack
        }
        ptr->func(ptr->argi, ptr->argf, ptr->argp);
        QUEUE_O_ADJ(_net_MsgQue, 1);
    }

#if NO_SYS
    /* handle timers (already done in tcpip.c when NO_SYS=0) */
    sys_check_timeouts();

#if ENABLE_LOOPBACK && !LWIP_NETIF_LOOPBACK_MULTITHREADING
    /* check for loopback packets on all netifs */
    netif_poll_all();
#endif
#endif

    return( 0 );
}

/*----------------------------------------------------------------------------*/
/**
 * Initialize network protocol stack and network device.
 */
int net_init(void)
{
#if !NO_SYS
    sys_sem_t  init_sem;

    if (sys_sem_new(&init_sem, 0) != ERR_OK) {
        return( -1 );
    }
    tcpip_init(net_init_done, &init_sem);
    sys_sem_wait(&init_sem);
    sys_sem_free(&init_sem);
#else
    lwip_init();
    net_init_done(NULL);
#endif

    return( 0 );
}


//lint -esym(522, net_init_apps)
static void net_init_apps(void)
{
//    void tcpecho_init(void);
//    void udpecho_init(void);

//    tcpecho_init();
//    udpecho_init();
}


static void net_init_done(void *arg)
{
    sys_sem_t   *init_sem = arg;

    QUEUE_INIT(_net_MsgQue);    // Init net_poll_thread handler queue
    LWIP_ERROR( "net_init_done: failed of create a new semaphore"
               , sys_sem_new(&_net_MsgSem, QUEUE_FREE(_net_MsgQue)) == ERR_OK
               , goto INIT_COMPLETE
              );
    LOG_INFO("Loading TCP/IP stack to lwIP v%s", LWIP_VERSION_STRING);

    net_hostname_init();        // Init hostname
    netdev_sysyem_init();       // Init and Add all network device to system
    net_init_apps();            // Init system apps

    sys_thread_new(  NET_POLL_THREAD_NAME
                   , net_poll_thread, NULL
                   , NET_POLL_THREAD_STKB
                   , NET_POLL_THREAD_PRIO
                  );            // Creat net_poll_thread

    INIT_COMPLETE:              // Initialization complete
    if (init_sem != NULL) { sys_sem_signal(init_sem); }
}


/*----------------------------------------------------------------------------*/
#if LWIP_NETIF_STATUS_CALLBACK
//lint -esym(401, net_status_callback)
//lint -esym(528, net_status_callback)
/**
 * Network device interface callback function for the device up/down or address changed
 */
__WEAK void net_status_callback(void *netif)
{
    struct netif *netifc = netif;

    if (netif_is_up(netifc))
    {
        LOG_INFO("Network interface \"%c%c\" UP", netifc->name[0], netifc->name[1]);
#if LWIP_IPV4
        LOG_INFO( "Network interface \"%c%c\" IP address : %s"
                 , netifc->name[0], netifc->name[1], inet_ntoa(*netif_ip4_addr(netifc))
                );
        LOG_INFO( "Network interface \"%c%c\" Subnet Mask: %s"
                 , netifc->name[0], netifc->name[1], inet_ntoa(*netif_ip4_netmask(netifc))
                );
        LOG_INFO( "Network interface \"%c%c\" Def-Gateway: %s"
                 , netifc->name[0], netifc->name[1], inet_ntoa(*netif_ip4_gw(netifc))
                );
#endif
#if LWIP_IPV6
#endif
    } else {
        LOG_INFO("Network interface \"%c%c\" DOWN", netifc->name[0], netifc->name[1]);
    }
}
#endif


#if LWIP_NETIF_LINK_CALLBACK
//lint -esym(401, net_link_callback)
//lint -esym(528, net_link_callback)
/**
 * Network device interface callback function for the device link-up/link-down
 */
__WEAK void net_link_callback(void *netif)
{
    struct netif *netifc = netif;

    if (netif_is_link_up(netifc)) {
        LOG_INFO("Network interface \"%c%c\" Link-UP",   netifc->name[0], netifc->name[1]);
    } else {
        LOG_INFO("Network interface \"%c%c\" Link-DOWN", netifc->name[0], netifc->name[1]);
    }
}
#endif


/*----------------------------------------------------------------------------*/
/**
 * Initialize hostname by read from config item
 */
static void net_hostname_init(void)
{
    const char *str = rtcfg_string_read(
        &_netcfg_hostname, _net_hostname, sizeof(_net_hostname)
    );

    if (str == NULL) {
        str =  HOST_NAME;
    }
    if (str != (_net_hostname)) {
        strncpy(_net_hostname, str, sizeof(_net_hostname));
        _net_hostname[sizeof(_net_hostname) - 1] = '\0';
    }
}


/**
 * Set hostname and write to config item
 */
int net_hostname_set(const char *str)
{
    if (str == NULL) {
        str =  HOST_NAME;
    }
    if (str != (_net_hostname)) {
        strncpy(_net_hostname, str, sizeof(_net_hostname));
        _net_hostname[sizeof(_net_hostname) - 1] = '\0';
    }
    return( rtcfg_string_write(&_netcfg_hostname, _net_hostname) );
}


/**
 * Get the hostname
 */
const char *net_hostname_get(void)
{
    return( _net_hostname );
}


/**
 * Get the server port
 */
uint16_t net_srvport_get(void)
{
    return( (uint16_t)rtcfg_int32_read(&_netcfg_srvport) );
}


/*----------------------------------------------------------------------------*/
//lint -esym(534, cli_fprintf, cli_fechof)
/**
 * 显示默认网络接口设备 DHCP 自动分配 IP 地址
 */
static int cmd_lwip_autoip(int cli, const char* args)
{
#if LWIP_IPV4
    char                buf[20];
#endif
    const netdev_info_t *netdev;
    struct netif        *netif;
    UNUSED_VAR(args);

    if((netdev = netdev_device_get(0)) == NULL) {
        cli_fprintf(cli, "Unfound network interface device"ENDL);
        return( 1 );
    }
    if((netif = netdev->netif) == NULL) {
        cli_fprintf(cli, "Invalid network interface device"ENDL);
        return( 1 );
    }

#if LWIP_IPV4
    if((netdev_dhcp_read(NULL) == 0) || !netif_is_up(netif) || !netif_is_link_up(netif)) {
        cli_fprintf(cli, "Auto Ip: 0.0.0.0"  "; ");
        cli_fprintf(cli, "Auto Mask: 0.0.0.0""; ");
        cli_fprintf(cli, "Auto Gw: 0.0.0.0"  ENDL);
    } else {
        cli_fprintf(cli, "Auto Ip: %s"  "; ", inet_ntoa_r(*netif_ip4_addr(netif)   , buf, sizeof(buf)));
        cli_fprintf(cli, "Auto Mask: %s""; ", inet_ntoa_r(*netif_ip4_netmask(netif), buf, sizeof(buf)));
        cli_fprintf(cli, "Auto Gw: %s"  ENDL, inet_ntoa_r(*netif_ip4_gw(netif)     , buf, sizeof(buf)));
    }
#endif

    return( 0 );
}


/**
 * 显示 lwIP 协议栈信息
 */
static int cmd_lwip_stats(int cli, const char* args)
{
#if LWIP_IPV4
    char            buf[20];
#endif
    struct netif   *netif;
    UNUSED_VAR(cli);
    UNUSED_VAR(args);

    LWIP_PLATFORM_DIAG(("\n----------------------------------------\n"));

    NETIF_FOREACH(netif)
    {
        LWIP_PLATFORM_DIAG(( "netif: \"%c%c\" %s\n\t"
                            , netif->name[0], netif->name[1]
                            , netif_is_link_up(netif) ? "Link-UP" : "Link-DOWN"
                          ));
#if LWIP_IPV4
        LWIP_PLATFORM_DIAG(( "IP address:  %s\n\t"
                            , inet_ntoa_r(*netif_ip4_addr(netif), buf, sizeof(buf))
                          ));
        LWIP_PLATFORM_DIAG(( "Subnet Mask: %s\n\t"
                            , inet_ntoa_r(*netif_ip4_netmask(netif), buf, sizeof(buf))
                          ));
        LWIP_PLATFORM_DIAG(( "Def-Gateway: %s\n"
                            , inet_ntoa_r(*netif_ip4_gw(netif), buf, sizeof(buf))
                          ));
#endif
    }

#if LWIP_STATS_DISPLAY
    stats_display();    // Display of lwIP statistics
#endif

    LWIP_PLATFORM_DIAG(("\n----------------------------------------\n\n"));
    return( 0 );
}


#if(NET_CUST_PSS_INS == 0)
static int cmd_ip_read(int cli, const char* args)
{
    char        buf[16 + 4];
    const char *str;
    UNUSED_VAR(args);

    if((str = netdev_ip_read(NULL, buf, sizeof(buf))) == NULL) {
        cli_fechof(cli, "Error of read IP address from config"ENDL);
        return( 1 );
    }
    cli_fprintf(cli, "%s"ENDL, str);
    return( 0 );
}


static int cmd_ip_write(int cli, const char* args)
{
    char    buf[16 + 4];

    if (sscanf(args, "%*s%17s", buf) != 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    if (netdev_ip_write(NULL, buf) != 0) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    cli_fechof(cli, "Set the config IP address to %s"ENDL, buf);
    return( 0 );
}
#endif


#if(NET_CUST_PSS_INS == 0)
static int cmd_mask_read(int cli, const char* args)
{
    char        buf[16 +4];
    const char *str;
    UNUSED_VAR(args);

    if((str = netdev_mask_read(NULL, buf, sizeof(buf))) == NULL) {
        cli_fechof(cli, "Error of read Subnet-Mask from config"ENDL);
        return( 1 );
    }
    cli_fprintf(cli, "%s"ENDL, str);
    return( 0 );
}


static int cmd_mask_write(int cli, const char* args)
{
    char    buf[16 + 4];

    if (sscanf(args, "%*s%17s", buf) != 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    if (netdev_mask_write(NULL, buf) != 0) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    cli_fechof(cli, "Set the config Subnet-Mask to %s"ENDL, buf);
    return( 0 );
}
#endif


#if(NET_CUST_PSS_INS == 0)
static int cmd_gw_read(int cli, const char* args)
{
    char        buf[16 + 4];
    const char *str;
    UNUSED_VAR(args);

    if((str = netdev_gw_read(NULL, buf, sizeof(buf))) == NULL) {
        cli_fechof(cli, "Error of read Default-Gateway from config"ENDL);
        return( 1 );
    }
    cli_fprintf(cli, "%s"ENDL, str);
    return( 0 );
}


static int cmd_gw_write(int cli, const char* args)
{
    char    buf[16 + 4];

    if (sscanf(args, "%*s%17s", buf) != 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    if (netdev_gw_write(NULL, buf) != 0) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    cli_fechof(cli, "Set the config Default-Gateway to %s"ENDL, buf);
    return( 0 );
}
#endif


#if(NET_CUST_PSS_INS == 0)
static int cmd_dhcp_read(int cli, const char* args)
{
    int32_t     dhcp;
    UNUSED_VAR(args);

    if((dhcp = netdev_dhcp_read(NULL)) < 0) {
        cli_fechof(cli, "Error of read DHCP status from config"ENDL);
        return( 1 );
    }
    cli_fprintf(cli, (dhcp > 0) ? "Auto"ENDL : "Static"ENDL);
    return( 0 );
}


static int cmd_dhcp_write(int cli, const char* args)
{
    char    buf[16];
    int32_t dhcp;

    if (sscanf(args, "%*s%7s", buf) != 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    if (cli_strtrimcmp(buf, "Static") == 0) {
        dhcp = 0;
    } else
    if (cli_strtrimcmp(buf, "Auto") == 0) {
        dhcp = 1;
    } else {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    cli_fechof(cli, "Set the config DHCP status to %s"ENDL, buf);
    netdev_dhcp_write(NULL, dhcp);  //lint !e534
    return( 0 );
}
#endif


static int cmd_mac_read(int cli, const char* args)
{
    char        buf[20];
    const char *str;
    UNUSED_VAR(args);

    if((str = netdev_ethmac_read(NULL, buf, sizeof(buf))) == NULL) {
        cli_fechof(cli, "Error of read MAC address from config"ENDL);
        return( 1 );
    }
    cli_fprintf(cli, "%s"ENDL, str);
    return( 0 );
}


static int cmd_mac_write(int cli, const char* args)
{
    char    buf[20];

    if (sscanf(args, "%*s%19s", buf) != 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    if (netdev_ethmac_write(NULL, buf) != 0) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    cli_fechof(cli, "Set the config MAC address to %s"ENDL, buf);
    return( 0 );
}


static int cmd_port_read(int cli, const char* args)
{
    UNUSED_VAR(args);

    cli_fprintf(cli, "%li"ENDL, rtcfg_int32_read(&_netcfg_srvport));
    return( 0 );
}


static int cmd_port_write(int cli, const char* args)
{
    int32_t  port;

    if (sscanf(args, "%*s%li", &port) != 1) {      //lint !e706
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    else {
        rtcfg_int32_write(&_netcfg_srvport, port); //lint !e534
        cli_fechof(cli, "Set the config Socket Port to %li"ENDL, port);
        return( 0 );
    }
}


#if(NET_CUST_PSS_INS == 1)
static int cmd_ipaddr_query(int cli, const char* args)
{
#if LWIP_IPV4
    char                buf[20];
    struct in_addr      ipaddr[3];
#endif
    const netdev_info_t *netdev;
    struct netif        *netif;
    int                  dhcp;
    UNUSED_VAR(args);

    if((dhcp = netdev_dhcp_read(NULL)) < 0) {
        cli_fechof(cli, "Error of read DHCP status from config"ENDL);
        return( 1 );
    }
#if LWIP_IPV4
    memset(&(ipaddr[0]), 0, sizeof(ipaddr[0]));
    memset(&(ipaddr[1]), 0, sizeof(ipaddr[1]));
    memset(&(ipaddr[2]), 0, sizeof(ipaddr[2])); //lint !e866
#endif

    if (dhcp > 0)
    {
        if((netdev = netdev_device_get(0)) != NULL) {
            if((netif = netdev->netif) != NULL) {
                if(netif_is_up(netif) && netif_is_link_up(netif)) {
#if LWIP_IPV4
                    memcpy(&(ipaddr[0]), netif_ip4_addr   (netif), sizeof(ipaddr[0]));
                    memcpy(&(ipaddr[1]), netif_ip4_netmask(netif), sizeof(ipaddr[1]));
                    memcpy(&(ipaddr[2]), netif_ip4_gw     (netif), sizeof(ipaddr[2])); //lint !e866
#endif
                }
            }
        }
        cli_fprintf(cli, "AUTO");
    }
    else
    {
        if (netdev_ipaddr_get(NULL, ipaddr) < 0) {
            cli_fechof(cli, "Error of read IP address from config"ENDL);
            return( 1 );
        }
        cli_fprintf(cli, "MAN");
    }
#if LWIP_IPV4
    cli_fprintf(cli, ",%s"    , inet_ntoa_r(ipaddr[0], buf, sizeof(buf))); //lint !e740
    cli_fprintf(cli, ",%s"    , inet_ntoa_r(ipaddr[1], buf, sizeof(buf))); //lint !e740
    cli_fprintf(cli, ",%s"ENDL, inet_ntoa_r(ipaddr[2], buf, sizeof(buf))); //lint !e740
#endif

    return( 0 );
}


static int cmd_ipaddr_conf(int cli, const char* args)
{
    int     num;
    char    mod[8];
    char    ip [16 + 4];
    char    msk[16 + 4];
    char    gw [16 + 4];

    // 兼容 :SYST:COMM:LAN:CONF "MAN,192.168.1.11,255.255.255.0,192.168.1.1" 
    if((num = sscanf(args, "%*s \" %7[^ \t\r\n\",], %19[.0123456789], %19[.0123456789], %19[.0123456789]", mod, ip, msk, gw)) < 1) {
        num = sscanf(args, "%*s " "%7[^ \t\r\n\",], %19[.0123456789], %19[.0123456789], %19[.0123456789]", mod, ip, msk, gw);
    }
    switch(num)
    {
    case 4  :
        if (netdev_ip_write(NULL, cli_strdelspace(ip)) != 0) {
            goto ERR_ARG;
        }
        if (netdev_mask_write(NULL, cli_strdelspace(msk)) != 0) {
            goto ERR_ARG;
        }
        if (netdev_gw_write(NULL, cli_strdelspace(gw)) != 0) {
            goto ERR_ARG;
        }
    case 1  :   //lint !e616 !e825
        if (cli_strtrimcmp(mod, "AUTO") == 0) {
            netdev_dhcp_write(NULL, 1); //lint !e534
            return( 0 );
        }
        if (cli_strtrimcmp(mod, "MAN") == 0) {
            netdev_dhcp_write(NULL, 0); //lint !e534
            return( 0 );
        }
        break;
    default :   goto ERR_ARG;
    }

  ERR_ARG:
    cli_fechof(cli, "Error of command format"ENDL);
    return( 1 );
}


static void net_ipaddr_update_do(void *arg)
{
    netdev_ipaddr_updata(arg);  //lint !e534
}                               //lint !e818
static int cmd_ipaddr_update(int cli, const char* args)
{
    UNUSED_VAR(cli);
    UNUSED_VAR(args);
//  sys_timeout(0, net_ipaddr_update_do, NULL);
//  return( 0 );

    if (tcpip_timeout(0, net_ipaddr_update_do, NULL) == ERR_OK) {
        cli_fechof(cli, "Success for apply the IP and Subnet-Mask and Gateway"ENDL);
        return( 0 );
    } else {
        cli_fechof(cli, "Failed for apply the IP and Subnet-Mask and Gateway"ENDL);
        return( 1 );
    }
}
#endif


/**
 * Initialize shell command of network protocol stack
 */
int net_cmd_init(void)
{
    if (cli_cmd_reg("**SHOW:AUTOIP?", cmd_lwip_autoip, "Show of DHCP assign IP address") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg("**SHOW:LWIP?"  , cmd_lwip_stats , "Show of lwIP statistics") < 0) {
        return( -1 );
    }
#if(NET_CUST_PSS_INS == 0)
    if (cli_cmd_reg("**Read:IP"     , cmd_ip_read    , "The IP address Query") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg("**Set:IP"      , cmd_ip_write   , "The IP address Config") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg("**Read:Mask"   , cmd_mask_read  , "The Subnet Mask Query") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg("**Set:Mask"    , cmd_mask_write , "The Subnet Mask Config") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg("**Read:GW"     , cmd_gw_read    , "The Gateway address Query") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg("**Set:GW"      , cmd_gw_write   , "The Gateway address Config") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg("**Read:Mode"   , cmd_dhcp_read  , "The Mode of IP Query") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg("**Set:Mode"    , cmd_dhcp_write , "The Mode of IP Config, Static | Auto") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg("**Read:MAC"    , cmd_mac_read   , "The MAC address Query") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg("**Set:MAC"     , cmd_mac_write  , "The MAC address Config") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg("**Read:Port"   , cmd_port_read  , "The socket Port Query") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg("**Set:Port"    , cmd_port_write , "The socket Port Config") < 0) {
        return( -1 );
    }
#else
    if (cli_cmd_reg(":SYST:COMM:LAN:MAC?" , cmd_mac_read     , "The MAC address Query") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg(":SYST:COMM:LAN:MAC"  , cmd_mac_write    , "The MAC address Config (Reboot Apply)") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg(":SYST:COMM:LAN:CONF?", cmd_ipaddr_query , "The NIC IP Query") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg(":SYST:COMM:LAN:CONF" , cmd_ipaddr_conf  , "The NIC IP Config, AUTO | MAN") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg(":SYST:COMM:LAN:UPD"  , cmd_ipaddr_update, "The NIC IP arguments Update") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg(":SYST:COMM:LAN:PORT?", cmd_port_read    , "The socket Port Query") < 0) {
        return( -1 );
    }
    if (cli_cmd_reg(":SYST:COMM:LAN:PORT" , cmd_port_write   , "The socket Port Config (Reboot Apply)") < 0) {
        return( -1 );
    }
#endif

    return( 0 );
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

