/*******************************************************************************
 * @file    Flash_STM32xx.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/7/27
 * @brief   CMSIS-Driver for STM32xxx Embedded Flash.\n
 *          1. 将 STM32xxx 内部 Flash 在分为 IAP 和 EFS 2个区域.\n
 *          2. IAP 区域为整个 Flash 区域.\n
 *          3. EFS 区域[地址/长度]可配置, 默认为 Flash 尾部未使用区域, 一般用于文件系统.\n
 *          4. 接口为 CMSIS-Driver Flash Interface, 使用物理地址访问.\n
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/7/27 --- Linghu -- the first version, Suport STM32F10x family
 *
 * @par Change Logs:
 *      2019/11/26 -- Linghu -- Add Suport STM32F2 family
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -emacro({506}, *)        // Constant value Boolean
//lint -emacro({702}, *)        // Shift right of signed quantity
//lint -emacro({774}, *)        // Boolean within 'if' always evaluates to True/False
//lint -emacro({835}, *)        // A zero has been given as right argument to operator 'xx'
//lint -emacro({10}, ARM_Driver_Flash_)
//lint -emacro({31}, ARM_Driver_Flash_)

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "FLASH"
#include "emb_log.h"
#define  ONLY_USED_REGS
#include "board.h"              // Device's defines

#include <string.h>
#include <stdbool.h>
#include "Driver_Flash.h"
#include "emb_critical.h"
#include "defines.h"            // Used: ARRAY_SIZE(), MIN(), MAX(), UNUSED_VAR()
#include "emb_delay.h"
#include "Flash_STM32xx.h"


/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup          Flash_STM32xx
 * @{
 ******************************************************************************/

#ifndef IAP_FLASH_NUM
#define IAP_FLASH_NUM             0     //!< (C) IAP Flash driver number
#endif

#ifndef EFS_FLASH_NUM
#define EFS_FLASH_NUM            -1     //!< (C) EFS Flash driver number, (< 0)Disable
#endif

#ifndef EFS_FLASH_ADDR
#define EFS_FLASH_ADDR            0     //!< (C) EFS Flash addr, (== 0)Auto
#endif
#ifndef EFS_FLASH_SIZE
#define EFS_FLASH_SIZE            0     //!< (C) EFS Flash size, (== 0)Auto
#endif


/*******************************************************************************
 * @}
 * @}
 * @addtogroup Flash_STM32xx
 * @{
 * @addtogroup              Pravate_Macros
 * @{
 ******************************************************************************/

#define FLASH_INVALID_ADDR      SRAM_BASE       // Invalid flash address

#  if  (FLASH_PAGE_SIZE == 4)
#define FLASH_PAGE_TYPE         uint32_t        // Data type of FLASH_PAGE_SIZE
#elif  (FLASH_PAGE_SIZE == 2)
#define FLASH_PAGE_TYPE         uint16_t
#else
#define FLASH_PAGE_TYPE         uint8_t
#endif


/*******************************************************************************
 * @}
 * @addtogroup              Pravate_Variables
 * @{
 ******************************************************************************/

static ARM_FLASH_SECTOR  _Flash_Sectors[]  = FLASH_SECTORS; // Table of Flash Sectors

static ARM_FLASH_STATUS  _Flash_Status = {                  // Flash Status
    0,                  // busy:  Flash busy flag
    0,                  // error: Error flag
    0
};

static struct _ARM_FLASH_INFO  _IAP_FlashInfo = {           // IAP Flash Information
    NULL,               // sector_info:  Sector layout information (NULL: Uniform sectors)
    0,                  // sector_count: Number of sectors
    0,                  // sector_size:  Uniform sector size in bytes (0: sector_info used)
    FLASH_PAGE_SIZE,    // page_size:    Optimal programming page size in bytes
    FLASH_PROGRAM_UNIT, // program_unit: Smallest programmable unit in bytes
    FLASH_ERASED_VALUE, // erased_value: Contents of erased memory (usually 0xFF)
  #if (ARM_FLASH_API_VERSION > 0x201U)
    { 0U, 0U, 0U }
  #endif
};

#if (EFS_FLASH_NUM >= 0)
static struct _ARM_FLASH_INFO  _EFS_FlashInfo = {           // EFS Flash Information
    NULL,               // sector_info:  Sector layout information (NULL: Uniform sectors)
    0,                  // sector_count: Number of sectors
    0,                  // sector_size:  Uniform sector size in bytes (0: sector_info used)
    FLASH_PAGE_SIZE,    // page_size:    Optimal programming page size in bytes
    FLASH_PROGRAM_UNIT, // program_unit: Smallest programmable unit in bytes
    FLASH_ERASED_VALUE, // erased_value: Contents of erased memory (usually 0xFF)
  #if (ARM_FLASH_API_VERSION > 0x201U)
    { 0U, 0U, 0U }
  #endif
};
#endif


/*******************************************************************************
 * @}
 * @addtogroup              Pravate_Functions
 * @{
 ******************************************************************************/
/**
 * Wait for a FLASH operation to complete.
 *
 * @param[in]  Timeout  Timeout maximum flash operationtimeout
 *
 * @retval     (== 0) Succeed
 * @retval     (!= 0) TimeOut or Error
 */
__STATIC_INLINE int32_t Flash_WaitComplete(uint32_t Timeout)
{
    uint32_t  msec = GetSysMsec();

    // Wait for the FLASH operation to complete
    while(   ((FLASH->SR  & FLASH_SR_BSY) != 0)
    #if defined(STM32F1) && (defined(STM32F10X_XL) || defined(FLASH_BANK2_END))
          || ((FLASH->SR2 & FLASH_SR_BSY) != 0)
    #endif
         )
    {
        if((GetSysMsec() - msec) >= Timeout) {
            LOG_ERROR("End of operation timeout: %lu", Timeout);
            return( -1 );   // TimeOut
        }
    }

    // Clear FLASH End of Operation bit
    if((FLASH->SR & FLASH_SR_EOP) != 0) {
        FLASH->SR = FLASH_SR_EOP;
    }
#if defined(STM32F1) && (defined(STM32F10X_XL) || defined(FLASH_BANK2_END))
    if((FLASH->SR2 & FLASH_SR_EOP) != 0) {
        FLASH->SR2 = FLASH_SR_EOP;
    }
#endif

    // Check FLASH Operation Error Flag
    if((FLASH->SR & FLASH_SR_ERR_FLAG) != 0) {
        LOG_ERROR("Operation error, FLASH SR: 0x%08lX", (uint32_t)FLASH->SR);
        FLASH->SR = FLASH_SR_ERR_FLAG;
        return( -2 );   // Error
    }
#if defined(STM32F1) && (defined(STM32F10X_XL) || defined(FLASH_BANK2_END))
    if((FLASH->SR2 & FLASH_SR_ERR_FLAG) != 0) {
        LOG_ERROR("Operation error, FLASH SR2: 0x%08lX", (uint32_t)FLASH->SR2);
        FLASH->SR2 = FLASH_SR_ERR_FLAG;
        return( -2 );   // Error
    }
#endif

    // Clear Program and Erase Operation bit
    FLASH->CR  &= ~FLASH_CR_OPT_FLAG;
#if defined(STM32F1) && (defined(STM32F10X_XL) || defined(FLASH_BANK2_END))
    FLASH->CR2 &= ~FLASH_CR_OPT_FLAG;
#endif

    return( 0 );
}


