/*******************************************************************************
 * @file    Cmd_TCP.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.3
 * @date    2019/6/27
 * @brief   
 *******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) Wuhan Precise Co.,Ltd</center></h2>
 *
 * The information and source code contained herein is the exclusive
 * property of Wuhan Precise Electronic Technology Co.,Ltd. And may
 * not be disclosed, examined or reproduced in whole or in part without
 * explicit written authorization from the company.
 *
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2022/8/17 -- Linghu -- Fixed the high network delay bug by the nagle
 *
 * @par Change Logs:
 *      2022/8/15 -- Linghu -- Add Event Recorder Execution Statistics D(15)
 *
 * @par Change Logs:
 *      2020/9/27 -- Linghu -- Add support Multi-Port Single-Connect
 *
 * @par Change Logs:
 *      2019/6/27 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -emacro(572, htons, ntohs, htonl, ntohl)
//lint -emacro(835, htons, ntohs, htonl, ntohl)
//lint -emacro(845, htons, ntohs, htonl, ntohl)
//lint -emacro(665, FD_SET, FD_CLR, FD_ISSET)
//lint -emacro(702, FD_SET, FD_CLR, FD_ISSET, htons, htonl)
//lint -emacro(717, FD_SET, FD_CLR, FD_ISSET)
//lint --emacro({835}, FD_SET, FD_CLR, FD_ISSET)
//lint ++d__rev(v)=(v)

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "Cmd_TCP"
#include "Cmd_TCP.h"

#ifdef   _RTE_                  // CMSIS_device_header
#include "RTE_Components.h"     // Component selection
#endif// _RTE_
#ifdef    RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif

#ifndef  RTE_Network_Core
#include "emb_log.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "cmd/Cmd_App.h"
#include "shell/cli_shell.h"
#else
#include "assert.h"
#define  LOG_INFO(...)
#define  LOG_ASSERT(...)        assert_msg(0, __VA_ARGS__)
#include "Drv_LED.h"
#include "rl_net.h"
#include "Cmd_App.h"
#include "cli_shell.h"
#endif

#include <stdint.h>
#include <string.h>
#include <cmsis_os2.h>
#include "net_user.h"


/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup              Cmd_TCP
 * @{
 ******************************************************************************/

#ifndef NET_MAX_CONNECT
#define NET_MAX_CONNECT              -1         //!< (C) 网络服务最大连接数, (<= 0) Disable
#endif

#ifndef NET_MULTI_PORT
#define NET_MULTI_PORT                0         //!< (C) 单端口多连接(0) or 多端口单连接(1)
#endif

#ifndef NET_BIND_PORT_SLOT
#define NET_BIND_PORT_SLOT            0         //!< (C) 绑定 TCP端口 与 Slot位置
#endif                                          //   即: 9009 --> Slot 1 无条件转发
                                                //   即: 9010 --> Slot 2 无条件转发

#ifndef NET_SRV_THREAD_STKB
#define NET_SRV_THREAD_STKB          1024       //!< (C) 网络服务线程栈大小(以字节为单位)
#endif
#ifndef NET_SRV_THREAD_PRIO
#define NET_SRV_THREAD_PRIO   osPriorityNormal1 //!< (C) 网络服务线程优先级
#endif

#ifndef NET_CMD_STR_EN
#define NET_CMD_STR_EN               1          //!< (R) 是(1)/否(0) 支持字符串命令
#endif
#ifndef NET_CMD_BIN_EN
#define NET_CMD_BIN_EN               0          //!< (R) 是(1)/否(0) 支持二进制命令
#endif
#ifndef NET_FORWARD_IDX
#define NET_FORWARD_IDX             -1          //!< (R) 转发索引, (< 0)非目的接口
#endif
#ifndef NET_FORWARD_SRC
#define NET_FORWARD_SRC             -1          //!< (R) 转发索引, (< 0)非默认源接口
#endif


/*******************************************************************************
 * @}
 * @}
 */
#if (NET_MAX_CONNECT > 0)
/* @addtogroup Cmd_TCP
 * @{
 * @addtogroup              Pravate_Macros
 * @{
 ******************************************************************************/

