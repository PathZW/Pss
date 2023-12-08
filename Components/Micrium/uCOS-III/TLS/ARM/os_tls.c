/*
*********************************************************************************************************
*                                              uC/OS-III
*                                        The Real-Time Kernel
*
*                    Copyright 2009-2020 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                 THREAD LOCAL STORAGE (TLS) MANAGEMENT
*                                           ARMCC IMPLEMENTATION
*
* File    : os_tls.c
* Version : V3.08.00
*
* Change Logs: 2021/2/25 --- Linghu0060@qq.com, the first version
*********************************************************************************************************
*/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -emacro(717, CPU_CRITICAL_ENTER, CPU_CRITICAL_EXIT)

#define  MICRIUM_SOURCE
#include "../../Source/os.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "cmsis_compiler.h"     // Used: __USED

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_tls__c = "$Id: $";
#endif
/*
************************************************************************************************************************
*                                                     LOCAL DEFINES
************************************************************************************************************************
*/
#ifndef OS_MUTEX_CLIB_NUM
#define OS_MUTEX_CLIB_NUM       5u      // Number of Mutexes used by standard C/C++ library
#endif

#ifdef  __MICROLIB
#define LIBSPACE_TLS            0u
#else
#define LIBSPACE_TLS            24u     //   (LIBSPACE_SIZE / sizeof(OS_TLS))
#endif
#define LIBSPACE_SIZE           96u     // Bytes of static data used by standard C/C++ library

#ifndef OS_CFG_TLS_TBL_SIZE
#define OS_CFG_TLS_TBL_SIZE     0u
#endif

#if !defined(__CC_ARM) && !(defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
#error "Only used ARMCC compiler!"
#endif
#if (OS_CFG_TLS_TBL_SIZE < LIBSPACE_TLS)
#error "OS_CFG_TLS_TBL_SIZE too small!"
#endif


/*
************************************************************************************************************************
*                                                   LOCAL DATA TYPES
************************************************************************************************************************
*/


/*
************************************************************************************************************************
*                                                   LOCAL VARIABLES
************************************************************************************************************************
*/

static  CPU_DATA            OS_TLS_NextAvailID = (CPU_DATA)0;             /* Next available TLS ID                    */
#if (OS_CFG_TLS_TBL_SIZE > LIBSPACE_TLS)
static  OS_TLS_DESTRUCT_PTR OS_TLS_DestructPtrTbl[OS_CFG_TLS_TBL_SIZE - LIBSPACE_TLS] = {0};
#endif


/*
************************************************************************************************************************
*                                                    LOCAL FUNCTIONS
************************************************************************************************************************
*/


/*
************************************************************************************************************************
*                                          ALLOCATE THE NEXT AVAILABLE TLS ID
*
* Description: This function is called to obtain the ID of the next free TLS (Task Local Storage) register 'id'
*
* Arguments  : p_err       is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE               if the call was successful
*                            OS_ERR_TLS_NO_MORE_AVAIL  if you are attempting to assign more TLS than you declared
*                                                           available through OS_CFG_TLS_TBL_SIZE.
*
* Returns    : The next available TLS 'id' or OS_CFG_TLS_TBL_SIZE if an error is detected.
************************************************************************************************************************
*/

OS_TLS_ID  OS_TLS_GetID (OS_ERR  *p_err)
{
    OS_TLS_ID  id;
    CPU_SR_ALLOC();


#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_TLS_ID)OS_CFG_TLS_TBL_SIZE);
    }
#endif

    CPU_CRITICAL_ENTER();   //lint --e(587)
    if ((OS_TLS_NextAvailID + LIBSPACE_TLS) >= OS_CFG_TLS_TBL_SIZE) { /* See if we exceeded the number of IDs available   */
       *p_err = OS_ERR_TLS_NO_MORE_AVAIL;                             /* Yes, cannot allocate more TLS                    */
        CPU_CRITICAL_EXIT();
        return ((OS_TLS_ID)OS_CFG_TLS_TBL_SIZE);
    }

    id    = OS_TLS_NextAvailID;									      /* Assign the next available ID                     */
    OS_TLS_NextAvailID++;										      /* Increment available ID for next request          */
    CPU_CRITICAL_EXIT();
   *p_err = OS_ERR_NONE;
    return (id);
}


