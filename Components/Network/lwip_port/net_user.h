/*******************************************************************************
 * @file    net_user.h
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
#ifndef __NET_USER_H__
#define __NET_USER_H__
/******************************************************************************/

// Do not include any lwIP header files directly or indirectly
#include <stdint.h>     // Used: uint32_t

/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/**
 * @addtogroup Net_User
 * @{
 * @addt
 * @addtogroup              Exported_Types
 * @{
 ******************************************************************************/
/**
 * Function prototype for the network event callback function.
 *
 * @param  argi   Additional argument to pass to callback function
 * @param  argf   Additional argument to pass to callback function
 * @param  argp   Additional argument to pass to callback function
 */
typedef void (*net_event_fn)(uint32_t argi, void *argf, void *argp);

/**
 * Function prototype for the network poll callback function.
 *
 * @param  netif   The network device interface
 */
typedef void (*net_poll_fn)(void *netif);


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
/**
 * Callback a network function in the thread context.
 *
 * @param[in]  func   The network poll callback function
 * @param[in]  netif  The network device interface
 */
void net_callpoll_thd(net_poll_fn  func, void *netif);

/**
 * Callback a network function in the ISR context.
 *
 * @param[in]  func   The network poll callback function
 * @param[in]  netif  The network device interface
 */
void net_callpoll_isr(net_poll_fn  func, void *netif);

/**
 * Callback a specific function in the thread context.
 *
 * @param[in]  func   the function to callback
 * @param[in]  argi   Additional argument to pass to callback function
 * @param[in]  argf   Additional argument to pass to callback function
 * @param[in]  argp   Additional argument to pass to callback function
 */
void net_callback_thd(net_event_fn func, uint32_t argi, void *argf, void *argp);

/**
 * Callback a specific function in the ISR context.
 *
 * @param[in]  func   the function to callback
 * @param[in]  argi   Additional argument to pass to callback function
 * @param[in]  argf   Additional argument to pass to callback function
 * @param[in]  argp   Additional argument to pass to callback function
 */
void net_callback_isr(net_event_fn func, uint32_t argi, void *argf, void *argp);


/*----------------------------------------------------------------------------*/
/**
 * The main function of network protocol stack. It handle protocol timeouts
 * and polling network device event.
 *
 * @retval  (>= 0) Call the function again
 * @retval  ( < 0) End of network protocol stack
 */
int net_main(void);

/**
 * Initialize network protocol stack and network device.
 *
 * @retval  (== 0) Success
 * @retval  (!= 0) Failed
 */
int net_init(void);

/**
 * Initialize shell command of network protocol stack
 *
 * @retval  (== 0) succeed
 * @retval  (!= 0) failed
 */
int net_cmd_init(void);

/**
 * Network device interface callback function for the device up/down or address changed
 *
 * @param[in]  netif   The network device interface
 */
void net_status_callback(void *netif);

/**
 * Network device interface callback function for the device link-up/link-down
 *
 * @param[in]  netif   The network device interface
 */
void net_link_callback  (void *netif);


/*----------------------------------------------------------------------------*/
/**
 * Set hostname and write to config item
 *
 * @param[in]  hostname   hostname string to be set and write, (NULL) used default value
 *
 * @retval  (== 0) Succeed
 * @retval  (!= 0) failed
 */
int net_hostname_set(const char* hostname);

/**
 * Get the hostname
 */
const char *net_hostname_get(void);

/**
 * Get the server port
 */
uint16_t net_srvport_get(void);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __NET_USER_H__
/*****************************  END OF FILE  **********************************/