/**
 * Flush the instruction and data caches
 */
__STATIC_INLINE void Flash_CachesFlush(void)
{
#if defined(STM32F2) || defined(STM32F4)
    if((FLASH->ACR & FLASH_ACR_ICEN) != 0)
    {   // Flush instruction cache
        FLASH->ACR &= ~FLASH_ACR_ICEN;
        FLASH->ACR |=  FLASH_ACR_ICRST;
        FLASH->ACR &= ~FLASH_ACR_ICRST;
        FLASH->ACR |=  FLASH_ACR_ICEN;
    }
    if((FLASH->ACR & FLASH_ACR_DCEN) != 0)
    {   // Flush data cache
        FLASH->ACR &= ~FLASH_ACR_DCEN;
        FLASH->ACR |=  FLASH_ACR_DCRST;
        FLASH->ACR &= ~FLASH_ACR_DCRST;
        FLASH->ACR |=  FLASH_ACR_DCEN;
    }
#endif
}


/**
 * Unlock the FLASH control register access
 */
__STATIC_INLINE void Flash_CtrlUnlock(void)
{
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
#if defined(STM32F1) && (defined(STM32F10X_XL) || defined(FLASH_BANK2_END))
    FLASH->KEYR2 = FLASH_KEY1;
    FLASH->KEYR2 = FLASH_KEY2;
#endif
}


/**
 * Locks the FLASH control register access
 */
__STATIC_INLINE void Flash_CtrlLocked(void)
{
    FLASH->CR  |= FLASH_CR_LOCK;
#if defined(STM32F1) && (defined(STM32F10X_XL) || defined(FLASH_BANK2_END))
    FLASH->CR2 |= FLASH_CR_LOCK;
#endif
}


/**
 * Address to Flash Sector Number or Address
 *
 * @param[in]  addr   Address
 *
 * @retval  (!= UINT32_MAX) Flash Sector Number or Address
 * @retval  (== UINT32_MAX) Failed
 */
__STATIC_INLINE uint32_t Flash_AddrToSector(uint32_t addr)
{
    uint32_t    start, end;
    uint32_t    i;

    for(i = 0;  i < ARRAY_SIZE(_Flash_Sectors);  i++)
    {
        start = _Flash_Sectors[i].start;
        end   = _Flash_Sectors[i].end;

        if (start > end) {
            continue;       // Invalid Sector in _Flash_Sectors[]
        }
        if((start <= addr) && (addr <= end)) {
            break;
        }
    }

#  if defined(STM32F1) || defined(STM32F3)
    return( (i < ARRAY_SIZE(_Flash_Sectors)) ? _Flash_Sectors[i].start : UINT32_MAX );
#elif defined(STM32F2) || defined(STM32F4)
    return( (i < ARRAY_SIZE(_Flash_Sectors)) ? i : UINT32_MAX );
#else
#error "Unsupported stm32 family."
#endif
}


/**
 * Flash erase operation
 *
 * @param[in]  sector   sector number or sector address
 */
__STATIC_INLINE void Flash_OptErase(uint32_t sector)
{
#if defined(STM32F1) || defined(STM32F3)
#if defined(STM32F1) && (defined(STM32F10X_XL) || defined(FLASH_BANK2_END))
    if (sector > FLASH_BANK1_END) {
        FLASH->CR2 |= FLASH_CR_PER;
        FLASH->AR2  = sector;
        FLASH->CR2 |= FLASH_CR_STRT;
    } else
#endif
    {   FLASH->CR |= FLASH_CR_PER;
        FLASH->AR  = sector;
        FLASH->CR |= FLASH_CR_STRT;
    }
#elif defined(STM32F2) || defined(STM32F4)
    FLASH->CR &= ~FLASH_CR_PSIZE;
    FLASH->CR |= (  (FLASH_PAGE_SIZE == 4) ? FLASH_PSIZE_WORD
                  : (FLASH_PAGE_SIZE == 2) ? FLASH_PSIZE_HALF_WORD
                  :                          FLASH_PSIZE_BYTE
                 );
    FLASH->CR &= ~FLASH_CR_SNB;
    FLASH->CR |=  FLASH_CR_SER | (sector << 3);
    FLASH->CR |=  FLASH_CR_STRT;
#else
#error "Unsupported stm32 family."
#endif
}


/**
 * Program BYTE(8-bit) at a specified address.
 */
__STATIC_INLINE void Flash_OptProgByte(uint32_t addr, uint8_t data)
{
#if defined(STM32F1) || defined(STM32F3)
    union _byte_word{ uint8_t byte[2]; uint16_t word; }  byte_word;

    if (addr & 0x1UL) {
        addr &= ~1UL
        byte_word.byte[0] = ((const uint8_t *)addr)[0];
        byte_word.byte[1] = data;
    } else {
        byte_word.byte[0] = data;
        byte_word.byte[1] = ((const uint8_t *)addr)[1];
    }
#if defined(STM32F1) && (defined(STM32F10X_XL) || defined(FLASH_BANK2_END))
    if (addr > FLASH_BANK1_END) {
        FLASH->CR2 |= FLASH_CR_PG;
    } else
#endif
    {   FLASH->CR  |= FLASH_CR_PG;   }
    ((volatile uint16_t *)addr)[0] = byte_word.word;

#elif defined(STM32F2) || defined(STM32F4)
    FLASH->CR &= ~FLASH_CR_PSIZE;
    FLASH->CR |=  FLASH_PSIZE_BYTE;
    FLASH->CR |=  FLASH_CR_PG;
    ((volatile uint8_t *)addr)[0]  = data;
#else
#error "Unsupported stm32 family."
#endif
}


/**
 * Program FLASH_PAGE_SIZE at a specified address.
 */
