/*******************************************************************************
 * @file    net_device.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/11/13
 * @brief   Network device abstraction
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/11/13 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __NET_DEVICE_H__
#define __NET_DEVICE_H__
/******************************************************************************/

// Do not include any lwIP header files directly or indirectly
#include <stdint.h>             // Used: uint8_t
#include <arpa/inet.h>
#include "emb_rtcfg.h"

/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/**
 * @addtogroup Net_Device
 * @{
 * @addtogroup              Exported_Types
 * @{
 ******************************************************************************/

typedef struct netcfg_eth_s {   //!< Ethernet private config
    rtcfg_string_t      hwaddr; //!< hardware address
    rtcfg_int32_t       vlan;   //!< vlan ID
} netcfg_eth_t;

typedef struct netcfg_ip4_s {   //!< IPv4 protocol config
    rtcfg_int32_t       dhcp;   //!< DHCP client
    rtcfg_string_t      ip;     //!< IP address
    rtcfg_string_t      mask;   //!< Subnet-Mask
    rtcfg_string_t      gw;     //!< Default-Gateway
    rtcfg_string_t      dns1;   //!< Primary DNS
    rtcfg_string_t      dns2;   //!< Secondary DNS
} netcfg_ip4_t;

typedef struct netcfg_ip6_s {   //!< IPv6 protocol config
    rtcfg_int32_t       dhcp;   //!< key of DHCP client
    rtcfg_string_t      ip;     //!< key of IP address
    rtcfg_string_t      dns1;   //!< Primary DNS
    rtcfg_string_t      dns2;   //!< Secondary DNS
} netcfg_ip6_t;

typedef struct netdev_info_s {      //!< Network device information
    const char         *ifname;     //!< device interface name
    rtcfg_int32_t       cfgmtu;     //!< MTU value
    const netcfg_ip4_t *cfgip4;     //!< device config of IPv4
    const netcfg_ip6_t *cfgip6;     //!< device config of IPv6
    const void         *cfgpriv;    //!< device config of private

    void               *netif;      //!< device interface
    void               *state;      //!< device private state,

    int  (*inpkt)(void *pkt, void *netif); //!< fun for passed packet to IP layer
    int  (*init )(void *netif);     //!< device function for Initialize
    void (*poll )(void *netif);     //!< device function for Poll
    int  (*link )(void *netif);     //!< device function for Link status Check
    unsigned            invl;       //!< Interval of link status check
} netdev_info_t;


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
/**
 * Init and Add all network device to system
 */
void netdev_sysyem_init(void);

/**
 * Get the network device from network device table
 */
/**
 * Get the network device from network device table
 * 
 * @param[in]  idx  Index for network device table, (0) default device
 * 
 * @return     const netdev_info_t* 
 */
const netdev_info_t *netdev_device_get(unsigned idx);

/**
 * Add a network device to the system.
 *
 * @param[in]  device   network device, (NULL) default device
 *
 * @retval  (== 0) succeed
 * @retval  (!= 0) failed
 */
int netdev_device_add(const netdev_info_t *device);

/**
 * Remove a network device from the system.
 *
 * @param[in]  device   network device, (NULL) default device
 *
 * @retval  (== 0) succeed
 * @retval  (!= 0) failed
 */
int netdev_device_remove(const netdev_info_t *device);

/**
 * Update the IP and Mask and Gateway from config item 
 * 
 * @param[in]  device   network device, (NULL) used default device
 *
 * @retval  (== 0) succeed
 * @retval  (!= 0) failed
 */
int netdev_ipaddr_updata(const netdev_info_t *device);


/*----------------------------------------------------------------------------*/
/**
 * Get binary IP and Mask and Gateway from config item
 *
 * @param[in]  device   network device, (NULL) default device
 * @param[out] addr     buffer of get binary IP address
 *
 * @retval  (>= 0) number of the get binary IP address
 * @retval  ( < 0) failed
 */
int netdev_ipaddr_get(const netdev_info_t *device, struct in_addr addr[3]);

