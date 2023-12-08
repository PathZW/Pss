/*******************************************************************************
 * @file    board.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/4/26
 * @brief   Device's defines for the hardware board
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/4/26 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __BOARD_H__
#define __BOARD_H__
/******************************************************************************/

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#include <stdint.h>
#include "Board_Device.h"


/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/**
 * @addtogroup Board
 * @{
 * @addtogroup              Exported_Macros
 * @{
 ******************************************************************************/

#ifndef ROM_MAIN_ADDR
#define ROM_MAIN_ADDR       0       //!< (C) ��ROM�� ��ַ
#endif
#ifndef ROM_MAIN_SIZE
#define ROM_MAIN_SIZE       0       //!< (C) ��ROM�� ��С
#endif
#ifndef ROM_RESV_SIZE
#define ROM_RESV_SIZE       0       //!< (C) ��ROM�� ������С(BootLoader)
#endif

#ifndef RAM_MAIN_ADDR
#define RAM_MAIN_ADDR       0       //!< (C) ��RAM�� ��ַ
#endif
#ifndef RAM_MAIN_SIZE
#define RAM_MAIN_SIZE       0       //!< (C) ��RAM�� ��С
#endif

/*----------------------------------------------------------------------------*/
#ifndef ROM_LINK_START
#define ROM_LINK_START      0       //!< (C) ��������ʹ�õ� Ĭ��ROM�� ��ʼ��ַ
#endif
#ifndef ROM_LINK_LIMIT
#define ROM_LINK_LIMIT      0       //!< (C) ��������ʹ�õ� Ĭ��ROM�� ������ַ
#endif

#ifndef RAM_LINK_START
#define RAM_LINK_START      0       //!< (C) ��������ʹ�õ� Ĭ��RAM�� ��ʼ��ַ
#endif
#ifndef RAM_LINK_LIMIT
#define RAM_LINK_LIMIT      0       //!< (C) ��������ʹ�õ� Ĭ��RAM�� ������ַ
#endif

#ifndef HEAP_EXT_BASE
#define HEAP_EXT_BASE          RAM_LINK_LIMIT         //!< (C) std Heap ��չ�ڴ��ַ
#endif
#ifndef HEAP_EXT_SIZE
#define HEAP_EXT_SIZE (RAM_MAIN_SIZE - RAM_LINK_SIZE) //!< (C) std Heap ��չ�ڴ��С
#endif


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
/**
 * ���Ӳ����ʼ��, һ���ʼ�� CPUʱ�����ڴ�, �Ա� C runtime ����
 */
void Board_BareInit(void);

/**
 * Ӳ����ʼ��, RTOS δ��ʼ��
 */
void Board_Init(void);

/**
 * Ӳ������, RTOS δ��ʼ��
 */
void Board_Start(void);

/**
 * Ӳ����λ, ����ϵͳ����
 */
void Board_Reset(void);


/*----------------------------------------------------------------------------*/
/**
 * Ӳ����ʱ��ʼ�������³�ʼ��. (�ں�ʱ�� �� C runtime δ��ʼ��)
 */
void Board_DelayInit(void);

/** 
 * ��ȡϵͳ����ʱ��Ӳ������(���뵥λ), ע���ܹ��� ISR �е���
 */
uint32_t Board_GetMsec(void);

/** 
 * ΢����ʱ, ע���ܹ��� ISR �е���
 */
void Board_DelayUS(uint16_t uSec);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __BOARD_H__
/*****************************  END OF FILE  **********************************/

