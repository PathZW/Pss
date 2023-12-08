
/*
 * Auto generated Run-Time-Environment Configuration File
 *      *** Do not modify ! ***
 *
 * Project: 'STM32F407ZETx' 
 * Target:  'STM32F4xx' 
 */

#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H


/*
 * Define the Device Header File: 
 */
#define CMSIS_device_header "stm32f4xx.h"


/*  ARM::CMSIS:RTOS2:Keil RTX5:Source:5.5.1 */
#define RTE_CMSIS_RTOS2                 /* CMSIS-RTOS2 */
#define RTE_CMSIS_RTOS2_RTX5            /* CMSIS-RTOS2 Keil RTX5 */
#define RTE_CMSIS_RTOS2_RTX5_SOURCE     /* CMSIS-RTOS2 Keil RTX5 Source */

#ifdef  _USE_UCOS_III_
#define RTE_CMSIS_RTOS2_UCOS3
#undef  RTE_CMSIS_RTOS2_RTX5
#undef  RTE_CMSIS_RTOS2_RTX5_SOURCE
#undef  RTE_CMSIS_RTOS2_FreeRTOS
#endif


#ifdef  RTE_USED_EventRecorder
/*  Keil.ARM Compiler::Compiler:Event Recorder:DAP:1.4.0 */
#define RTE_Compiler_EventRecorder
#define RTE_Compiler_EventRecorder_DAP
#else
#define EVR_RTX_DISABLE
#endif


/*  Keil.ARM Compiler::Compiler:I/O:File:File System:1.2.0 */
#define RTE_Compiler_IO_File            /* Compiler I/O: File */
#define RTE_Compiler_IO_File_FS         /* Compiler I/O: File (File System) */

/*  Keil.ARM Compiler::Compiler:I/O:STDERR:User:1.2.0 */
#define RTE_Compiler_IO_STDERR          /* Compiler I/O: STDERR */
#define RTE_Compiler_IO_STDERR_User     /* Compiler I/O: STDERR User */

/*  Keil.ARM Compiler::Compiler:I/O:STDIN:User:1.2.0 */
#define RTE_Compiler_IO_STDIN           /* Compiler I/O: STDIN */
#define RTE_Compiler_IO_STDIN_User      /* Compiler I/O: STDIN User */

/*  Keil.ARM Compiler::Compiler:I/O:STDOUT:User:1.2.0 */
#define RTE_Compiler_IO_STDOUT          /* Compiler I/O: STDOUT */
#define RTE_Compiler_IO_STDOUT_User     /* Compiler I/O: STDOUT User */

/*  Keil.ARM Compiler::Compiler:I/O:TTY:User:1.2.0 */
#define RTE_Compiler_IO_TTY             /* Compiler I/O: TTY */
#define RTE_Compiler_IO_TTY_User        /* Compiler I/O: TTY User */


#ifdef  RTE_USED_FS
/*  Keil.MDK-Pro::File System:CORE:LFN:6.12.0 */
#define RTE_FileSystem_Core             /* File System Core */
#define RTE_FileSystem_LFN              /* File System with Long Filename support */
#define RTE_FileSystem_Release          /* File System Release Version */

/*  Keil.MDK-Pro::File System:Drive:NOR:6.12.0 */
#define RTE_FileSystem_Drive_NOR_0      /* File System NOR Flash Drive 0 */

#else
#undef  RTE_Compiler_IO_File_FS
#endif//RTE_USED_FS


/*  Keil::Device:STM32Cube Framework:STM32CubeMX:1.1.0 */
#define RTE_DEVICE_FRAMEWORK_CLASSIC


#endif /* RTE_COMPONENTS_H */
