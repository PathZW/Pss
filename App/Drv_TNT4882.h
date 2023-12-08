/*******************************************************************************
 * @file    Drv_TNT4882.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/9/11
 * @brief   Driver for GPIB interface Chip of TNT4882.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/9/11 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __DRV_TNT4882_H__
#define __DRV_TNT4882_H__
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup Drv_TNT4882
 * @{
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/

int  TNT4882_SendBegin(uint8_t *data, int len);
int  TNT4882_SendWait (void);

int  TNT4882_RecvBegin(uint8_t *data, int len);
int  TNT4882_RecvWait (void);

int  TNT4882_Init(uint8_t GpibAddr);
void TNT4882_EventFunReg( void (*Fun_ListenEnd)(void), void (*Fun_TalkEnable)(void) );


void     GPIB_AddrInit(void);
uint8_t  GPIB_AddrGet (void);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __DRV_TNT4882_H__
/*****************************  END OF FILE  **********************************/