/*
************************************************************************************************************************
*                                        GET THE CURRENT VALUE OF A TLS REGISTER
*
* Description: This function is called to obtain the current value of a TLS register
*
* Arguments  : p_tcb     is a pointer to the OS_TCB of the task you want to read the TLS register from.  If 'p_tcb' is
*                        a NULL pointer then you will get the TLS register of the current task.
*
*              id        is the 'id' of the desired TLS register.  Note that the 'id' must be less than
*                        'OS_TLS_NextAvailID'
*
*              p_err     is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE            if the call was successful
*                            OS_ERR_OS_NOT_RUNNING  if the kernel has not started yet
*                            OS_ERR_TLS_ID_INVALID  if the 'id' is greater or equal to OS_TLS_NextAvailID
*                            OS_ERR_TLS_NOT_EN      if the task was created by specifying that TLS support was not
*                                                     needed for the task
*
* Returns    : The current value of the task's TLS register or 0 if an error is detected.
*
* Note(s)    : 1) p_tcb->Opt contains options passed to OSTaskCreate().  One of these options (OS_OPT_TASK_NO_TLS) is
*                 used to specify that the user doesn't want TLS support for the task being created.  In other words,
*                 by default, TLS support is enabled if OS_CFG_TLS_TBL_SIZE is defined and > 0 so the user must
*                 specifically indicate that he/she doesn't want TLS supported for a task.
************************************************************************************************************************
*/

OS_TLS  OS_TLS_GetValue (OS_TCB     *p_tcb,
                         OS_TLS_ID   id,
                         OS_ERR     *p_err)
{
    OS_TLS    value;
    CPU_SR_ALLOC();


#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_TLS)0);
    }
#endif


// #if OS_CFG_ARG_CHK_EN > 0u
    if (id >= OS_TLS_NextAvailID) {                               /* Caller must specify an ID that's been assigned   */
       *p_err = OS_ERR_TLS_ID_INVALID;
        return ((OS_TLS)0);
    }
// #endif

    CPU_CRITICAL_ENTER();
    if (p_tcb == (OS_TCB *)0) {                                   /* Does caller want to use current task's TCB?      */
        p_tcb = OSTCBCurPtr;                                      /* Yes                                              */
        if (OSTCBCurPtr == (OS_TCB *)0) {                         /* Is the kernel running?                           */
            CPU_CRITICAL_EXIT();                                  /* No, then caller cannot specify NULL              */
           *p_err = OS_ERR_OS_NOT_RUNNING;
            return ((OS_TLS)0);
        }
    }
    if ((p_tcb->Opt & OS_OPT_TASK_NO_TLS) == OS_OPT_NONE) {       /* See if TLS is available for this task            */
        value = p_tcb->TLS_Tbl[id];                               /* Yes                                              */
        CPU_CRITICAL_EXIT();
       *p_err = OS_ERR_NONE;
        return ((OS_TLS)value);
    } else {
        CPU_CRITICAL_EXIT();                                      /* No                                               */
       *p_err = OS_ERR_TLS_NOT_EN;
        return ((OS_TLS)0);
    }
}   //lint !e818


/*
************************************************************************************************************************
*                                          DEFINE TLS DESTRUCTOR FUNCTION
*
* Description: This function is called by the user to assign a 'destructor' function to a specific TLS.  When a task is
*              deleted, all the destructors are called for all the task's TLS for which there is a destructor function
*              defined.  In other when a task is deleted, all the non-NULL functions present in OS_TLS_DestructPtrTbl[]
*              will be called.
*
* Arguments  : id          is the ID of the TLS destructor to set
*
*              p_destruct  is a pointer to a function that is associated with a specific TLS register and is called when
*                          a task is deleted.  The prototype of such functions is:
*
*                            void  MyDestructFunction (OS_TCB     *p_tcb,
*                                                      OS_TLS_ID   id,
*                                                      OS_TLS      value);
*
*                          you can specify a NULL pointer if you don't want to have a fucntion associated with a TLS
*                          register.  A NULL pointer (i.e. no function associated with a TLS register) is the default
*                          value placed in OS_TLS_DestructPtrTbl[].
*
*              p_err       is a pointer to an error return code.  The possible values are:
*
*                            OS_ERR_NONE             The call was successful.
*                            OS_ERR_TLS_ID_INVALID   You you specified an invalid TLS ID
*
* Returns    : none
*
* Note       : none
************************************************************************************************************************
*/

