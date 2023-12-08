/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*
* Filename : bsp_int_armv7m.c
*            Copy from "Micrium_STM32L4R9I-EVAL_Blinky.zip/ST/BSP/Interrupts/bsp_int_armv7m.c"
*
* Change Logs: 2020/8/7 --- Linghu0060@qq.com, the first version
*              2021/1/19 -- Linghu0060@qq.com, add cmsis-rtos2 support
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#ifdef   _RTE_                  // RTE_CMSIS_RTOS2
#include "RTE_Components.h"     // Component selection
#endif// _RTE_
#include  <cmsis_compiler.h>    // Used: __ALIGNED()  __VECTOR_TABLE

#include  <cpu.h> 
#if defined(_USE_UCOS_III_) || defined(RTE_CMSIS_RTOS2)
#include  <os.h>
#endif 

#include  "bsp_int.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*
* Note(s) : (1) The Cortex-M "Vector Table Offset Register" section states the following:
*
*               You must align the offset to the number of exception entries in the vector table. The
*               minimum alignment is 32 words, enough for up to 16 interrupts. For more interrupts,
*               adjust the alignment by rounding up to the next power of two. For example, if you require
*               21 interrupts, the alignment must be on a 64-word boundary because the required table
*               size is 37 words, and the next power of two is 64. SEE YOUR VENDOR DOCUMENTATION FOR THE
*               ALIGNMENT DETAILS FOR YOUR DEVICE.
*********************************************************************************************************
*/

#ifndef  INT_ID_MAX_NBR
#define  INT_ID_MAX_NBR              240u                       /* Max. number of ext. interrupt sources. (Check MCU RM)*/
#endif

                                                                /* 111 VTOR entries; next power of 2 is 128             */
#define  INT_VTOR_TBL_SIZE         (INT_ID_MAX_NBR + CPU_INT_EXT0)
#define  INT_VTOR_TBL_ALIGNMENT    (0x200uL)                    /* 128 * 4 = 512 words. See note 1                      */


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_IntHandler(int int_id);

extern  void  Reset_Handler    (void);                      /* Reset Handler         */
extern  void  NMI_Handler      (void);                      /* NMI Handler           */
extern  void  HardFault_Handler(void);                      /* Hard Fault Handler    */
static  void  BSP_ISR_I004(void)  { BSP_IntHandler(-12); }  /* MPU Fault Handler     */
static  void  BSP_ISR_I005(void)  { BSP_IntHandler(-11); }  /* Bus Fault Handler     */
static  void  BSP_ISR_I006(void)  { BSP_IntHandler(-10); }  /* Usage Fault Handler   */
#if 0
static  void  BSP_ISR_I007(void)  { BSP_IntHandler(-9) ; }  /* Reserved              */
static  void  BSP_ISR_I008(void)  { BSP_IntHandler(-8) ; }  /* Reserved              */
static  void  BSP_ISR_I009(void)  { BSP_IntHandler(-7) ; }  /* Reserved              */
static  void  BSP_ISR_I010(void)  { BSP_IntHandler(-6) ; }  /* Reserved              */
#endif
static  void  BSP_ISR_I011(void)  { BSP_IntHandler(-5) ; }  /* SVCall Handler        */
static  void  BSP_ISR_I012(void)  { BSP_IntHandler(-4) ; }  /* Debug Monitor Handler */
#if 0
static  void  BSP_ISR_I013(void)  { BSP_IntHandler(-3) ; }  /* Reserved              */
#endif
#if !defined(_USE_UCOS_III_) && !defined(RTE_CMSIS_RTOS2)
static  void  BSP_ISR_I014(void)  { BSP_IntHandler(-2) ; }  /* PendSV Handler        */
static  void  BSP_ISR_I015(void)  { BSP_IntHandler(-1) ; }  /* SysTick Handler       */
#endif

