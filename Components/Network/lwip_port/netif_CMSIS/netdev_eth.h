/*******************************************************************************
 * @file    netdev_eth.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/11/14
 * @brief   Ethernet interface abstraction layer
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/11/14 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __NETDEV_ETH_H__
#define __NETDEV_ETH_H__
/******************************************************************************/

// Do not include any lwIP header files directly or indirectly
#include <stdint.h>             // Used: uint32_t
#include "Driver_ETH_MAC.h"
#include "Driver_ETH_PHY.h"

/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/**
 * @addtogroup NetDev_ETH
 * @{
 * @addtogroup              Exported_Macros
 * @{
 ******************************************************************************/
#ifndef NET_DEV_ETH_STATE_DEFS
/**
 * Defined a ethernet interface device state data. 
 *
 * @param[in]  drv   ethernet MAC and PHY CMSIS driver number
 */
#define NET_DEV_eth_STATE_DEFS(drv)                                   \
    extern ARM_DRIVER_ETH_MAC        ARM_Driver_ETH_MAC_(drv);        \
    extern ARM_DRIVER_ETH_PHY        ARM_Driver_ETH_PHY_(drv);        \
    extern const netdev_info_t   _NET_DEV_INFO_NAME(eth, drv);        \
    static void   ATPASTE3(netdev_eth, drv, _notify)(uint32_t event)  \
    {                                                                 \
        netdev_eth_notify(_NET_DEV_INFO_NAME(eth, drv).netif, event); \
    }                                                                 \
    static netdev_eth_t  _NET_DEV_STATE_NAME(eth, drv) =              \
    {                                                                 \
        &_NET_DEV_INFO_NAME(eth, drv)        ,  /* device  */         \
        &ARM_Driver_ETH_MAC_(drv)            ,  /* mac     */         \
        &ARM_Driver_ETH_PHY_(drv)            ,  /* phy     */         \
        (ATPASTE3(netdev_eth, drv, _notify)) ,  /* notify  */         \
        (ARM_ETH_LINK_DOWN)                  ,  /* link    */         \
        0                                       /* init_ok */         \
    }
#endif//NET_DEV_eth_STATE_DEFS


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Types
 * @{
 ******************************************************************************/

typedef struct netdev_eth_s {           //!< Ethernet interface device state data
    const void                *device;  //!< Network device information
    ARM_DRIVER_ETH_MAC        *mac;     //!< CMSIS-Driver MAC
    ARM_DRIVER_ETH_PHY        *phy;     //!< CMSIS-Driver PHY
    ARM_ETH_MAC_SignalEvent_t  notify;  //!< Ethernet notify event callback function
    ARM_ETH_LINK_STATE         link;    //!< Ethernet Link State
    uint32_t                   init_ok; //!< Ethernet initialized successfully
} netdev_eth_t;


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/

    int  netdev_eth_init(void *netif);
//  void netdev_eth_poll(void *netif);
    int  netdev_eth_link(void *netif);

    void netdev_eth_notify(void *netif, uint32_t event);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __NETDEV_ETH_H__
/*****************************  END OF FILE  **********************************/

