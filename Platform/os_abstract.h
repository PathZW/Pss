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


#define OS_WAIT_FOREVER     osWaitForever   // 无限等待超时值

#define OS_PRIO_LOW             6           // 线程优先级: 最低
#define OS_PRIO_BELOW_NORMAL    5           // 线程优先级: 低于普通
#define OS_PRIO_NORMAL          4           // 线程优先级: 普通
#define OS_PRIO_ABOVE_NORMAL    3           // 线程优先级: 高于普通
#define OS_PRIO_HIGH            2           // 线程优先级: 最高
#define OS_PRIO_REAL_TIME       1           // 线程优先级: 实时, 一般用于驱动


/*----------------------------------------------------------------------------*/
/**
 * 确认内核是否开始运行, (false)未启动, (true)已启动
 */
#define OS_KERNEL_RUNNING()         (osKernelGetState() > osKernelReady)

/**
 * 锁定内核任务调度器, 返回值以前的锁定状态[ (0)未锁定, (>0)已锁定, (<0)发生错误 ]
 */
#define OS_KERNEL_LOCK()            (int32_t)osKernelLock()

/**
 * 解锁内核任务调度器, 返回值以前的锁定状态[ (0)未锁定, (>0)已锁定, (<0)发生错误 ]
 */
#define OS_KERNEL_UNLOCK()          (int32_t)osKernelUnlock()

/**
 * 获取内核Tick频率值
 */
#define OS_KERNEL_TICK_FRQ_GET()    (uint32_t)osKernelGetTickFreq()

/**
 * 获取内核Tick计数值
 */
#define OS_KERNEL_TICK_CNT_GET()    (uint32_t)osKernelGetTickCount()

/**
 * 获取内核Click频率值
 */
#define OS_KERNEL_CLK_FRQ_GET()     (uint32_t)osKernelGetSysTimerFreq()

/**
 * 获取内核Click计数值
 */
#define OS_KERNEL_CLK_CNT_GET()     (uint32_t)osKernelGetSysTimerCount()


/*----------------------------------------------------------------------------*/
/**
 * 线程创建
 * @param      thread_id  线程标识, 必须为 OS_THREAD_ID 类型
 * @param[in]  kname      线程名称, 在线程被删除前必须保持不变
 * @param[in]  fun        线程入口函数, void fun(void* arg)
 * @param[in]  arg        线程入口函数参数
 * @param[in]  stk_mem    线程堆栈指针, (NULL)自动分配
 * @param[in]  stk_size   线程堆栈大小, 以字节为单位
 * @param[in]  prio       线程的优先级, 有效范围(1~~6), (1)为最高优先级
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
 * 线程删除
 * @param[in]  thread_id  线程标识, 必须为 OS_THREAD_ID 类型
 */
#define OS_THREAD_DELETE(thread_id)                         \
        do{ if (osThreadTerminate(thread_id) != osOK) {     \
                assert_msg(0, "ERROR: OS_THREAD_DELETE()"); \
            }                                               \
        } while(0)

/**
 * 线程优先级设置
 * @param      thread_id  线程标识, 必须为 OS_THREAD_ID 类型
 * @param[in]  prio       线程的优先级, 有效范围(1~~6), (1)为最高优先级
 */
#define OS_THREAD_PRIO_SET(thread_id, prio)                                         \
        do{ if (osThreadSetPriority((thread_id), _OS_PRIO_CONVERT(prio)) != osOK) { \
                assert_msg(0, "ERROR: OS_THREAD_DELETE()");                         \
            }                                                                       \
        } while(0)

/**
 * 线程挂起
 * @param[in]  thread_id  线程标识, 必须为 OS_THREAD_ID 类型
 */
#define OS_THREAD_SUSPEND(thread_id)                         \
        do{ if (osThreadSuspend(thread_id) != osOK) {        \
                assert_msg(0, "ERROR: OS_THREAD_SUSPEND()"); \
            }                                                \
        } while(0)

/**
 * 线程恢复
 * @param[in]  thread_id  线程标识, 必须为 OS_THREAD_ID 类型
 */
#define OS_THREAD_RESUME(thread_id)                         \
        do{ if (osThreadResume(thread_id) != osOK) {        \
                assert_msg(0, "ERROR: OS_THREAD_RESUME()"); \
            }                                               \
        } while(0)

/**
 * 退出当前线程
 */
#define OS_THREAD_EXIT()    osThreadExit()

/**
 * 当前线程让出处理器资源
 */
#define OS_THREAD_YIELD()   (void)osThreadYield()


/**
 * 线程堆栈声明(声明一个内存对齐属性的数组)
 * @param      name  线程堆栈声明符号名
 * @param[in]  size  线程堆栈大小,  以字节为单位
 */
#define OS_THREAD_STK_DEF(name, size)   uint64_t name[((size) + 7) / 8]

/**
 *  线程对象标识类型
 */
#define OS_THREAD_ID            osThreadId_t

#define _OS_PRIO_CONVERT(prio)  (osPriority_t)((int32_t)osPriorityISR - \
                                ((6 < (prio)) ? 6 : (((prio) < 1) ? 1 : (prio))) * 8);

/*----------------------------------------------------------------------------*/
/**
 * 互斥量创建
 * @param      mutex_id  互斥量标识, 必须为 OS_MUTEX_ID 类型
 * @param[in]  kname     互斥量名称, 在互斥量被删除前必须保持不变
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
 * 互斥量删除
 * @param[in]   mutex_id  互斥量标识, 必须为 OS_MUTEX_ID 类型
 */
#define OS_MUTEX_DELETE(mutex_id)                           \
        do{ if (osMutexDelete(mutex_id) != osOK) {          \
                assert_msg(0, "ERROR: OS_MUTEX_DELETE()");  \
            }                                               \
        } while(0)

/**
 * 进入互斥量保护区域
 * @param[in]   mutex_id  互斥量标识, 必须为 OS_MUTEX_ID 类型
 * @param[in]   timeout   等待超时值, (0)不等待立即返回, (OS_WAIT_FOREVER)无限等待
 * @code
 *  OS_MUTEX_ENTER(mutex_id, OS_WAIT_FOREVER)
 *  {
 *      ...
 *      break;  // 安全离开互斥量保护区域
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
 * 离开互斥量保护区域
 * @param[in]   mutex_id         互斥量标识, 必须为 OS_MUTEX_ID 类型
 * @param[in]   handler_timeout  超时处理, OS_MUTEX_ENTER() 超时后执行
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
 * 互斥量对象标识类型
 */
#define OS_MUTEX_ID     osMutexId_t

/**
 * 预定义的互斥量等待超时处理
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
 * 定义一个与行号相关的符号名
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

