/*******************************************************************************
 * @file    lwipopts.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/11/13
 * @brief   lwIP Options Configuration
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/11/13 -- Linghu -- the first version
 ******************************************************************************/
#ifndef LWIP_LWIPOPTS_H
#define LWIP_LWIPOPTS_H
/******************************************************************************/
#ifndef EXTERN_C
#ifdef  __cplusplus
#define EXTERN_C        extern "C"
#else
#define EXTERN_C        extern
#endif
#endif
#ifndef ENABLE
#define ENABLE          1
#endif
#ifndef DISABLE
#define DISABLE         0
#endif
/*******************************************************************************
 * @{
 * @brief   NO_SYS, Timers, Core locking, MPU, Thread options, Ref: lwip/opt.h
 * @{
 */
#if ENABLE  /*------------------------------- System -------------------------*/
#   define LWIP_NO_ASSERT                       0       // lwIP 断言(assert)
#   define LWIP_DEBUG_EN                        1       // lwIP 调试输出
#   define LWIP_STATS                           1       // lwIP 统计信息功能

#   define NO_SYS                               0       // 是否使用操作系统
#   define LWIP_TIMERS                          1       // 使用 lwIP 提供的定时器
//# define LWIP_TIMERS_CUSTOM                   0       // 用户提供定时器实现函数
#   define LWIP_TCPIP_TIMEOUT                   1       // 启用 tcpip_timeout()

#   define LWIP_NETCONN_SEM_PER_THREAD          1       // 启用线程局部信号量
#   define LWIP_NETCONN_FULLDUPLEX              1       // 允许连接在不同线程同时操作

//# define LWIP_MPU_COMPATIBLE                  0
//# define SYS_LIGHTWEIGHT_PROT                 1       // 任务/中断保护(内存分配与释放)
#   define LWIP_TCPIP_CORE_LOCKING              1       // 互斥量而非回调保护协议栈操作
#   define LWIP_TCPIP_CORE_LOCKING_INPUT        1       // 互斥量保护 tcpip_input()

#if !NO_SYS
#   define LWIP_ASSERT_CORE_LOCKED()  LWIP_ASSERT( "Function called without core lock" \
                                                  , sys_core_is_locked()               \
                                                 )
#   define LWIP_MARK_TCPIP_THREAD()                 sys_mark_tcpip_thread()
    EXTERN_C void sys_mark_tcpip_thread(void);
    EXTERN_C int  sys_core_is_locked   (void);
#endif
#endif
#if ENABLE  /*-------------------- - Thread options --------------------------*/
#   define DEFAULT_THREAD_NAME       "lwIP_Thread"      // 线程名: lwIP 默认线程
#   define DEFAULT_THREAD_STKB           1024           // 线程栈: lwIP 默认线程
#   define DEFAULT_THREAD_PRIO      osPriorityNormal    // 优先级: lwIP 默认线程

#   define NET_POLL_THREAD_NAME      "lwIP_Driver"      // 线程名: 网络驱动轮询
#   define NET_POLL_THREAD_STKB          1024           // 线程栈: 网络驱动轮询
#   define NET_POLL_THREAD_PRIO      osPriorityHigh     // 优先级: 网络驱动轮询
#   define NET_POLL_QUEUE_SIZE            16            // 队列长: 网络驱动轮询

#   define TCPIP_THREAD_NAME          "lwIP_Core"       // 线程名: lwIP 内核线程
#   define TCPIP_THREAD_STKB             1024           // 线程栈: lwIP 内核线程
#   define TCPIP_THREAD_PRIO      osPriorityAboveNormal // 优先级: lwIP 内核线程
//# define TCPIP_MBOX_SIZE                8             // 邮箱长: lwIP 内核线程
//# define LWIP_TCPIP_THREAD_ALIVE()

//# define SLIPIF_THREAD_NAME        "slipif_loop"      // 线程名: slipif 驱动线程
//# define SLIPIF_THREAD_STKB             0             // 线程栈: slipif 驱动线程
//# define SLIPIF_THREAD_PRIO        osPriorityHigh     // 优先级: slipif 驱动线程

#   define SNMP_THREAD_PRIO         DEFAULT_THREAD_PRIO // 优先级: SNMP 管理线程
#   define SNMP_THREAD_STKB         DEFAULT_THREAD_STKB // 线程栈: SNMP 管理线程