__STATIC_INLINE void Flash_OptProgPage(uint32_t addr, uint32_t data)
{
#if defined(STM32F1) || defined(STM32F3)
#if defined(STM32F1) && (defined(STM32F10X_XL) || defined(FLASH_BANK2_END))
    if (addr > FLASH_BANK1_END) {
        FLASH->CR2 |= FLASH_CR_PG;
    } else
#endif
    {   FLASH->CR  |= FLASH_CR_PG;   }
    ((volatile FLASH_PAGE_TYPE *)addr)[0]  = data;

#elif defined(STM32F2) || defined(STM32F4)
    FLASH->CR &= ~FLASH_CR_PSIZE;
    FLASH->CR |= (  (FLASH_PAGE_SIZE == 4) ? FLASH_PSIZE_WORD
                  : (FLASH_PAGE_SIZE == 2) ? FLASH_PSIZE_HALF_WORD
                  :                          FLASH_PSIZE_BYTE
                 );
    FLASH->CR |=  FLASH_CR_PG;
    ((volatile FLASH_PAGE_TYPE *)addr)[0]  = data;
#else
#error "Unsupported stm32 family."
#endif
}


/**
 * Read data from Flash physical address.
 *
 * @param[in]  addr   Physical address.
 * @param[out] data   Pointer to a buffer storing the data read from Flash.
 * @param[in]  cnt    Number of bytes to read.
 *
 * @return  number of bytes read or \ref execution_status
 */
static int32_t Flash_Read(uint32_t addr, void *data, uint32_t cnt)
{
//  LOG_VERB("Read Address: 0x%08lX, Bytes: %lu", addr, cnt);
    memcpy(data, (void *)addr, cnt);
    return( cnt );  //lint !e713  " Loss of precision "
}


/**
 * Program data to Flash physical address.
 *
 * @param[in]  addr   Physical address.
 * @param[in]  data   Pointer to a buffer containing the data to be programmed to Flash.
 * @param[in]  cnt    Number of bytes to program.
 *
 * @return  number of bytes programmed or \ref execution_status
 */
static int32_t Flash_Program(uint32_t addr, const void *data, uint32_t cnt)
{
    uint32_t  temp;
    uint32_t  i;
    uint32_t  num = cnt;

//  LOG_VERB("Program Address: 0x%08lX, Bytes: %lu", addr, cnt);
    Flash_CtrlUnlock();

    for(i = MIN((FLASH_PAGE_SIZE - addr) % FLASH_PAGE_SIZE, num);  i > 0;  i--)
    {
        temp = ((const uint8_t *)data)[0];

        //---------- Flash Program -----------------------------
        if (Flash_WaitComplete(FLASH_TIMEOUT_PROGRAM) != 0) {
            cnt = (uint32_t)ARM_DRIVER_ERROR;  num = 0;  break;
        }
        Flash_OptProgByte(addr, temp);  //lint !e734    // Program
        if (Flash_WaitComplete(FLASH_TIMEOUT_PROGRAM) != 0) {
            cnt = (uint32_t)ARM_DRIVER_ERROR;  num = 0;  break;
        }

        //---------- Program Check -----------------------------
        if (temp != ((const uint8_t *)addr)[0]) {
            LOG_ERROR(  "Error Program Check, Address: 0x%08lX, Bytes: %u, P/R: 0x%02X/0x%02X"
                      , (uint32_t) addr
                      , (unsigned) sizeof(uint8_t)
                      , (unsigned) temp
                      , (unsigned) ((const uint8_t *)addr)[0]
                     );
            cnt = (uint32_t)ARM_DRIVER_ERROR;  num = 0;  break;
        }

        //------------ Next Data -------------------------------
        num  -= sizeof(uint8_t);
        addr += sizeof(uint8_t);
        data  = sizeof(uint8_t) + ((const uint8_t *)data);
    }

    for(i = num / FLASH_PAGE_SIZE;  i > 0;  i--)
    {
        temp = ((const FLASH_PAGE_TYPE *)data)[0];  // 注意存在非对齐内存访问

        //---------- Flash Program -----------------------------
        if (Flash_WaitComplete(FLASH_TIMEOUT_PROGRAM) != 0) {
            cnt = (uint32_t)ARM_DRIVER_ERROR;  num = 0;  break;
        }
        Flash_OptProgPage(addr, temp);                  // Program
        if (Flash_WaitComplete(FLASH_TIMEOUT_PROGRAM) != 0) {
            cnt = (uint32_t)ARM_DRIVER_ERROR;  num = 0;  break;
        }

        //---------- Program Check -----------------------------
        if (temp != ((const FLASH_PAGE_TYPE *)addr)[0]) {
            LOG_ERROR(  "Error Program Check, Address: 0x%08lX, Bytes: %u, P/R: 0x%02lX/0x%02lX"
                      , (uint32_t) addr
                      , (unsigned) sizeof(FLASH_PAGE_TYPE)
                      , (uint32_t) temp
                      , (uint32_t) ((const FLASH_PAGE_TYPE *)addr)[0]
                     );
            cnt = (uint32_t)ARM_DRIVER_ERROR;  num = 0;  break;
        }

        //------------ Next Data -------------------------------
        num  -= FLASH_PAGE_SIZE;
        addr += FLASH_PAGE_SIZE;
        data  = FLASH_PAGE_SIZE + ((const uint8_t *)data);
    }

    for(i = num;  i > 0;  i--)
    {
        temp = ((const uint8_t *)data)[0];

        //---------- Flash Program -----------------------------
        if (Flash_WaitComplete(FLASH_TIMEOUT_PROGRAM) != 0) {
            cnt = (uint32_t)ARM_DRIVER_ERROR;  num = 0;  break;
        }
        Flash_OptProgByte(addr, temp);  //lint !e734    // Program
        if (Flash_WaitComplete(FLASH_TIMEOUT_PROGRAM) != 0) {
            cnt = (uint32_t)ARM_DRIVER_ERROR;  num = 0;  break;
        }

        //---------- Program Check -----------------------------
        if (temp != ((const uint8_t *)addr)[0]) {
            LOG_ERROR(  "Error Program Check, Address: 0x%08lX, Bytes: %u, P/R: 0x%02X/0x%02X"
                      , (uint32_t) addr
                      , (unsigned) sizeof(uint8_t)
                      , (unsigned) temp
                      , (unsigned) ((const uint8_t *)addr)[0]
                     );
            cnt = (uint32_t)ARM_DRIVER_ERROR;  num = 0;  break;
        }

        //------------ Next Data -------------------------------
        num  -= sizeof(uint8_t);
        addr += sizeof(uint8_t);
        data  = sizeof(uint8_t) + ((const uint8_t *)data);
    }

    Flash_CtrlLocked();
    if (num != 0) {
        return( ARM_DRIVER_ERROR );
    }
    return( cnt );  //lint !e713
}


/**
 * Erase Flash.
 *
 * @param[in]  addr   Physical address
 *
 * @return  \ref execution_status
 */