static  void  BSP_ISR_E000(void)  { BSP_IntHandler(0)  ; }
static  void  BSP_ISR_E001(void)  { BSP_IntHandler(1)  ; }
static  void  BSP_ISR_E002(void)  { BSP_IntHandler(2)  ; }
static  void  BSP_ISR_E003(void)  { BSP_IntHandler(3)  ; }
static  void  BSP_ISR_E004(void)  { BSP_IntHandler(4)  ; }
static  void  BSP_ISR_E005(void)  { BSP_IntHandler(5)  ; }
static  void  BSP_ISR_E006(void)  { BSP_IntHandler(6)  ; }
static  void  BSP_ISR_E007(void)  { BSP_IntHandler(7)  ; }
static  void  BSP_ISR_E008(void)  { BSP_IntHandler(8)  ; }
static  void  BSP_ISR_E009(void)  { BSP_IntHandler(9)  ; }
static  void  BSP_ISR_E010(void)  { BSP_IntHandler(10) ; }
static  void  BSP_ISR_E011(void)  { BSP_IntHandler(11) ; }
static  void  BSP_ISR_E012(void)  { BSP_IntHandler(12) ; }
static  void  BSP_ISR_E013(void)  { BSP_IntHandler(13) ; }
static  void  BSP_ISR_E014(void)  { BSP_IntHandler(14) ; }
static  void  BSP_ISR_E015(void)  { BSP_IntHandler(15) ; }
static  void  BSP_ISR_E016(void)  { BSP_IntHandler(16) ; }
static  void  BSP_ISR_E017(void)  { BSP_IntHandler(17) ; }
static  void  BSP_ISR_E018(void)  { BSP_IntHandler(18) ; }
static  void  BSP_ISR_E019(void)  { BSP_IntHandler(19) ; }
static  void  BSP_ISR_E020(void)  { BSP_IntHandler(20) ; }
static  void  BSP_ISR_E021(void)  { BSP_IntHandler(21) ; }
static  void  BSP_ISR_E022(void)  { BSP_IntHandler(22) ; }
static  void  BSP_ISR_E023(void)  { BSP_IntHandler(23) ; }
static  void  BSP_ISR_E024(void)  { BSP_IntHandler(24) ; }
static  void  BSP_ISR_E025(void)  { BSP_IntHandler(25) ; }
static  void  BSP_ISR_E026(void)  { BSP_IntHandler(26) ; }
static  void  BSP_ISR_E027(void)  { BSP_IntHandler(27) ; }
static  void  BSP_ISR_E028(void)  { BSP_IntHandler(28) ; }
static  void  BSP_ISR_E029(void)  { BSP_IntHandler(29) ; }
static  void  BSP_ISR_E030(void)  { BSP_IntHandler(30) ; }
static  void  BSP_ISR_E031(void)  { BSP_IntHandler(31) ; }
static  void  BSP_ISR_E032(void)  { BSP_IntHandler(32) ; }
static  void  BSP_ISR_E033(void)  { BSP_IntHandler(33) ; }
static  void  BSP_ISR_E034(void)  { BSP_IntHandler(34) ; }
static  void  BSP_ISR_E035(void)  { BSP_IntHandler(35) ; }
static  void  BSP_ISR_E036(void)  { BSP_IntHandler(36) ; }
static  void  BSP_ISR_E037(void)  { BSP_IntHandler(37) ; }
static  void  BSP_ISR_E038(void)  { BSP_IntHandler(38) ; }
static  void  BSP_ISR_E039(void)  { BSP_IntHandler(39) ; }
static  void  BSP_ISR_E040(void)  { BSP_IntHandler(40) ; }
static  void  BSP_ISR_E041(void)  { BSP_IntHandler(41) ; }
static  void  BSP_ISR_E042(void)  { BSP_IntHandler(42) ; }
static  void  BSP_ISR_E043(void)  { BSP_IntHandler(43) ; }
static  void  BSP_ISR_E044(void)  { BSP_IntHandler(44) ; }
static  void  BSP_ISR_E045(void)  { BSP_IntHandler(45) ; }
static  void  BSP_ISR_E046(void)  { BSP_IntHandler(46) ; }
static  void  BSP_ISR_E047(void)  { BSP_IntHandler(47) ; }
static  void  BSP_ISR_E048(void)  { BSP_IntHandler(48) ; }
static  void  BSP_ISR_E049(void)  { BSP_IntHandler(49) ; }
static  void  BSP_ISR_E050(void)  { BSP_IntHandler(50) ; }
static  void  BSP_ISR_E051(void)  { BSP_IntHandler(51) ; }
static  void  BSP_ISR_E052(void)  { BSP_IntHandler(52) ; }
static  void  BSP_ISR_E053(void)  { BSP_IntHandler(53) ; }
static  void  BSP_ISR_E054(void)  { BSP_IntHandler(54) ; }
static  void  BSP_ISR_E055(void)  { BSP_IntHandler(55) ; }
static  void  BSP_ISR_E056(void)  { BSP_IntHandler(56) ; }
static  void  BSP_ISR_E057(void)  { BSP_IntHandler(57) ; }
static  void  BSP_ISR_E058(void)  { BSP_IntHandler(58) ; }
static  void  BSP_ISR_E059(void)  { BSP_IntHandler(59) ; }
static  void  BSP_ISR_E060(void)  { BSP_IntHandler(60) ; }
static  void  BSP_ISR_E061(void)  { BSP_IntHandler(61) ; }
static  void  BSP_ISR_E062(void)  { BSP_IntHandler(62) ; }
static  void  BSP_ISR_E063(void)  { BSP_IntHandler(63) ; }
static  void  BSP_ISR_E064(void)  { BSP_IntHandler(64) ; }
static  void  BSP_ISR_E065(void)  { BSP_IntHandler(65) ; }
static  void  BSP_ISR_E066(void)  { BSP_IntHandler(66) ; }
static  void  BSP_ISR_E067(void)  { BSP_IntHandler(67) ; }
static  void  BSP_ISR_E068(void)  { BSP_IntHandler(68) ; }
static  void  BSP_ISR_E069(void)  { BSP_IntHandler(69) ; }
static  void  BSP_ISR_E070(void)  { BSP_IntHandler(70) ; }
static  void  BSP_ISR_E071(void)  { BSP_IntHandler(71) ; }
static  void  BSP_ISR_E072(void)  { BSP_IntHandler(72) ; }
static  void  BSP_ISR_E073(void)  { BSP_IntHandler(73) ; }
static  void  BSP_ISR_E074(void)  { BSP_IntHandler(74) ; }
static  void  BSP_ISR_E075(void)  { BSP_IntHandler(75) ; }
static  void  BSP_ISR_E076(void)  { BSP_IntHandler(76) ; }
static  void  BSP_ISR_E077(void)  { BSP_IntHandler(77) ; }
static  void  BSP_ISR_E078(void)  { BSP_IntHandler(78) ; }
static  void  BSP_ISR_E079(void)  { BSP_IntHandler(79) ; }
static  void  BSP_ISR_E080(void)  { BSP_IntHandler(80) ; }
static  void  BSP_ISR_E081(void)  { BSP_IntHandler(81) ; }
static  void  BSP_ISR_E082(void)  { BSP_IntHandler(82) ; }
static  void  BSP_ISR_E083(void)  { BSP_IntHandler(83) ; }
static  void  BSP_ISR_E084(void)  { BSP_IntHandler(84) ; }
static  void  BSP_ISR_E085(void)  { BSP_IntHandler(85) ; }
static  void  BSP_ISR_E086(void)  { BSP_IntHandler(86) ; }
static  void  BSP_ISR_E087(void)  { BSP_IntHandler(87) ; }
static  void  BSP_ISR_E088(void)  { BSP_IntHandler(88) ; }
static  void  BSP_ISR_E089(void)  { BSP_IntHandler(89) ; }
static  void  BSP_ISR_E090(void)  { BSP_IntHandler(90) ; }
static  void  BSP_ISR_E091(void)  { BSP_IntHandler(91) ; }
static  void  BSP_ISR_E092(void)  { BSP_IntHandler(92) ; }
static  void  BSP_ISR_E093(void)  { BSP_IntHandler(93) ; }
static  void  BSP_ISR_E094(void)  { BSP_IntHandler(94) ; }
static  void  BSP_ISR_E095(void)  { BSP_IntHandler(95) ; }
static  void  BSP_ISR_E096(void)  { BSP_IntHandler(96) ; }
static  void  BSP_ISR_E097(void)  { BSP_IntHandler(97) ; }
static  void  BSP_ISR_E098(void)  { BSP_IntHandler(98) ; }
static  void  BSP_ISR_E099(void)  { BSP_IntHandler(99) ; }
static  void  BSP_ISR_E100(void)  { BSP_IntHandler(100); }
static  void  BSP_ISR_E101(void)  { BSP_IntHandler(101); }
static  void  BSP_ISR_E102(void)  { BSP_IntHandler(102); }
static  void  BSP_ISR_E103(void)  { BSP_IntHandler(103); }
static  void  BSP_ISR_E104(void)  { BSP_IntHandler(104); }
static  void  BSP_ISR_E105(void)  { BSP_IntHandler(105); }
static  void  BSP_ISR_E106(void)  { BSP_IntHandler(106); }
static  void  BSP_ISR_E107(void)  { BSP_IntHandler(107); }
static  void  BSP_ISR_E108(void)  { BSP_IntHandler(108); }
static  void  BSP_ISR_E109(void)  { BSP_IntHandler(109); }
static  void  BSP_ISR_E110(void)  { BSP_IntHandler(110); }
static  void  BSP_ISR_E111(void)  { BSP_IntHandler(111); }
static  void  BSP_ISR_E112(void)  { BSP_IntHandler(112); }
static  void  BSP_ISR_E113(void)  { BSP_IntHandler(113); }
static  void  BSP_ISR_E114(void)  { BSP_IntHandler(114); }
static  void  BSP_ISR_E115(void)  { BSP_IntHandler(115); }
static  void  BSP_ISR_E116(void)  { BSP_IntHandler(116); }
static  void  BSP_ISR_E117(void)  { BSP_IntHandler(117); }
static  void  BSP_ISR_E118(void)  { BSP_IntHandler(118); }
static  void  BSP_ISR_E119(void)  { BSP_IntHandler(119); }
static  void  BSP_ISR_E120(void)  { BSP_IntHandler(120); }
static  void  BSP_ISR_E121(void)  { BSP_IntHandler(121); }
static  void  BSP_ISR_E122(void)  { BSP_IntHandler(122); }
static  void  BSP_ISR_E123(void)  { BSP_IntHandler(123); }
static  void  BSP_ISR_E124(void)  { BSP_IntHandler(124); }
static  void  BSP_ISR_E125(void)  { BSP_IntHandler(125); }
static  void  BSP_ISR_E126(void)  { BSP_IntHandler(126); }
static  void  BSP_ISR_E127(void)  { BSP_IntHandler(127); }
static  void  BSP_ISR_E128(void)  { BSP_IntHandler(128); }
static  void  BSP_ISR_E129(void)  { BSP_IntHandler(129); }
static  void  BSP_ISR_E130(void)  { BSP_IntHandler(130); }
static  void  BSP_ISR_E131(void)  { BSP_IntHandler(131); }
static  void  BSP_ISR_E132(void)  { BSP_IntHandler(132); }
static  void  BSP_ISR_E133(void)  { BSP_IntHandler(133); }
static  void  BSP_ISR_E134(void)  { BSP_IntHandler(134); }
static  void  BSP_ISR_E135(void)  { BSP_IntHandler(135); }
static  void  BSP_ISR_E136(void)  { BSP_IntHandler(136); }
static  void  BSP_ISR_E137(void)  { BSP_IntHandler(137); }
static  void  BSP_ISR_E138(void)  { BSP_IntHandler(138); }
static  void  BSP_ISR_E139(void)  { BSP_IntHandler(139); }
static  void  BSP_ISR_E140(void)  { BSP_IntHandler(140); }
static  void  BSP_ISR_E141(void)  { BSP_IntHandler(141); }
static  void  BSP_ISR_E142(void)  { BSP_IntHandler(142); }
static  void  BSP_ISR_E143(void)  { BSP_IntHandler(143); }
static  void  BSP_ISR_E144(void)  { BSP_IntHandler(144); }
static  void  BSP_ISR_E145(void)  { BSP_IntHandler(145); }
static  void  BSP_ISR_E146(void)  { BSP_IntHandler(146); }
static  void  BSP_ISR_E147(void)  { BSP_IntHandler(147); }
static  void  BSP_ISR_E148(void)  { BSP_IntHandler(148); }
static  void  BSP_ISR_E149(void)  { BSP_IntHandler(149); }
static  void  BSP_ISR_E150(void)  { BSP_IntHandler(150); }
static  void  BSP_ISR_E151(void)  { BSP_IntHandler(151); }
static  void  BSP_ISR_E152(void)  { BSP_IntHandler(152); }
static  void  BSP_ISR_E153(void)  { BSP_IntHandler(153); }
static  void  BSP_ISR_E154(void)  { BSP_IntHandler(154); }
static  void  BSP_ISR_E155(void)  { BSP_IntHandler(155); }
static  void  BSP_ISR_E156(void)  { BSP_IntHandler(156); }
static  void  BSP_ISR_E157(void)  { BSP_IntHandler(157); }
static  void  BSP_ISR_E158(void)  { BSP_IntHandler(158); }
static  void  BSP_ISR_E159(void)  { BSP_IntHandler(159); }
static  void  BSP_ISR_E160(void)  { BSP_IntHandler(160); }
static  void  BSP_ISR_E161(void)  { BSP_IntHandler(161); }
static  void  BSP_ISR_E162(void)  { BSP_IntHandler(162); }
static  void  BSP_ISR_E163(void)  { BSP_IntHandler(163); }
static  void  BSP_ISR_E164(void)  { BSP_IntHandler(164); }
static  void  BSP_ISR_E165(void)  { BSP_IntHandler(165); }
static  void  BSP_ISR_E166(void)  { BSP_IntHandler(166); }
static  void  BSP_ISR_E167(void)  { BSP_IntHandler(167); }
static  void  BSP_ISR_E168(void)  { BSP_IntHandler(168); }
static  void  BSP_ISR_E169(void)  { BSP_IntHandler(169); }
static  void  BSP_ISR_E170(void)  { BSP_IntHandler(170); }
static  void  BSP_ISR_E171(void)  { BSP_IntHandler(171); }
static  void  BSP_ISR_E172(void)  { BSP_IntHandler(172); }
static  void  BSP_ISR_E173(void)  { BSP_IntHandler(173); }
static  void  BSP_ISR_E174(void)  { BSP_IntHandler(174); }
static  void  BSP_ISR_E175(void)  { BSP_IntHandler(175); }
static  void  BSP_ISR_E176(void)  { BSP_IntHandler(176); }
static  void  BSP_ISR_E177(void)  { BSP_IntHandler(177); }
static  void  BSP_ISR_E178(void)  { BSP_IntHandler(178); }
static  void  BSP_ISR_E179(void)  { BSP_IntHandler(179); }
static  void  BSP_ISR_E180(void)  { BSP_IntHandler(180); }
static  void  BSP_ISR_E181(void)  { BSP_IntHandler(181); }
static  void  BSP_ISR_E182(void)  { BSP_IntHandler(182); }
static  void  BSP_ISR_E183(void)  { BSP_IntHandler(183); }
static  void  BSP_ISR_E184(void)  { BSP_IntHandler(184); }
static  void  BSP_ISR_E185(void)  { BSP_IntHandler(185); }
static  void  BSP_ISR_E186(void)  { BSP_IntHandler(186); }
static  void  BSP_ISR_E187(void)  { BSP_IntHandler(187); }
static  void  BSP_ISR_E188(void)  { BSP_IntHandler(188); }
static  void  BSP_ISR_E189(void)  { BSP_IntHandler(189); }
static  void  BSP_ISR_E190(void)  { BSP_IntHandler(190); }
static  void  BSP_ISR_E191(void)  { BSP_IntHandler(191); }
static  void  BSP_ISR_E192(void)  { BSP_IntHandler(192); }
static  void  BSP_ISR_E193(void)  { BSP_IntHandler(193); }
static  void  BSP_ISR_E194(void)  { BSP_IntHandler(194); }
static  void  BSP_ISR_E195(void)  { BSP_IntHandler(195); }
static  void  BSP_ISR_E196(void)  { BSP_IntHandler(196); }
static  void  BSP_ISR_E197(void)  { BSP_IntHandler(197); }
static  void  BSP_ISR_E198(void)  { BSP_IntHandler(198); }
static  void  BSP_ISR_E199(void)  { BSP_IntHandler(199); }
static  void  BSP_ISR_E200(void)  { BSP_IntHandler(200); }
static  void  BSP_ISR_E201(void)  { BSP_IntHandler(201); }
static  void  BSP_ISR_E202(void)  { BSP_IntHandler(202); }
static  void  BSP_ISR_E203(void)  { BSP_IntHandler(203); }
static  void  BSP_ISR_E204(void)  { BSP_IntHandler(204); }
static  void  BSP_ISR_E205(void)  { BSP_IntHandler(205); }
static  void  BSP_ISR_E206(void)  { BSP_IntHandler(206); }
static  void  BSP_ISR_E207(void)  { BSP_IntHandler(207); }
static  void  BSP_ISR_E208(void)  { BSP_IntHandler(208); }
static  void  BSP_ISR_E209(void)  { BSP_IntHandler(209); }
static  void  BSP_ISR_E210(void)  { BSP_IntHandler(210); }
static  void  BSP_ISR_E211(void)  { BSP_IntHandler(211); }
static  void  BSP_ISR_E212(void)  { BSP_IntHandler(212); }
static  void  BSP_ISR_E213(void)  { BSP_IntHandler(213); }
static  void  BSP_ISR_E214(void)  { BSP_IntHandler(214); }
static  void  BSP_ISR_E215(void)  { BSP_IntHandler(215); }
static  void  BSP_ISR_E216(void)  { BSP_IntHandler(216); }
static  void  BSP_ISR_E217(void)  { BSP_IntHandler(217); }
static  void  BSP_ISR_E218(void)  { BSP_IntHandler(218); }
static  void  BSP_ISR_E219(void)  { BSP_IntHandler(219); }
static  void  BSP_ISR_E220(void)  { BSP_IntHandler(220); }
static  void  BSP_ISR_E221(void)  { BSP_IntHandler(221); }
static  void  BSP_ISR_E222(void)  { BSP_IntHandler(222); }
static  void  BSP_ISR_E223(void)  { BSP_IntHandler(223); }
static  void  BSP_ISR_E224(void)  { BSP_IntHandler(224); }
static  void  BSP_ISR_E225(void)  { BSP_IntHandler(225); }
static  void  BSP_ISR_E226(void)  { BSP_IntHandler(226); }
static  void  BSP_ISR_E227(void)  { BSP_IntHandler(227); }
static  void  BSP_ISR_E228(void)  { BSP_IntHandler(228); }
static  void  BSP_ISR_E229(void)  { BSP_IntHandler(229); }
static  void  BSP_ISR_E230(void)  { BSP_IntHandler(230); }
static  void  BSP_ISR_E231(void)  { BSP_IntHandler(231); }
static  void  BSP_ISR_E232(void)  { BSP_IntHandler(232); }
static  void  BSP_ISR_E233(void)  { BSP_IntHandler(233); }
static  void  BSP_ISR_E234(void)  { BSP_IntHandler(234); }
static  void  BSP_ISR_E235(void)  { BSP_IntHandler(235); }
static  void  BSP_ISR_E236(void)  { BSP_IntHandler(236); }
static  void  BSP_ISR_E237(void)  { BSP_IntHandler(237); }
static  void  BSP_ISR_E238(void)  { BSP_IntHandler(238); }
static  void  BSP_ISR_E239(void)  { BSP_IntHandler(239); }


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