//# define DEFAULT_RAW_RECVMBOX_SIZE      8             // 接收邮箱: RAW 协议
//# define DEFAULT_UDP_RECVMBOX_SIZE      8             // 接收邮箱: UDP 协议
//# define DEFAULT_TCP_RECVMBOX_SIZE      8             // 接收邮箱: TCP 协议
//# define DEFAULT_ACCEPTMBOX_SIZE        8             // 接收邮箱: Accept 操作

#   ifdef  DEFAULT_THREAD_STKB
#   define DEFAULT_THREAD_STACKSIZE  DEFAULT_THREAD_STKB
#   endif
#   ifdef  TCPIP_THREAD_STKB
#   define TCPIP_THREAD_STACKSIZE     TCPIP_THREAD_STKB
#   endif
#   ifdef  SLIPIF_THREAD_STKB
#   define SLIPIF_THREAD_STACKSIZE    SLIPIF_THREAD_STKB
#   endif
#   ifdef  SNMP_THREAD_STKB
#   define SNMP_STACK_SIZE             SNMP_THREAD_STKB
#   endif
#endif


/*******************************************************************************
 * @}
 * @brief   Heap Memory, Memory Pool AND Pbuf, Ref: lwip/opt.h
 * @{
 */
#if ENABLE  /*----------------------- Memory 选项 -----------------------------*/
#   define MEM_ALIGNMENT                    4       // 内存对齐尺寸

#   define MEM_LIBC_MALLOC                  1       // 使用 C 库的堆内存分配
#   define MEM_USE_POOLS                    0       // 内存池 模拟 堆内存分配
//# define MEM_USE_POOLS_TRY_BIGGER_POOL    0       // 堆内存分配失败时, 尝试使用更大的内存池
//# define MEM_OVERFLOW_CHECK               0       // 堆内存 溢出检测
//# define MEM_SANITY_CHECK                 0       // 堆内存 健全性检测

#   define MEMP_MEM_MALLOC                  0       // 使用堆内存替代 内存池分配
#   define MEMP_USE_CUSTOM_POOLS            0       // 自定义内存池 lwippools.h
//# define MEMP_MEM_INIT                    0       // 内存池 强制初始化
//# define MEMP_OVERFLOW_CHECK              0       // 内存池 溢出检测
//# define MEMP_SANITY_CHECK                0       // 内存池 健全性检测

//# define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 0 // 允许从中断服务函数释放内存(1)
#endif
#if ENABLE  /*----------------------- Memory 分配 -----------------------------*/
#   if !defined(MEM_SIZE) && !MEM_LIBC_MALLOC && !MEM_USE_POOLS
#   define MEM_SIZE                    (16 * 1024)  // 堆内存大小
#   endif

#   ifndef PBUF_POOL_SIZE
#   define PBUF_POOL_SIZE                   16 //16 // PBUF_POOL 数量, 用于接收驱动层数据包
#   endif
#   ifndef PBUF_POOL_BUFSIZE
#   define PBUF_POOL_BUFSIZE               1520     // PBUF_POOL 大小, 用于接收驱动层数据包
#   endif
//# define PBUF_LINK_HLEN           (14 + ETH_PAD_SIZE)
//# define PBUF_LINK_ENCAPSULATION_HLEN     0

#   ifndef MEMP_NUM_RAW_PCB
#   define MEMP_NUM_RAW_PCB                 4  //4  // RAW 控制块: raw_pcb
#   endif
#   ifndef MEMP_NUM_UDP_PCB
#   define MEMP_NUM_UDP_PCB                 4  //4  // UDP 控制块: udp_pcb
#   endif
#   ifndef MEMP_NUM_TCP_PCB
#   define MEMP_NUM_TCP_PCB                 24 //5  // TCP 控制块: tcp_pcb
#   endif
#   ifndef MEMP_NUM_TCP_PCB_LISTEN
#   define MEMP_NUM_TCP_PCB_LISTEN          24 //8  // TCP 监听数: tcp_pcb_listen
#   endif
#   ifndef MEMP_NUM_NETCONN
#   define MEMP_NUM_NETCONN    NUM_NETCONN_DFT //4  // NETCONN 和 Socket API: netconn
#   endif
#   define NUM_NETCONN_DFT  (MEMP_NUM_UDP_PCB + MEMP_NUM_TCP_PCB + MEMP_NUM_TCP_PCB_LISTEN)

