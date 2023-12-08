/*******************************************************************************
 * [目录/文件]说明
 */

./ARM_Compiler/ ----------------- ARM 编译器扩展
./ARM_Compiler/Config/ ---------- Copy from MDK5 "./PACK/Keil/ARM_Compiler/1.6.1/Config/"   配置模板
./ARM_Compiler/Include/ --------- Copy from MDK5 "./PACK/Keil/ARM_Compiler/1.6.1/Include/"
./ARM_Compiler/Source/ ---------- Copy from MDK5 "./PACK/Keil/ARM_Compiler/1.6.1/Source/"
./ARM_Compiler/EventRecorder.scvd Copy from MDK5 "./PACK/Keil/ARM_Compiler/1.6.1/EventRecorder.scvd"   Keil 调试组件


./CMSIS/ ------------------------ CMSIS 组件
./CMSIS/Driver/ ----------------- Copy from MDK5 "./PACK/ARM/CMSIS/5.6.0/CMSIS/Driver/"    CMSIS Peripheral Driver Interface
./CMSIS/Include/ ---------------- Copy from MDK5 "./PACK/ARM/CMSIS/5.6.0/CMSIS/Include/"   CMSIS Core Support Include Files
./CMSIS/RTOS2/ ------------------ Copy from MDK5 "./PACK/ARM/CMSIS/5.6.0/CMSIS/RTOS2/"     CMSIS RTOS2 for RTX5


./CMSIS-Driver/ ----------------- CMSIS-Driver 驱动实现(官方)
                                  Copy from MDK5 "./PACK/ARM/CMSIS-Driver/2.4.1/"

./CMSIS-Driver-Ext/ ------------- CMSIS-Driver 驱动扩展(非官方)

./CMSIS-Driver-STM32F1/ --------- CMSIS-Driver 驱动实现(官方STM32F1xx)
                                  Copy from MDK5 "./PACK/Keil/STM32F1xx_DFP/2.3.0/RTE_Driver/"
                                  Modified: USART_STM32F10x.c : 1753 行 RTS I/O 口配置错误

./CMSIS-Driver-STM32F4/ --------- CMSIS-Driver 驱动实现(官方STM32F4xx)
                                  Copy from MDK5 "./PACK/Keil/STM32F4xx_DFP/2.14.0/CMSIS/Driver/"
                                  Modified: I2C_STM32F4xx.c : 1496 行, 结束 Master接收 BUG


./STM32F4xx_HAL_Device/ --------- STM32F4xx HAL 外设库相关 CMSIS Device 定义
                                  Copy from MDK5 "./PACK/Keil/STM32F4xx_DFP/2.14.0/Drivers/CMSIS/Device/ST/STM32F4xx/"

./STM32F4xx_HAL_Driver/ --------- STM32F4xx HAL 外设库
                                  Copy from MDK5 "./PACK/Keil/STM32F4xx_DFP/2.14.0/Drivers/STM32F4xx_HAL_Driver/"

./STM32F4xx_StdPeriph_Device/ --- STM32F4xx STD 外设库相关 CMSIS Device 定义
                                  Copy from "en.stm32f4_dsp_stdperiph_lib.zip/STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/CMSIS/Device/ST/STM32F4xx/"
                                  Modified: "./Source/Templates/system_stm32f4xx.c" : 452 行, 修复 C Runtime 加载前使用 SystemCoreClockUpdate() 出错 BUG

./STM32F4xx_StdPeriph_Driver/ --- STM32F4xx STD 外设库
                                  Copy from "en.stm32f4_dsp_stdperiph_lib.zip/STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/"
                                  Modified: "./src/stm32f4xx_rcc.c"" : 153 行, 修复 C Runtime 加载前使用 RCC_GetClocksFreq() 出错 BUG


./STM32xx_Board/ ---------------- STM32xx 板级通用代码
./board.h ----------------------- 板级通用定义
./stdio_Retarget.h -------------- 板级标准流(stdio)重定向定义

