/*******************************************************************************
 * [Ŀ¼/�ļ�]˵��
 */

./ARM_Compiler/ ----------------- ARM ��������չ
./ARM_Compiler/Config/ ---------- Copy from MDK5 "./PACK/Keil/ARM_Compiler/1.6.1/Config/"   ����ģ��
./ARM_Compiler/Include/ --------- Copy from MDK5 "./PACK/Keil/ARM_Compiler/1.6.1/Include/"
./ARM_Compiler/Source/ ---------- Copy from MDK5 "./PACK/Keil/ARM_Compiler/1.6.1/Source/"
./ARM_Compiler/EventRecorder.scvd Copy from MDK5 "./PACK/Keil/ARM_Compiler/1.6.1/EventRecorder.scvd"   Keil �������


./CMSIS/ ------------------------ CMSIS ���
./CMSIS/Driver/ ----------------- Copy from MDK5 "./PACK/ARM/CMSIS/5.6.0/CMSIS/Driver/"    CMSIS Peripheral Driver Interface
./CMSIS/Include/ ---------------- Copy from MDK5 "./PACK/ARM/CMSIS/5.6.0/CMSIS/Include/"   CMSIS Core Support Include Files
./CMSIS/RTOS2/ ------------------ Copy from MDK5 "./PACK/ARM/CMSIS/5.6.0/CMSIS/RTOS2/"     CMSIS RTOS2 for RTX5


./CMSIS-Driver/ ----------------- CMSIS-Driver ����ʵ��(�ٷ�)
                                  Copy from MDK5 "./PACK/ARM/CMSIS-Driver/2.4.1/"

./CMSIS-Driver-Ext/ ------------- CMSIS-Driver ������չ(�ǹٷ�)

./CMSIS-Driver-STM32F1/ --------- CMSIS-Driver ����ʵ��(�ٷ�STM32F1xx)
                                  Copy from MDK5 "./PACK/Keil/STM32F1xx_DFP/2.3.0/RTE_Driver/"
                                  Modified: USART_STM32F10x.c : 1753 �� RTS I/O �����ô���

./CMSIS-Driver-STM32F4/ --------- CMSIS-Driver ����ʵ��(�ٷ�STM32F4xx)
                                  Copy from MDK5 "./PACK/Keil/STM32F4xx_DFP/2.14.0/CMSIS/Driver/"
                                  Modified: I2C_STM32F4xx.c : 1496 ��, ���� Master���� BUG


./STM32F4xx_HAL_Device/ --------- STM32F4xx HAL �������� CMSIS Device ����
                                  Copy from MDK5 "./PACK/Keil/STM32F4xx_DFP/2.14.0/Drivers/CMSIS/Device/ST/STM32F4xx/"

./STM32F4xx_HAL_Driver/ --------- STM32F4xx HAL �����
                                  Copy from MDK5 "./PACK/Keil/STM32F4xx_DFP/2.14.0/Drivers/STM32F4xx_HAL_Driver/"

./STM32F4xx_StdPeriph_Device/ --- STM32F4xx STD �������� CMSIS Device ����
                                  Copy from "en.stm32f4_dsp_stdperiph_lib.zip/STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/CMSIS/Device/ST/STM32F4xx/"
                                  Modified: "./Source/Templates/system_stm32f4xx.c" : 452 ��, �޸� C Runtime ����ǰʹ�� SystemCoreClockUpdate() ���� BUG

./STM32F4xx_StdPeriph_Driver/ --- STM32F4xx STD �����
                                  Copy from "en.stm32f4_dsp_stdperiph_lib.zip/STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/"
                                  Modified: "./src/stm32f4xx_rcc.c"" : 153 ��, �޸� C Runtime ����ǰʹ�� RCC_GetClocksFreq() ���� BUG


./STM32xx_Board/ ---------------- STM32xx �弶ͨ�ô���
./board.h ----------------------- �弶ͨ�ö���
./stdio_Retarget.h -------------- �弶��׼��(stdio)�ض�����

