/*******************************************************************************
 * @file    emb_prompt.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/5/6
 * @brief   Embedded platform defines for the prompt of system status
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/5/6 -- Linghu -- the first version
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

#include <stdint.h>
#include "defines.h"            // Used: __INLINE

#define __Q_SIZE  uint8_t
#include "emb_queue.h"
#include "emb_delay.h"
#include "emb_prompt.h"


/******************************************************************************/
/**
 * @addtogroup __Config
 * @{
 * @addtogroup              Emb_Platform
 * @{
 ******************************************************************************/
/**
 * ϵͳ����״̬��ʾ������(һ��ʹ��˫ɫLED)
 *
 * ע��: [CPUʱ�� / �ڴ� / C���п�]����δ��ʼ��
 */
#ifndef SYS_PROMPT_LED_INIT
#define SYS_PROMPT_LED_INIT()                   //!< (C) ϵͳ��ʾ LED ��ʼ��
#endif

#ifndef SYS_PROMPT_LED_OFF
#define SYS_PROMPT_LED_OFF()                    //!< (C) ϵͳ��ʾ LED �ر�
#endif

#ifndef SYS_PROMPT_LED_RED
#define SYS_PROMPT_LED_RED()                    //!< (C) ϵͳ��ʾ LED ����ɫ��
#endif

#ifndef SYS_PROMPT_LED_GREEN
#define SYS_PROMPT_LED_GREEN()                  //!< (C) ϵͳ��ʾ LED ����ɫ��
#endif

#ifndef SYS_PROMPT_LED_YELLOW
#define SYS_PROMPT_LED_YELLOW()                 //!< (C) ϵͳ��ʾ LED ����ɫ��
#endif


/*******************************************************************************
 * @}
 * @}
 * @addtogroup Emb_Prompt
 * @{
 * @addtogroup          Private_Constants
 * @{
 ******************************************************************************/

#define SYS_PROMPT_ASSERT       1       // ��������
#define SYS_PROMPT_BOOTING      2       // ��������
#define SYS_PROMPT_UPGRADE      3       // �����̼�
#define SYS_PROMPT_ERROR        4       // ������ʾ
#define SYS_PROMPT_WARNING      5       // ������ʾ
#define SYS_PROMPT_INITING      6       // ���ڳ�ʼ��
#define SYS_PROMPT_NORMAL       7       // ��������
#define SYS_PROMPT_INVALID     255      // ��Ч״̬

#define SYS_PROMPT_HOLD         3       // ������˸����
#define SYS_PROMPT_INVL        200      // ��ʱ���(����)


/*******************************************************************************
 * @}
 * @addtogroup          Private_Variables
 * @{
 ******************************************************************************/

static QUEUE_TYPE(uint8_t, 8) _prompt_queue  = {0}; // ϵͳ��ʾ����
static uint32_t               _prompt_timers =  0 ; // ϵͳ��ʾ��ʱ��(1ms��λ)
static uint8_t                _prompt_counts =  0 ; // ϵͳ��ʾ����(200ms��λ)
static uint8_t                _prompt_busy   =  0 ; // æ��־


/*******************************************************************************
 * @}
 * @addtogroup          Private_Functions
 * @{
 ******************************************************************************/

static __INLINE void SysPrompt_doPoll(uint8_t status)
{
    while(_prompt_busy) ;
    _prompt_busy = 1;   // ����æ״̬

    if (status == SYS_PROMPT_INVALID)
    {   // �Ӷ��� ��ȡ ��ʾ״̬
        switch (QUEUE_ITEM(_prompt_queue)) {
        default:                    // �����ж����ʾ״̬
            if (_prompt_counts >= (SYS_PROMPT_HOLD * 4)) {
                _prompt_counts  = 0;
                QUEUE_O_ADJ(_prompt_queue, 1);
            }
        case 1 : //lint !e616 !e825 // ���н�һ����ʾ״̬
            QUEUE_PEEK(_prompt_queue, 0, &status);
            break;
        case 0 :                    // ���п�, ����ʾ״̬
            break;
        }
    }

    switch (status) {
    default :
    case SYS_PROMPT_ASSERT  :   // ��������
        QUEUE_INIT(_prompt_queue); SYS_PROMPT_LED_RED();     break;
    case SYS_PROMPT_BOOTING :   // ��������
        QUEUE_INIT(_prompt_queue); SYS_PROMPT_LED_GREEN();   break;
    case SYS_PROMPT_UPGRADE :   // �����̼�
        QUEUE_INIT(_prompt_queue); SYS_PROMPT_LED_YELLOW();  break;
    case SYS_PROMPT_ERROR   :   // ������ʾ
        if(_prompt_counts & 0x01){ SYS_PROMPT_LED_RED();   }
        else                     { SYS_PROMPT_LED_OFF();   } break;
    case SYS_PROMPT_WARNING :   // ������ʾ
        if(_prompt_counts & 0x02){ SYS_PROMPT_LED_RED();   }
        else                     { SYS_PROMPT_LED_OFF();   } break;
    case SYS_PROMPT_INITING :   // ���ڳ�ʼ��
        if(_prompt_counts & 0x01){ SYS_PROMPT_LED_GREEN(); }
        else                     { SYS_PROMPT_LED_OFF();   } break;
    case SYS_PROMPT_NORMAL  :   // ��������
        if(_prompt_counts & 0x02){ SYS_PROMPT_LED_GREEN(); }
        else                     { SYS_PROMPT_LED_OFF();   } break;
    }

    _prompt_counts++;
    _prompt_busy = 0;   // �˳�æ״̬
}


static __INLINE void SysPrompt_doSet(uint8_t status)
{
    uint8_t  last =  SYS_PROMPT_INVALID;

    if(!(QUEUE_IS_FULL(_prompt_queue)))  // ���з���
    {
        if (QUEUE_ITEM(_prompt_queue)) { // ���зǿ�
            QUEUE_PEEK(_prompt_queue, -1, &last);
        }
        if (last != status) { // �������������������ͬ״̬
            QUEUE_PUSH((_prompt_queue), status);
        }
    }
}


void SysPrompt_Assert(void)
{   // to turn on LED
    SysPrompt_doPoll(SYS_PROMPT_ASSERT);
}


void SysPrompt_Booting(void)
{   // to turn on LED
    SysPrompt_doPoll(SYS_PROMPT_BOOTING);
}


void SysPrompt_Upgrade(void)
{   // to turn on LED
    SysPrompt_doPoll(SYS_PROMPT_UPGRADE);
}


void SysPrompt_Error(void)
{   // to insert queue
    SysPrompt_doSet(SYS_PROMPT_ERROR);
}


void SysPrompt_Warning(void)
{   // to insert queue
    SysPrompt_doSet(SYS_PROMPT_WARNING);
}


void SysPrompt_Initing(void)
{   // to insert queue
    SysPrompt_doSet(SYS_PROMPT_INITING);
}


void SysPrompt_Normal(void)
{   // to insert queue
    SysPrompt_doSet(SYS_PROMPT_NORMAL);
}


void SysPromptStateInit(void)
{
    QUEUE_INIT(_prompt_queue);
    _prompt_timers = 0;
    _prompt_counts = 0;
    _prompt_busy   = 0;

    SYS_PROMPT_LED_INIT();
}


void SysPromptStatePoll(void)
{
    uint32_t          now;

    now = GetSysMsec();
    if((now - _prompt_timers) > (uint32_t)INT32_MAX) {
        return;
    }
    _prompt_timers = now + SYS_PROMPT_INVL;

    SysPrompt_doPoll(SYS_PROMPT_INVALID);   // ��˸����
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