void  OS_TLS_SetDestruct (OS_TLS_ID            id,
                          OS_TLS_DESTRUCT_PTR  p_destruct,
                          OS_ERR              *p_err)
{
#if (OS_CFG_TLS_TBL_SIZE > LIBSPACE_TLS)
    CPU_SR_ALLOC();
#endif
    (void)p_destruct;


#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

// #if OS_CFG_ARG_CHK_EN > 0u
    if (id >= OS_TLS_NextAvailID) {                             /* See if we exceeded the number of TLS IDs available */
       *p_err = OS_ERR_TLS_ID_INVALID;
        return;
    }
// #endif

#if (OS_CFG_TLS_TBL_SIZE > LIBSPACE_TLS)
    if (OS_TLS_DestructPtrTbl[id] != (OS_TLS_DESTRUCT_PTR)0) {  /* Can only assign a destructor once                  */
       *p_err = OS_ERR_TLS_DESTRUCT_ASSIGNED;
        return;
    }

    CPU_CRITICAL_ENTER();
    OS_TLS_DestructPtrTbl[id] = p_destruct;
    CPU_CRITICAL_EXIT();
   *p_err                     = OS_ERR_NONE;
#endif
}


/*
************************************************************************************************************************
*                                       SET THE CURRENT VALUE OF A TASK TLS REGISTER
*
* Description: This function is called to change the current value of a task TLS register.
*
* Arguments  : p_tcb     is a pointer to the OS_TCB of the task you want to set the task's TLS register for.  If 'p_tcb'
*                        is a NULL pointer then you will change the TLS register of the current task.
*
*              id        is the 'id' of the desired task TLS register.  Note that the 'id' must be less than
*                        'OS_TLS_NextAvailID'
*
*              value     is the desired value for the task TLS register.
*
*              p_err     is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE            if the call was successful
*                            OS_ERR_OS_NOT_RUNNING  if the kernel has not started yet
*                            OS_ERR_TLS_ID_INVALID  if you specified an invalid TLS ID
*                            OS_ERR_TLS_NOT_EN      if the task was created by specifying that TLS support was not
*                                                     needed for the task
*
* Returns    : none
*
* Note(s)    : 1) p_tcb->Opt contains options passed to OSTaskCreate().  One of these options (OS_OPT_TASK_NO_TLS) is
*                 used to specify that the user doesn't want TLS support for the task being created.  In other words,
*                 by default, TLS support is enabled if OS_CFG_TLS_TBL_SIZE is defined and > 0 so the user must
*                 specifically indicate that he/she doesn't want TLS supported for a task.
************************************************************************************************************************
*/

void  OS_TLS_SetValue (OS_TCB     *p_tcb,
                       OS_TLS_ID   id,
                       OS_TLS      value,
                       OS_ERR     *p_err)
{
    CPU_SR_ALLOC();


#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

// #if OS_CFG_ARG_CHK_EN > 0u
    if (id >= OS_TLS_NextAvailID) {                               /* Caller must specify an ID that's been assigned   */
       *p_err = OS_ERR_TLS_ID_INVALID;
        return;
    }
// #endif

    CPU_CRITICAL_ENTER();                                         /* Does caller want to use current task's TCB?      */
    if (p_tcb == (OS_TCB *)0) {                                   /* Yes                                              */
        p_tcb = OSTCBCurPtr;                                      /* Is the kernel running?                           */
        if (OSTCBCurPtr == (OS_TCB *)0) {                         /* No, then caller cannot specify NULL              */
            CPU_CRITICAL_EXIT();
           *p_err = OS_ERR_OS_NOT_RUNNING;
            return;
        }
    }
    if ((p_tcb->Opt & OS_OPT_TASK_NO_TLS) == OS_OPT_NONE) {       /* See if TLS is available for this task            */
        p_tcb->TLS_Tbl[id] = value;                               /* Yes                                              */
        CPU_CRITICAL_EXIT();
       *p_err              = OS_ERR_NONE;
    } else {
        CPU_CRITICAL_EXIT();                                      /* No                                               */
       *p_err              = OS_ERR_TLS_NOT_EN;
    }
}


