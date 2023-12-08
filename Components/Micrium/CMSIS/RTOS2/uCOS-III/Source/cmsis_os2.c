/*******************************************************************************
 * @file    cmsis_os2.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2021/3/1
 * @brief   CMSIS RTOS2 wrapper for uCOS-III
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2021/1/19 -- Linghu -- the first version
 *******************************************************************************
 *
 * Limitations :
 *     1. osKernelSuspend() and osKernelResume() are not supported.
 *     2. osKernelGetTickCount() is limited to a 32-bit return value.
 *     3. osThreadDetach(), osThreadJoin() and attribute osThreadJoinable are not supported
 *        (osThreadNew() returns NULL when osThreadJoinable attribute is specified).
 *
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint -esym(788, osKernelState_t::osKernel*, os_err::OS_ERR_*)
//lint -esym(534, printf, sprintf, snprintf)
//lint -emacro(717, CPU_CRITICAL_ENTER, CPU_CRITICAL_EXIT)


#include <stdlib.h>
#include <stdio.h>
#include "RTE_Components.h"     // Component selection

#include "cmsis_os2.h"          // ::CMSIS:RTOS2
#include "cmsis_compiler.h"
#include "os_tick.h"
#include "os.h"                 // ARM.uCOS3::RTOS:Core


/*******************************************************************************
 * @addtogroup CMSIS_OS2_uCOS3
 * @{
 * @addtogroup              Private_Constants
 * @{
 ******************************************************************************/
//lint -emacro((7??), KERNEL_NAME, KERNEL_VER_MAJOR, KERNEL_VER_MINOR, KERNEL_VER_REV, KERNEL_VERSION)
//lint -emacro((8??), KERNEL_NAME, KERNEL_VER_MAJOR, KERNEL_VER_MINOR, KERNEL_VER_REV, KERNEL_VERSION)

#if (OS_CFG_TASK_STK_REDZONE_EN == 0u)
#error "Must enable stack redzone. ref: file os_task.c V3.08.00, function OSTaskDel(), line 628"
#endif
#ifdef  OS_FUNC_CHECKED_INTR_NESTING
#undef  OS_FUNC_CHECKED_INTR_NESTING
#endif


/* Kernel version and identification string definition (major.minor.rev: mmnnnrrrr dec) */
#define KERNEL_NAME             "uCOS-III" 

#define KERNEL_VER_MAJOR        ((((uint32_t)OS_VERSION) % 1000000) / 10000)
#define KERNEL_VER_MINOR        ((((uint32_t)OS_VERSION) %   10000) /   100)
#define KERNEL_VER_REV          ((((uint32_t)OS_VERSION) %     100) /     1)

#define KERNEL_VERSION          ((KERNEL_VER_MAJOR * 10000000) | (KERNEL_VER_MINOR * 10000) | KERNEL_VER_REV)

/* Additional task options. Note : ref OS_OPT_TASK_STK_CHK and OS_OPT_TASK_STK_CLR */
#if (OS_CFG_STAT_TASK_STK_CHK_EN > 0u)
#define OS_OPT_TASK_CMSIS_RTOS  (OS_OPT_TASK_SAVE_FP | OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR)
#else
#define OS_OPT_TASK_CMSIS_RTOS   OS_OPT_TASK_SAVE_FP
#endif
#define OS_OPT_TASK_ALLOC_TCB   ((OS_OPT)(0x4000u))     /* Alloc memory with task control block   */
#define OS_OPT_TASK_ALLOC_STK   ((OS_OPT)(0x8000u))     /* Alloc memory with task stack           */


/*******************************************************************************
 * @}
 * @addtogroup              Private_Macros
 * @{
 ******************************************************************************/

#ifndef __ARM_ARCH_6M__
#define __ARM_ARCH_6M__         0
#endif
#ifndef __ARM_ARCH_7M__
#define __ARM_ARCH_7M__         0
#endif
#ifndef __ARM_ARCH_7EM__
#define __ARM_ARCH_7EM__        0
#endif
#ifndef __ARM_ARCH_8M_MAIN__
#define __ARM_ARCH_8M_MAIN__    0
#endif
#ifndef __ARM_ARCH_7A__
#define __ARM_ARCH_7A__         0
#endif