static int32_t Flash_Erase(uint32_t addr)
{
    uint32_t sector;
    int32_t  status = ARM_DRIVER_OK;

//  LOG_VERB("Erase Addres: 0x%08lX", addr);
    if ((sector = Flash_AddrToSector(addr)) == UINT32_MAX) {
        return( ARM_DRIVER_ERROR );
    }
    Flash_CtrlUnlock();

    if (Flash_WaitComplete(FLASH_TIMEOUT_ERASE) != 0) {
        status = ARM_DRIVER_ERROR;
    }
    else {
        Flash_OptErase(sector);     // Erase
        if (Flash_WaitComplete(FLASH_TIMEOUT_ERASE) != 0) {
            status = ARM_DRIVER_ERROR;
        }
        Flash_CachesFlush();
    }

    Flash_CtrlLocked();
    return( status );
}


/******************************************************************************/
/**
 * Initialize the Flash information.
 *
 * @return  \ref execution_status
 */
static int32_t Flash_Init(void)
{
    uint32_t    addr1, addr2;
    uint32_t    size;
    uint32_t    sec1, sec2;

    if ((_IAP_FlashInfo.sector_info != NULL) || (_IAP_FlashInfo.sector_size != 0)) {
        return( ARM_DRIVER_ERROR_SPECIFIC );    // Has been initialized
    }

    _IAP_FlashInfo.sector_info  = NULL;
    _IAP_FlashInfo.sector_count = 0;
    _IAP_FlashInfo.sector_size  = 0;
    _IAP_FlashInfo.page_size    = FLASH_PAGE_SIZE;
    _IAP_FlashInfo.program_unit = FLASH_PROGRAM_UNIT;
    _IAP_FlashInfo.erased_value = FLASH_ERASED_VALUE;

#if (EFS_FLASH_NUM >= 0)
    _EFS_FlashInfo.sector_info  = NULL;
    _EFS_FlashInfo.sector_count = 0;
    _EFS_FlashInfo.sector_size  = 0;
    _EFS_FlashInfo.page_size    = FLASH_PAGE_SIZE;
    _EFS_FlashInfo.program_unit = FLASH_PROGRAM_UNIT;
    _EFS_FlashInfo.erased_value = FLASH_ERASED_VALUE;
#endif

    /*------------------------- IAP Flash ------------------------------------*/
    if((addr1 = FLASH_START_ADDR) < FLASH_BASE) {
        addr1 = FLASH_BASE;                     // IAP 开始地址
    }
    addr2 = FLASH_BASE + FLASH_TOTAL_SIZE - 1;  // IAP 结束地址

    for(sec1 = 0;  sec1 < ARRAY_SIZE(_Flash_Sectors);  sec1++)
    {  // 根据扇区信息表、EFS Flash开始地址 查找 IAP 开始扇区
        if (_Flash_Sectors[sec1].start > _Flash_Sectors[sec1].end) {
            continue;   // Invalid Sector in _Flash_Sectors[]
        }
        if (_Flash_Sectors[sec1].start >= addr1) {
            break;
        }
    }
    for(size = 0, sec2 = sec1;  sec2 < ARRAY_SIZE(_Flash_Sectors);  sec2++)
    {   // 根据扇区信息表、Flash开始地址、Flash大小 计算 IAP 大小与扇区数
        size += _Flash_Sectors[sec2].end + 1 - _Flash_Sectors[sec2].start;
        if (_Flash_Sectors[sec2].end >= addr2) {
            break;
        }
    }
    addr2 = addr1 + size - 1;
    _IAP_FlashInfo.sector_info  = &(_Flash_Sectors[sec1]);
    _IAP_FlashInfo.sector_count =  (sec2 + 1 - sec1);
    _IAP_FlashInfo.sector_size  =   0;

    if( !((1 <= _IAP_FlashInfo.sector_count) && (_IAP_FlashInfo.sector_count <= ARRAY_SIZE(_Flash_Sectors)))
       || (size == 0)
      ) {
        LOG_WARN("IAP Flash initialization failed: 0x%08lX - 0x%08lX", addr1, addr2);
        return( ARM_DRIVER_ERROR_PARAMETER );   // 无效的 IAP Flash
    }
    LOG_INFO(  "IAP Flash Address: 0x%08lX, Size: 0x%08lX, Sectors: %lu"
             , _IAP_FlashInfo.sector_info->start
             ,  size
             , _IAP_FlashInfo.sector_count
            );
    if ((_IAP_FlashInfo.sector_info->start != FLASH_BASE) || (size != FLASH_TOTAL_SIZE)) {
        LOG_INFO(  "IAP Flash from 0x%08lX:0x%08lX changed to 0x%08lX:0x%08lX"
                 , (uint32_t)FLASH_BASE
                 , (uint32_t)FLASH_TOTAL_SIZE
                 , (uint32_t)_IAP_FlashInfo.sector_info->start
                 , (uint32_t)size
                );
    }

    /*------------------------- EFS Flash ------------------------------------*/
#if (EFS_FLASH_NUM >= 0)
    if((size = EFS_FLASH_SIZE) == 0) {  //lint !e506 !e774  // 自动分配 EFS 大小
        size = MIN(FLASH_EFS_DEF_SIZE, FLASH_BASE + FLASH_TOTAL_SIZE - ROM_LINK_LIMIT);
    }
    if((addr1 = EFS_FLASH_ADDR) == 0) { //lint !e506 !e774  // 自动分配 EFS 地址
        addr1 = MAX(FLASH_BASE + FLASH_TOTAL_SIZE - size, ROM_LINK_LIMIT);
    }
    if (addr1 < FLASH_BASE) {
        addr1 = FLASH_BASE;                                 // EFS 开始地址
    }
    if((addr2 = (addr1 + size - 1)) > (FLASH_BASE + FLASH_TOTAL_SIZE - 1)) {
        addr2 =  FLASH_BASE + FLASH_TOTAL_SIZE - 1;         // EFS 结束地址
    }
    for(sec1 = 0;  sec1 < ARRAY_SIZE(_Flash_Sectors);  sec1++)
    {  // 根据扇区信息表、EFS Flash开始地址 查找 EFS 开始扇区
        if (_Flash_Sectors[sec1].start > _Flash_Sectors[sec1].end) {
            continue;   // Invalid Sector in _Flash_Sectors[]
        }
        if (_Flash_Sectors[sec1].start >= addr1) {
            break;
        }
    }
    for(size = 0, sec2 = sec1;  sec2 < ARRAY_SIZE(_Flash_Sectors);  sec2++)
    {   // 根据扇区信息表、Flash开始地址、Flash大小 计算 EFS 大小与扇区数
        size += _Flash_Sectors[sec2].end + 1 - _Flash_Sectors[sec2].start;
        if (_Flash_Sectors[sec2].end >= addr2) {
            break;
        }
    }
    addr2 = addr1 + size - 1;
    _EFS_FlashInfo.sector_info  = &(_Flash_Sectors[sec1]);
    _EFS_FlashInfo.sector_count =  (sec2 + 1 - sec1);
    _EFS_FlashInfo.sector_size  =   0;

    if( !((1 <= _EFS_FlashInfo.sector_count) && (_EFS_FlashInfo.sector_count <= ARRAY_SIZE(_Flash_Sectors)))
       || (size == 0)
      ) {
        LOG_WARN("EFS Flash initialization failed: 0x%08lX - 0x%08lX", addr1, addr2);
        return( ARM_DRIVER_ERROR_PARAMETER );   // 无效的 EFS Flash
    }
    if(   ((ROM_LINK_START <= addr1) && (addr1 <= ROM_LINK_LIMIT))  //lint !e568 !e685 !e775
       || ((ROM_LINK_START <= addr2) && (addr2 <= ROM_LINK_LIMIT))  //lint !e568 !e685 !e775
      ) {
        LOG_WARN("EFS Flash regional overlapping: 0x%08lX - 0x%08lX", addr1, addr2);
        return( ARM_DRIVER_ERROR_PARAMETER );   // 无效的 EFS Flash
    }
    LOG_INFO(  "EFS Flash Address: 0x%08lX, Size: 0x%08lX, Sectors: %lu"
             , _EFS_FlashInfo.sector_info->start
             , size
             , _EFS_FlashInfo.sector_count
            );
    if((_EFS_FlashInfo.sector_info->start != EFS_FLASH_ADDR) || (size != EFS_FLASH_SIZE)) {
        LOG_INFO(  "EFS Flash from 0x%08lX:0x%08lX changed to 0x%08lX:0x%08lX"
                 , (uint32_t)EFS_FLASH_ADDR
                 , (uint32_t)EFS_FLASH_SIZE
                 , (uint32_t)_EFS_FlashInfo.sector_info->start
                 , (uint32_t)size
                );
    }
#endif

    return( ARM_DRIVER_OK );
}


