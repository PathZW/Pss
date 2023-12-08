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
#   define LWIP_NO_ASSERT                       0       // lwIP ����(assert)
#   define LWIP_DEBUG_EN                        1       // lwIP �������
#   define LWIP_STATS                           1       // lwIP ͳ����Ϣ����

#   define NO_SYS                               0       // �Ƿ�ʹ�ò���ϵͳ
#   define LWIP_TIMERS                          1       // ʹ�� lwIP �ṩ�Ķ�ʱ��
//# define LWIP_TIMERS_CUSTOM                   0       // �û��ṩ��ʱ��ʵ�ֺ���
#   define LWIP_TCPIP_TIMEOUT                   1       // ���� tcpip_timeout()

#   define LWIP_NETCONN_SEM_PER_THREAD          1       // �����ֲ߳̾��ź���
#   define LWIP_NETCONN_FULLDUPLEX              1       // ���������ڲ�ͬ�߳�ͬʱ����

//# define LWIP_MPU_COMPATIBLE                  0
//# define SYS_LIGHTWEIGHT_PROT                 1       // ����/�жϱ���(�ڴ�������ͷ�)
#   define LWIP_TCPIP_CORE_LOCKING              1       // ���������ǻص�����Э��ջ����
#   define LWIP_TCPIP_CORE_LOCKING_INPUT        1       // ���������� tcpip_input()

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
#   define DEFAULT_THREAD_NAME       "lwIP_Thread"      // �߳���: lwIP Ĭ���߳�
#   define DEFAULT_THREAD_STKB           1024           // �߳�ջ: lwIP Ĭ���߳�
#   define DEFAULT_THREAD_PRIO      osPriorityNormal    // ���ȼ�: lwIP Ĭ���߳�

#   define NET_POLL_THREAD_NAME      "lwIP_Driver"      // �߳���: ����������ѯ
#   define NET_POLL_THREAD_STKB          1024           // �߳�ջ: ����������ѯ
#   define NET_POLL_THREAD_PRIO      osPriorityHigh     // ���ȼ�: ����������ѯ
#   define NET_POLL_QUEUE_SIZE            16            // ���г�: ����������ѯ

#   define TCPIP_THREAD_NAME          "lwIP_Core"       // �߳���: lwIP �ں��߳�
#   define TCPIP_THREAD_STKB             1024           // �߳�ջ: lwIP �ں��߳�
#   define TCPIP_THREAD_PRIO      osPriorityAboveNormal // ���ȼ�: lwIP �ں��߳�
//# define TCPIP_MBOX_SIZE                8             // ���䳤: lwIP �ں��߳�
//# define LWIP_TCPIP_THREAD_ALIVE()

//# define SLIPIF_THREAD_NAME        "slipif_loop"      // �߳���: slipif �����߳�
//# define SLIPIF_THREAD_STKB             0             // �߳�ջ: slipif �����߳�
//# define SLIPIF_THREAD_PRIO        osPriorityHigh     // ���ȼ�: slipif �����߳�

#   define SNMP_THREAD_PRIO         DEFAULT_THREAD_PRIO // ���ȼ�: SNMP �����߳�
#   define SNMP_THREAD_STKB         DEFAULT_THREAD_STKB // �߳�ջ: SNMP �����߳�

//# define DEFAULT_RAW_RECVMBOX_SIZE      8             // ��������: RAW Э��
//# define DEFAULT_UDP_RECVMBOX_SIZE      8             // ��������: UDP Э��
//# define DEFAULT_TCP_RECVMBOX_SIZE      8             // ��������: TCP Э��
//# define DEFAULT_ACCEPTMBOX_SIZE        8             // ��������: Accept ����

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
#if ENABLE  /*----------------------- Memory ѡ�� -----------------------------*/
#   define MEM_ALIGNMENT                    4       // �ڴ����ߴ�

#   define MEM_LIBC_MALLOC                  1       // ʹ�� C ��Ķ��ڴ����
#   define MEM_USE_POOLS                    0       // �ڴ�� ģ�� ���ڴ����
//# define MEM_USE_POOLS_TRY_BIGGER_POOL    0       // ���ڴ����ʧ��ʱ, ����ʹ�ø�����ڴ��
//# define MEM_OVERFLOW_CHECK               0       // ���ڴ� ������
//# define MEM_SANITY_CHECK                 0       // ���ڴ� ��ȫ�Լ��

#   define MEMP_MEM_MALLOC                  0       // ʹ�ö��ڴ���� �ڴ�ط���
#   define MEMP_USE_CUSTOM_POOLS            0       // �Զ����ڴ�� lwippools.h
//# define MEMP_MEM_INIT                    0       // �ڴ�� ǿ�Ƴ�ʼ��
//# define MEMP_OVERFLOW_CHECK              0       // �ڴ�� ������
//# define MEMP_SANITY_CHECK                0       // �ڴ�� ��ȫ�Լ��

//# define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 0 // ������жϷ������ͷ��ڴ�(1)
#endif
#if ENABLE  /*----------------------- Memory ���� -----------------------------*/
#   if !defined(MEM_SIZE) && !MEM_LIBC_MALLOC && !MEM_USE_POOLS
#   define MEM_SIZE                    (16 * 1024)  // ���ڴ��С
#   endif

#   ifndef PBUF_POOL_SIZE
#   define PBUF_POOL_SIZE                   16 //16 // PBUF_POOL ����, ���ڽ������������ݰ�
#   endif
#   ifndef PBUF_POOL_BUFSIZE
#   define PBUF_POOL_BUFSIZE               1520     // PBUF_POOL ��С, ���ڽ������������ݰ�
#   endif
//# define PBUF_LINK_HLEN           (14 + ETH_PAD_SIZE)
//# define PBUF_LINK_ENCAPSULATION_HLEN     0

#   ifndef MEMP_NUM_RAW_PCB
#   define MEMP_NUM_RAW_PCB                 4  //4  // RAW ���ƿ�: raw_pcb
#   endif
#   ifndef MEMP_NUM_UDP_PCB
#   define MEMP_NUM_UDP_PCB                 4  //4  // UDP ���ƿ�: udp_pcb
#   endif
#   ifndef MEMP_NUM_TCP_PCB
#   define MEMP_NUM_TCP_PCB                 24 //5  // TCP ���ƿ�: tcp_pcb
#   endif
#   ifndef MEMP_NUM_TCP_PCB_LISTEN
#   define MEMP_NUM_TCP_PCB_LISTEN          24 //8  // TCP ������: tcp_pcb_listen
#   endif
#   ifndef MEMP_NUM_NETCONN
#   define MEMP_NUM_NETCONN    NUM_NETCONN_DFT //4  // NETCONN �� Socket API: netconn
#   endif
#   define NUM_NETCONN_DFT  (MEMP_NUM_UDP_PCB + MEMP_NUM_TCP_PCB + MEMP_NUM_TCP_PCB_LISTEN)