#if (__ARM_ARCH_7A__ == 1U)
#define CPSR_MASKBIT_I          0x80U       /* CPSR mask bits     */
#define CPSR_MODE_USER          0x10U       /* CPSR mode bitmasks */
#define CPSR_MODE_SYSTEM        0x1FU       /* CPSR mode bitmasks */
#endif


/* Check if in IRQ Mode */
#if   (__ARM_ARCH_6M__ || __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ || __ARM_ARCH_8M_MAIN__)
#define IS_IRQ_MODE()            (__get_IPSR() != 0U)
#elif (__ARM_ARCH_7A__)
#define IS_IRQ_MODE()           ((__get_mode() != CPSR_MODE_USER) && (__get_mode() != CPSR_MODE_SYSTEM))
#else
#define IS_IRQ_MODE()            (__get_IPSR() != 0U)       // unknow architecture
#endif

/* Check if IRQ is Masked */
#if  ((__ARM_ARCH_7M__ == 1U) || (__ARM_ARCH_7EM__ == 1U) || (__ARM_ARCH_8M_MAIN__ == 1U))
#define IS_IRQ_MASKED()         ((__get_PRIMASK() != 0U) || (__get_BASEPRI() != 0U))
#elif (__ARM_ARCH_6M__ == 1U)
#define IS_IRQ_MASKED()          (__get_PRIMASK() != 0U)
#elif (__ARM_ARCH_7A__ == 1U)
#define IS_IRQ_MASKED()         ((__get_CPSR() & CPSR_MASKBIT_I) != 0U)
#else
#define IS_IRQ_MASKED()          (__get_PRIMASK() != 0U)    // unknow architecture
#endif


/* Calculate memory size for an aligned buffer - returns the next highest */
#define MEM_ALIGN_SIZE_STK(size)    (((size) + CPU_CFG_ADDR_SIZE * 2U - 1U) & ~(CPU_CFG_ADDR_SIZE * 2U - 1U))


/*******************************************************************************
 * @}
 * @addtogroup              Private_Types
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup              Private_Variables
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup              Private_Prototypes
 * @{
 ******************************************************************************/
//lint -esym(522, BSP_ClkInit, BSP_IntInit, BSP_OS_TickInit, Mem_Init, CPU_IntDis, CPU_Init, App_OS_SetAllHooks)

__WEAK void BSP_ClkInit        (void) {}    /* Initialize the main clock          */
__WEAK void BSP_IntInit        (void) {}    /* Initialize the interrupt           */
__WEAK void BSP_OS_TickInit    (void) {}    /* Initialize kernel tick timer       */
__WEAK void Mem_Init           (void) {}    /* Initialize Memory Managment Module */
__WEAK void CPU_IntDis         (void) {}    /* Disable all Interrupts             */
__WEAK void CPU_Init           (void) {}    /* Initialize the uC/CPU services     */
__WEAK void App_OS_SetAllHooks (void) {}    /* Set all applications hooks         */


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
/**
 * Get the current RTOS Kernel state.
 */
__STATIC_FORCEINLINE osKernelState_t OS_CoreStateGet (void)
{
    if (OSInitialized != OS_TRUE) {
        return( osKernelInactive );         /* uCOS3 uninitialized                                  */
    }
    if (OSRunning != OS_STATE_OS_RUNNING) {
        return( osKernelReady );            /* uCOS3 unstarted                                      */
    }
    if (OSSchedLockNestingCtr == 0U) {
        return( osKernelRunning );          /* uCOS3 scheduler is unlocked                          */
    } else {
        return( osKernelLocked );           /* uCOS3 scheduler is locked                            */
    }
}


/**
 * Get the number of times interrupt nesting.
 */
