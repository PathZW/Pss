/*******************************************************************************
 * @file    sys_arch.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/11/13
 * @brief   lwIP operating system emulation layer.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2021/5/7 -- Linghu -- the first version, support for uCOS-III.
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint +fie                     // Integer model for Enum flag
//lint -e818                    // Pointer parameter 'xxx' could be declared to const
//lint -emacro(717, SYS_STATS_*)    // do ... while(0)
//#ifdef _lint
//#undef  LWIP_ASSERT
//#define LWIP_ASSERT(...)
//#endif

#include "os.h"

#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"


/******************************************************************************/

#ifndef LWIP_SYS_ARCH_MALLOC
#define LWIP_SYS_ARCH_MALLOC            0   // Used memory alloc from lwIP heap
#endif

#ifndef LWIP_SYS_MUTEX_NAME
#define LWIP_SYS_MUTEX_NAME         "lwIP_Mutex"
#endif

#ifndef LWIP_SYS_SEM_NAME
#define LWIP_SYS_SEM_NAME           "lwIP_Sem"
#endif

#ifndef LWIP_SYS_MBOX_NAME
#define LWIP_SYS_MBOX_NAME          "lwIP_Mbox"
#endif
#ifndef LWIP_SYS_MBOX_SIZE
#define LWIP_SYS_MBOX_SIZE              8
#endif

#ifndef LWIP_SYS_THREAD_NAME
#define LWIP_SYS_THREAD_NAME        "lwIP_Thread"
#endif
#ifndef LWIP_SYS_THREAD_STKB
#define LWIP_SYS_THREAD_STKB           1024
#endif
#ifndef LWIP_SYS_THREAD_STKB_MIN
#define LWIP_SYS_THREAD_STKB_MIN        512
#endif

#define MEM_ALIGN_SIZE_STK(size)   ( ((size) +  (CPU_CFG_ADDR_SIZE * 2U - 1U)) \
                                             & ~(CPU_CFG_ADDR_SIZE * 2U - 1U)  \
                                   )


/*************************** thread-local semaphore ***************************/
#ifndef osThreadSemInit
#define osThreadSemInit()
#define osThreadSemGet()        ((void *)OSTCBCurPtr)
#define osThreadSemAlloc()
#define osThreadSemFree()
#endif


/*************************** tick / milliseconds ******************************/
#ifndef osMSecTickInit
#if 1
#define osMSecTickInit()
#define osTickToMSec(tick)      (tick)
#define osMSecToTick(msec)      (msec)
#if !defined(OS_CFG_TICK_RATE_HZ) && (OS_CFG_TICK_RATE_HZ != 1000)
#error "OS_CFG_APP.h, OS_CFG_TICK_RATE_HZ must be 1000"
#endif
#else
#define osMSecTickInit()
#define osTickToMSec(tick)      ((tick) * 1000 / OS_CFG_TICK_RATE_HZ)
#define osMSecToTick(msec)      ((msec) * OS_CFG_TICK_RATE_HZ / 1000)
#endif
#endif



/*************************** Mutex ********************************************/
#if !NO_SYS
err_t sys_mutex_new(sys_mutex_t *mutex)
{
    OS_ERR      err;

    OSMutexCreate(mutex, LWIP_SYS_MUTEX_NAME, &err);
    if (err == OS_ERR_NONE) {
        SYS_STATS_INC_USED(mutex);
        return( ERR_OK );
    }
    SYS_STATS_INC(mutex.err);
    return( ERR_MEM );
}

void sys_mutex_free(sys_mutex_t *mutex)
{
    OS_ERR      err;

    OSMutexDel(mutex, OS_OPT_DEL_ALWAYS, &err); //lint !e534
    LWIP_ASSERT("Failed to free Mutex", err == OS_ERR_NONE);
    LWIP_UNUSED_ARG(err);
    SYS_STATS_DEC(mutex.used);
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
    OS_ERR      err;

    OSMutexPend(mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &err);
    LWIP_ASSERT("Failed to lock Mutex", (err == OS_ERR_NONE) || (err == OS_ERR_MUTEX_OWNER));
    LWIP_UNUSED_ARG(err);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
    OS_ERR      err;

    OSMutexPost(mutex, OS_OPT_POST_NONE, &err);
    LWIP_ASSERT("Failed to unlock Mutex", (err == OS_ERR_NONE) || (err == OS_ERR_MUTEX_NESTING));
    LWIP_UNUSED_ARG(err);
}

