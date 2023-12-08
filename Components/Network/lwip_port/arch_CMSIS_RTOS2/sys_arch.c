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
 *      2020/3/20 --- Linghu -- add function sys_thread_exit()
 *
 * @par Change Logs:
 *      2019/12/18 -- Linghu -- add LWIP_NETCONN_SEM_PER_THREAD support for RTX5,
 *                              used Signals BIT13 ~ 15 to thread-local semaphore,
 *                              thread Signals BIT0 ~ 12 used for MDK USB stack.
 *
 * @par Change Logs:
 *      2019/12/11 -- Linghu -- add LWIP_SYS_ARCH_MALLOC support for RTX5.
 *                              used memory alloc from lwIP heap.
 *
 * @par Change Logs:
 *      2019/11/29 -- Linghu -- add LWIP_TCPIP_CORE_LOCKING support for CMSIS-RTOS2
 *
 * @par Change Logs:
 *      2019/11/13 -- Linghu -- the first version, support for CMSIS-RTOS2.
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
//lint -emacro(835, osFlagsWaitAny, osFlagsNoClear)

#ifdef   _RTE_                  // RTE_CMSIS_RTOS2_RTX5
#include "RTE_Components.h"     // Component selection
#endif
#ifdef   RTE_CMSIS_RTOS2_RTX5
#include "rtx_os.h"             // RTX5
#endif
#include "cmsis_os2.h"

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

#ifndef LWIP_SEM_FLAGS_MASK
#define LWIP_SEM_FLAGS_MASK         0x0000E000
#endif
#ifndef LWIP_SEM_FLAGS_ONE
#define LWIP_SEM_FLAGS_ONE          0x00002000
#endif
#ifndef LWIP_SEM_FLAGS_INC
#define LWIP_SEM_FLAGS_INC          0x00002000
#endif


/*************************** thread-local semaphore ***************************/
#if !NO_SYS && LWIP_NETCONN_SEM_PER_THREAD

#ifdef RTE_CMSIS_RTOS2_RTX5
#define osThreadSemInit()
#define osThreadSemGet()            ((sys_sem_t *)osThreadGetId())
#define osThreadSemAlloc()
#define osThreadSemFree()

static osStatus_t osThreadSemIsValid(sys_sem_t *sem)
{
    osRtxThread_t *thread = (osRtxThread_t *)sem;  //lint !e740 !e826

    if ((thread == NULL) || (thread->id != osRtxIdThread)) {
        return( osErrorParameter );
    }
    switch (thread->state & osRtxThreadStateMask) {
    case osRtxThreadInactive   :
    case osRtxThreadReady      :
    case osRtxThreadRunning    :
    case osRtxThreadBlocked    :
    case osRtxThreadTerminated :  return( osOK  );
    default                    :  return( osErrorParameter );
    }
}

static osStatus_t osThreadSemAcquire(u32_t timeout)
{
    osStatus_t     status;
    uint32_t       flags;
    osRtxThread_t *thread;
    SYS_ARCH_DECL_PROTECT(lev);

    flags = osThreadFlagsWait(LWIP_SEM_FLAGS_MASK, osFlagsWaitAny | osFlagsNoClear, timeout);
    if (flags & osFlagsError) {
        return( (osStatus_t)flags ); // osError osErrorTimeout osErrorResource osErrorParameter
    }
    SYS_ARCH_PROTECT(lev);
    if ((thread = osRtxInfo.thread.run.curr) == NULL) {
        status  = osError;
    } else
    if(!(thread->thread_flags & LWIP_SEM_FLAGS_MASK)) {
        status  = osError;
    } else {
        status  = osOK;
        thread->thread_flags -= LWIP_SEM_FLAGS_ONE;
    }
    SYS_ARCH_UNPROTECT(lev);

    if (status != osOK) {
        return( status );            // osError
    }
    if((flags = osThreadFlagsSet(thread, 0)) & osFlagsError) {
        return( (osStatus_t)flags ); // osError osErrorParameter osErrorResource
    }
    return( osOK );
}