#define CMD_CLI_FLAGS(bSTR, bBIN)   /*lint -save -w1 */            \
                                    (  ((bSTR) ? CLI_EXEC_STR : 0) \
                                     | ((bBIN) ? CLI_EXEC_BIN : 0) \
                                    )                              \
                                    /*lint -restore */

#if (NET_BIND_PORT_SLOT != 0)
#undef  NET_MULTI_PORT
#define NET_MULTI_PORT              1   // 绑定 TCP端口 与 Slot位置, 必须为多端口
#define NET_CLI_NEW_OPT           ( CMD_CLI_FLAGS(NET_CMD_STR_EN, NET_CMD_BIN_EN) | CLI_NO_SEARCH )
#else
#define NET_CLI_NEW_OPT             CMD_CLI_FLAGS(NET_CMD_STR_EN, NET_CMD_BIN_EN)
#endif

#ifdef  RTE_Network_Core
#if    (NET_MAX_CONNECT > 10) && (NET_MULTI_PORT != 0)
#undef  NET_MAX_CONNECT
#define NET_MAX_CONNECT             10  // MDK TCP/IP v7.10 最大支持 20个Socket 连接
#endif
#define socklen_t                   int
#define net_srvport_get()           cfg_netport_get()
extern long cfg_netport_get(void);
#endif


#ifndef ATOMIC_READ_32
#define ATOMIC_READ_32(var, ret)    ((ret) = (var))     // 原子读
#endif
#ifndef ATOMIC_WRITE_32
#define ATOMIC_WRITE_32(var, val)   ((var) = (val))     // 原子写
#endif

#if (NET_MULTI_PORT == 0)
#define NET_SRV_PORT_NUM                  1             //!< 网络服务端口数量
#else
#define NET_SRV_PORT_NUM            NET_MAX_CONNECT
#endif


/*******************************************************************************
 * @}
 * @addtogroup              Pravate_Variables
 * @{
 ******************************************************************************/

typedef struct net_device_ {
    cli_device_t        func;       // Operating function
    int                 sock;       // Socket
    struct sockaddr_in  addr;       // IP address and port number
    uint8_t             endl;       // Append Line end character
    const char         *name;       // Name of Network device
} net_device_t;


static net_device_t      _Net_CliDevice[NET_MAX_CONNECT];
static uint16_t          _Net_SrvPort;

static const char *const _Net_DeviceName[NET_MAX_CONNECT] = {
        "Network_1"
    #if (NET_MAX_CONNECT > 1)
      , "Network_2"
    #endif
    #if (NET_MAX_CONNECT > 2)
      , "Network_3"
    #endif
    #if (NET_MAX_CONNECT > 3)
      , "Network_4"
    #endif
    #if (NET_MAX_CONNECT > 4)
      , "Network_5"
    #endif
    #if (NET_MAX_CONNECT > 5)
      , "Network_6"
    #endif
    #if (NET_MAX_CONNECT > 6)
      , "Network_7"
    #endif
    #if (NET_MAX_CONNECT > 7)
      , "Network_8"
    #endif
    #if (NET_MAX_CONNECT > 8)
      , "Network_9"
    #endif
    #if (NET_MAX_CONNECT > 9)
      , "Network_10"
    #endif
    #if (NET_MAX_CONNECT > 10)
      , "Network_11"
    #endif
    #if (NET_MAX_CONNECT > 11)
      , "Network_12"
    #endif
    #if (NET_MAX_CONNECT > 12)
      , "Network_13"
    #endif
    #if (NET_MAX_CONNECT > 13)
      , "Network_14"
    #endif
    #if (NET_MAX_CONNECT > 14)
      , "Network_15"
    #endif
    #if (NET_MAX_CONNECT > 15)
      , "Network_16"
    #endif
    #if (NET_MAX_CONNECT > 16)
      , "Network_17"
    #endif
    #if (NET_MAX_CONNECT > 17)
      , "Network_18"
    #endif
    #if (NET_MAX_CONNECT > 18)
      , "Network_19"
    #endif
    #if (NET_MAX_CONNECT > 19)
      , "Network_20"
    #endif
    #if (NET_MAX_CONNECT > 20)
      , "Network_21"
    #endif
    #if (NET_MAX_CONNECT > 21)
      , "Network_22"
    #endif
    #if (NET_MAX_CONNECT > 22)
      , "Network_23"
    #endif
    #if (NET_MAX_CONNECT > 23)
      , "Network_24"
    #endif
    #if (NET_MAX_CONNECT > 24)
      , "Network_25"
    #endif
    #if (NET_MAX_CONNECT > 25)
      , "Network_26"
    #endif
    #if (NET_MAX_CONNECT > 26)
      , "Network_27"
    #endif
    #if (NET_MAX_CONNECT > 27)
      , "Network_28"
    #endif
    #if (NET_MAX_CONNECT > 28)
      , "Network_29"
    #endif
    #if (NET_MAX_CONNECT > 29)
      , "Network_30"
    #endif
    #if (NET_MAX_CONNECT > 30)
      , "Network_31"
    #endif
    #if (NET_MAX_CONNECT > 31)
      , "Network_32"
    #endif
    #if (NET_MAX_CONNECT > 32)
    #error "Macro NET_MAX_CONNECT is too large"
    #endif
};