#   define MEMP_NUM_NETBUF                  2  //2  // NETCONN 和 Socket API: netbuf
#   define MEMP_NUM_PBUF                    4  //16 // PBUF_ROM/PBUF_REF 发送: pbuf
#   define MEMP_NUM_TCP_SEG                 32 //16 // 排队的TCP段: tcp_seg
#   define MEMP_NUM_REASSDATA               4  //5  // 排队重组的IP包: ip_reassdata
#   define MEMP_NUM_FRAG_PBUF               8  //15 // 排队发送的IP分片: pbuf_custom_ref
#   define MEMP_NUM_ARP_QUEUE               32 //30 // ARP队列数量: etharp_q_entry
#   define MEMP_NUM_IGMP_GROUP              8  //8  // 组播网络接口数量: igmp_group
#   define MEMP_NUM_SELECT_CB               4  //4  // lwip_select_cb
#   define MEMP_NUM_TCPIP_MSG_API           8  //8  // tcpip_msg (callback/timeout API)
#   define MEMP_NUM_TCPIP_MSG_INPKT         8  //8  // tcpip_msg (incoming packets)
#   define MEMP_NUM_NETDB                   1  //1  // Used: getaddrinfo()
#   define MEMP_NUM_LOCALHOSTLIST           1  //1  // Used: dns_local_addhost()

#   define MEMP_NUM_SYS_TIMEOUT   (2 + LWIP_NUM_SYS_TIMEOUT_INTERNAL)  // sys_timeo
//# define MEMP_NUM_API_MSG                    MEMP_NUM_TCPIP_MSG_API  // api_msg
//# define MEMP_NUM_DNS_API_MSG                MEMP_NUM_TCPIP_MSG_API  // dns_api_msg
//# define MEMP_NUM_SOCKET_SETGETSOCKOPT_DATA  MEMP_NUM_TCPIP_MSG_API  // lwip_setgetsockopt_data
//# define MEMP_NUM_NETIFAPI_MSG               MEMP_NUM_TCPIP_MSG_API  // netifapi_msg
//# define MEMP_NUM_ALTCP_PCB                  MEMP_NUM_TCP_PCB        // altcp_pcb
#endif


/*******************************************************************************
 * @}
 * @brief   IPv4 options, Ref: lwip/opt.h
 * @{
 */
#if ENABLE  /*--------------------------- IPv4 -------------------------------*/
#   define LWIP_IPV4                        1
#   define IP_FORWARD                       0           // IP 包转发(跨网卡)
#   define IP_REASSEMBLY                    1           // IP 断点续传(接收)
#   define IP_FRAG                          1           // IP 断点发送
//# define IP_OPTIONS_ALLOWED               1           // IP 选项允许
//# define IP_REASS_MAXAGE                  15          // IP 分段数据包最大时间
//# define IP_REASS_MAX_PBUFS  (MEMP_NUM_REASSDATA * 2) // IP 分段组装条数
//# define IP_DEFAULT_TTL                   255         // IP 默认 TTL
//# define IP_SOF_BROADCAST                 0           // IP SOF 广播
//# define IP_SOF_BROADCAST_RECV            0           // IP SOF 广播过滤
//# define IP_FORWARD_ALLOW_TX_ON_RX_NETIF  0
#endif
#if ENABLE  /*--------------------- ARP (地址解析协议) ------------------------*/
#   define LWIP_ARP                         1
#   define ARP_TABLE_SIZE                   10          // ARP 缓存表数
//# define ARP_MAXAGE                       300         // ARP 缓存表生存期
//# define ARP_QUEUEING                     0           // 排队 ARP 请求
//# define ARP_QUEUE_LEN                    3           // ARP 请求队列长度
#   define ETHARP_SUPPORT_VLAN              0           // ARP vLan 支持
#   define LWIP_ETHERNET                 LWIP_ARP       // 以太网接口支持
#   define ETH_PAD_SIZE                     2           // 以太网包头前填充字节
//# define ETHARP_SUPPORT_STATIC_ENTRIES    0           // ARP 静态表支持
//# define ETHARP_TABLE_MATCH_NETIF  !LWIP_SINGLE_NETIF
#endif
#if ENABLE  /*---------------- DHCP and AutoIP (IP 地址分配) ------------------*/
#   define LWIP_DHCP                        1           // 启用 DHCP 模块
#   define LWIP_AUTOIP                      0           // 启用 AutoIP 模块
//# define DHCP_DOES_ARP_CHECK  (LWIP_DHCP && LWIP_ARP) // ARP 地址检查
//# define LWIP_DHCP_AUTOIP_COOP            0           // 同时启用 DHCP 与 AutoIP
//# define LWIP_DHCP_AUTOIP_COOP_TRIES      9           // 同时启用时 DHCP 重试次数
//# define LWIP_DHCP_GET_NTP_SRV            0
//# define LWIP_DHCP_MAX_NTP_SERVERS        1
//# define LWIP_DHCP_MAX_DNS_SERVERS  DNS_MAX_SERVERS
//# define LWIP_DHCP_BOOTP_FILE             0
#endif
#if ENABLE  /*----------------------- ICMP and IGMP --------------------------*/
#   define LWIP_ICMP                        1           // 启用 ICMP 模块(Ping 支持)
#   define LWIP_IGMP                        0           // 启用 IGMP 模块(组播支持)
#endif