#   define MEMP_NUM_NETBUF                  2  //2  // NETCONN �� Socket API: netbuf
#   define MEMP_NUM_PBUF                    4  //16 // PBUF_ROM/PBUF_REF ����: pbuf
#   define MEMP_NUM_TCP_SEG                 32 //16 // �Ŷӵ�TCP��: tcp_seg
#   define MEMP_NUM_REASSDATA               4  //5  // �Ŷ������IP��: ip_reassdata
#   define MEMP_NUM_FRAG_PBUF               8  //15 // �Ŷӷ��͵�IP��Ƭ: pbuf_custom_ref
#   define MEMP_NUM_ARP_QUEUE               32 //30 // ARP��������: etharp_q_entry
#   define MEMP_NUM_IGMP_GROUP              8  //8  // �鲥����ӿ�����: igmp_group
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
#   define IP_FORWARD                       0           // IP ��ת��(������)
#   define IP_REASSEMBLY                    1           // IP �ϵ�����(����)
#   define IP_FRAG                          1           // IP �ϵ㷢��
//# define IP_OPTIONS_ALLOWED               1           // IP ѡ������
//# define IP_REASS_MAXAGE                  15          // IP �ֶ����ݰ����ʱ��
//# define IP_REASS_MAX_PBUFS  (MEMP_NUM_REASSDATA * 2) // IP �ֶ���װ����
//# define IP_DEFAULT_TTL                   255         // IP Ĭ�� TTL
//# define IP_SOF_BROADCAST                 0           // IP SOF �㲥
//# define IP_SOF_BROADCAST_RECV            0           // IP SOF �㲥����
//# define IP_FORWARD_ALLOW_TX_ON_RX_NETIF  0
#endif
#if ENABLE  /*--------------------- ARP (��ַ����Э��) ------------------------*/
#   define LWIP_ARP                         1
#   define ARP_TABLE_SIZE                   10          // ARP �������
//# define ARP_MAXAGE                       300         // ARP �����������
//# define ARP_QUEUEING                     0           // �Ŷ� ARP ����
//# define ARP_QUEUE_LEN                    3           // ARP ������г���
#   define ETHARP_SUPPORT_VLAN              0           // ARP vLan ֧��
#   define LWIP_ETHERNET                 LWIP_ARP       // ��̫���ӿ�֧��
#   define ETH_PAD_SIZE                     2           // ��̫����ͷǰ����ֽ�
//# define ETHARP_SUPPORT_STATIC_ENTRIES    0           // ARP ��̬��֧��
//# define ETHARP_TABLE_MATCH_NETIF  !LWIP_SINGLE_NETIF
#endif
#if ENABLE  /*---------------- DHCP and AutoIP (IP ��ַ����) ------------------*/
#   define LWIP_DHCP                        1           // ���� DHCP ģ��
#   define LWIP_AUTOIP                      0           // ���� AutoIP ģ��
//# define DHCP_DOES_ARP_CHECK  (LWIP_DHCP && LWIP_ARP) // ARP ��ַ���
//# define LWIP_DHCP_AUTOIP_COOP            0           // ͬʱ���� DHCP �� AutoIP
//# define LWIP_DHCP_AUTOIP_COOP_TRIES      9           // ͬʱ����ʱ DHCP ���Դ���
//# define LWIP_DHCP_GET_NTP_SRV            0
//# define LWIP_DHCP_MAX_NTP_SERVERS        1
//# define LWIP_DHCP_MAX_DNS_SERVERS  DNS_MAX_SERVERS
//# define LWIP_DHCP_BOOTP_FILE             0
#endif
#if ENABLE  /*----------------------- ICMP and IGMP --------------------------*/
#   define LWIP_ICMP                        1           // ���� ICMP ģ��(Ping ֧��)
#   define LWIP_IGMP                        0           // ���� IGMP ģ��(�鲥֧��)
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
#if ENABLE  /*-------------------------- RAW Э�� ----------------------------*/
#   define LWIP_RAW                         1
#endif
#if ENABLE  /*-------------------------- UDP Э�� ----------------------------*/
#   define LWIP_UDP                         1
//# define LWIP_UDPLITE                     0
#endif
#if ENABLE  /*-------------------------- TCP Э�� ----------------------------*/
#   define LWIP_TCP                         1
#   define LWIP_ALTCP                       0           // altcp API
#   define LWIP_ALTCP_TLS                   0           // altcp API �� TLS ֧��
#   define TCP_MSS          /* 536 */      1460         // TCP MSS (����ĳ���)
#   define TCP_WND                     (4 * TCP_MSS)    // TCP ����
#   define TCP_SND_BUF                 (4 * TCP_MSS)    // TCP ���ͻ���

#   define TCP_MAXRTX                       12          // TCP ����ش�����
#   define TCP_SYNMAXRTX                    6           // TCP syn ����ش�����
#   define LWIP_TCP_SACK_OUT                0           // TCP SACK ѡ��
#   define TCP_LISTEN_BACKLOG               0           // TCP backlog ѡ��
#   define TCP_DEFAULT_LISTEN_BACKLOG      255          // TCP backlog Ĭ��ֵ
#   define TCP_OVERSIZE               (TCP_MSS / 4)     // TCP ƴ������
#   define LWIP_TCP_TIMESTAMPS              0           // TCP ʱ���

//# define TCP_QUEUE_OOSEQ               LWIP_TCP       // ��ֹ�Խ�ʡ�ڴ�
//# define LWIP_WND_SCALE                   0
//# define TCP_RCV_SCALE                    0
#endif
#if ENABLE  /*------------------ NETCONN and Socket API ----------------------*/
#   define LWIP_NETCONN                     1
#   define LWIP_SOCKET                      1

#   define LWIP_COMPAT_SOCKETS              2           // BSD �׽��ֺ�����
#   define LWIP_POSIX_SOCKETS_IO_NAMES      0           // POSIX �����׽��ֺ���
#   define LWIP_SOCKET_OFFSET               0           // �׽����ļ������ƫ����

#   define LWIP_TCP_KEEPALIVE               1           // TCP ����(�������)
#   define TCP_KEEPIDLE_DEFAULT             10000UL     // TCP ����ʱ��(����)
#   define TCP_KEEPINTVL_DEFAULT            2000UL      // TCP ��������(����)
#   define TCP_KEEPCNT_DEFAULT              5U          // TCP ���������

#   define LWIP_SO_SNDTIMEO                 1           // ѡ��: SO_SNDTIMEO
#   define LWIP_SO_RCVTIMEO                 1           // ѡ��: SO_RCVTIMEO
#   define LWIP_SO_SNDRCVTIMEO_NONSTANDARD  0           // ��ʱʱ������: int / timeval
#   define LWIP_SO_RCVBUF                   1           // ѡ��: SO_RCVBUF
#   define RECV_BUFSIZE_DEFAULT             8192        // Ĭ��ֵ: RCVBUF
//# define LWIP_SO_LINGER                   0           // ѡ��: SO_LINGER
//# define SO_REUSE                         0           // ѡ��: SO_REUSEADDR
//# define SO_REUSE_RXTOALL                 0
//# define LWIP_FIONREAD_LINUXMODE          0           // linux ������
//# define LWIP_SOCKET_SELECT               1           // ����֧��: select()
//# define LWIP_SOCKET_POLL                 1           // ����֧��: poll()
#endif
#if ENABLE  /*------------------------- DNS ģ�� -----------------------------*/
#   define LWIP_DNS                         1
//# define DNS_TABLE_SIZE                   4           // DNS ����¼��
//# define DNS_MAX_NAME_LENGTH              256         // DNS ����󳤶�
//# define DNS_MAX_SERVERS                  2           // DNS �����������
//# define DNS_MAX_RETRIES                  4           // DNS ������Դ���
//# define DNS_DOES_NAME_CHECK              1           // DNS ��ӦУ��
//# define DNS_LOCAL_HOSTLIST               0           // DNS ���������б�
//# define DNS_LOCAL_HOSTLIST_IS_DYNAMIC    0           // DNS ���������б���޸�
//# define LWIP_DNS_SUPPORT_MDNS_QUERIES    0           // mDNS ��ѯ֧��
#endif


/*******************************************************************************
 * @}
 * @brief   Network Interfaces AND Checksum options, Ref: lwip/opt.h
 * @{
 */
#if ENABLE
#   define LWIP_CHECKSUM_CTRL_PER_NETIF     0           // �ӿ� ��/ӲУ�� ������
#   define LWIP_SINGLE_NETIF                0           // ��֧�ֵ�һ����ӿ�
#   define LWIP_NETIF_HOSTNAME              1           // hostname
#   define LWIP_NETIF_API                   0           // �ӿ��̰߳�ȫ API
#   define LWIP_NETIF_STATUS_CALLBACK       1           // ״̬�ı�ص�����(up/down)
#   define LWIP_NETIF_EXT_STATUS_CALLBACK   0           // ��չ�ص�����
#   define LWIP_NETIF_LINK_CALLBACK         1           // ����״̬�ص�����(link)
#   define LWIP_NETIF_REMOVE_CALLBACK       0           // �ӿ���ȥ�ص�����(remove)
#   define LWIP_NETIF_HWADDRHINT            0           // �ӿڻ�����·���ַ
//# define LWIP_NETIF_TX_SINGLE_PBUF        0           // ����ʱ pbuf ������
//# define LWIP_NUM_NETIF_CLIENT_DATA       0

//# define LWIP_HAVE_LOOPIF                 1           // �ػ��ӿ�: 127.0.0.1
//# define LWIP_LOOPIF_MULTICAST            0           // �ػ��ӿ�: �ಥ/IGMP
//# define LWIP_NETIF_LOOPBACK              0           // �ػ��ӿ�: �ӿ��Լ���ַ
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
#   define LWIP_STATS_DISPLAY               1           // ͳ����Ϣ: stats_display()

#   define LINK_STATS                       1           // ͳ����Ϣ: ��·��
#   define ETHARP_STATS                     0           // ͳ����Ϣ: ARP
#   define IP_STATS                         0           // ͳ����Ϣ: IP
#   define IPFRAG_STATS                     0           // ͳ����Ϣ: IP��Ƭ
#   define ICMP_STATS                       0           // ͳ����Ϣ: ICMP
#   define IGMP_STATS                       0           // ͳ����Ϣ: IGMP
#   define UDP_STATS                        0           // ͳ����Ϣ: UDP
#   define TCP_STATS                        0           // ͳ����Ϣ: TCP
#   define MEM_STATS                        1           // ͳ����Ϣ: ���ڴ�
#   define MEMP_STATS                       1           // ͳ����Ϣ: �ڴ��
#   define SYS_STATS                        1           // ͳ����Ϣ: ����ϵͳ
#   define IP6_STATS                        0           // ͳ����Ϣ: IPv6
#   define ICMP6_STATS                      0           // ͳ����Ϣ: IPv6 ICMP
#   define IP6_FRAG_STATS                   0           // ͳ����Ϣ: IPv6��Ƭ
#   define MLD6_STATS                       0           // ͳ����Ϣ: IPv6 MDL
#   define ND6_STATS                        0           // ͳ����Ϣ: IPv6 ND
#   define MIB2_STATS                       1           // ͳ����Ϣ: SNMP MIB2
#endif
#if ENABLE
#   define LWIP_DBG_TYPES_ON    (           0           /* ������Ϣ: ���� */ \
                                 /* | LWIP_DBG_TRACE */                     \
                                 /* | LWIP_DBG_STATE */                     \
                                 /* | LWIP_DBG_FRESH */                     \
                                    | LWIP_DBG_ON                           \
                                )