#if 0
extern  const int           __INITIAL_SP;                       // Top of Stack
#endif
extern const CPU_INT32U     __Vectors_Size;
extern const CPU_FNCT_VOID  __VECTOR_TABLE[];

static       CPU_FNCT_VOID    VectorTbl_REG[INT_VTOR_TBL_SIZE]; // Used to register ISR function

static const CPU_FNCT_VOID    VectorTbl_ROM[] __ALIGNED(INT_VTOR_TBL_ALIGNMENT) = {
  #if 0                                                         // Redefined Vector Table
    (CPU_FNCT_VOID)&__INITIAL_SP,
  #else
    0                     ,             /* Top of Stack          */
  #endif
    Reset_Handler         ,             /* Reset Handler         */
    NMI_Handler           ,             /* NMI Handler           */
    HardFault_Handler     ,             /* Hard Fault Handler    */
    BSP_ISR_I004          ,             /* MPU Fault Handler     */
    BSP_ISR_I005          ,             /* Bus Fault Handler     */
    BSP_ISR_I006          ,             /* Usage Fault Handler   */
    0                     ,             /* Reserved              */
    0                     ,             /* Reserved              */
    0                     ,             /* Reserved              */
    0                     ,             /* Reserved              */
    BSP_ISR_I011          ,             /* SVCall Handler        */
    BSP_ISR_I012          ,             /* Debug Monitor Handler */
    0                     ,             /* Reserved              */
  #if !defined(_USE_UCOS_III_) && !defined(RTE_CMSIS_RTOS2)
    BSP_ISR_I014          ,             /* PendSV Handler        */
    BSP_ISR_I015          ,             /* SysTick Handler       */
  #else
    OS_CPU_PendSVHandler  ,
    OS_CPU_SysTickHandler ,
  #endif

    BSP_ISR_E000, BSP_ISR_E001, BSP_ISR_E002, BSP_ISR_E003, BSP_ISR_E004, BSP_ISR_E005, BSP_ISR_E006, BSP_ISR_E007,
    BSP_ISR_E008, BSP_ISR_E009, BSP_ISR_E010, BSP_ISR_E011, BSP_ISR_E012, BSP_ISR_E013, BSP_ISR_E014, BSP_ISR_E015,
    BSP_ISR_E016, BSP_ISR_E017, BSP_ISR_E018, BSP_ISR_E019, BSP_ISR_E020, BSP_ISR_E021, BSP_ISR_E022, BSP_ISR_E023,
    BSP_ISR_E024, BSP_ISR_E025, BSP_ISR_E026, BSP_ISR_E027, BSP_ISR_E028, BSP_ISR_E029, BSP_ISR_E030, BSP_ISR_E031,
    BSP_ISR_E032, BSP_ISR_E033, BSP_ISR_E034, BSP_ISR_E035, BSP_ISR_E036, BSP_ISR_E037, BSP_ISR_E038, BSP_ISR_E039,
    BSP_ISR_E040, BSP_ISR_E041, BSP_ISR_E042, BSP_ISR_E043, BSP_ISR_E044, BSP_ISR_E045, BSP_ISR_E046, BSP_ISR_E047,
    BSP_ISR_E048, BSP_ISR_E049, BSP_ISR_E050, BSP_ISR_E051, BSP_ISR_E052, BSP_ISR_E053, BSP_ISR_E054, BSP_ISR_E055,
    BSP_ISR_E056, BSP_ISR_E057, BSP_ISR_E058, BSP_ISR_E059, BSP_ISR_E060, BSP_ISR_E061, BSP_ISR_E062, BSP_ISR_E063,
    BSP_ISR_E064, BSP_ISR_E065, BSP_ISR_E066, BSP_ISR_E067, BSP_ISR_E068, BSP_ISR_E069, BSP_ISR_E070, BSP_ISR_E071,
    BSP_ISR_E072, BSP_ISR_E073, BSP_ISR_E074, BSP_ISR_E075, BSP_ISR_E076, BSP_ISR_E077, BSP_ISR_E078, BSP_ISR_E079,
    BSP_ISR_E080, BSP_ISR_E081, BSP_ISR_E082, BSP_ISR_E083, BSP_ISR_E084, BSP_ISR_E085, BSP_ISR_E086, BSP_ISR_E087,
    BSP_ISR_E088, BSP_ISR_E089, BSP_ISR_E090, BSP_ISR_E091, BSP_ISR_E092, BSP_ISR_E093, BSP_ISR_E094, BSP_ISR_E095,
    BSP_ISR_E096, BSP_ISR_E097, BSP_ISR_E098, BSP_ISR_E099, BSP_ISR_E100, BSP_ISR_E101, BSP_ISR_E102, BSP_ISR_E103,
    BSP_ISR_E104, BSP_ISR_E105, BSP_ISR_E106, BSP_ISR_E107, BSP_ISR_E108, BSP_ISR_E109, BSP_ISR_E110, BSP_ISR_E111,
    BSP_ISR_E112, BSP_ISR_E113, BSP_ISR_E114, BSP_ISR_E115, BSP_ISR_E116, BSP_ISR_E117, BSP_ISR_E118, BSP_ISR_E119,
    BSP_ISR_E120, BSP_ISR_E121, BSP_ISR_E122, BSP_ISR_E123, BSP_ISR_E124, BSP_ISR_E125, BSP_ISR_E126, BSP_ISR_E127,
    BSP_ISR_E128, BSP_ISR_E129, BSP_ISR_E130, BSP_ISR_E131, BSP_ISR_E132, BSP_ISR_E133, BSP_ISR_E134, BSP_ISR_E135,
    BSP_ISR_E136, BSP_ISR_E137, BSP_ISR_E138, BSP_ISR_E139, BSP_ISR_E140, BSP_ISR_E141, BSP_ISR_E142, BSP_ISR_E143,
    BSP_ISR_E144, BSP_ISR_E145, BSP_ISR_E146, BSP_ISR_E147, BSP_ISR_E148, BSP_ISR_E149, BSP_ISR_E150, BSP_ISR_E151,
    BSP_ISR_E152, BSP_ISR_E153, BSP_ISR_E154, BSP_ISR_E155, BSP_ISR_E156, BSP_ISR_E157, BSP_ISR_E158, BSP_ISR_E159,
    BSP_ISR_E160, BSP_ISR_E161, BSP_ISR_E162, BSP_ISR_E163, BSP_ISR_E164, BSP_ISR_E165, BSP_ISR_E166, BSP_ISR_E167,
    BSP_ISR_E168, BSP_ISR_E169, BSP_ISR_E170, BSP_ISR_E171, BSP_ISR_E172, BSP_ISR_E173, BSP_ISR_E174, BSP_ISR_E175,
    BSP_ISR_E176, BSP_ISR_E177, BSP_ISR_E178, BSP_ISR_E179, BSP_ISR_E180, BSP_ISR_E181, BSP_ISR_E182, BSP_ISR_E183,
    BSP_ISR_E184, BSP_ISR_E185, BSP_ISR_E186, BSP_ISR_E187, BSP_ISR_E188, BSP_ISR_E189, BSP_ISR_E190, BSP_ISR_E191,
    BSP_ISR_E192, BSP_ISR_E193, BSP_ISR_E194, BSP_ISR_E195, BSP_ISR_E196, BSP_ISR_E197, BSP_ISR_E198, BSP_ISR_E199,
    BSP_ISR_E200, BSP_ISR_E201, BSP_ISR_E202, BSP_ISR_E203, BSP_ISR_E204, BSP_ISR_E205, BSP_ISR_E206, BSP_ISR_E207,
    BSP_ISR_E208, BSP_ISR_E209, BSP_ISR_E210, BSP_ISR_E211, BSP_ISR_E212, BSP_ISR_E213, BSP_ISR_E214, BSP_ISR_E215,
    BSP_ISR_E216, BSP_ISR_E217, BSP_ISR_E218, BSP_ISR_E219, BSP_ISR_E220, BSP_ISR_E221, BSP_ISR_E222, BSP_ISR_E223,
    BSP_ISR_E224, BSP_ISR_E225, BSP_ISR_E226, BSP_ISR_E227, BSP_ISR_E228, BSP_ISR_E229, BSP_ISR_E230, BSP_ISR_E231,
    BSP_ISR_E232, BSP_ISR_E233, BSP_ISR_E234, BSP_ISR_E235, BSP_ISR_E236, BSP_ISR_E237, BSP_ISR_E238, BSP_ISR_E239
};