/*
************************************************************************************************************************
************************************************************************************************************************
*                                             uC/OS-III INTERNAL FUNCTIONS
*                                         DO NOT CALL FROM THE APPLICATION CODE
************************************************************************************************************************
************************************************************************************************************************
*/

/*
************************************************************************************************************************
*                                       INITIALIZE THE TASK LOCAL STORAGE SERVICES
*
* Description: This function is called by uC/OS-III to initialize the TLS id allocator.
*
*              This function also initializes an array containing function pointers.  There is one function associated
*                  to each task TLS register and all the functions (assuming non-NULL) will be called when the task is
*                  deleted.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TLS_Init (OS_ERR *p_err)
{
    *p_err = OS_ERR_NONE;
}


/*
************************************************************************************************************************
*                                                  TASK CREATE HOOK
*
* Description: This function is called by OSTaskCreate()
*
* Arguments  : p_tcb     is a pointer to the OS_TCB of the task being created.
*
* Returns    : none
*
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application should not call it.
*
*              2) OSTaskCreate() clears all entries in p_tcb->TLS_Tbl[] before calling OS_TLS_TaskCreate() so no need
*                 to this here.
************************************************************************************************************************
*/

void  OS_TLS_TaskCreate (OS_TCB  *p_tcb)
{
    (void)p_tcb;
}


/*
************************************************************************************************************************
*                                                  TASK DELETE HOOK
*
* Description: This function is called by OSTaskDel()
*
* Arguments  : p_tcb     is a pointer to the OS_TCB of the task being deleted.
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TLS_TaskDel (OS_TCB  *p_tcb)
{
#if (OS_CFG_TLS_TBL_SIZE > LIBSPACE_TLS)
    OS_TLS_ID             id;
    OS_TLS_DESTRUCT_PTR   p_dst;


    for(id = 0;  id < OS_TLS_NextAvailID;  id++) {  /* Call all the destructors associated with the TLS IDs         */
        if((p_dst = OS_TLS_DestructPtrTbl[id]) != (OS_TLS_DESTRUCT_PTR)0) {
            p_dst(p_tcb, id, p_tcb->TLS_Tbl[id]);
        }
    }
#else
    (void)p_tcb;
#endif
}


/*
************************************************************************************************************************
*                                                  TASK SWITCH HOOK
*
* Description: This function is called by OSSched() and OSIntExit() just prior to calling the context switch code
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application should not call it.
*
*              2) It's assumed that OSTCBCurPtr points to the task being switched out and OSTCBHighRdyPtr points to the
*                 task being switched in.
************************************************************************************************************************
*/

void  OS_TLS_TaskSw (void)
{
    if ((OSTCBHighRdyPtr->Opt & OS_OPT_TASK_NO_TLS) == OS_OPT_NONE) {    /* See if TLS is available for this task     */
    }
}


/*
************************************************************************************************************************
************************************************************************************************************************
*                                   C/C++ Standard Library Multithreading Interface
************************************************************************************************************************
************************************************************************************************************************
*/
#if (( defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))) && !defined(__MICROLIB))

/* static data used by standard C/C++ library */
static uint32_t     clib_libspace[LIBSPACE_SIZE/sizeof(uint32_t)];


/* Provide libspace for current thread */
__USED
void *__user_perthread_libspace (void);
void *__user_perthread_libspace (void)
{
    OS_TCB     *p_tcb = OSTCBCurPtr;

    if((p_tcb == (OS_TCB *)0) || (OSIntNestingCtr > 0u)) {
        return( clib_libspace );
    }
    return( &(p_tcb->TLS_Tbl[OS_CFG_TLS_TBL_SIZE - LIBSPACE_TLS]) ); //lint !e778
}