int sys_mutex_valid(sys_mutex_t *mutex)
{
    return( (mutex != NULL) && (mutex->Type == OS_OBJ_TYPE_MUTEX) );
}

void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
    if (mutex != NULL) {
        mutex->Type = OS_OBJ_TYPE_NONE;
    }
}
#endif /* NO_SYS */


/*************************** Semaphore ****************************************/
#if !NO_SYS
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    OS_ERR      err;

    OSSemCreate(sem, LWIP_SYS_SEM_NAME, count, &err);
    if (err == OS_ERR_NONE) {
        SYS_STATS_INC_USED(sem);
        return( ERR_OK );
    }
    SYS_STATS_INC(sem.err);
    return( ERR_MEM );
}

void sys_sem_free(sys_sem_t *sem)
{
    OS_ERR      err;

    OSSemDel(sem, OS_OPT_DEL_ALWAYS, &err); //lint !e534
    LWIP_ASSERT("Failed to free Semaphore", err == OS_ERR_NONE);
    LWIP_UNUSED_ARG(err);
    SYS_STATS_DEC(sem.used);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    OS_ERR      err;
    u32_t       time;

    if((timeout != 0) && ((timeout = osMSecToTick(timeout)) == 0)) { //lint !e774
        timeout  = 1;
    }
    time = OSTimeGet(&err);

    if (OSTCBCurPtr != ((void *)sem)) {
        OSSemPend(sem, timeout, OS_OPT_PEND_BLOCKING, NULL, &err); //lint !e534
    } else {
        OSTaskSemPend( timeout, OS_OPT_PEND_BLOCKING, NULL, &err); //lint !e534
    }

    if (err == OS_ERR_NONE) {
        time = osTickToMSec(OSTimeGet(&err) - time);
        return( (time == SYS_ARCH_TIMEOUT) ? (SYS_ARCH_TIMEOUT - 1) : time );
    }
    LWIP_ASSERT("Failed to wait Semaphore", err == OS_ERR_TIMEOUT);
    return( SYS_ARCH_TIMEOUT );
}

void sys_sem_signal(sys_sem_t *sem)
{
    OS_ERR      err;

    if((sem != NULL) && (sem->Type == OS_OBJ_TYPE_SEM)) {
        OSSemPost(            sem, OS_OPT_POST_1   , &err); //lint !e534
    } else {
        OSTaskSemPost((void *)sem, OS_OPT_POST_NONE, &err); //lint !e534
    }
    LWIP_ASSERT("Failed to signal Semaphore", err == OS_ERR_NONE);
    LWIP_UNUSED_ARG(err);
}

void sys_sem_signal_fromisr(sys_sem_t *sem)
{
    sys_sem_signal(sem);
}

int sys_sem_valid(sys_sem_t *sem)
{
//  return( (sem != NULL) && (sem->Type == OS_OBJ_TYPE_SEM) );
    return( sem != NULL );
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
//  if (sem != NULL) {
//      sem->Type = OS_OBJ_TYPE_NONE;
//  }
    LWIP_UNUSED_ARG(sem);
}

void sys_thread_sem_new(void)
{
    osThreadSemAlloc();
}

void sys_thread_sem_free(void)
{
    osThreadSemFree();
}

sys_sem_t *sys_thread_sem_get(void)
{
    return( osThreadSemGet() );
}
#endif /* NO_SYS */