/*
*********************************************************************************************************
*********************************************************************************************************
*                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              BSP_IntInit()
*
* Description : Initialize interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : (1) The Cortex-M "Vector Table Offset Register" section states the following:
*
*                   You must align the offset to the number of exception entries in the vector table.
*                   The minimum alignment is 32 words, enough for up to 16 interrupts. For more
*                   interrupts, adjust the alignment by rounding up to the next power of two. For example,
*                   if you require 21 interrupts, the alignment must be on a 64-word boundary because the
*                   required table size is 37 words, and the next power of two is 64. SEE YOUR VENDOR
*                   DOCUMENTATION FOR THE ALIGNMENT DETAILS FOR YOUR DEVICE.
*********************************************************************************************************
*/

void  BSP_IntInit (void)
{
    int  int_pos;
    CPU_SR_ALLOC();

    for (int_pos = 0u;  int_pos < INT_VTOR_TBL_SIZE;  int_pos++)
    {
        CPU_CRITICAL_ENTER();
        VectorTbl_REG[int_pos] = __VECTOR_TABLE[int_pos];   /* Copy current table into Register table               */
        CPU_CRITICAL_EXIT();

        if((int_pos == CPU_INT_PENDSV) || (int_pos == CPU_INT_SYSTICK) || (int_pos >= CPU_INT_EXT0))
        {                                                   /* Set external intr. to KA interrupt priority boundary */
            if (CPU_IntSrcPrioGet(int_pos) < CPU_CFG_KA_IPL_BOUNDARY) {
                CPU_IntSrcPrioSet(int_pos ,  CPU_CFG_KA_IPL_BOUNDARY, CPU_INT_KA);
            }
        }
    }

    CPU_CRITICAL_ENTER();
    CPU_REG_SCB_VTOR = (CPU_INT32U)&VectorTbl_ROM[0u];      /* See note 1.                                          */
    CPU_CRITICAL_EXIT();
}