__STATIC_FORCEINLINE uint32_t OS_CoreIntrNestingGet (void)
{
    return( (uint32_t)OSIntNestingCtr );
}


/**
 * Get the task name.
 */
__STATIC_FORCEINLINE const char *OS_TaskNameGet (OS_TCB *p_tcb)
{
#if (OS_CFG_DBG_EN > 0U)
    const char     *name;
    CPU_SR_ALLOC();

    CPU_CRITICAL_ENTER();
    name = p_tcb->NamePtr;
    CPU_CRITICAL_EXIT();

    return( name );
#else
    ((void)p_tcb);
    return( NULL );
#endif
}   //lint !e818


/**
 * Get the task of the current running.
 */
__STATIC_FORCEINLINE OS_TCB *OS_TaskCurrentGet (void)
{
    OS_TCB  *p_tcb;
    CPU_SR_ALLOC();

    CPU_CRITICAL_ENTER();
    p_tcb = OSTCBCurPtr;
    CPU_CRITICAL_EXIT();

    return( p_tcb );
}


/**
 * Get current state of a task.
 */
__STATIC_FORCEINLINE osThreadState_t OS_TaskStateGet (OS_TCB *p_tcb)
{  
    osThreadState_t  state;
    CPU_SR_ALLOC();

    CPU_CRITICAL_ENTER();
    switch (p_tcb->TaskState) {
    case OS_TASK_STATE_RDY                    : state = (p_tcb != OSTCBCurPtr) ?
                                                         osThreadReady         :
                                                         osThreadRunning   ;  break;
    case OS_TASK_STATE_DLY                    : state =  osThreadBlocked   ;  break;
    case OS_TASK_STATE_PEND                   : state =  osThreadBlocked   ;  break;
    case OS_TASK_STATE_PEND_TIMEOUT           : state =  osThreadBlocked   ;  break;
    case OS_TASK_STATE_SUSPENDED              : state =  osThreadBlocked   ;  break;
    case OS_TASK_STATE_DLY_SUSPENDED          : state =  osThreadBlocked   ;  break;
    case OS_TASK_STATE_PEND_SUSPENDED         : state =  osThreadBlocked   ;  break;
    case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED : state =  osThreadBlocked   ;  break;
    case OS_TASK_STATE_DEL                    : state =  osThreadTerminated;  break;
    default                                   : state =  osThreadInactive  ;  break;
    }
    CPU_CRITICAL_EXIT();

    return( state );
}   //lint !e818


/**
 * Get stack size of a task.
 */
__STATIC_FORCEINLINE uint32_t OS_TaskStackSizeGet (OS_TCB *p_tcb)
{
    uint32_t    size;
    CPU_SR_ALLOC();

    CPU_CRITICAL_ENTER();
    size = (uint32_t)(p_tcb->StkSize);
    CPU_CRITICAL_EXIT();

    return( size );
}   //lint !e818


/**
 * Get current priority of a task.
 */
__STATIC_FORCEINLINE OS_PRIO OS_TaskPriorityGet (OS_TCB *p_tcb)
{
    OS_PRIO     prio;
    CPU_SR_ALLOC();

    CPU_CRITICAL_ENTER();
    prio = p_tcb->Prio;
    CPU_CRITICAL_EXIT();

    return( prio );
}   //lint !e818


/**
 * Get the tasks counter
 */
__STATIC_FORCEINLINE uint32_t OS_TaskCountGet(void)
{
    uint32_t    cnt;
    CPU_SR_ALLOC();

    CPU_CRITICAL_ENTER();
    cnt = (uint32_t)OSTaskQty;
    CPU_CRITICAL_EXIT();

    return( cnt );
}