static osStatus_t osThreadSemRelease(sys_sem_t *sem)
{
    osStatus_t     status;
    uint32_t       flags;
    osRtxThread_t *thread = (osRtxThread_t *)sem;  //lint !e740 !e826
    SYS_ARCH_DECL_PROTECT(lev);

    if (thread == NULL) {
        return( osErrorParameter );  // osErrorParameter
    }
    SYS_ARCH_PROTECT(lev);
    if (thread->id != osRtxIdThread) {
        status  = osErrorParameter;
    } else
    if((thread->thread_flags & LWIP_SEM_FLAGS_MASK) == LWIP_SEM_FLAGS_MASK) {
        status  = osErrorResource;
    } else {
        status  = osOK;
        thread->thread_flags += LWIP_SEM_FLAGS_ONE;
    }
    SYS_ARCH_UNPROTECT(lev);

    if (status != osOK) {
        return( status );            // osError
    }
    if((flags = osThreadFlagsSet(thread, 0)) & osFlagsError) {
        return( (osStatus_t)flags ); // osError osErrorParameter osErrorResource
    }
    return( osOK );
}
#else   // RTE_CMSIS_RTOS2_RTX5
#error "not supported RTOS"
#endif  // RTE_CMSIS_RTOS2_RTX5

#else   // !NO_SYS && LWIP_NETCONN_SEM_PER_THREAD

#define osThreadSemInit()
#define osThreadSemGet()            ( NULL )
#define osThreadSemAlloc()
#define osThreadSemFree()

#define osThreadSemIsValid(sem)     (osErrorParameter)
#define osThreadSemAcquire(timeout) (osErrorParameter)
#define osThreadSemRelease(sem)     (osErrorParameter)
#endif  // !NO_SYS && LWIP_NETCONN_SEM_PER_THREAD


/*************************** tick / milliseconds ******************************/
//lint -emacro({774}, osTickToMSec, osMSecToTick)

#if 0
static u8_t  _num_tick2ms;  // Numerator   of tick to milliseconds
static u8_t  _den_tick2ms;  // Denominator of tick to milliseconds

static __INLINE u32_t gcd(u32_t a, u32_t b)
{   // Greatest common divisor
    while( b ^= a ^= b ^= a %= b ) ;  //lint !e720 !e564
    return( a );
}

static void osMSecTickInit(void)
{
    u32_t  m, num, den;
    u32_t  tick_freq = osKernelGetTickFreq();

    m   = gcd(tick_freq, 1000);             // Greatest common divisor
    num = 1000 / m;
    den = tick_freq / m;

    LWIP_ASSERT(  "Kernel tick frequency Out of Range"
                ,    (num <= UINT8_MAX) && (num != 0)
                  && (den <= UINT8_MAX) && (den != 0)
               );
    _num_tick2ms = (u8_t)num;
    _den_tick2ms = (u8_t)den;
}
#define osTickToMSec(tick)  (((tick) * _num_tick2ms + _den_tick2ms / 2) / _den_tick2ms)
#define osMSecToTick( ms )  ((( ms ) * _den_tick2ms + _num_tick2ms / 2) / _num_tick2ms)
#else

static void osMSecTickInit(void)
{
    LWIP_ASSERT("Kernel tick frequency must be 1000Hz", osKernelGetTickFreq() == 1000);
}
#define osTickToMSec(tick)  (tick)
#define osMSecToTick( ms )  ( ms )
#endif