/*
*********************************************************************************************************
*                                             BSP_IntClear()
*
* Description : Clear interrupt.
*
* Argument(s) : int_id    Interrupt to clear. (-15) ~ (239)
*
* Return(s)   : none.
*
* Note(s)     : (1) An interrupt does not need to be cleared within the interrupt controller.
*********************************************************************************************************
*/

void  BSP_IntClear (int int_id)
{
    int  int_pos;
    CPU_SR_ALLOC();

    if((int_pos = (int_id + CPU_INT_EXT0)) < INT_VTOR_TBL_SIZE)
    {
        CPU_CRITICAL_ENTER();
        VectorTbl_REG[int_pos] = __VECTOR_TABLE[int_pos];
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntDisable()
*
* Description : Disable interrupt.
*
* Argument(s) : int_id    Interrupt to disable. (-15) ~ (239)
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDisable (int int_id)
{
    int  int_pos;

    if((int_pos = (int_id + CPU_INT_EXT0)) < INT_VTOR_TBL_SIZE)
    {
        CPU_IntSrcDis(int_pos);
    }
}


/*
*********************************************************************************************************
*                                             BSP_IntEnable()
*
* Description : Enable interrupt.
*
* Argument(s) : int_id    Interrupt to enable. (-15) ~ (239)
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntEnable (int int_id)
{
    int  int_pos;

    if((int_pos = (int_id + CPU_INT_EXT0)) < INT_VTOR_TBL_SIZE)
    {
        CPU_IntSrcEn(int_pos);
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id         Interrupt for which vector will be set. (-15) ~ (239)
*
*               int_prio       Priority of interrupt to be set.
*
*               int_type       Type of interrupt(Depends on CPU Architecture).
*                                  CPU_INT_NKA  For interrupts that do not make any OS service(MUTEX, FLAGS,
*                                               etc.) calls in its handler.
*                                  CPU_INT_KA   For interrupts that make OS service(MUTEX, FLAGS,
*                                               etc.) calls in its handler.
*
*               isr_handler    Handler to assign
*
* Return(s)   : none.
*
* Note(s)     : ISR handler does not need to called OSIntEnter() and OSIntExit()
*********************************************************************************************************
*/

void  BSP_IntVectSet (int int_id, unsigned int_prio, unsigned int_type, void (*isr_handler)(void))
{
    CPU_INT16U  int_pos;
    CPU_SR_ALLOC();

    if((int_pos = (int_id + CPU_INT_EXT0)) < INT_VTOR_TBL_SIZE)
    {
        CPU_IntSrcPrioSet(int_pos, int_prio, int_type);

        CPU_CRITICAL_ENTER();
        VectorTbl_REG[int_pos] = isr_handler;
        CPU_CRITICAL_EXIT();
    }
}


/*----------------------------------------------------------------------------*/
/**
 * 中断服务函数注册
 *
 * @param[in]  isrID    中断号, (0) ~ (255)
 * @param[in]  PrePri   抢占优先级
 * @param[in]  SubPri   子优先级
 * @param[in]  isrFunc  中断处理函数
 *
 * @note  1. 默认中断服务函数参考 startup_xxx.s 文件 \n
 *        2. 直接定义 ISR 如 void SVC_Handler(void) 也可处理中断 \n
 *        3. 中断服务函数不需要调用 OSIntEnter() 和 OSIntExit() 函数 \n
 */
void  BSP_IntFuncReg (unsigned char isrID, unsigned char PrePri, unsigned char SubPri, void (*isrFunc)(void))
{
    CPU_INT08U  type;
    CPU_INT08U  prio;
    CPU_INT32U  bit_pre;
    CPU_INT32U  bit_sub;
    CPU_INT32U  pri_gpr = ((CPU_REG_SCB_AIRCR & (CPU_INT32U)0x700) >> 0x08);

    if((bit_pre = (7u - pri_gpr)) > CPU_CFG_NVIC_PRIO_BITS) {
        bit_pre = CPU_CFG_NVIC_PRIO_BITS;   // 
    }
    if((bit_sub = (pri_gpr + CPU_CFG_NVIC_PRIO_BITS - 7u)) > CPU_CFG_NVIC_PRIO_BITS) {
        bit_sub = 0;                        // ((pri_gpr + CPU_CFG_NVIC_PRIO_BITS) < 7u)
    }
    prio = ((PrePri & ((1u << bit_pre) - 1u)) << bit_sub) | (SubPri & ((1u << bit_sub) - 1u));
    type = (prio < CPU_CFG_KA_IPL_BOUNDARY) ? CPU_INT_NKA : CPU_INT_KA;

    BSP_IntVectSet(isrID - CPU_INT_EXT0, prio, type, isrFunc);
    CPU_IntSrcEn(isrID);
}


/**
 * Central interrupt handler.
 *
 * @param[in]  int_id   Interrupt that will be handled. (-15) ~ (239)
 */
static void BSP_IntHandler(int int_id)
{
    CPU_INT16U      int_pos;
    CPU_FNCT_VOID   isr;
    CPU_SR_ALLOC();

  #if defined(_USE_UCOS_III_) || defined(RTE_CMSIS_RTOS2)
    CPU_CRITICAL_ENTER();       /* Tell the OS that we are starting an ISR            */
    OSIntEnter();
    CPU_CRITICAL_EXIT();
  #endif

    if((int_pos = (int_id + CPU_INT_EXT0)) < INT_VTOR_TBL_SIZE)
    {
        CPU_CRITICAL_ENTER();
        isr = VectorTbl_REG[int_pos];
        CPU_CRITICAL_EXIT();

        if (isr != (CPU_FNCT_VOID)0) {
            isr();
        }
    }

  #if defined(_USE_UCOS_III_) || defined(RTE_CMSIS_RTOS2)
    OSIntExit();                /* Tell the OS that we are leaving the ISR            */
  #endif
}