/*******************************************************************************
 * @}
 * @addtogroup              Pravate_Functions
 * @{
 ******************************************************************************/
/**
 * Open a network devices of CLI
 *
 * @param[in]  device   Network devices of CLI.
 * @param[in]  cli      Command Line Interface.
 *
 * @retval  (>= 0) Succeed
 * @retval  ( < 0) Failed
 */
static int Net_DeviceOpen(net_device_t* device, int cli)
{
    struct sockaddr *addrptr = (struct sockaddr*)&( device->addr );  //lint !e740
    socklen_t        addrlen =              sizeof( device->addr );

    (void)device;
    device->endl = 0;   // Not exist append Line end character

    memset(&device->addr, 0, sizeof(device->addr));
    getpeername(device->sock, addrptr, &addrlen); //lint !e534

    if (NET_FORWARD_IDX >= 0) { //lint !e506 !e774
        LOG_INFO("CLI \"%s\" open  on TCP %s:%u, Forward Dst on Idx: %d"
                 ,           device->name
                 ,       inet_ntoa(device->addr.sin_addr)
                 , (unsigned)ntohs(device->addr.sin_port)
                 , (int     )NET_FORWARD_IDX
                );
        Cmd_ForwardSetIF((unsigned)NET_FORWARD_IDX, cli);           //lint !e534
    } else
    if (NET_FORWARD_SRC >= 0) { //lint !e506 !e774
        LOG_INFO("CLI \"%s\" opened on TCP %s:%u, Default Src on Idx: %d"
                 ,       device->name
                 ,       inet_ntoa(device->addr.sin_addr)
                 , (unsigned)ntohs(device->addr.sin_port)
                 , (int     )NET_FORWARD_SRC
                );
        Cmd_ForwardToIdx((unsigned)NET_FORWARD_SRC, cli, 0, NULL);  //lint !e534
    } else {
        LOG_INFO("CLI \"%s\" opened on TCP %s:%u"
                 ,       device->name
                 ,       inet_ntoa(device->addr.sin_addr)
                 , (unsigned)ntohs(device->addr.sin_port)
                );
    }
    return( 0 );
}


/**
 * Close a network devices of CLI
 *
 * @param[in]  device   Network devices of CLI.
 * @param[in]  cli      Command Line Interface.
 *
 * @retval  (>= 0) Succeed
 * @retval  ( < 0) Failed
 */
static int Net_DeviceClose(net_device_t* device, int cli)
{
    int              sock;
    struct sockaddr *addrptr = (struct sockaddr*)&( device->addr );  //lint !e740
    socklen_t        addrlen =              sizeof( device->addr );

    getpeername(device->sock, addrptr, &addrlen); //lint !e534
    LOG_INFO("CLI \"%s\" closed on TCP %s:%u"
             ,       device->name
             ,       inet_ntoa(device->addr.sin_addr)
             , (unsigned)ntohs(device->addr.sin_port)
            );
    ATOMIC_READ_32( device->sock, sock);
    ATOMIC_WRITE_32(device->sock, -1  );
    Cmd_ForwardRemIF(cli);    //lint !e534  " Ignoring return value "

    if(sock < 0)  return( 0 );
    return( closesocket((int)sock) );
}


