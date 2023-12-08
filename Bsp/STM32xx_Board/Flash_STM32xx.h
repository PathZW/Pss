/*******************************************************************************
 * @file    Flash_STM32xx.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/7/27
 * @brief   CMSIS-Driver for STM32xxx Embedded Flash.\n
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 ******************************************************************************/
#ifndef __FLASH_STM32XX_H__
#define __FLASH_STM32XX_H__
#ifdef  __cplusplus
extern  "C"
{
#endif
/*******************************************************************************
 * @addtogroup Flash_STM32xx
 * @{
 * @addtogroup              Pravate_defines
 * @{
 ******************************************************************************/

#define ARM_FLASH_DRV_VERSION   ARM_DRIVER_VERSION_MAJOR_MINOR(1, 1)

#define FLASH_EVENT_READY       0           // 1: Support Flash Ready event
#define FLASH_ERASE_CHIP        0           // 1: Support EraseChip operation
#define FLASH_ERASED_VALUE    0xFF          // Contents of erased memory

/*----------------------------- STM32F1xx ------------------------------------*/
#if defined(STM32F1)

#define FLASH_PAGE_SIZE          2          // Programming page size in bytes
#define FLASH_PROGRAM_UNIT       1          // Smallest programmable unit in bytes
#define FLASH_DATA_WIDTH         0          // Data width: 0=8bit, 1=16bit, 2=32bit

#if defined(STM32F10X_LD)
#define FLASH_SECTOR_COUNT       32         // Number of Sectors
#define FLASH_SECTOR_SIZE    (1 * 1024)     // Sector size: 0=sector_info used
#elif defined(STM32F10X_MD)
#define FLASH_SECTOR_COUNT      128
#define FLASH_SECTOR_SIZE    (1 * 1024)
#elif defined(STM32F10X_HD)
#define FLASH_SECTOR_COUNT      256
#define FLASH_SECTOR_SIZE    (2 * 1024)
#elif defined(STM32F10X_CL)
#define FLASH_SECTOR_COUNT      128
#define FLASH_SECTOR_SIZE    (2 * 1024)
#elif defined(STM32F10X_XL)
#define FLASH_SECTOR_COUNT      512
#define FLASH_SECTOR_SIZE    (2 * 1024)
#endif

#ifndef FLASH_START_ADDR
#define FLASH_START_ADDR     FLASH_BASE     // Flash Memory address
#endif
#ifndef FLASH_TOTAL_SIZE                    // Flash Memory size
#define FLASH_TOTAL_SIZE  (((const uint16_t *)0x1FFFF7E0)[0] * (uint32_t)1024)
#endif
#define FLASH_EFS_DEF_SIZE  MAX(16, (FLASH_TOTAL_SIZE / 4))

/*----------------------------- STM32F2xx ------------------------------------*/
#elif defined(STM32F2)

#define FLASH_PAGE_SIZE          4          // Programming page size in bytes
#define FLASH_PROGRAM_UNIT       1          // Smallest programmable unit in bytes
#define FLASH_DATA_WIDTH         0          // Data width: 0=8bit, 1=16bit, 2=32bit

#define FLASH_SECTOR_COUNT       0          // Number of Sectors
#define FLASH_SECTOR_SIZE        0          // Sector size: 0=sector_info used

#ifndef FLASH_START_ADDR
#define FLASH_START_ADDR     FLASH_BASE     // Flash Memory address
#endif
#ifndef FLASH_TOTAL_SIZE                    // Flash Memory size
#define FLASH_TOTAL_SIZE  (((const uint16_t *)0x1FFF7A22)[0] * (uint32_t)1024)
#endif
#define FLASH_EFS_DEF_SIZE  MAX(0x40000, (FLASH_TOTAL_SIZE / 4))

#define FLASH_SECTORS   {                                                            \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00000000, 0x04000), /* Sector size  16kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00004000, 0x04000), /* Sector size  16kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00008000, 0x04000), /* Sector size  16kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x0000C000, 0x04000), /* Sector size  16kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00010000, 0x10000), /* Sector size  64kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00020000, 0x20000), /* Sector size 128kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00040000, 0x20000), /* Sector size 128kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00060000, 0x20000), /* Sector size 128kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00080000, 0x20000), /* Sector size 128kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x000A0000, 0x20000), /* Sector size 128kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x000C0000, 0x20000), /* Sector size 128kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x000E0000, 0x20000)  /* Sector size 128kB */ \
}