/*************************** Mailbox ******************************************/
#if !NO_SYS
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    OS_ERR      err;

    if (size <= 0) {
        size  = LWIP_SYS_MBOX_SIZE;
    }
    OSQCreate(mbox, LWIP_SYS_MBOX_NAME, size, &err); //lint !e734

    if (err == OS_ERR_NONE) {
        SYS_STATS_INC_USED(mbox);
        return( ERR_OK );
    }
    SYS_STATS_INC(mbox.err);
    return( ERR_MEM );
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    OS_ERR      err;

    OSQDel(mbox, OS_OPT_DEL_ALWAYS, &err); //lint !e534
    LWIP_ASSERT("Failed to free mbox", err == OS_ERR_NONE);
    LWIP_UNUSED_ARG(err);
    SYS_STATS_DEC(mbox.used);
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    OS_ERR      err;

    while(1) //lint !e716
    {
        if (sys_mbox_trypost(mbox, msg) == ERR_OK) {
            return;
        }
        OSTimeDly(2, OS_OPT_TIME_DLY, &err);
    }
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    OS_ERR      err;

    OSQPost(mbox, msg, 0, OS_OPT_POST_FIFO, &err);
    if (err == OS_ERR_NONE) {
        return( ERR_OK );
    }
    LWIP_ASSERT("Failed to post mbox", (err == OS_ERR_MSG_POOL_EMPTY) || (err == OS_ERR_Q_MAX));
    return( ERR_MEM );
}

err_t sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg)
{
    return( sys_mbox_trypost(mbox, msg) );
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    OS_ERR      err;
    OS_MSG_SIZE size;
    u32_t       time;

    if((timeout != 0) && ((timeout = osMSecToTick(timeout)) == 0)) { //lint !e774
        timeout  = 1;
    }
    time = OSTimeGet(&err);

    *msg = OSQPend(mbox, timeout, OS_OPT_PEND_BLOCKING, &size, NULL, &err);
    LWIP_UNUSED_ARG(size);

    if (err == OS_ERR_NONE) {
        time = osTickToMSec(OSTimeGet(&err) - time);
        return( (time == SYS_ARCH_TIMEOUT) ? (SYS_ARCH_TIMEOUT - 1) : time );
    }
    LWIP_ASSERT("Failed to fetch mbox", err == OS_ERR_TIMEOUT);
    return( SYS_ARCH_TIMEOUT );
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    OS_ERR      err;
    OS_MSG_SIZE size;

    *msg = OSQPend(mbox, 0, OS_OPT_PEND_NON_BLOCKING, &size, NULL, &err);
    LWIP_UNUSED_ARG(size);

    if (err == OS_ERR_NONE) {
        return( 0 );
    }
    LWIP_ASSERT("Failed to tryfetch mbox", err == OS_ERR_PEND_WOULD_BLOCK);
    return( SYS_MBOX_EMPTY );
}

int sys_mbox_valid(sys_mbox_t *mbox)
{
    return( (mbox != NULL) && (mbox->Type == OS_OBJ_TYPE_Q) );
}

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    if (mbox != NULL) {
        mbox->Type = OS_OBJ_TYPE_NONE;
    }
}
#endif /* NO_SYS */


/*************************** System *******************************************/
#if !NO_SYS
/**
 * Create and starts a new thread.
 *
 * @param[in]  name       human-readable name for the thread
 * @param[in]  thread     thread-function
 * @param[in]  arg        parameter passed to 'thread'
 * @param[in]  stacksize  stack size in bytes, 0 -- used RTOS  default value
 * @param[in]  prio       priority of the thread
 *
 * @return  thread ID for reference by other functions or NULL in case of error
 */