/*******************************************************************************
 * @}
 * @brief   IPv6 options, Ref: lwip/opt.h
 * @{
 */
#if DISABLE /*--------------------------- IPv6 -------------------------------*/
#   define LWIP_IPV6                        0
#endif


/*******************************************************************************
 * @}
 * @brief   RAW UDP TCP NETCONN Socket options, Ref: lwip/opt.h
 * @{
 */
#if ENABLE  /*-------------------------- RAW 协议 ----------------------------*/
#   define LWIP_RAW                         1
#endif
#if ENABLE  /*-------------------------- UDP 协议 ----------------------------*/
#   define LWIP_UDP                         1
//# define LWIP_UDPLITE                     0
#endif
#if ENABLE  /*-------------------------- TCP 协议 ----------------------------*/
#   define LWIP_TCP                         1
#   define LWIP_ALTCP                       0           // altcp API
#   define LWIP_ALTCP_TLS                   0           // altcp API 的 TLS 支持
#   define TCP_MSS          /* 536 */      1460         // TCP MSS (最大报文长度)
#   define TCP_WND                     (4 * TCP_MSS)    // TCP 窗口
#   define TCP_SND_BUF                 (4 * TCP_MSS)    // TCP 发送缓存

#   define TCP_MAXRTX                       12          // TCP 最大重传次数
#   define TCP_SYNMAXRTX                    6           // TCP syn 最大重传次数
#   define LWIP_TCP_SACK_OUT                0           // TCP SACK 选项
#   define TCP_LISTEN_BACKLOG               0           // TCP backlog 选项
#   define TCP_DEFAULT_LISTEN_BACKLOG      255          // TCP backlog 默认值
#   define TCP_OVERSIZE               (TCP_MSS / 4)     // TCP 拼包长度
#   define LWIP_TCP_TIMESTAMPS              0           // TCP 时间戳

//# define TCP_QUEUE_OOSEQ               LWIP_TCP       // 禁止以节省内存
//# define LWIP_WND_SCALE                   0
//# define TCP_RCV_SCALE                    0
#endif
#if ENABLE  /*------------------ NETCONN and Socket API ----------------------*/
#   define LWIP_NETCONN                     1
#   define LWIP_SOCKET                      1

#   define LWIP_COMPAT_SOCKETS              2           // BSD 套接字函数名
#   define LWIP_POSIX_SOCKETS_IO_NAMES      0           // POSIX 风格的套接字函数
#   define LWIP_SOCKET_OFFSET               0           // 套接字文件描叙符偏移量

#   define LWIP_TCP_KEEPALIVE               1           // TCP 保活(死链检测)
#   define TCP_KEEPIDLE_DEFAULT             10000UL     // TCP 保活时间(毫秒)
#   define TCP_KEEPINTVL_DEFAULT            2000UL      // TCP 保活包间隔(毫秒)
#   define TCP_KEEPCNT_DEFAULT              5U          // TCP 保活包数量

