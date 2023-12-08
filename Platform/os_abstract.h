/*******************************************************************************
 * @file    os_abstract.h
 * @author  Linghu
 * @version v1.0.0
 * @date    2019/5/22
 * @brief   User define for CMSIS-RTOS2
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/5/22 -- Linghu -- the first version
 ******************************************************************************/
//lint -emacro(506, assert_msg)         " Constant value Boolean "
//lint -emacro(717, OS_THREAD_CREATE)   " do ... while(0) "
//lint -emacro(717, OS_THREAD_DELETE)   " do ... while(0) "
//lint -emacro(717, OS_THREAD_PRIO_SET) " do ... while(0) "
//lint -emacro(717, OS_THREAD_SUSPEND)  " do ... while(0) "
//lint -emacro(717, OS_THREAD_RESUME)   " do ... while(0) "
//lint -emacro(506, _OS_PRIO_CONVERT)   " Constant value Boolean "

//lint -emacro(717, OS_MUTEX_CREATE)    " do ... while(0) "
//lint -emacro(717, OS_MUTEX_DELETE)    " do ... while(0) "
//lint -emacro(527, OS_MUTEX_ENTER)     " Unreachable code at token 'xxxx' "
//lint -emacro(527, OS_MUTEX_LEAVE)     " Unreachable code at token 'xxxx' "
//lint -emacro(717, OS_MUTEX_LEAVE)     " do ... while(0) "
//lint -emacro(788, OS_MUTEX_LEAVE)     " enum constant 'xxxx' not used within defaulted switch "

//lint -emacro(717, OS_RWLOCK_CREATE)   " do ... while(0) "
//lint -emacro(717, OS_RWLOCK_DELETE)   " do ... while(0) "
//lint -emacro(801, OS_RWLOCK_RD_ENTER) " Use of goto is deprecated "
//lint -emacro(717, OS_RWLOCK_RD_LEAVE) " do ... while(0) "
//lint -emacro(801, OS_RWLOCK_RD_LEAVE) " Use of goto is deprecated "
//lint -emacro(717, OS_RWLOCK_WR_LEAVE) " do ... while(0) "

#include "assert.h" //lint !e451    // Header file repeatedly included
#include "stdint.h"
#include "cmsis_os2.h"