//# define LWIP_DBG_MIN_LEVEL      LWIP_DBG_LEVEL_ALL   // ������Ϣ: �ȼ�
#   define LWIP_DBG_MIN_LEVEL    LWIP_DBG_LEVEL_WARNING // ������Ϣ: �ȼ�

#   define NETIF_DEBUG                LWIP_DBG_ON       // ������Ϣ: netif
//# define ETHARP_DEBUG               LWIP_DBG_ON       // ������Ϣ: ARP
//# define PBUF_DEBUG                 LWIP_DBG_ON       // ������Ϣ: pbuf.c
//# define API_LIB_DEBUG              LWIP_DBG_ON       // ������Ϣ: api_lib.c
//# define API_MSG_DEBUG              LWIP_DBG_ON       // ������Ϣ: api_msg.c
//# define SOCKETS_DEBUG              LWIP_DBG_ON       // ������Ϣ: sockets.c
//# define ICMP_DEBUG                 LWIP_DBG_ON       // ������Ϣ: icmp.c
//# define IGMP_DEBUG                 LWIP_DBG_ON       // ������Ϣ: igmp.c
//# define INET_DEBUG                 LWIP_DBG_ON       // ������Ϣ: inet.c
//# define IP_DEBUG                   LWIP_DBG_ON       // ������Ϣ: IP
//# define IP_REASS_DEBUG             LWIP_DBG_ON       // ������Ϣ: ip_frag.c
//# define RAW_DEBUG                  LWIP_DBG_ON       // ������Ϣ: raw.c
//# define MEM_DEBUG                  LWIP_DBG_ON       // ������Ϣ: mem.c
//# define MEMP_DEBUG                 LWIP_DBG_ON       // ������Ϣ: memp.c
//# define SYS_DEBUG                  LWIP_DBG_ON       // ������Ϣ: sys.c
//# define TIMERS_DEBUG               LWIP_DBG_ON       // ������Ϣ: timers.c
//# define TCP_DEBUG                  LWIP_DBG_ON       // ������Ϣ: TCP
//# define TCP_INPUT_DEBUG            LWIP_DBG_ON       // ������Ϣ: tcp_in.c
//# define TCP_FR_DEBUG               LWIP_DBG_ON       // ������Ϣ: tcp_in.c
//# define TCP_RTO_DEBUG              LWIP_DBG_ON       // ������Ϣ: TCP �ش�
//# define TCP_CWND_DEBUG             LWIP_DBG_ON       // ������Ϣ: TCP ӵ������
//# define TCP_WND_DEBUG              LWIP_DBG_ON       // ������Ϣ: tcp_in.c
//# define TCP_OUTPUT_DEBUG           LWIP_DBG_ON       // ������Ϣ: tcp_out.c
//# define TCP_RST_DEBUG              LWIP_DBG_ON       // ������Ϣ: TCP RST ��Ϣ
//# define TCP_QLEN_DEBUG             LWIP_DBG_ON       // ������Ϣ: TCP ���г���
//# define UDP_DEBUG                  LWIP_DBG_ON       // ������Ϣ: UDP
//# define TCPIP_DEBUG                LWIP_DBG_ON       // ������Ϣ: tcpip.c
//# define SLIP_DEBUG                 LWIP_DBG_ON       // ������Ϣ: slipif.c
//# define DHCP_DEBUG                 LWIP_DBG_ON       // ������Ϣ: dhcp.c
//# define AUTOIP_DEBUG               LWIP_DBG_ON       // ������Ϣ: autoip.c
//# define DNS_DEBUG                  LWIP_DBG_ON       // ������Ϣ: DNS
//# define IP6_DEBUG                  LWIP_DBG_ON       // ������Ϣ: IPv6
//# define DHCP6_DEBUG                LWIP_DBG_ON       // ������Ϣ: dhcp6.c

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