/**
 * Read data from a network devices of CLI
 *
 * @param[in]  device   Network devices of CLI
 * @param[in]  buff     Pointer to buffer for read
 * @param[in]  size     length of buffer (in bytes)
 *
 * @retval  (>= 0) Number of bytes successfully read
 * @retval  ( = 0) Except of network
 */
static int Net_DeviceRead(net_device_t* device, void* buff, int size)
{
    int              len;
    struct sockaddr *addrptr = (struct sockaddr*)&( device->addr );  //lint !e740
    socklen_t        addrlen =              sizeof( device->addr );
    int              sock;          ATOMIC_READ_32( device->sock, sock );

    if (sock < 0) return( -1 );

//  if (device->endl) {     // Exist append Line end character
//      device->endl = 0;   return( 0 );
//  }
    if ((len = recvfrom((int)sock, buff, size, 0, addrptr, &addrlen)) <= 0) { //lint !e732
        return( -1 );
    }
//  if ((((char *)buff)[len - 1] != '\n') && (((char *)buff)[len - 1] != '\r')) {
//      device->endl = 1;   // Append Line end character
//  }

  #ifdef RTE_Compiler_EventRecorder
    EventStartDx(15, buff, len); //lint !e534 !e732 !e737 !e835
  #endif
    return( len );
}


/**
 * Write data to a network devices of CLI
 *
 * @param[in]  device   Network devices of CLI
 * @param[in]  data     Pointer to data for write
 * @param[in]  size     length of data (in bytes)
 *
 * @retval  (>= 0) Number of bytes successfully written
 * @retval  ( < 0) Except of network
 */
static int Net_DeviceWrite(net_device_t* device, const void* data, int size)
{
    int              len;
    struct sockaddr *addrptr = (struct sockaddr*)&( device->addr ); //lint !e740
    socklen_t        addrlen =              sizeof( device->addr );
    int              sock;          ATOMIC_READ_32( device->sock, sock );

    if (sock < 0) {
        return( -1 );
    }
    len = sendto((int)sock, data, size, 0, addrptr, addrlen);   //lint !e732

  #ifdef RTE_Compiler_EventRecorder
    EventStopDx(15, data, len);  //lint !e534 !e732 !e737 !e835
  #endif
    return( len );
}


/**
 * Alloc a network devices of CLI
 *
 * @param[in]  sock   socket descriptor 
 * @param[in]  idx    network devices index, (< 0)auto search
 *
 * @return  A pointer to network devices, (NULL)Failed
 */
static net_device_t* Net_DeviceAlloc(int sock, int idx)
{
    static size_t   i_Pos = 0;
    int             tmp;
    size_t          i, m;

    if (idx < 0) {
        i = 0;
        m = NET_MAX_CONNECT - 1;
    } else {
        i = idx;        //lint !e732
        m = idx;        //lint !e732
        i_Pos = idx;    //lint !e732
    }

    for(;  i <= m;  i++)
    {
        ATOMIC_READ_32(_Net_CliDevice[i_Pos].sock, tmp);
        if (tmp >= 0)
        {
            if (++i_Pos == NET_MAX_CONNECT)  i_Pos = 0;
            continue;
        }
        ATOMIC_WRITE_32(_Net_CliDevice[i_Pos].sock, sock);
        return( &_Net_CliDevice[i_Pos] );
    }

    return( NULL );
}


/**
 * Initialize all network devices of CLI
 */
static void Net_DeviceInit(void)
{
    for(size_t i = 0;  i < NET_MAX_CONNECT;  i++)
    {
        _Net_CliDevice[i].func.open  =  (cli_open_fn )Net_DeviceOpen;
        _Net_CliDevice[i].func.close =  (cli_close_fn)Net_DeviceClose;
        _Net_CliDevice[i].func.read  =  (cli_read_fn )Net_DeviceRead;
        _Net_CliDevice[i].func.write =  (cli_write_fn)Net_DeviceWrite;
        _Net_CliDevice[i].name       =  _Net_DeviceName[i];
        ATOMIC_WRITE_32(_Net_CliDevice[i].sock, -1  );
    }
    _Net_SrvPort = net_srvport_get(); //lint !e734  // Server Port of Config value
}


/*----------------------------------------------------------------------------*/
/**
 * Server thread function of Network.
 */