#   define LWIP_SO_SNDTIMEO                 1           // 选项: SO_SNDTIMEO
#   define LWIP_SO_RCVTIMEO                 1           // 选项: SO_RCVTIMEO
#   define LWIP_SO_SNDRCVTIMEO_NONSTANDARD  0           // 超时时间类型: int / timeval
#   define LWIP_SO_RCVBUF                   1           // 选项: SO_RCVBUF
#   define RECV_BUFSIZE_DEFAULT             8192        // 默认值: RCVBUF
//# define LWIP_SO_LINGER                   0           // 选项: SO_LINGER
//# define SO_REUSE                         0           // 选项: SO_REUSEADDR
//# define SO_REUSE_RXTOALL                 0
//# define LWIP_FIONREAD_LINUXMODE          0           // linux 兼容性
//# define LWIP_SOCKET_SELECT               1           // 函数支持: select()
//# define LWIP_SOCKET_POLL                 1           // 函数支持: poll()
#endif
#if ENABLE  /*------------------------- DNS 模块 -----------------------------*/
#   define LWIP_DNS                         1
//# define DNS_TABLE_SIZE                   4           // DNS 最大记录数
//# define DNS_MAX_NAME_LENGTH              256         // DNS 名最大长度
//# define DNS_MAX_SERVERS                  2           // DNS 服务器最大数
//# define DNS_MAX_RETRIES                  4           // DNS 最大重试次数
//# define DNS_DOES_NAME_CHECK              1           // DNS 响应校验
//# define DNS_LOCAL_HOSTLIST               0           // DNS 本地主机列表
//# define DNS_LOCAL_HOSTLIST_IS_DYNAMIC    0           // DNS 本地主机列表可修改
//# define LWIP_DNS_SUPPORT_MDNS_QUERIES    0           // mDNS 查询支持
#endif


/*******************************************************************************
 * @}
 * @brief   Network Interfaces AND Checksum options, Ref: lwip/opt.h
 * @{
 */
#if ENABLE
#   define LWIP_CHECKSUM_CTRL_PER_NETIF     0           // 接口 软/硬校验 可配置
#   define LWIP_SINGLE_NETIF                0           // 仅支持单一网络接口
#   define LWIP_NETIF_HOSTNAME              1           // hostname
#   define LWIP_NETIF_API                   0           // 接口线程安全 API
#   define LWIP_NETIF_STATUS_CALLBACK       1           // 状态改变回调函数(up/down)
#   define LWIP_NETIF_EXT_STATUS_CALLBACK   0           // 扩展回调函数
#   define LWIP_NETIF_LINK_CALLBACK         1           // 链接状态回调函数(link)
#   define LWIP_NETIF_REMOVE_CALLBACK       0           // 接口移去回调函数(remove)
#   define LWIP_NETIF_HWADDRHINT            0           // 接口缓存链路层地址
//# define LWIP_NETIF_TX_SINGLE_PBUF        0           // 发送时 pbuf 单缓存
//# define LWIP_NUM_NETIF_CLIENT_DATA       0

//# define LWIP_HAVE_LOOPIF                 1           // 回环接口: 127.0.0.1
//# define LWIP_LOOPIF_MULTICAST            0           // 回环接口: 多播/IGMP
//# define LWIP_NETIF_LOOPBACK              0           // 回环接口: 接口自己地址
//# define LWIP_LOOPBACK_MAX_PBUFS          0
//# define LWIP_HAVE_LOOPIF  (LWIP_NETIF_LOOPBACK && !LWIP_SINGLE_NETIF)
#endif


/*******************************************************************************
 * @}
 * @brief   Hook options, Ref: lwip/opt.h
 * @{
 */
#if DISABLE
#   define LWIP_HOOK_FILENAME "path/to/my/lwip_hooks.h"
#   define LWIP_HOOK_TCP_ISN(local_ip, local_port, remote_ip, remote_port)
#endif


/*******************************************************************************
 * @}
 * @brief   Statistics AND Debugging options, Ref: lwip/opt.h
 * @{
 */
#if LWIP_STATS
#   define LWIP_STATS_DISPLAY               1           // 统计信息: stats_display()

#   define LINK_STATS                       1           // 统计信息: 链路层
#   define ETHARP_STATS                     0           // 统计信息: ARP
#   define IP_STATS                         0           // 统计信息: IP
#   define IPFRAG_STATS                     0           // 统计信息: IP分片
#   define ICMP_STATS                       0           // 统计信息: ICMP
#   define IGMP_STATS                       0           // 统计信息: IGMP
#   define UDP_STATS                        0           // 统计信息: UDP
#   define TCP_STATS                        0           // 统计信息: TCP
#   define MEM_STATS                        1           // 统计信息: 堆内存
#   define MEMP_STATS                       1           // 统计信息: 内存池
#   define SYS_STATS                        1           // 统计信息: 操作系统
#   define IP6_STATS                        0           // 统计信息: IPv6
#   define ICMP6_STATS                      0           // 统计信息: IPv6 ICMP
#   define IP6_FRAG_STATS                   0           // 统计信息: IPv6分片
#   define MLD6_STATS                       0           // 统计信息: IPv6 MDL
#   define ND6_STATS                        0           // 统计信息: IPv6 ND
#   define MIB2_STATS                       1           // 统计信息: SNMP MIB2
#endif
#if ENABLE
#   define LWIP_DBG_TYPES_ON    (           0           /* 调试信息: 类型 */ \
                                 /* | LWIP_DBG_TRACE */                     \
                                 /* | LWIP_DBG_STATE */                     \
                                 /* | LWIP_DBG_FRESH */                     \
                                    | LWIP_DBG_ON                           \
                                )