__STATIC_FORCEINLINE uint32_t OS_TaskEnumerate(OS_TCB **tcb_array, uint32_t array_items)
{
#if (OS_CFG_DBG_EN > 0u)
    OS_TCB     *p_tcb;
    uint32_t    cnt;
    CPU_SR_ALLOC();

    {
        CPU_CRITICAL_ENTER();
        p_tcb = OSTaskDbgListPtr;
        CPU_CRITICAL_EXIT();
    }
    for(cnt = 0;  (cnt < array_items) && (p_tcb != (OS_TCB *)0);  cnt++)
    {
        tcb_array[cnt] = p_tcb;

        CPU_CRITICAL_ENTER();
        p_tcb = p_tcb->DbgNextPtr;
        CPU_CRITICAL_EXIT();
    }
    return( cnt );

#else
    ((void)p_tcb_array);  ((void)array_items);
    return( 0 );
#endif
}


/*----------------------------------------------------------------------------*/
/**
 * Initialize the RTOS Kernel.
 */
osStatus_t osKernelInitialize (void)
{
    OS_ERR           err;
    osKernelState_t  knlState = OS_CoreStateGet();


    if (IS_IRQ_MODE()) {
        return( osErrorISR );
    }
  #ifdef OS_FUNC_CHECKED_INTR_NESTING
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osErrorISR );
    }
  #endif
    if (knlState == osKernelReady) {
        return( osOK );
    }
    if (knlState != osKernelInactive) {
        return( osError );
    }

    BSP_ClkInit();                          /* Initialize the main clock                            */
    BSP_IntInit();                          /* Initialize the interrupt vector table.               */
    BSP_OS_TickInit();                      /* Initialize kernel tick timer                         */

    Mem_Init();                             /* Initialize Memory Managment Module                   */
    CPU_IntDis();                           /* Disable all Interrupts                               */
    CPU_Init();                             /* Initialize the uC/CPU services                       */

    OSInit(&err);                           /* Initialize uC/OS-III                                 */
    if (err != OS_ERR_NONE) {
        return( osError );
    }
    return( osOK );
}


/**
 * Get RTOS Kernel Information.
 */
osStatus_t osKernelGetInfo (osVersion_t *version, char *id_buf, uint32_t id_size)
{
    if (version != NULL) {
        version->api    = KERNEL_VERSION;   /* Version encoding is major.minor.rev: mmnnnrrrr dec   */
        version->kernel = KERNEL_VERSION;
    }
    if ((id_buf != NULL) && (id_size != 0U)) {
        snprintf(id_buf, id_size, "%s V%u.%u.%u",           KERNEL_NAME
                                                , (unsigned)KERNEL_VER_MAJOR
                                                , (unsigned)KERNEL_VER_MINOR
                                                , (unsigned)KERNEL_VER_REV
                );
    }
    return( osOK );
}


/**
 * Get the current RTOS Kernel state.
 */
osKernelState_t osKernelGetState (void)
{
    return( OS_CoreStateGet() );
}


/**
 * Start the RTOS Kernel scheduler.
 */
osStatus_t osKernelStart (void)
{
    OS_ERR           err;
    osKernelState_t  knlState = OS_CoreStateGet();


    if (IS_IRQ_MODE()) {
        return( osErrorISR );
    }
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osErrorISR );
    }
    if (knlState != osKernelReady) {
        return( osError );
    }

    OSSchedRoundRobinCfg(OS_TRUE, 0, &err); /* round-robin to be enabled                            */
    if (err != OS_ERR_NONE) {
        return( osError );
    }

    OSStart(&err);                          /* Start multitasking (i.e. give control to uC/OS-III)  */
    if (err != OS_ERR_NONE) {
        return( osError );
    }
    return( osOK );
}


/**
 * Lock the RTOS Kernel scheduler.
 */
int32_t osKernelLock (void)
{
    OS_ERR           err;
    osKernelState_t  knlState = OS_CoreStateGet();


    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( (int32_t)osErrorISR );
    }
  #ifdef OS_FUNC_CHECKED_INTR_NESTING
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osErrorISR );
    }
  #endif

    switch (knlState) {
    case osKernelRunning :
        OSSchedLock(&err);
        if (err == OS_ERR_NONE) {
            return( 0 );
        }
        break;

    case osKernelLocked :
        return( 1 );

    default :  break;
    }

    return( (int32_t)osError );
}