/******************************************************************************/
#ifndef __OS_ABSTRACT_H__
#define __OS_ABSTRACT_H__
#ifdef  __cplusplus
extern  "C"
{
#endif
/*******************************************************************************
 * @addtogroup          OS_Abstract_declare
 * @{
 ******************************************************************************/

#define delay_ms(msec)      (void)osDelay(msec)
#define delay_us(usec)
#define delay_ms_to(msec)   (void)osDelayUntil(msec)
#define delay_us_to(usec)
#define count_ms_get()      (uint32_t)osKernelGetTickCount()
#define count_us_get()


#define OS_WAIT_FOREVER     osWaitForever   // ���޵ȴ���ʱֵ

#define OS_PRIO_LOW             6           // �߳����ȼ�: ���
#define OS_PRIO_BELOW_NORMAL    5           // �߳����ȼ�: ������ͨ
#define OS_PRIO_NORMAL          4           // �߳����ȼ�: ��ͨ
#define OS_PRIO_ABOVE_NORMAL    3           // �߳����ȼ�: ������ͨ
#define OS_PRIO_HIGH            2           // �߳����ȼ�: ���
#define OS_PRIO_REAL_TIME       1           // �߳����ȼ�: ʵʱ, һ����������


/*----------------------------------------------------------------------------*/
/**
 * ȷ���ں��Ƿ�ʼ����, (false)δ����, (true)������
 */
#define OS_KERNEL_RUNNING()         (osKernelGetState() > osKernelReady)

/**
 * �����ں����������, ����ֵ��ǰ������״̬[ (0)δ����, (>0)������, (<0)�������� ]
 */
#define OS_KERNEL_LOCK()            (int32_t)osKernelLock()

/**
 * �����ں����������, ����ֵ��ǰ������״̬[ (0)δ����, (>0)������, (<0)�������� ]
 */
#define OS_KERNEL_UNLOCK()          (int32_t)osKernelUnlock()

/**
 * ��ȡ�ں�TickƵ��ֵ
 */
#define OS_KERNEL_TICK_FRQ_GET()    (uint32_t)osKernelGetTickFreq()

/**
 * ��ȡ�ں�Tick����ֵ
 */
#define OS_KERNEL_TICK_CNT_GET()    (uint32_t)osKernelGetTickCount()

/**
 * ��ȡ�ں�ClickƵ��ֵ
 */
#define OS_KERNEL_CLK_FRQ_GET()     (uint32_t)osKernelGetSysTimerFreq()

/**
 * ��ȡ�ں�Click����ֵ
 */
#define OS_KERNEL_CLK_CNT_GET()     (uint32_t)osKernelGetSysTimerCount()


/*----------------------------------------------------------------------------*/
/**
 * �̴߳���
 * @param      thread_id  �̱߳�ʶ, ����Ϊ OS_THREAD_ID ����
 * @param[in]  kname      �߳�����, ���̱߳�ɾ��ǰ���뱣�ֲ���
 * @param[in]  fun        �߳���ں���, void fun(void* arg)
 * @param[in]  arg        �߳���ں�������
 * @param[in]  stk_mem    �̶߳�ջָ��, (NULL)�Զ�����
 * @param[in]  stk_size   �̶߳�ջ��С, ���ֽ�Ϊ��λ
 * @param[in]  prio       �̵߳����ȼ�, ��Ч��Χ(1~~6), (1)Ϊ������ȼ�
 */
#define OS_THREAD_CREATE(thread_id, kname, fun, arg, stk_mem, stk_size, prio)  \
        do{ osThreadAttr_t      DEF_NAME_LINE(_thd_attr);                      \
            DEF_NAME_LINE(_thd_attr).name       = (kname);                     \
            DEF_NAME_LINE(_thd_attr).attr_bits  = osThreadDetached;            \
            DEF_NAME_LINE(_thd_attr).cb_mem     = NULL;                        \
            DEF_NAME_LINE(_thd_attr).cb_size    = 0;                           \
            DEF_NAME_LINE(_thd_attr).stack_mem  = (stk_mem);                   \
            DEF_NAME_LINE(_thd_attr).stack_size = (stk_size);                  \
            DEF_NAME_LINE(_thd_attr).priority   = _OS_PRIO_CONVERT(prio);      \
            DEF_NAME_LINE(_thd_attr).tz_module  = 0;                           \
            DEF_NAME_LINE(_thd_attr).reserved   = 0;                           \
            (thread_id) = osThreadNew((fun), (arg), &DEF_NAME_LINE(_thd_attr));\
            if ((thread_id) == NULL) {                                         \
                assert_msg(0, "ERROR: OS_THREAD_CREATE()");                    \
            }                                                                  \
        } while(0)

/**
 * �߳�ɾ��
 * @param[in]  thread_id  �̱߳�ʶ, ����Ϊ OS_THREAD_ID ����
 */
#define OS_THREAD_DELETE(thread_id)                         \
        do{ if (osThreadTerminate(thread_id) != osOK) {     \
                assert_msg(0, "ERROR: OS_THREAD_DELETE()"); \
            }                                               \
        } while(0)

/**
 * �߳����ȼ�����
 * @param      thread_id  �̱߳�ʶ, ����Ϊ OS_THREAD_ID ����
 * @param[in]  prio       �̵߳����ȼ�, ��Ч��Χ(1~~6), (1)Ϊ������ȼ�
 */
#define OS_THREAD_PRIO_SET(thread_id, prio)                                         \
        do{ if (osThreadSetPriority((thread_id), _OS_PRIO_CONVERT(prio)) != osOK) { \
                assert_msg(0, "ERROR: OS_THREAD_DELETE()");                         \
            }                                                                       \
        } while(0)

/**
 * �̹߳���
 * @param[in]  thread_id  �̱߳�ʶ, ����Ϊ OS_THREAD_ID ����
 */
#define OS_THREAD_SUSPEND(thread_id)                         \
        do{ if (osThreadSuspend(thread_id) != osOK) {        \
                assert_msg(0, "ERROR: OS_THREAD_SUSPEND()"); \
            }                                                \
        } while(0)

/**
 * �ָ̻߳�
 * @param[in]  thread_id  �̱߳�ʶ, ����Ϊ OS_THREAD_ID ����
 */
#define OS_THREAD_RESUME(thread_id)                         \
        do{ if (osThreadResume(thread_id) != osOK) {        \
                assert_msg(0, "ERROR: OS_THREAD_RESUME()"); \
            }                                               \
        } while(0)

/**
 * �˳���ǰ�߳�
 */
#define OS_THREAD_EXIT()    osThreadExit()

/**
 * ��ǰ�߳��ó���������Դ
 */
#define OS_THREAD_YIELD()   (void)osThreadYield()


/**
 * �̶߳�ջ����(����һ���ڴ�������Ե�����)
 * @param      name  �̶߳�ջ����������
 * @param[in]  size  �̶߳�ջ��С,  ���ֽ�Ϊ��λ
 */
#define OS_THREAD_STK_DEF(name, size)   uint64_t name[((size) + 7) / 8]

/**
 *  �̶߳����ʶ����
 */
#define OS_THREAD_ID            osThreadId_t

#define _OS_PRIO_CONVERT(prio)  (osPriority_t)((int32_t)osPriorityISR - \
                                ((6 < (prio)) ? 6 : (((prio) < 1) ? 1 : (prio))) * 8);