//# define LWIP_DBG_MIN_LEVEL      LWIP_DBG_LEVEL_ALL   // 调试信息: 等级
#   define LWIP_DBG_MIN_LEVEL    LWIP_DBG_LEVEL_WARNING // 调试信息: 等级

#   define NETIF_DEBUG                LWIP_DBG_ON       // 调试信息: netif
//# define ETHARP_DEBUG               LWIP_DBG_ON       // 调试信息: ARP
//# define PBUF_DEBUG                 LWIP_DBG_ON       // 调试信息: pbuf.c
//# define API_LIB_DEBUG              LWIP_DBG_ON       // 调试信息: api_lib.c
//# define API_MSG_DEBUG              LWIP_DBG_ON       // 调试信息: api_msg.c
//# define SOCKETS_DEBUG              LWIP_DBG_ON       // 调试信息: sockets.c
//# define ICMP_DEBUG                 LWIP_DBG_ON       // 调试信息: icmp.c
//# define IGMP_DEBUG                 LWIP_DBG_ON       // 调试信息: igmp.c
//# define INET_DEBUG                 LWIP_DBG_ON       // 调试信息: inet.c
//# define IP_DEBUG                   LWIP_DBG_ON       // 调试信息: IP
//# define IP_REASS_DEBUG             LWIP_DBG_ON       // 调试信息: ip_frag.c
//# define RAW_DEBUG                  LWIP_DBG_ON       // 调试信息: raw.c
//# define MEM_DEBUG                  LWIP_DBG_ON       // 调试信息: mem.c
//# define MEMP_DEBUG                 LWIP_DBG_ON       // 调试信息: memp.c
//# define SYS_DEBUG                  LWIP_DBG_ON       // 调试信息: sys.c
//# define TIMERS_DEBUG               LWIP_DBG_ON       // 调试信息: timers.c
//# define TCP_DEBUG                  LWIP_DBG_ON       // 调试信息: TCP
//# define TCP_INPUT_DEBUG            LWIP_DBG_ON       // 调试信息: tcp_in.c
//# define TCP_FR_DEBUG               LWIP_DBG_ON       // 调试信息: tcp_in.c
//# define TCP_RTO_DEBUG              LWIP_DBG_ON       // 调试信息: TCP 重传
//# define TCP_CWND_DEBUG             LWIP_DBG_ON       // 调试信息: TCP 拥塞处理
//# define TCP_WND_DEBUG              LWIP_DBG_ON       // 调试信息: tcp_in.c
//# define TCP_OUTPUT_DEBUG           LWIP_DBG_ON       // 调试信息: tcp_out.c
//# define TCP_RST_DEBUG              LWIP_DBG_ON       // 调试信息: TCP RST 消息
//# define TCP_QLEN_DEBUG             LWIP_DBG_ON       // 调试信息: TCP 队列长度
//# define UDP_DEBUG                  LWIP_DBG_ON       // 调试信息: UDP
//# define TCPIP_DEBUG                LWIP_DBG_ON       // 调试信息: tcpip.c
//# define SLIP_DEBUG                 LWIP_DBG_ON       // 调试信息: slipif.c
//# define DHCP_DEBUG                 LWIP_DBG_ON       // 调试信息: dhcp.c
//# define AUTOIP_DEBUG               LWIP_DBG_ON       // 调试信息: autoip.c
//# define DNS_DEBUG                  LWIP_DBG_ON       // 调试信息: DNS
//# define IP6_DEBUG                  LWIP_DBG_ON       // 调试信息: IPv6
//# define DHCP6_DEBUG                LWIP_DBG_ON       // 调试信息: dhcp6.c

#if defined(LWIP_NO_ASSERT) && (LWIP_NO_ASSERT) && !defined(LWIP_NOASSERT)
#   define  LWIP_NOASSERT
#endif
#if defined(LWIP_DEBUG_EN)  && (LWIP_DEBUG_EN)  && !defined(LWIP_DEBUG)
#   define  LWIP_DEBUG
#endif
#endif


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#undef ENABLE
#undef DISABLE
#endif  // LWIP_LWIPOPTS_H
/*****************************  END OF FILE  **********************************/