/**
 * Unlock the RTOS Kernel scheduler.
 */
int32_t osKernelUnlock (void)
{
    OS_ERR           err;
    osKernelState_t  knlState = OS_CoreStateGet();


    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( (int32_t)osErrorISR );
    }
  #ifdef OS_FUNC_CHECKED_INTR_NESTING
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osErrorISR );
    }
  #endif

    switch (knlState) {
    case osKernelRunning :
        return( 0 );

    case osKernelLocked :
        OSSchedUnlock(&err);
        if (err == OS_ERR_NONE) {
            return( 1 );
        }
        if (err == OS_ERR_SCHED_LOCKED) {
            return( 1 );
        }
        break;

    default :  break;
    }

    return( (int32_t)osError );
}


/**
 * Restore the RTOS Kernel scheduler lock state.
 */
int32_t osKernelRestoreLock (int32_t lock)
{
    OS_ERR           err;
    osKernelState_t  knlState = OS_CoreStateGet();


    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( (int32_t)osErrorISR );
    }
  #ifdef OS_FUNC_CHECKED_INTR_NESTING
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osErrorISR );
    }
  #endif

    switch (knlState) {
    case osKernelRunning :
        if (lock == 0) {
            return( 0 );                    /* (0) Restore state to unlocked                        */
        }
        if (lock != 1) {
            break;
        }
        OSSchedLock(&err);                  /* (1) Restore state to locked                          */
        if (err == OS_ERR_NONE) {
            return( 1 );
        }
        break;

    case osKernelLocked :
        if (lock == 1) {
            return( 1 );                    /* (1) Restore state to locked                          */
        }
        if (lock != 0) {
            break;
        }
        OSSchedUnlock(&err);                /* (0) Restore state to unlocked                        */
        if (err == OS_ERR_NONE) {
            return( 0 );
        }
        if (err == OS_ERR_SCHED_LOCKED) {
            return( 1 );
        }
        break;

    default :  break;
    }

    return( (int32_t)osError );
}


/**
 * Get the RTOS kernel tick count.
 */
uint32_t osKernelGetTickCount (void)
{
    OS_ERR      err;

    return( (uint32_t)OSTimeGet(&err) );
}


/**
 * Get the RTOS kernel tick frequency.
 */
uint32_t osKernelGetTickFreq (void)
{
    return( (uint32_t)OSCfg_TickRate_Hz );
}


/**
 * Get the RTOS kernel system timer count.
 */
uint32_t osKernelGetSysTimerCount (void)
{
    // todo:
    return( 0 );
}


/**
 * Get the RTOS kernel system timer frequency.
 */
uint32_t osKernelGetSysTimerFreq (void)
{
    // todo:
    return( 0 );
}


/*----------------------------------------------------------------------------*/
/**
 * Create a thread and add it to Active Threads.
 */
osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr)
{
    OS_ERR          err;
    OS_OPT          opt;
    const char     *name;
    void           *cb_mem;
    uint32_t        cb_size;
    void           *stack_mem;
    uint32_t        stack_size;
    osPriority_t    priority;


    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( NULL );
    }
  #ifdef OS_FUNC_CHECKED_INTR_NESTING
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osErrorISR );
    }
  #endif
 // if (func == NULL) {
 //     return( NULL );
 // }

    if (attr != NULL) {
        if (attr->attr_bits & osThreadJoinable) {
            return( NULL );                     /* Un-support joinable mode thread                  */
        }
        name       = attr->name;
        cb_mem     = attr->cb_mem;
        cb_size    = attr->cb_size;
        stack_mem  = attr->stack_mem;
        stack_size = attr->stack_size;
        priority   = attr->priority;
    } else {
        name       = NULL;
        cb_mem     = NULL;
        cb_size    = 0;
        stack_mem  = NULL;
        stack_size = 0;
        priority   = osPriorityNone;
    }
    opt = OS_OPT_TASK_CMSIS_RTOS;

    if (priority == osPriorityNone) {
        priority =  osPriorityNormal;
    } else {
        if((priority < osPriorityIdle) || (priority > osPriorityISR)) {
            return NULL;                        /* Error with priority                              */
        }
    }

    if (cb_mem != NULL) {
        if (cb_size < sizeof(OS_TCB)) {
            return( NULL );                     /* Error with of provided memory for control block  */
        }
    } else {
        if (cb_size != 0) {
            return( NULL );                     /* Error with of provided memory for control block  */
        }
        opt |= OS_OPT_TASK_ALLOC_TCB;
    }

    if (stack_mem != NULL) {
        if (stack_size == 0) {
            return( NULL );                     /* Error with of provided memory for stack size     */
        }
    } else {
        if (stack_size == 0) {
            stack_size =  MEM_ALIGN_SIZE_STK(OS_CFG_DEFAULT_STKB);
        } else {
            stack_size =  MEM_ALIGN_SIZE_STK(stack_size);
        }
        opt |= OS_OPT_TASK_ALLOC_STK;
    }

    if (cb_mem == NULL) {
        if (stack_mem == NULL) {
            cb_mem    =  malloc(stack_size + MEM_ALIGN_SIZE_STK(sizeof(OS_TCB)));
            stack_mem =  (uint8_t *)cb_mem + MEM_ALIGN_SIZE_STK(sizeof(OS_TCB)); //lint !e613
        } else {
            cb_mem    =  malloc(sizeof(OS_TCB));
        }
        if (cb_mem == NULL) {
            return( NULL );                     /* Error with of memory alloc                       */
        }
    } else {
        if (stack_mem == NULL) {
            stack_mem =  malloc(stack_size);
        }
        if (stack_mem == NULL) {
            return( NULL );                     /* Error with of memory alloc                       */
        }
    }

    OSTaskCreate( (OS_TCB     *) cb_mem,        /* Create the thread                                */
                  (CPU_CHAR   *) name,
                  (OS_TASK_PTR ) func,
                  (void       *) argument,
                  (OS_PRIO     ) OS_PRIO_TO_UCOS3(priority),
                  (CPU_STK    *) stack_mem,
                  (CPU_STK_SIZE) OS_CFG_TASK_STK_REDZONE_DEPTH,
                  (CPU_STK_SIZE) OS_STKB_TO_STKW(stack_size),
                  (OS_MSG_QTY  ) 0u,
                  (OS_TICK     ) 0u,
                  (void       *) NULL,
                  (OS_OPT      ) opt,
                  (OS_ERR     *)&err
                );

    if (err != OS_ERR_NONE) {
        if (opt & OS_OPT_TASK_ALLOC_TCB) {
            free(cb_mem);
        }
        else if (opt & OS_OPT_TASK_ALLOC_STK) {
            free(stack_mem);    //lint !e673       free for 'modified' data
        }
        return( NULL );         //lint !e593       possibly not freed 'cb_mem' and  'stack_mem'
    }
    return( cb_mem );           //lint !e429       Custodial pointer 'stack_mem' has not been freed
}


/**
 * Get name of a thread.
 */
const char *osThreadGetName (osThreadId_t thread_id)
{
    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( NULL );
    }
    if (OS_CoreIntrNestingGet() > 0U) {
        return( NULL );
    }
    if (thread_id == NULL) {
        return( NULL );
    }

    return( OS_TaskNameGet((OS_TCB *)thread_id) );
}


/**
 * Return the thread ID of the current running thread.
 */
osThreadId_t osThreadGetId (void)
{
    return( (osThreadId_t)OS_TaskCurrentGet() );
}


/**
 * Get current state of a thread.
 */
osThreadState_t osThreadGetState (osThreadId_t thread_id)
{
    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( osThreadError );
    }
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osThreadError );
    }
    if (thread_id == NULL) {
        return( osThreadError );
    }

    return( OS_TaskStateGet((OS_TCB *)thread_id) );
}