/*----------------------------- STM32F4xx ------------------------------------*/
#elif defined(STM32F4)

#define FLASH_PAGE_SIZE          4          // Programming page size in bytes
#define FLASH_PROGRAM_UNIT       1          // Smallest programmable unit in bytes
#define FLASH_DATA_WIDTH         0          // Data width: 0=8bit, 1=16bit, 2=32bit

#define FLASH_SECTOR_COUNT       0          // Number of Sectors
#define FLASH_SECTOR_SIZE        0          // Sector size: 0=sector_info used

#ifndef FLASH_START_ADDR
#define FLASH_START_ADDR     FLASH_BASE     // Flash Memory address
#endif
#ifndef FLASH_TOTAL_SIZE                    // Flash Memory size
#define FLASH_TOTAL_SIZE  (((const uint16_t *)0x1FFF7A22)[0] * (uint32_t)1024)
#endif
#define FLASH_EFS_DEF_SIZE  MAX(0x40000, (FLASH_TOTAL_SIZE / 4))

#if defined(STM32F405xx) || defined(STM32F415xx) ||                             \
    defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F40_41xxx ) || \
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F427_437xx) || \
    defined(STM32F429xx) || defined(STM32F439xx) || defined(STM32F429_439xx)
#define FLASH_SECTORS   {                                                            \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00000000, 0x04000), /* Sector size  16kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00004000, 0x04000), /* Sector size  16kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00008000, 0x04000), /* Sector size  16kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x0000C000, 0x04000), /* Sector size  16kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00010000, 0x10000), /* Sector size  64kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00020000, 0x20000), /* Sector size 128kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00040000, 0x20000), /* Sector size 128kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00060000, 0x20000), /* Sector size 128kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x00080000, 0x20000), /* Sector size 128kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x000A0000, 0x20000), /* Sector size 128kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x000C0000, 0x20000), /* Sector size 128kB */ \
    ARM_FLASH_SECTOR_INFO(FLASH_BASE + 0x000E0000, 0x20000)  /* Sector size 128kB */ \
}
#endif

#if 0
static ARM_FLASH_SECTOR  _Flash_SectorD = {
    ARM_FLASH_SECTOR_INFO(0x00000000, 0x04000),    /* Sector size  16kB */
    ARM_FLASH_SECTOR_INFO(0x00004000, 0x04000),    /* Sector size  16kB */
    ARM_FLASH_SECTOR_INFO(0x00008000, 0x04000),    /* Sector size  16kB */
    ARM_FLASH_SECTOR_INFO(0x0000C000, 0x04000),    /* Sector size  16kB */
    ARM_FLASH_SECTOR_INFO(0x00010000, 0x10000),    /* Sector size  64kB */
    ARM_FLASH_SECTOR_INFO(0x00020000, 0x20000),    /* Sector size 128kB */
    ARM_FLASH_SECTOR_INFO(0x00040000, 0x20000),    /* Sector size 128kB */
    ARM_FLASH_SECTOR_INFO(0x00060000, 0x20000),    /* Sector size 128kB */
    ARM_FLASH_SECTOR_INFO(0x00080000, 0x00000),    /* Sector size   0   */
    ARM_FLASH_SECTOR_INFO(0x00080000, 0x00000),    /* Sector size   0   */
    ARM_FLASH_SECTOR_INFO(0x00080000, 0x00000),    /* Sector size   0   */
    ARM_FLASH_SECTOR_INFO(0x00080000, 0x00000),    /* Sector size   0   */
    ARM_FLASH_SECTOR_INFO(0x00080000, 0x04000),    /* Sector size  16kB */
    ARM_FLASH_SECTOR_INFO(0x00084000, 0x04000),    /* Sector size  16kB */
    ARM_FLASH_SECTOR_INFO(0x00088000, 0x04000),    /* Sector size  16kB */
    ARM_FLASH_SECTOR_INFO(0x0008C000, 0x04000),    /* Sector size  16kB */
    ARM_FLASH_SECTOR_INFO(0x00090000, 0x10000),    /* Sector size  64kB */
    ARM_FLASH_SECTOR_INFO(0x000A0000, 0x20000),    /* Sector size 128kB */
    ARM_FLASH_SECTOR_INFO(0x000C0000, 0x20000),    /* Sector size 128kB */
    ARM_FLASH_SECTOR_INFO(0x000E0000, 0x20000),    /* Sector size 128kB */
    ARM_FLASH_SECTOR_INFO(0x00100000, 0x00000),    /* Sector size   0kB */
    ARM_FLASH_SECTOR_INFO(0x00100000, 0x00000),    /* Sector size   0kB */
    ARM_FLASH_SECTOR_INFO(0x00100000, 0x00000),    /* Sector size   0kB */
    ARM_FLASH_SECTOR_INFO(0x00100000, 0x00000)     /* Sector size   0kB */
};
#endif

#else
#error "Unsupported stm32 family."
#endif


/******************************************************************************/

#ifndef FLASH_TIMEOUT_PROGRAM
#define FLASH_TIMEOUT_PROGRAM          10       // Timeout of Flash Program
#endif
#ifndef FLASH_TIMEOUT_ERASE
#define FLASH_TIMEOUT_ERASE           40000     // Timeout of Flash Erase
#endif

#ifndef FLASH_KEY1
#define FLASH_KEY1                  0x45670123U  // FPEC Key1
#endif
#ifndef FLASH_KEY2
#define FLASH_KEY2                  0xCDEF89ABU  // FPEC Key2
#endif

#ifndef FLASH_PSIZE_BYTE
#define FLASH_PSIZE_BYTE            0x00000000U
#endif
#ifndef FLASH_PSIZE_HALF_WORD
#define FLASH_PSIZE_HALF_WORD       0x00000100U
#endif
#ifndef FLASH_PSIZE_WORD
#define FLASH_PSIZE_WORD            0x00000200U
#endif
#ifndef FLASH_PSIZE_DOUBLE_WORD
#define FLASH_PSIZE_DOUBLE_WORD     0x00000300U
#endif

#if defined(STM32F1)    /*----------------- STM32F1 Family -------------------*/
#   ifndef FLASH_BANK1_END
#   define FLASH_BANK1_END      ((uint32_t)0x807FFFF)
#   endif
#   define FLASH_SR_ERR_FLAG    (FLASH_SR_PGERR | FLASH_SR_WRPRTERR)
#   define FLASH_CR_OPT_FLAG    (FLASH_CR_PG | FLASH_CR_PER)

#elif defined(STM32F2)  /*----------------- STM32F2 Family -------------------*/
#   define FLASH_SR_ERR_FLAG    (FLASH_SR_SOP | FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR)
#   define FLASH_CR_OPT_FLAG    (FLASH_CR_PG | FLASH_CR_SER | FLASH_CR_SNB)

#elif defined(STM32F3)  /*----------------- STM32F3 Family -------------------*/
#   define FLASH_SR_ERR_FLAG    (FLASH_SR_PGERR | FLASH_SR_WRPERR)
#   define FLASH_CR_OPT_FLAG    (FLASH_CR_PG | FLASH_CR_PER)

#elif defined(STM32F4)  /*----------------- STM32F4 Family -------------------*/
#   ifndef FLASH_SR_RDERR
#   define FLASH_SR_ERR_FLAG    (FLASH_SR_SOP | FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR)
#   else
#   define FLASH_SR_ERR_FLAG    (FLASH_SR_SOP | FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR | FLASH_SR_RDERR)
#   endif
#   define FLASH_CR_OPT_FLAG    (FLASH_CR_PG | FLASH_CR_SER | FLASH_CR_SNB)
#else
#error "Unsupported stm32 family."
#endif


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __FLASH_STM32XX_H__
/*****************************  END OF FILE  **********************************/