/*----------------------------------------------------------------------------*/

/* Mutexes used by standard C/C++ library */
static OS_MUTEX     clib_mutex_cb[OS_MUTEX_CLIB_NUM] = {0};
static OS_MUTEX *   clib_mutex_id[OS_MUTEX_CLIB_NUM] = {0};

/* Define mutex object and function prototypes */
typedef void *mutex;


/* Create mutex */
__USED
int _mutex_initialize(mutex *m);
int _mutex_initialize(mutex *m)
{
    uint32_t    i;
    OS_MUTEX   *p_mutex = NULL;
    OS_ERR      err = OS_ERR_NONE;
    CPU_SR_ALLOC();


    for(m[0] = NULL, i = 0;  i < OS_MUTEX_CLIB_NUM;  i++)
    {
        CPU_CRITICAL_ENTER();
        if (clib_mutex_id[i] == NULL) {             // Mutex alloc from static memory
            clib_mutex_id[i] =  p_mutex = &clib_mutex_cb[i];
            CPU_CRITICAL_EXIT();
            break;
        }
        CPU_CRITICAL_EXIT();
    }
    if (i >= OS_MUTEX_CLIB_NUM) {
        p_mutex = calloc(1, sizeof(OS_MUTEX));      // Mutex alloc from dynamic memory
    }
    if (p_mutex == NULL) {
        for(;;) ;                                   // Mutex alloc fail
    }

    OSMutexCreate(p_mutex, "clib_Mutex", &err);     // Create the Mutex
    if (err == OS_ERR_NONE) {
        m[0] = p_mutex;                             // Return Mutex id
        return( 1 );
    }

    if (i < OS_MUTEX_CLIB_NUM) {
        CPU_CRITICAL_ENTER();
        clib_mutex_id[i] = NULL;                    // Mutex create fail from static memory
        CPU_CRITICAL_EXIT();
    } else {
        free(p_mutex);  //lint !e673                // Mutex create fail from dynamic memory
    }
    for(;;) ;                                       // Mutex create fail

//  return( 0 );
}


/* Delete mutex */
__USED
void _mutex_free(mutex *m);
void _mutex_free(mutex *m)
{
    uint32_t    i;
    OS_ERR      err;
    CPU_SR_ALLOC();


    (void)OSMutexDel((OS_MUTEX *) m[0],             // Deletes the Mutex
                     (OS_OPT    ) OS_OPT_DEL_ALWAYS,
                     (OS_ERR   *)&err);

    for(i = 0;  i < OS_MUTEX_CLIB_NUM;  i++)
    {
        CPU_CRITICAL_ENTER();
        if (clib_mutex_id[i] == m[0]) {
            clib_mutex_id[i] =  NULL;               // Mutex alloc from static memory
            CPU_CRITICAL_EXIT();
            break;
        }
        CPU_CRITICAL_EXIT();
    }
    if (i >= OS_MUTEX_CLIB_NUM) {
        free(m[0]);                                 // Mutex alloc from dynamic memory
    }
}   //lint !e818


/* Acquire mutex */
__USED
void _mutex_acquire(mutex *m);
void _mutex_acquire(mutex *m)
{
    OS_ERR      err;

    if (OSRunning == OS_STATE_OS_RUNNING)
    {
        OSMutexPend((OS_MUTEX *) m[0],
                    (OS_TICK   ) 0u,
                    (OS_OPT    ) OS_OPT_PEND_BLOCKING,
                    (CPU_TS   *) 0,
                    (OS_ERR   *)&err);
    }
}   //lint !e818


/* Release mutex */
__USED
void _mutex_release(mutex *m);
void _mutex_release(mutex *m)
{
    OS_ERR      err;

    if (OSRunning == OS_STATE_OS_RUNNING)
    {
        OSMutexPost((OS_MUTEX *) m[0],
                    (OS_OPT    ) OS_OPT_POST_NONE,
                    (OS_ERR   *)&err);
    }
}   //lint !e818

#endif