/**
 * Get stack size of a thread.
 */
uint32_t osThreadGetStackSize (osThreadId_t thread_id)
{
    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( 0 );
    }
    if (OS_CoreIntrNestingGet() > 0U) {
        return( 0 );
    }
    if (thread_id == NULL) {
        return( 0 );
    }

    return( OS_STKW_TO_STKB(OS_TaskStackSizeGet((OS_TCB *)thread_id)) );
}


/**
 * Get available stack space of a thread based on stack watermark recording during execution.
 */
uint32_t osThreadGetStackSpace (osThreadId_t thread_id)
{
#if (OS_CFG_STAT_TASK_STK_CHK_EN > 0u)
    OS_ERR          err;
    CPU_STK_SIZE    space;
    CPU_STK_SIZE    used;


    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( 0 );
    }
  #ifdef OS_FUNC_CHECKED_INTR_NESTING
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osErrorISR );
    }
  #endif
    if (thread_id == NULL) {
        return( 0 );
    }

    OSTaskStkChk((OS_TCB *)thread_id, &space, &used, &err);
    if (err != OS_ERR_NONE) {
        return( 0 );
    }
    return( OS_STKW_TO_STKB(space) );
#else
    ((void)thread_id);
    return( 0 );
#endif
}


/**
 * Change priority of a thread.
 */
osStatus_t osThreadSetPriority (osThreadId_t thread_id, osPriority_t priority)
{
    OS_ERR          err;


    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( osErrorISR );
    }
  #ifdef OS_FUNC_CHECKED_INTR_NESTING
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osErrorISR );
    }
  #endif
    if (thread_id == NULL) {
        return( osErrorParameter );
    }

    OSTaskChangePrio((OS_TCB *)thread_id, OS_PRIO_TO_UCOS3(priority), &err);
    switch (err) {
    default                          :  return( osErrorResource  );
    case OS_ERR_OS_NOT_RUNNING       :  return( osErrorResource  );
    case OS_ERR_PRIO_INVALID         :  return( osErrorParameter );
    case OS_ERR_STATE_INVALID        :  return( osErrorResource  );
    case OS_ERR_TASK_CHANGE_PRIO_ISR :  return( osErrorISR       );
    case OS_ERR_NONE                 :  return( osOK             );
    }
}


/**
 * Get current priority of a thread.
 */
osPriority_t osThreadGetPriority (osThreadId_t thread_id)
{
    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( osPriorityError );
    }
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osPriorityError );
    }
    if (thread_id == NULL) {
        return( osPriorityError );
    }

    return( OS_PRIO_TO_CMSIS(OS_TaskPriorityGet((OS_TCB *)thread_id)) );
}


/**
 * Pass control to next thread that is in state READY.
 */
osStatus_t osThreadYield (void)
{
    OS_ERR          err;


    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( osErrorISR );
    }
  #ifdef OS_FUNC_CHECKED_INTR_NESTING
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osErrorISR );
    }
  #endif

    OSSchedRoundRobinYield(&err);               /* voluntarily give up a task's time slot           */
    switch (err) {
    case OS_ERR_NONE                 :  return( osOK       );
    case OS_ERR_ROUND_ROBIN_1        :  return( osOK       );
    case OS_ERR_ROUND_ROBIN_DISABLED :  return( osOK       );
    case OS_ERR_SCHED_LOCKED         :  return( osOK       );
    case OS_ERR_YIELD_ISR            :  return( osErrorISR );
    default                          :  return( osError    );
    }
}


/**
 * Suspend execution of a thread.
 */