/******************************************************************************/
/**
 * Get driver version.
 *
 * @return  \ref ARM_DRIVER_VERSION
 */
static ARM_DRIVER_VERSION GetVersion(void)
{
    static const ARM_DRIVER_VERSION  DriverVersion = {
        ARM_FLASH_API_VERSION,
        ARM_FLASH_DRV_VERSION
    };

    return( DriverVersion );
}


/**
 * Get driver capabilities.
 *
 * @return  \ref ARM_FLASH_CAPABILITIES
 */
static ARM_FLASH_CAPABILITIES GetCapabilities(void)
{
    static const ARM_FLASH_CAPABILITIES  DriverCapabilities = {
        FLASH_EVENT_READY,
        FLASH_DATA_WIDTH,
        FLASH_ERASE_CHIP,
      #if (ARM_FLASH_API_VERSION > 0x200U)
        0U    /* reserved */
      #endif
    };

    return( DriverCapabilities );
}


/**
 * Initialize the Flash Interface.
 *
 * @param[in]  cb_event  Pointer to \ref ARM_Flash_SignalEvent
 *
 * @return  \ref execution_status
 */
static int32_t Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    int32_t     status;
    CRITICAL_SECTION_ALLOC();
    UNUSED_VAR(cb_event);

    LOG_VERB("Initialize the Flash Driver");
    CRITICAL_SECTION_ENTER();
    switch (status = Flash_Init())
    {
    case ARM_DRIVER_OK:
        _Flash_Status.busy  = 0;
        _Flash_Status.error = 0;
        break;

    case ARM_DRIVER_ERROR_SPECIFIC:
        status = ARM_DRIVER_OK;
        break;

    default:
        _Flash_Status.busy  = 0;
        _Flash_Status.error = 1;
        break;
    }
    CRITICAL_SECTION_EXIT();

    return( status );
}


/**
 * De-initialize the Flash Interface.
 *
 * @return  \ref execution_status
 */
static int32_t Uninitialize(void)
{
    LOG_VERB("De-initialize the Flash Driver");
    return( ARM_DRIVER_OK );
}


/**
 * Control the Flash interface power.
 *
 * @param[in]  state  Power state
 *
 * @return  \ref execution_status
 */
static int32_t PowerControl(ARM_POWER_STATE state)
{
    UNUSED_VAR(state);

    return( ARM_DRIVER_OK );
}


/**
 * Get Flash status.
 *
 * @return  Flash status \ref ARM_FLASH_STATUS
 */
static ARM_FLASH_STATUS GetStatus(void)
{
    ARM_FLASH_STATUS   status;
    CRITICAL_SECTION_ALLOC();

    CRITICAL_SECTION_ENTER();
    status = _Flash_Status;
    CRITICAL_SECTION_EXIT();

    return( status );
}


/**
 * Erase complete Flash.
 *
 * @return  \ref execution_status
 */
static int32_t EraseChip(void)
{
    return( ARM_DRIVER_ERROR_UNSUPPORTED );
}


/*----------------------------------------------------------------------------*/
/**
 * IAP Flash logical address map to physical address.
 *
 * @param[in]  addr   Logical address, start of 0.
 * @param[in]  len    Logical address area length.
 *
 * @return  Physical address, Failed to return FLASH_INVALID_ADDR.
 */
__STATIC_INLINE uint32_t IAP_AddrMap(uint32_t addr, uint32_t len)
{
    uint32_t    start, end;
    uint32_t    sec;

    for(sec = 0;  sec < _IAP_FlashInfo.sector_count;  sec++)
    {   // 查找开始地址所在扇区
        start = _IAP_FlashInfo.sector_info[sec].start;
        end   = _IAP_FlashInfo.sector_info[sec].end;

        if (start > end) {
            continue;   // Invalid Sector in _IAP_FlashInfo.sector_info[]
        }
        if (addr >= start) {
            break;
        }
    }
    for(;  sec < _IAP_FlashInfo.sector_count;  sec++)
    {   // 查找结束地址所以在扇区
        start = _IAP_FlashInfo.sector_info[sec].start;
        end   = _IAP_FlashInfo.sector_info[sec].end;

        if (start > end) {
            continue;   // Invalid Sector in _IAP_FlashInfo.sector_info[]
        }
        if ((addr + len - 1) <= end) {
            break;
        }
    }
    if (sec >= _IAP_FlashInfo.sector_count) {
        LOG_ERROR("IAP Flash Operation error, Address: 0x%08lX, Bytes: %lu", addr, len);
        return( FLASH_INVALID_ADDR );   // 开始或结束地址错误
    }
    return( addr );
}