sys_thread_t sys_thread_new(const char* name, lwip_thread_fn thread, void* arg, int stacksize, int prio)
{                       // 请注意: sys_thread_new() 创建的线程不能退出,
    OS_ERR      err;    //        线程退出会导致 mem_malloc() 分配的内存泄漏
    OS_TCB     *p_tcb;
    void       *p_stk;
    int         size;

    if((size = MEM_ALIGN_SIZE_STK(stacksize)) == 0) {          //lint !e713 !e737
        size = MEM_ALIGN_SIZE_STK(LWIP_SYS_THREAD_STKB);
    }
    if (size < MEM_ALIGN_SIZE_STK(LWIP_SYS_THREAD_STKB_MIN)) { //lint !e574 !e737
        size = MEM_ALIGN_SIZE_STK(LWIP_SYS_THREAD_STKB_MIN);
    }
    p_tcb = mem_malloc(size + MEM_ALIGN_SIZE_STK(sizeof(OS_TCB))); //lint !e737
    p_stk = ((u8_t *)p_tcb) + MEM_ALIGN_SIZE_STK(sizeof(OS_TCB)) ;
    LWIP_ASSERT("Failed to create thread", p_tcb != NULL);

    OSTaskCreate( (OS_TCB     *)  p_tcb,                                        // p_tcb
                  (CPU_CHAR   *)((name == NULL) ? LWIP_SYS_THREAD_NAME : name), // p_name
                  (OS_TASK_PTR )  thread,                                       // p_task
                  (void       *)  arg,                                          // p_arg
                  (OS_PRIO     )  OS_PRIO_TO_UCOS3(prio),                       // prio
                  (CPU_STK    *)  p_stk,                                        // p_stk_base
                  (CPU_STK_SIZE) (OS_CFG_TASK_STK_REDZONE_DEPTH + 4),           // stk_limit
                  (CPU_STK_SIZE)  OS_STKB_TO_STKW(size),  //lint !e573 !e737    // stk_size
                  (OS_MSG_QTY  )  0u,                                           // q_size
                  (OS_TICK     )  0u,                                           // time_quanta
                  (void       *)  NULL,                                         // p_ext
                  (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),   // opt
                  (OS_ERR     *) &err                                           // p_err
                );
    LWIP_ASSERT("Failed to create thread", err == OS_ERR_NONE);

    LWIP_UNUSED_ARG(err);
    return( p_tcb );
}

/**
 * Terminates the calling thread, not lwIP port
 */
void sys_thread_exit(void)
{
    OS_ERR      err;

    OSTaskDel(NULL, &err);
    LWIP_UNUSED_ARG(err);
}

void sys_arch_msleep(u32_t ms)
{
    OS_ERR      err;

    if (ms == 0) {
        return;
    }
    if((ms = osMSecToTick(ms)) == 0) { //lint !e774
        ms =  1;
    }
    OSTimeDly(ms, OS_OPT_TIME_DLY, &err);
    LWIP_ASSERT("Failed to sleep", err == OS_ERR_NONE);
    LWIP_UNUSED_ARG(err);
}
#endif /* !NO_SYS */


u32_t sys_jiffies(void)
{
    OS_ERR      err;
    OS_TICK     tick;

    tick = OSTimeGet(&err);
    LWIP_ASSERT("Failed on sys_jiffies()", err == OS_ERR_NONE);
    LWIP_UNUSED_ARG(err);
    return( tick );
}

u32_t sys_now(void)
{
    OS_ERR      err;
    OS_TICK     tick;

    tick = OSTimeGet(&err);
    LWIP_ASSERT("Failed on sys_jiffies()", err == OS_ERR_NONE);
    LWIP_UNUSED_ARG(err);
    return( osTickToMSec(tick) );
}

void sys_init(void)
{
    osMSecTickInit();
    osThreadSemInit();
}


/*************************** TCPIP thread LOCK ********************************/
#if !NO_SYS && LWIP_TCPIP_CORE_LOCKING
extern sys_mutex_t  lock_tcpip_core;

void sys_mark_tcpip_thread(void)
{
}

int sys_core_is_locked(void)
{
    return(  (lock_tcpip_core.Type == OS_OBJ_TYPE_MUTEX)
           ? (lock_tcpip_core.OwnerTCBPtr == OSTCBCurPtr)
           : 1
          );
}

#elif !NO_SYS && !LWIP_TCPIP_CORE_LOCKING
static sys_thread_t tcpip_thread_id;

void sys_mark_tcpip_thread(void)
{
    tcpip_thread_id = OSTCBCurPtr;
}

int sys_core_is_locked(void)
{
    return( (OSTaskQty != 0) && (tcpip_thread_id == OSTCBCurPtr) );
}
#endif


/*****************************  END OF FILE  **********************************/