static void Net_SrvThread(void *arg)
{
    static const unsigned long  sock_mode = 0;
    static const int            opt_on    = 1;
    struct sockaddr_in          addr;
    int                         server[NET_SRV_PORT_NUM];
    int                         client;
    net_device_t               *device;
    int                         maxfdp;
    struct fd_set               fds;
    int                         i;

    (void)arg;
    Net_DeviceInit();   // Initialize all network devices of CLI

    for(i = maxfdp = 0;  i < NET_SRV_PORT_NUM;  i++)
    {
        memset(&addr, 0, sizeof(addr));
        addr.sin_family      =  AF_INET;
        addr.sin_port        =  htons(_Net_SrvPort + i); //lint !e734
        addr.sin_addr.s_addr =  htonl(INADDR_ANY);

        if ((server[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            LOG_ASSERT( "socket(AF_INET, SOCK_STREAM, 0)" );
        }
        if (bind(server[i], (struct sockaddr *)&addr, sizeof(addr)) < 0) { //lint !e740
            LOG_ASSERT("bind(server[i], (struct sockaddr *)&addr, sizeof(addr))");
        }
        if (ioctlsocket(server[i], FIONBIO, (unsigned long *)&sock_mode) < 0) {
            LOG_ASSERT("ioctlsocket(server[i], FIONBIO, (unsigned long *)&sock_mode)");
        }
        if (listen(server[i], (NET_SRV_PORT_NUM > 1) ? 1 : NET_MAX_CONNECT) < 0) { //lint !e506
            LOG_ASSERT("listen(server[i], NET_MAX_CONNECT)");
        }
        if (maxfdp < server[i]) {
            maxfdp = server[i];     // Used for select()
        }
        LOG_INFO("TCP Listen on %s:%u, Max Connect: %u", inet_ntoa(addr.sin_addr), (unsigned)ntohs(addr.sin_port), NET_MAX_CONNECT);
    }
    maxfdp += 1;

    while (1)   //lint !e716  " while(1) "
    {
        FD_ZERO(&fds);
        for(i = 0;  i < NET_SRV_PORT_NUM;  i++){ FD_SET(server[i], &fds); }
        if (select(maxfdp, &fds, NULL, NULL, NULL) <= 0) {
            continue;
        }

        for(i = 0;  i < NET_SRV_PORT_NUM;  i++)
        {
            if (!FD_ISSET(server[i], &fds)) {
                continue;
            }

            if ((client = accept(server[i], NULL, NULL)) < 0) {
                break;
            }
            if (ioctlsocket(client, FIONBIO, (unsigned long *)&sock_mode) < 0) {
                closesocket(client);    //lint !e534
                break;
            }
            if (setsockopt(client, SOL_SOCKET, SO_KEEPALIVE, (void *)&opt_on, sizeof(opt_on)) < 0) {
                closesocket(client);    //lint !e534
                break;
            }
            if (setsockopt(client, IPPROTO_TCP, TCP_NODELAY, (void *)&opt_on, sizeof(opt_on)) < 0) {
                closesocket(client);    //lint !e534
                break;
            }
            if ((device = Net_DeviceAlloc(client, ((NET_SRV_PORT_NUM == 1) ? -1 : i))) == NULL) { //lint !e506
                closesocket(client);    //lint !e534    // Alloc a network devices of CLI
                break;
            }
            if (cli_shell_new((cli_device_t*)device, device->name, NET_CLI_NEW_OPT) < 0) {
                closesocket(client);    //lint !e534    // Open a CLI
                break;
            }
        }
    }
}


void Cmd_TcpInit(void)
{
    static const osThreadAttr_t  _Net_SrvThreadAttr = {
        "Net_Server Thread", 0,  NULL, 0, NULL, NET_SRV_THREAD_STKB, NET_SRV_THREAD_PRIO, 0, 0
    };

    if (osThreadNew(Net_SrvThread, NULL, &_Net_SrvThreadAttr) == NULL) {
        LOG_ASSERT("osThreadNew(Net_SrvThread, NULL, &_Net_SrvThreadAttr)");
    }
#ifdef  RTE_Network_Core
    LED_SetON(DRV_LED_LAN);
#endif
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#endif
/*****************************  END OF FILE  **********************************/