/*************************** Mutex ********************************************/
#if !NO_SYS
err_t sys_mutex_new(sys_mutex_t *mutex)
{   //lint --e{641}     // converting enum to int
    osMutexAttr_t  attr = { LWIP_SYS_MUTEX_NAME, osMutexRecursive | osMutexPrioInherit | osMutexRobust, NULL, 0 };
    osMutexId_t    id;

  #if defined(RTE_CMSIS_RTOS2_RTX5) && (LWIP_SYS_ARCH_MALLOC)
    attr.cb_mem  =  mem_malloc(LWIP_MEM_ALIGN_SIZE(sizeof(osRtxMutex_t)));
    if (attr.cb_mem == NULL) {
        SYS_STATS_INC(mutex.err);
        return( ERR_MEM );
    }
    attr.cb_size =  LWIP_MEM_ALIGN_SIZE(sizeof(osRtxMutex_t));
  #endif

    if((id = osMutexNew(&attr)) != NULL) {
        *mutex = id;
        SYS_STATS_INC_USED(mutex);
        return( ERR_OK );
    }

  #if defined(RTE_CMSIS_RTOS2_RTX5) && (LWIP_SYS_ARCH_MALLOC)
    mem_free(attr.cb_mem);
  #endif
    SYS_STATS_INC(mutex.err);
    return( ERR_MEM );
}

void sys_mutex_free(sys_mutex_t *mutex)
{
    osStatus_t  status = osMutexDelete(*mutex);
    LWIP_ASSERT("Failed to free Mutex", status == osOK);
    LWIP_UNUSED_ARG(status);
  #if defined(RTE_CMSIS_RTOS2_RTX5) && (LWIP_SYS_ARCH_MALLOC)
    mem_free(*mutex);
  #endif
    SYS_STATS_DEC(mutex.used);
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
    osStatus_t  status = osMutexAcquire(*mutex, osWaitForever);
    LWIP_ASSERT("Failed to lock Mutex", status == osOK);
    LWIP_UNUSED_ARG(status);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
    osStatus_t  status = osMutexRelease(*mutex);
    LWIP_ASSERT("Failed to unlock Mutex", status == osOK);
    LWIP_UNUSED_ARG(status);
}

int sys_mutex_valid(sys_mutex_t *mutex)
{
    return( ((mutex) != NULL) && (*(mutex) != NULL) );
}

void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
    if ( (mutex) != NULL) {
        *(mutex)  = NULL;
    }
}
#endif /* NO_SYS */


/*************************** Semaphore ****************************************/
#if !NO_SYS
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{   //lint --e{641}     // converting enum to int
    osSemaphoreAttr_t  attr = { LWIP_SYS_SEM_NAME, 0, NULL, 0 };
    osSemaphoreId_t    id;

  #if defined(RTE_CMSIS_RTOS2_RTX5) && (LWIP_SYS_ARCH_MALLOC)
    attr.cb_mem  =  mem_malloc(LWIP_MEM_ALIGN_SIZE(sizeof(osRtxSemaphore_t)));
    if (attr.cb_mem == NULL) {
        SYS_STATS_INC(sem.err);
        return( ERR_MEM );
    }
    attr.cb_size =  LWIP_MEM_ALIGN_SIZE(sizeof(osRtxSemaphore_t));
  #endif

    if((id = osSemaphoreNew(UINT16_MAX, count, &attr)) != NULL) {
        *sem = id;
        SYS_STATS_INC_USED(sem);
        return( ERR_OK );
    }

  #if defined(RTE_CMSIS_RTOS2_RTX5) && (LWIP_SYS_ARCH_MALLOC)
    mem_free(attr.cb_mem);
  #endif
    SYS_STATS_INC(sem.err);
    return( ERR_MEM );
}

void sys_sem_free(sys_sem_t *sem)
{
    osStatus_t  status = osSemaphoreDelete(*sem);
    LWIP_ASSERT("Failed to free Semaphore", status == osOK);
    LWIP_UNUSED_ARG(status);
  #if defined(RTE_CMSIS_RTOS2_RTX5) && (LWIP_SYS_ARCH_MALLOC)
    mem_free(*sem);
  #endif
    SYS_STATS_DEC(sem.used);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    osStatus_t  status;
    uint32_t    time;

    if (timeout == 0) {
        timeout =  osWaitForever;
    } else
    if((timeout = osMSecToTick(timeout)) == 0) {
        timeout =  1;
    }
    time   =  osKernelGetTickCount();
    status = ( (osThreadSemIsValid(sem) == osOK)
              ? osThreadSemAcquire(timeout)
              : osSemaphoreAcquire(*sem, timeout)
             );
    time   =  osTickToMSec(osKernelGetTickCount() - time);

    if (status != osOK)
    {
        LWIP_ASSERT( "Failed to wait Semaphore"
                    , (status == osErrorTimeout) || (status == osErrorResource)
                   );
        return( SYS_ARCH_TIMEOUT );
    }
    return( (time == SYS_ARCH_TIMEOUT) ? (SYS_ARCH_TIMEOUT - 1) : time );
}