/*----------------------------------------------------------------------------*/
/**
 * ����������
 * @param      mutex_id  ��������ʶ, ����Ϊ OS_MUTEX_ID ����
 * @param[in]  kname     ����������, �ڻ�������ɾ��ǰ���뱣�ֲ���
 */
#define OS_MUTEX_CREATE(mutex_id, kname)                                \
        do{ osMutexAttr_t  DEF_NAME_LINE(_mtx_attr) = {                 \
                NULL, osMutexRecursive | osMutexPrioInherit, NULL, 0u   \
            };                                                          \
            DEF_NAME_LINE(_mtx_attr).name = (kname);                    \
            (mutex_id) = osMutexNew(&DEF_NAME_LINE(_mtx_attr));         \
            if ((mutex_id) == NULL) {                                   \
                assert_msg(0, "ERROR: OS_MUTEX_CREATE()");              \
            }                                                           \
        } while(0)

/**
 * ������ɾ��
 * @param[in]   mutex_id  ��������ʶ, ����Ϊ OS_MUTEX_ID ����
 */
#define OS_MUTEX_DELETE(mutex_id)                           \
        do{ if (osMutexDelete(mutex_id) != osOK) {          \
                assert_msg(0, "ERROR: OS_MUTEX_DELETE()");  \
            }                                               \
        } while(0)

/**
 * ���뻥������������
 * @param[in]   mutex_id  ��������ʶ, ����Ϊ OS_MUTEX_ID ����
 * @param[in]   timeout   �ȴ���ʱֵ, (0)���ȴ���������, (OS_WAIT_FOREVER)���޵ȴ�
 * @code
 *  OS_MUTEX_ENTER(mutex_id, OS_WAIT_FOREVER)
 *  {
 *      ...
 *      break;  // ��ȫ�뿪��������������
 *      ...
 *  }
 *  OS_MUTEX_LEAVE(mutex_id, OS_MUTEX_TIMEOUT_HANDLER)
 * @endcode
 */
#define OS_MUTEX_ENTER(mutex_id, timeout)                               \
            switch (osMutexAcquire((mutex_id), (timeout))) {            \
            default:   assert_msg(0, "ERROR: OS_MUTEX_ENTER()"); break; \
            case osOK: do

/**
 * �뿪��������������
 * @param[in]   mutex_id         ��������ʶ, ����Ϊ OS_MUTEX_ID ����
 * @param[in]   handler_timeout  ��ʱ����, OS_MUTEX_ENTER() ��ʱ��ִ��
 */
#define OS_MUTEX_LEAVE(mutex_id, handler_timeout)             \
                while(0) ;                                    \
                if (osMutexRelease(mutex_id) != osOK) {       \
                    assert_msg(0, "ERROR: OS_MUTEX_LEAVE()"); \
                }                                             \
                break;                                        \
            case osErrorTimeout:                              \
            case osErrorResource:  handler_timeout;  break;   \
            }

/**
 * �����������ʶ����
 */
#define OS_MUTEX_ID     osMutexId_t

/**
 * Ԥ����Ļ������ȴ���ʱ����
 */
#define OS_MUTEX_TIMEOUT_HANDLER    assert_msg(0, "ERROR: Timeout of OS_MUTEX_ENTER()")


/*----------------------------------------------------------------------------*/

#define OS_RWLOCK_CREATE(rwlock_id, kname)                                \
        do{ osMutexAttr_t  DEF_NAME_LINE(_mtx_attr) = {                   \
                NULL, osMutexRecursive | osMutexPrioInherit, NULL, 0u     \
            };                                                            \
            DEF_NAME_LINE(_mtx_attr).name = (kname);                      \
            (rwlock_id)->r_mutex = osMutexNew(&DEF_NAME_LINE(_mtx_attr)); \
            if ((rwlock_id)->r_mutex == NULL) {                           \
                assert_msg(0, "ERROR: OS_MUTEX_CREATE()");                \
            }                                                             \
            (rwlock_id)->w_mutex = osMutexNew(&DEF_NAME_LINE(_mtx_attr)); \
            if ((rwlock_id)->w_mutex == NULL) {                           \
                assert_msg(0, "ERROR: OS_MUTEX_CREATE()");                \
            }                                                             \
        } while(0)