/**
 * Read data from Flash.
 *
 * @param[in]  addr  Data address.
 * @param[out] data  Pointer to a buffer storing the data read from Flash.
 * @param[in]  cnt   Number of data items to read.
 *
 * @return  number of data items read or \ref execution_status
 */
static int32_t IAP_ReadData(uint32_t addr, void* data, uint32_t cnt)
{
    int32_t     status;
    CRITICAL_SECTION_ALLOC();

    cnt <<= FLASH_DATA_WIDTH;
    LOG_VERB("IAP Flash Read, Address: 0x%08lX, Bytes: %lu", addr, cnt);
    if (data == NULL) {
        return(ARM_DRIVER_ERROR_PARAMETER);
    }
    if((addr = IAP_AddrMap(addr, cnt)) == FLASH_INVALID_ADDR) {
        return(ARM_DRIVER_ERROR_PARAMETER);
    }

    CRITICAL_SECTION_ENTER();
    if (_Flash_Status.busy) {
        CRITICAL_SECTION_EXIT();
        return( ARM_DRIVER_ERROR_BUSY );
    }
    _Flash_Status.busy  = 1;
    _Flash_Status.error = 0;
    CRITICAL_SECTION_EXIT();

    status = Flash_Read(addr, data, cnt);

    CRITICAL_SECTION_ENTER();
    if (status < 0) {
        _Flash_Status.error = 1;
    }
    else {
        status >>= FLASH_DATA_WIDTH;
    }
    _Flash_Status.busy = 0;
    CRITICAL_SECTION_EXIT();

    return( status );
}


/**
 * Program data to Flash.
 *
 * @param[in]  addr   Data address.
 * @param[in]  data   Pointer to a buffer containing the data to be programmed to Flash.
 * @param[in]  cnt    Number of data items to program.
 *
 * @return  number of data items programmed or \ref execution_status
 */
static int32_t IAP_ProgramData(uint32_t addr, const void* data, uint32_t cnt)
{
    int32_t     status;
    CRITICAL_SECTION_ALLOC();

    cnt <<= FLASH_DATA_WIDTH;
    LOG_VERB("IAP Flash Program, Address: 0x%08lX, Bytes: %lu", addr, cnt);
    if (data == NULL) {
        return(ARM_DRIVER_ERROR_PARAMETER);
    }
    if((addr = IAP_AddrMap(addr, cnt)) == FLASH_INVALID_ADDR) {
        return(ARM_DRIVER_ERROR_PARAMETER);
    }

    CRITICAL_SECTION_ENTER();
    if (_Flash_Status.busy) {
        CRITICAL_SECTION_EXIT();
        return( ARM_DRIVER_ERROR_BUSY );
    }
    _Flash_Status.busy  = 1;
    _Flash_Status.error = 0;
    CRITICAL_SECTION_EXIT();

    status = Flash_Program(addr, data, cnt);

    CRITICAL_SECTION_ENTER();
    if (status < 0) {
        _Flash_Status.error = 1;
    }
    else {
        status >>= FLASH_DATA_WIDTH;
    }
    _Flash_Status.busy = 0;
    CRITICAL_SECTION_EXIT();

    return( status );
}


/**
 * Erase Flash Sector.
 *
 * @param[in]  addr   Sector address
 *
 * @return  \ref execution_status
 */
static int32_t IAP_EraseSector(uint32_t addr)
{
    int32_t     status;
    CRITICAL_SECTION_ALLOC();

    LOG_VERB("IAP Flash Erase, Address: 0x%08lX", addr);
    if((addr = IAP_AddrMap(addr, 1)) == FLASH_INVALID_ADDR) {
        return(ARM_DRIVER_ERROR_PARAMETER);
    }

    CRITICAL_SECTION_ENTER();
    if (_Flash_Status.busy) {
        CRITICAL_SECTION_EXIT();
        return( ARM_DRIVER_ERROR_BUSY );
    }
    _Flash_Status.busy  = 1;
    _Flash_Status.error = 0;
    CRITICAL_SECTION_EXIT();

    status = Flash_Erase(addr);

    CRITICAL_SECTION_ENTER();
    if (status != ARM_DRIVER_OK) {
        _Flash_Status.error = 1;
    }
    _Flash_Status.busy = 0;
    CRITICAL_SECTION_EXIT();

    return( status );
}


/**
 * Get Flash information.
 *
 * @return  Pointer to Flash information \ref ARM_FLASH_INFO
 */
static ARM_FLASH_INFO* IAP_GetInfo(void)
{
    return( &_IAP_FlashInfo );
}


/* IAP Flash Driver Control Block */
ARM_DRIVER_FLASH  ARM_Driver_Flash_(IAP_FLASH_NUM) = {
    GetVersion,
    GetCapabilities,
    Initialize,
    Uninitialize,
    PowerControl,
    IAP_ReadData,
    IAP_ProgramData,
    IAP_EraseSector,
    EraseChip,
    GetStatus,
    IAP_GetInfo
};


#if (EFS_FLASH_NUM >= 0)
/*----------------------------------------------------------------------------*/
/**
 * EFS Flash logical address map to physical address.
 *
 * @param[in]  addr   Logical address, start of 0.
 * @param[in]  len    Logical address area length.
 *
 * @return  Physical address, Failed to return FLASH_INVALID_ADDR.
 */
__STATIC_INLINE uint32_t EFS_AddrMap(uint32_t addr, uint32_t len)
{
    uint32_t    start, end;
    uint32_t    sec;

    for(sec = 0;  sec < _EFS_FlashInfo.sector_count;  sec++)
    {   // 查找开始地址所在扇区
        start = _EFS_FlashInfo.sector_info[sec].start;
        end   = _EFS_FlashInfo.sector_info[sec].end;

        if (start > end) {
            continue;   // Invalid Sector in _EFS_FlashInfo.sector_info[]
        }
        if (addr >= start) {
            break;
        }
    }
    for(;  sec < _EFS_FlashInfo.sector_count;  sec++)
    {   // 查找结束地址所以在扇区
        start = _EFS_FlashInfo.sector_info[sec].start;
        end   = _EFS_FlashInfo.sector_info[sec].end;

        if (start > end) {
            continue;   // Invalid Sector in _EFS_FlashInfo.sector_info[]
        }
        if ((addr + len - 1) <= end) {
            break;
        }
    }
    if (sec >= _EFS_FlashInfo.sector_count) {
        LOG_ERROR("EFS Flash Operation error, Address: 0x%08lX, Bytes: %lu", addr, len);
        return( FLASH_INVALID_ADDR );   // 开始或结束地址错误
    }
    return( addr );
}