void sys_sem_signal(sys_sem_t *sem)
{
    osStatus_t  status = ( (osThreadSemIsValid(sem) == osOK)
                          ? osThreadSemRelease(sem)
                          : osSemaphoreRelease(*sem)
                         );
    LWIP_ASSERT("Failed to signal Semaphore", status == osOK);
    LWIP_UNUSED_ARG(status);
}

void sys_sem_signal_fromisr(sys_sem_t *sem)
{
    sys_sem_signal(sem);
}

int sys_sem_valid(sys_sem_t *sem)
{
    return( ((sem) != NULL) && (*(sem) != NULL) );
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
    if ( (sem) != NULL) {
        *(sem)  = NULL;
    }
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
{   //lint --e{641}     // converting enum to int
    osMessageQueueAttr_t  attr = { LWIP_SYS_MBOX_NAME, 0, NULL, 0, NULL, 0 };
    osMessageQueueId_t    id;

    if (size == 0) {
        size = LWIP_SYS_MBOX_SIZE;
    }
  #if defined(RTE_CMSIS_RTOS2_RTX5) && (LWIP_SYS_ARCH_MALLOC)
    attr.cb_mem  =  mem_malloc(  LWIP_MEM_ALIGN_SIZE(sizeof(osRtxMessageQueue_t))
                               + osRtxMessageQueueMemSize(size, sizeof(void *))  //lint !e737
                              );
    if (attr.cb_mem == NULL) {
        SYS_STATS_INC(mbox.err);
        return( ERR_MEM );
    }
    attr.cb_size =  LWIP_MEM_ALIGN_SIZE(sizeof(osRtxMessageQueue_t));
    attr.mq_mem  =  LWIP_MEM_ALIGN_SIZE(sizeof(osRtxMessageQueue_t)) + ((char *)attr.cb_mem);
    attr.mq_size =  osRtxMessageQueueMemSize(size, sizeof(void *));  //lint !e737
  #endif

    if((id = osMessageQueueNew((uint32_t)size, sizeof(void*), &attr)) != NULL) {
        *mbox = id;
        SYS_STATS_INC_USED(mbox);
        return( ERR_OK );
    }

  #if defined(RTE_CMSIS_RTOS2_RTX5) && (LWIP_SYS_ARCH_MALLOC)
    mem_free(attr.cb_mem);
  #endif
    SYS_STATS_INC(mbox.err);
    return( ERR_MEM );
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    osStatus_t  status = osMessageQueueDelete(*mbox);
    LWIP_ASSERT("Failed to free mbox", status == osOK);
    LWIP_UNUSED_ARG(status);
  #if defined(RTE_CMSIS_RTOS2_RTX5) && (LWIP_SYS_ARCH_MALLOC)
    mem_free(*mbox);
  #endif
    SYS_STATS_DEC(mbox.used);
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    osStatus_t  status = osMessageQueuePut(*mbox, &msg, 0, osWaitForever);
    LWIP_ASSERT("Failed to post mbox", status == osOK);
    LWIP_UNUSED_ARG(status);
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{   //lint --e{641}     // converting enum to int
    osStatus_t  status = osMessageQueuePut(*mbox, &msg, 0, 0);

    if (status != osOK)
    {
        LWIP_ASSERT( "Failed to trypost mbox"
                    , (status == osErrorTimeout) || (status == osErrorResource)
                   );
        SYS_STATS_INC(mbox.err);
        return( ERR_MEM );
    }
    return( ERR_OK );
}

err_t sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg)
{
    return( sys_mbox_trypost(mbox, msg) );
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    osStatus_t  status;
    uint32_t    time;

    if (timeout == 0) {
        timeout =  osWaitForever;
    } else
    if((timeout = osMSecToTick(timeout)) == 0) {
        timeout =  1;
    }
    time   = osKernelGetTickCount();
    status = osMessageQueueGet(*mbox, msg, NULL, timeout);
    time   = osTickToMSec(osKernelGetTickCount() - time);

    if (status != osOK)
    {
        LWIP_ASSERT( "Failed to fetch mbox"
                    , (status == osErrorTimeout) || (status == osErrorResource)
                   );
        return( SYS_ARCH_TIMEOUT );
    }
    return( (time == SYS_ARCH_TIMEOUT) ? (SYS_ARCH_TIMEOUT - 1) : time );
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    osStatus_t  status = osMessageQueueGet(*mbox, msg, NULL, 0);

    if (status != osOK)
    {
        LWIP_ASSERT( "Failed to tryfetch mbox"
                    , (status == osErrorTimeout) || (status == osErrorResource)
                   );
        return( SYS_MBOX_EMPTY );
    }
    return( 0 );
}

int sys_mbox_valid(sys_mbox_t *mbox)
{
    return( ((mbox) != NULL) && (*(mbox) != NULL) );
}

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    if ( (mbox) != NULL) {
        *(mbox)  = NULL;
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
{   //lint --e{641}     // converting enum to int
    osThreadAttr_t  attr;
    osThreadId_t    id;

    attr.name       = ((name == NULL) ? LWIP_SYS_THREAD_NAME : name);
    attr.attr_bits  =  osThreadDetached;
    attr.cb_mem     =  NULL;
    attr.cb_size    =  0;
    attr.stack_mem  =  NULL;
    attr.stack_size = ( ((stacksize >= LWIP_SYS_THREAD_STKB_MIN) || (stacksize == 0))
                       ? (stacksize) : (LWIP_SYS_THREAD_STKB)
                      );                    //lint !e732
//  attr.stack_size =  stacksize;           //lint !e732
    attr.priority   = (  (prio == 1                ) ? osPriorityNormal
                       : (prio < osPriorityLow     ) ? osPriorityLow
                       : (prio > osPriorityRealtime) ? osPriorityRealtime
                       :                              (osPriority_t)prio
                      );
//  attr.priority   = (osPriority_t)prio;   //lint !e641

    id = osThreadNew(thread, arg, &attr);
    LWIP_ASSERT("Failed to create thread", id != NULL);
    return( id );
}

/**
 * Terminates the calling thread, not lwIP port
 */
void sys_thread_exit(void)
{
    osThreadExit();
}

void sys_arch_msleep(u32_t ms)
{
    osStatus_t  status;

    if (ms == 0) {
        return;
    }
    if((ms = osMSecToTick(ms)) == 0) {
        ms =  1;
    }
    status = osDelay(ms);
    LWIP_ASSERT("Failed to sleep", status == osOK);
    LWIP_UNUSED_ARG(status);
}
#endif /* !NO_SYS */


u32_t sys_jiffies(void)
{
    return( osKernelGetTickCount() );
}

u32_t sys_now(void)
{
    return( osTickToMSec(osKernelGetTickCount()) );
}

void sys_init(void)
{
    osMSecTickInit();
    osThreadSemInit();
}


/*************************** TCPIP thread LOCK ********************************/
#if !NO_SYS && LWIP_TCPIP_CORE_LOCKING
extern osMutexId_t  lock_tcpip_core;

void sys_mark_tcpip_thread(void)
{
}

int sys_core_is_locked(void)
{
    return(  lock_tcpip_core
           ? (osThreadGetId() == osMutexGetOwner(lock_tcpip_core))
           : 1
          );
}

#elif !NO_SYS && !LWIP_TCPIP_CORE_LOCKING
static osThreadId_t tcpip_thread_id;

void sys_mark_tcpip_thread(void)
{
    tcpip_thread_id = osThreadGetId()
}

int sys_core_is_locked(void)
{
    return( (osThreadGetCount() != 0) && (osThreadGetId() == tcpip_thread_id) );
}
#endif


/*****************************  END OF FILE  **********************************/