#define OS_RWLOCK_DELETE(rwlock_id)                            \
        do{ if (osMutexDelete((rwlock_id)->r_mutex) != osOK) { \
                assert_msg(0, "ERROR: OS_MUTEX_DELETE()");     \
            }                                                  \
            if (osMutexDelete((rwlock_id)->w_mutex) != osOK) { \
                assert_msg(0, "ERROR: OS_MUTEX_DELETE()");     \
            }                                                  \
        } while(0)

#define OS_RWLOCK_RD_ENTER(rwlock_id)                                                    \
            if (osMutexAcquire((rwlock_id)->r_mutex, OS_WAIT_FOREVER) != osOK) {         \
                DEF_NAME_LINE(error): assert_msg(0, "ERROR: OS_RWLOCK_RD_ENTER()");      \
            }                                                                            \
            else {                                                                       \
                if (0 == ((rwlock_id)->readers)++) {                                     \
                    if (osMutexAcquire((rwlock_id)->w_mutex, OS_WAIT_FOREVER) != osOK) { \
                        goto DEF_NAME_LINE(error);                                       \
                    }                                                                    \
                }                                                                        \
                if (osMutexRelease((rwlock_id)->r_mutex) != osOK) {                      \
                    goto DEF_NAME_LINE(error);                                           \
                }                                                                        \
                do

#define OS_RWLOCK_RD_LEAVE(rwlock_id)                                                    \
                while(0) ;                                                               \
                if (osMutexAcquire((rwlock_id)->r_mutex, OS_WAIT_FOREVER) != osOK) {     \
                    DEF_NAME_LINE(error): assert_msg(0, "ERROR: OS_RWLOCK_RD_LEAVE()");  \
                }                                                                        \
                else {                                                                   \
                    if (--((rwlock_id)->readers) == 0) {                                 \
                        if (osMutexRelease((rwlock_id)->w_mutex) != osOK) {              \
                            goto DEF_NAME_LINE(error);                                   \
                        }                                                                \
                    }                                                                    \
                    if (osMutexRelease((rwlock_id)->r_mutex) != osOK) {                  \
                        goto DEF_NAME_LINE(error);                                       \
                    }                                                                    \
                }                                                                        \
            }

#define OS_RWLOCK_WR_ENTER(rwlock_id)                                            \
            if (osMutexAcquire((rwlock_id)->w_mutex, OS_WAIT_FOREVER) != osOK) { \
                assert_msg(0, "ERROR: OS_RWLOCK_WR_ENTER()");                    \
            }                                                                    \
            else {                                                               \
                do

#define OS_RWLOCK_WR_LEAVE(rwlock_id)                                            \
                while(0) ;                                                       \
                if (osMutexRelease((rwlock_id)->w_mutex) != osOK) {              \
                    assert_msg(0, "ERROR: OS_RWLOCK_WR_LEAVE()");                \
                }                                                                \
            }

#define OS_RWLOCK_ID        os_rwlock_id

typedef struct os_rwlock_ {
    osMutexId_t     r_mutex;
    osMutexId_t     w_mutex;
    int16_t         readers;
} os_rwlock_id;

/*----------------------------------------------------------------------------*/
#ifndef assert_msg
#define assert_msg(e, msg)      while(!e)
#endif

#if 0
#ifndef UNUSED_ARG
#define UNUSED_ARG(arg)         (void)arg
#endif

#ifndef ALIGN_ADDR
#define ALIGN_ADDR(align)
#endif
#ifndef ALIGN_SIZE
#define ALIGN_SIZE(size, align) (((size) + (align) - 1) & ~((align) - 1))
#endif
#endif

#ifndef DEF_NAME_LINE
/**
 * ����һ�����к���صķ�����
 */
#define DEF_NAME_LINE(name)     DEF_NAME_LINE1(name, __LINE__)
#define DEF_NAME_LINE1(x, y)    DEF_NAME_LINE2(x, y)
#define DEF_NAME_LINE2(x, y)    x##_##y
#endif


/*****************************  END OF FILE  ***********************************
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __OS_ABSTRACT_H__
/*****************************  END OF FILE  **********************************/