/**
 * Read data from Flash.
 *
 * @param[in]  addr  Data address.
 * @param[out] data  Pointer to a buffer storing the data read from Flash.
 * @param[in]  cnt   Number of data items to read.
 *
 * @return  number of data items read or \ref execution_status
 */
static int32_t EFS_ReadData(uint32_t addr, void* data, uint32_t cnt)
{
    int32_t     status;
    CRITICAL_SECTION_ALLOC();

    cnt <<= FLASH_DATA_WIDTH;
    LOG_VERB("EFS Flash Read, Address: 0x%08lX, Bytes: %lu", addr, cnt);
    if (data == NULL) {
        return(ARM_DRIVER_ERROR_PARAMETER);
    }
    if((addr = EFS_AddrMap(addr, cnt)) == FLASH_INVALID_ADDR) {
        return(ARM_DRIVER_ERROR_PARAMETER);
    }

    CRITICAL_SECTION_ENTER();
    if (_Flash_Status.busy) {
        CRITICAL_SECTION_EXIT();
        return( ARM_DRIVER_ERROR_BUSY );
    }
    _Flash_Status.busy  = 1;
    _Flash_Status.error = 0;
    CRITICAL_SECTION_EXIT();

    status = Flash_Read(addr, data, cnt);

    CRITICAL_SECTION_ENTER();
    if (status < 0) {
        _Flash_Status.error = 1;
    }
    else {
        status >>= FLASH_DATA_WIDTH;
    }
    _Flash_Status.busy = 0;
    CRITICAL_SECTION_EXIT();

    return( status );
}


/**
 * Program data to Flash.
 *
 * @param[in]  addr   Data address.
 * @param[in]  data   Pointer to a buffer containing the data to be programmed to Flash.
 * @param[in]  cnt    Number of data items to program.
 *
 * @return  number of data items programmed or \ref execution_status
 */
static int32_t EFS_ProgramData(uint32_t addr, const void* data, uint32_t cnt)
{
    int32_t     status;
    CRITICAL_SECTION_ALLOC();

    cnt <<= FLASH_DATA_WIDTH;
    LOG_VERB("EFS Flash Program, Address: 0x%08lX, Bytes: %lu", addr, cnt);
    if (data == NULL) {
        return(ARM_DRIVER_ERROR_PARAMETER);
    }
    if((addr = EFS_AddrMap(addr, cnt)) == FLASH_INVALID_ADDR) {
        return(ARM_DRIVER_ERROR_PARAMETER);
    }

    CRITICAL_SECTION_ENTER();
    if (_Flash_Status.busy) {
        CRITICAL_SECTION_EXIT();
        return( ARM_DRIVER_ERROR_BUSY );
    }
    _Flash_Status.busy  = 1;
    _Flash_Status.error = 0;
    CRITICAL_SECTION_EXIT();

    status = Flash_Program(addr, data, cnt);

    CRITICAL_SECTION_ENTER();
    if (status < 0) {
        _Flash_Status.error = 1;
    }
    else {
        status >>= FLASH_DATA_WIDTH;
    }
    _Flash_Status.busy = 0;
    CRITICAL_SECTION_EXIT();

    return( status );
}


/**
 * Erase Flash Sector.
 *
 * @param[in]  addr   Sector address
 *
 * @return  \ref execution_status
 */
static int32_t EFS_EraseSector(uint32_t addr)
{
    int32_t     status;
    CRITICAL_SECTION_ALLOC();

    LOG_VERB("EFS Flash Erase, Address: 0x%08lX", addr);
    if((addr = EFS_AddrMap(addr, 1)) == FLASH_INVALID_ADDR) {
        return(ARM_DRIVER_ERROR_PARAMETER);
    }

    CRITICAL_SECTION_ENTER();
    if (_Flash_Status.busy) {
        CRITICAL_SECTION_EXIT();
        return( ARM_DRIVER_ERROR_BUSY );
    }
    _Flash_Status.busy  = 1;
    _Flash_Status.error = 0;
    CRITICAL_SECTION_EXIT();

    status = Flash_Erase(addr);

    CRITICAL_SECTION_ENTER();
    if (status != ARM_DRIVER_OK) {
        _Flash_Status.error = 1;
    }
    _Flash_Status.busy = 0;
    CRITICAL_SECTION_EXIT();

    return( status );
}


/**
 * Get Flash information.
 *
 * @return  Pointer to Flash information \ref ARM_FLASH_INFO
 */
static ARM_FLASH_INFO* EFS_GetInfo(void)
{
    return( &_EFS_FlashInfo );
}


/* EFS Flash Driver Control Block */
ARM_DRIVER_FLASH  ARM_Driver_Flash_(EFS_FLASH_NUM) = {
    GetVersion,
    GetCapabilities,
    Initialize,
    Uninitialize,
    PowerControl,
    EFS_ReadData,
    EFS_ProgramData,
    EFS_EraseSector,
    EraseChip,
    GetStatus,
    EFS_GetInfo
};


#endif
/******************************************************************************/
//lint -save -w1
#if 0
static const uint32_t _xBuff[8] = {  0x5A5A5A5A
                                  , 0xA5A5A5A5
                                  , 0x96969696
                                  , 0x69696969
                                  , 0x3C3C3C3C
                                  , 0xC3C3C3C3
                                  , 0x1E1E1E1E
                                  , 0xE1E1E1E1
                                 };
static uint32_t       _rBuff[2048 / sizeof(uint32_t)];
static uint32_t       _wBuff[2048 / sizeof(uint32_t)];

static void Flash_WriteTest(ARM_DRIVER_FLASH *flashDev, uint32_t addr1, uint32_t addr2)
{
    uint32_t              i, j, addr, cnt;

    for(i = 0, cnt = sizeof(_wBuff) - i * 3, addr = addr1 + i;  (addr + cnt) <= addr2;)
    {
        for(j = 0;  j < ARRAY_SIZE(_wBuff);  j++)
        {
            _wBuff[j] =   ((addr ^ _xBuff[j % ARRAY_SIZE(_xBuff)]) & ~(ARRAY_SIZE(_wBuff) - 1))
                       | ((~j) % ARRAY_SIZE(_wBuff));
        }
        if (flashDev->ProgramData(addr, _wBuff, cnt) != cnt) {
            LOG_ASSERT("Error Program, Addr: 0x%08lX, Len: 0x%08lX", addr, cnt);
        }
        i     = (i + 1) % 4;
        cnt   = sizeof(_wBuff) - i * 3;
        addr += sizeof(_wBuff) + i;
    }
}