osStatus_t osThreadSuspend (osThreadId_t thread_id)
{
    OS_ERR          err;


    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( osErrorISR );
    }
  #ifdef OS_FUNC_CHECKED_INTR_NESTING
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osErrorISR );
    }
  #endif
    if (thread_id == NULL) {
        return( osErrorParameter );
    }

    OSTaskSuspend((OS_TCB *)thread_id, &err);   /* Suspends (or blocks) execution of a task         */
    switch (err) {
    case OS_ERR_NONE                     :  return( osOK            );
    case OS_ERR_OS_NOT_RUNNING           :  return( osErrorResource );
    case OS_ERR_SCHED_LOCKED             :  return( osErrorResource );
    case OS_ERR_STATE_INVALID            :  return( osErrorResource );
    case OS_ERR_TASK_SUSPEND_CTR_OVF     :  return( osErrorResource );
    case OS_ERR_TASK_SUSPEND_ISR         :  return( osErrorISR      );
    case OS_ERR_TASK_SUSPEND_IDLE        :  return( osErrorResource );
    case OS_ERR_TASK_SUSPEND_INT_HANDLER :  return( osErrorResource );
    default                              :  return( osErrorResource );
    }
}


/**
 * Resume execution of a thread.
 */
osStatus_t osThreadResume (osThreadId_t thread_id)
{
    OS_ERR          err;


    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( osErrorISR );
    }
  #ifdef OS_FUNC_CHECKED_INTR_NESTING
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osErrorISR );
    }
  #endif
    if (thread_id == NULL) {
        return( osErrorParameter );
    }

    OSTaskResume((OS_TCB *)thread_id, &err);    /* Resumes a task suspended                         */
    switch (err) {
    case OS_ERR_NONE               :  return( osOK            );
    case OS_ERR_OS_NOT_RUNNING     :  return( osErrorResource );
    case OS_ERR_STATE_INVALID      :  return( osErrorResource );
    case OS_ERR_TASK_NOT_SUSPENDED :  return( osErrorResource );
    case OS_ERR_TASK_RESUME_ISR    :  return( osErrorISR      );
    case OS_ERR_TASK_RESUME_SELF   :  return( osErrorResource );
    default                        :  return( osErrorResource );
    }
}


/**
 * Terminate execution of current running thread.
 */
__NO_RETURN void osThreadExit (void)
{
    OS_TaskReturn();
    for (;;) {}
}


/**
 * Terminate execution of a thread.
 */
osStatus_t osThreadTerminate (osThreadId_t thread_id)
{
    OS_ERR          err;


    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( osErrorISR );
    }
  #ifdef OS_FUNC_CHECKED_INTR_NESTING
    if (OS_CoreIntrNestingGet() > 0U) {
        return( osErrorISR );
    }
  #endif
    if (thread_id == NULL) {
        return( osErrorParameter );
    }

    OSTaskDel((OS_TCB *)thread_id, &err);       /* The task terminate and removes from RTOS core    */
    switch (err) {                              /* Alloc "TCB" and "STACK" free in OS_TLS_TaskSw()  */
    case OS_ERR_NONE             :  return( osOK            );
    case OS_ERR_OS_NOT_RUNNING   :  return( osErrorResource );
    case OS_ERR_STATE_INVALID    :  return( osErrorResource );
    case OS_ERR_TASK_DEL_IDLE    :  return( osErrorResource );
    case OS_ERR_TASK_DEL_INVALID :  return( osErrorResource );
    case OS_ERR_TASK_DEL_ISR     :  return( osErrorISR      );
    default                      :  return( osErrorResource );
    }
    // todo:
}


/**
 * Get number of active threads.
 */
uint32_t osThreadGetCount (void)
{
    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( 0 );
    }
    if (OS_CoreIntrNestingGet() > 0U) {
        return( 0 );
    }

    return( OS_TaskCountGet() );
}


/**
 * Enumerate active threads.
 */
uint32_t osThreadEnumerate (osThreadId_t *thread_array, uint32_t array_items)
{
    if (IS_IRQ_MODE() || IS_IRQ_MASKED()) {
        return( 0 );
    }
    if (OS_CoreIntrNestingGet() > 0U) {
        return( 0 );
    }
    if((thread_array == NULL) || (array_items == 0U)) {
        return( 0 );
    }

    return( OS_TaskEnumerate((OS_TCB **)thread_array, array_items) );
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

