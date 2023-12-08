/*******************************************************************************
 * @file    fs_user.c
 * @author  Linghu
 * @version v1.0.0
 * @date    2018/12/14
 * @brief   User define from file system.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2018/12/14 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
 
#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "FS"
#include "emb_log.h"

#include <stddef.h>
#include <stdint.h>
#include "RTE_Components.h"     // Used: RTE_FileSystem_Drive_XX
#include "rl_fs.h"              // Keil.MDK-Pro::File System
#include "fs_user.h"


/*******************************************************************************
 * @addtogroup FS_USER
 * @{
 * @addtogroup FS_USER_Pravate
 * @{
 * @addtogroup          FS_USER_Pravate_Functions
 * @{
 ******************************************************************************/
/**
 * Initialize and Mount the drive.
 *
 * @param[in]  drive    A string specifying the memory or storage device.
 */
static void fs_drv_mount(const char* drive)
{
    fsDriveInfo     info;

    if (finit(drive) != fsOK) {
        LOG_ERROR("Initialize drive(%.2s) Failed!", drive);
        return;
    }
    LOG_INFO("Initialize drive(%.2s) Succeed!", drive);

    switch (fmount(drive))
    {
    case fsOK:
        if (finfo(drive, &info) == fsOK) {
            int64_t  space = ffree(drive);
            LOG_INFO("Mount drive(%.2s) Succeed! Space: %lu/%lu", drive, (uint32_t)space, (uint32_t)info.capacity);
        }
        else {
            LOG_ERROR("Mount drive(%.2s) Succeed! But get information Failed!", drive);
        }
        break;

    case fsNoFileSystem:
        LOG_ERROR("Mount drive(%.2s) Succeed! But not formatted!", drive);
        break;

    default:
        LOG_ERROR("Mount drive(%.2s) Failed!", drive);
        funinit(drive);     //lint !e534  " Ignoring return value "
        break;
    } //lint !e788  " enum constant 'xxxx' not used within defaulted switch "
}


/*----------------------------------------------------------------------------*/
/**
 * Initialize file system and Mount all drive.
 */
void fs_init(void)
{
    void fs_drv_mount(const char* drive);

  #if defined(RTE_FileSystem_Drive_RAM) || defined(FS_Drive_RAM)
    fs_drv_mount("R0:");        /* File System RAM Drive            */
    (void)fformat ("R0:", NULL);
  #endif

  #if defined(RTE_FileSystem_Drive_MC_0) || defined(FS_Drive_MC_0)
    fs_drv_mount("M0:");        /* File System Memory Card Drive 0  */
  #endif
  #if defined(RTE_FileSystem_Drive_MC_1) || defined(FS_Drive_MC_1)
    fs_drv_mount("M1:");        /* File System Memory Card Drive 1  */
  #endif

  #if defined(RTE_FileSystem_Drive_NAND_0) || defined(FS_Drive_NAND_0)
    fs_drv_mount("N0:");        /* File System NAND Flash Drive 0   */
  #endif
  #if defined(RTE_FileSystem_Drive_NAND_1) || defined(FS_Drive_NAND_1)
    fs_drv_mount("N1:");        /* File System NAND Flash Drive 1   */
  #endif

  #if defined(RTE_FileSystem_Drive_NOR_0) || defined(FS_Drive_NOR_0)
    fs_drv_mount("F0:");        /* File System NOR Flash Drive 0    */
  #endif
  #if defined(RTE_FileSystem_Drive_NOR_1) || defined(FS_Drive_NOR_0)
    fs_drv_mount("F1:");        /* File System NOR Flash Drive 1    */
  #endif

  #if defined(RTE_FileSystem_Drive_USB_0) || defined(FS_Drive_USB_0)
    (void)fs_drv_mount("U0:");      /* File System USB Drive 0          */
  #endif
  #if defined(RTE_FileSystem_Drive_USB_1) || defined(FS_Drive_USB_1)
    (void)fs_drv_mount("U1:");      /* File System USB Drive 1          */
  #endif
}


/*----------------------------------------------------------------------------*/
/**
 * Creates an file system on memory or storage device.
 *
 * @param[in]  device   A string specifying the memory or storage device.
 * @param[in]  opt      A string specifying Specifies the format option.
 * @param[in]  data     Optional data.
 * @param[in]  len      data buffer length by bytes.
 *
 * @return     (0)Succeed, (Other)Failed.
 */
int fs_mkfs(const char* device, const char* opt, void* data, unsigned len)
{
    (void)data;  (void)len;

    return( fformat(device, opt) );  //lint !e641  " Converting enum 'xxx' to 'int' "
}


#if 0
/**
 * Mount drive to file system.
 *
 * @param[in]  drive    A string specifying the memory or storage device.
 *
 * @return     (0)Succeed, (Other)Failed.
 */
int fs_mount(const char* drive)
{
    return( fmount(drive) );
}


/**
 * Unmount drive from file system.
 *
 * @param[in]  drive    A string specifying the memory or storage device.
 *
 * @return     (0)Succeed, (Other)Failed.
 */
int fs_unmount(const char* drive)
{
    return( funmount(drive) );
}
#endif

/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