static void Flash_ReadTest(ARM_DRIVER_FLASH *flashDev, uint32_t addr1, uint32_t addr2)
{
    uint32_t              i, j, addr, cnt;

    for(i = 0, cnt = sizeof(_wBuff) - i * 3, addr = addr1 + i;  (addr + cnt) <= addr2;)
    {
        for(j = 0;  j < ARRAY_SIZE(_wBuff);  j++)
        {
            _wBuff[j] =   ((addr ^ _xBuff[j % ARRAY_SIZE(_xBuff)]) & ~(ARRAY_SIZE(_wBuff) - 1))
                       | ((~j) % ARRAY_SIZE(_wBuff));
            _rBuff[j] = 0;
        }
        if (flashDev->ReadData(addr, _rBuff, cnt) != cnt) {
            LOG_ASSERT("Error Read, Addr: 0x%08lX, Len: 0x%08lX", addr, cnt);
        }
        for(j = 0;  j < cnt;  j++)
        {
            if (((uint8_t *)_rBuff)[j] != ((uint8_t *)_wBuff)[j]) {
                LOG_ASSERT(  "Error Program Check, Addr: 0x%08lX, P/R: 0x%02lX / 0x%02lX"
                           , (uint32_t)addr + j
                           , (uint32_t)((uint8_t *)_wBuff)[j]
                           , (uint32_t)((uint8_t *)_rBuff)[j]
                          );
            }
        }
        i     = (i + 1) % 4;
        cnt   = sizeof(_wBuff) - i * 3;
        addr += sizeof(_wBuff) + i;
    }
}

static void Flash_EraseTest(ARM_DRIVER_FLASH *flashDev, uint32_t sec1, uint32_t sec2)
{
    ARM_FLASH_INFO  *flashInf = flashDev->GetInfo();
    uint32_t         adr1 = flashInf->sector_info[sec1 - 0].start;
    uint32_t         adr2 = flashInf->sector_info[sec2 - 1].end + 1;
    uint8_t          temp;
    uint32_t         i;

    for(i = sec1;  i < sec2;  i++)
    {
        if (flashDev->EraseSector(flashInf->sector_info[i].start) != ARM_DRIVER_OK) {
            LOG_ASSERT("Error Erase, Addr: 0x%08lX", flashInf->sector_info[i].start);
        }
    }
    for(i = adr1;  i < adr2;  i++)
    {
        flashDev->ReadData(i, &temp, sizeof(uint8_t));
        if (temp != flashInf->erased_value) {
            LOG_ASSERT(  "Error Erase Check, Addr: 0x%08lX, E/R: 0x%02lX/0x%02lX"
                       , (uint32_t) i
                       , (uint32_t) flashInf->erased_value
                       , (uint32_t) temp
                      );
        }
    }
}

void Flash_Test(void)
{
    ARM_FLASH_INFO     *flashInf0;
    ARM_FLASH_INFO     *flashInf1;
    ARM_DRIVER_FLASH   *flashDev0 = &(ARM_Driver_Flash_(IAP_FLASH_NUM));
    ARM_DRIVER_FLASH   *flashDev1 = &(ARM_Driver_Flash_(EFS_FLASH_NUM));
    const ARM_FLASH_CAPABILITIES capb0 = flashDev0->GetCapabilities();
    const ARM_FLASH_CAPABILITIES capb1 = flashDev1->GetCapabilities();

    // Initialize
    if (flashDev0->Initialize(NULL) != ARM_DRIVER_OK) {
        LOG_ASSERT("Error Initialize IAP Flash");
    }
    if (flashDev1->Initialize(NULL) != ARM_DRIVER_OK) {
        LOG_ASSERT("Error Initialize EFS Flash");
    }
    flashInf0 = flashDev0->GetInfo();
    flashInf1 = flashDev1->GetInfo();

    // PowerControl
    if (flashDev0->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK) {
        LOG_ASSERT("Error PowerControl IAP Flash");
    }
    if (flashDev1->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK) {
        LOG_ASSERT("Error PowerControl EFS Flash");
    }

    uint32_t  EFS_Sec1 = 0;
    uint32_t  EFS_Sec2 = flashInf1->sector_count;
    uint32_t  EFS_Adr1 = flashInf1->sector_info[EFS_Sec1 - 0].start;
    uint32_t  EFS_Adr2 = flashInf1->sector_info[EFS_Sec2 - 1].end + 1;
    uint32_t  IAP_Sec1 = 0;
    uint32_t  IAP_Sec2 = 0;
    uint32_t  IAP_Adr1 = 0;
    uint32_t  IAP_Adr2 = EFS_Adr1;
    for(IAP_Sec1 = 0;  IAP_Sec1 < flashInf0->sector_count;  IAP_Sec1++)
    {
        if (flashInf0->sector_info[IAP_Sec1].start > ROM_LINK_LIMIT) {
            break;
        }
    }
    if (IAP_Sec1 >= flashInf0->sector_count) {
        LOG_ASSERT("Can't test IAP Flash");  // 开始扇区无效
    }
    if ((IAP_Adr1 = flashInf0->sector_info[IAP_Sec1].start) > EFS_Adr1) {
        LOG_ASSERT("Can't test IAP Flash");  // 开始地址无效
    }
    for(IAP_Sec2 = IAP_Sec1;  IAP_Sec2 < flashInf0->sector_count;  IAP_Sec2++)
    {
        if (flashInf0->sector_info[IAP_Sec2].start >= EFS_Adr1) {
            break;
        }
    }
    if (flashInf0->sector_info[IAP_Sec2].start > EFS_Adr1) {
        LOG_ASSERT("Can't test IAP Flash");  // 结束扇区无效
    }

    // EraseSector
    Flash_EraseTest(flashDev0, IAP_Sec1, IAP_Sec2);
    Flash_EraseTest(flashDev1, EFS_Sec1, EFS_Sec2);

    // ProgramData
    Flash_WriteTest(flashDev0, IAP_Adr1, IAP_Adr2);
    LOG_INFO("IAP Flash Program Test ok");
    Flash_WriteTest(flashDev1, EFS_Adr1, EFS_Adr2);
    LOG_INFO("EFS Flash Program Test ok");

    // ReadData
    Flash_ReadTest(flashDev0, IAP_Adr1, IAP_Adr2);
    LOG_INFO("IAP Flash Read and Check Test ok");
    Flash_ReadTest(flashDev1, EFS_Adr1, EFS_Adr2);
    LOG_INFO("EFS Flash Read and Check Test ok");

    // EraseSector
    Flash_EraseTest(flashDev0, IAP_Sec1, IAP_Sec2);
    LOG_INFO("IAP Flash Erase and Check Test ok");
    Flash_EraseTest(flashDev1, EFS_Sec1, EFS_Sec2);
    LOG_INFO("EFS Flash Erase and Check Test ok");

    LOG_INFO("End of test");
    while (1)
    {
        DelayMS(100);
    }
}
#endif
//lint -restore


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