/**
 * Read IP string from config item
 *
 * @param[in]  device   network device, (NULL) default device
 * @param[out] buf      buffer for string to read into
 * @param[in]  len      buffer length in bytes
 *
 * @retval  (!= NULL) address string
 * @retval  (== NULL) failed
 */
const char *netdev_ip_read(const netdev_info_t *device, char *buf, unsigned len);

/**
 * Write IP string to config item
 *
 * @param[in]  device   network device, (NULL) default device
 * @param[in]  str      address string to write
 *
 * @retval  (== 0) succeed
 * @retval  (!= 0) failed
 */
int netdev_ip_write(const netdev_info_t *device, const char *str);

/**
 * Read Subnet-Mask string from config item
 *
 * @param[in]  device   network device, (NULL) default device
 * @param[out] buf      buffer for string to read into
 * @param[in]  len      buffer length in bytes
 *
 * @retval  (!= NULL) address string
 * @retval  (== NULL) failed
 */
const char *netdev_mask_read(const netdev_info_t *device, char *buf, unsigned len);

/**
 * Write Subnet-Mask address string to config item
 *
 * @param[in]  device   network device, (NULL) default device
 * @param[in]  str      address string to write
 *
 * @retval  (== 0) succeed
 * @retval  (!= 0) failed
 */
int netdev_mask_write(const netdev_info_t *device, const char *str);

/**
 * Read Default-Gateway address string from config item
 *
 * @param[in]  device   network device, (NULL) default device
 * @param[out] buf      buffer for string to read into
 * @param[in]  len      buffer length in bytes
 *
 * @retval  (!= NULL) address string
 * @retval  (== NULL) failed
 */
const char *netdev_gw_read(const netdev_info_t *device, char *buf, unsigned len);

/**
 * Write Default-Gateway address string to config item
 *
 * @param[in]  device   network device, (NULL) default device
 * @param[in]  str      address string to write
 *
 * @retval  (== 0) succeed
 * @retval  (!= 0) failed
 */
int netdev_gw_write(const netdev_info_t *device, const char *str);

/**
 * Read DHCP status value from config item
 *
 * @param[in]  device   network device, (NULL) default device
 *
 * @retval  ( > 0) enable
 * @retval  (== 0) disable
 * @retval  ( < 0) failed
 */
int netdev_dhcp_read(const netdev_info_t *device);

/**
 * Write DHCP status value to config item
 *
 * @param[in]  device   network device, (NULL) default device
 * @param[in]  dhcp     status value
 *
 * @retval  (== 0) succeed
 * @retval  (!= 0) failed
 */
int netdev_dhcp_write(const netdev_info_t *device, int dhcp);


/*----------------------------------------------------------------------------*/
/**
 * Convert from text address to a MAC address
 *
 * @param[in]  str    string of ascii MAC address
 * @param[out] addr   buffer of converted binary MAC address
 *
 * @retval  (>= 0) length of the binary MAC address
 * @retval  ( < 0) failed
 */
int netdev_ethmac_aton(const char *str, uint8_t addr[6]);

/**
 * Get ethernet binary MAC address from config item
 *
 * @param[in]  device   network device, (NULL) default device
 * @param[out] addr     buffer of get binary MAC address
 *
 * @retval  (>= 0) length of the get binary MAC address
 * @retval  ( < 0) failed
 */
int netdev_ethmac_get(const netdev_info_t *device, uint8_t addr[6]);

/**
 * Read MAC address string from config item
 *
 * @param[in]  device   network device, (NULL) default device
 * @param[out] buf      buffer for string to read into
 * @param[in]  len      buffer length in bytes
 *
 * @retval  (!= NULL) address string
 * @retval  (== NULL) failed
 */
const char *netdev_ethmac_read(const netdev_info_t *device, char *buf, unsigned len);

/**
 * Write MAC address string to config item
 *
 * @param[in]  device   network device, (NULL) default device
 * @param[in]  str      address string to write
 *
 * @retval  (== 0) succeed
 * @retval  (!= 0) failed
 */
int netdev_ethmac_write(const netdev_info_t *device, const char *str);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __NET_DEVICE_H__
/*****************************  END OF FILE  **********************************/

