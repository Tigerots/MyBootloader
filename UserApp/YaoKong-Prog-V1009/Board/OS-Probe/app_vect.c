/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                         EXCEPTION VECTORS
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : app_vect.c
* Version       : V1.00
* Programmer(s) : Brian Nagel
*********************************************************************************************************
*/

#include <includes.h>


#pragma language=extended

/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/

typedef  void  (*intfunc)(void);


typedef  union {
    intfunc  __fun;
    void    *__ptr;
} INTVECT_ELEM;

/*
*********************************************************************************************************
*                                              PROTOTYPES
*********************************************************************************************************
*/

       void  __program_start     (void);

static void  App_Spurious_ISR    (void);
static void  App_NMI_ISR         (void);
static void  App_Fault_ISR       (void);


/*
*********************************************************************************************************
*                                  Exception / Interrupt Vector Table
*********************************************************************************************************
*/

#pragma segment = "CSTACK"

#pragma location = "INTVEC"
__root  const  INTVECT_ELEM  AppVectTbl[] = {
    { .__ptr = __sfe("CSTACK")},        /*  0, SP start value.                                         */
    __program_start,                    /*  1, PC start value.                                         */
    App_NMI_ISR,                        /*  2, NMI                                                     */
    App_Fault_ISR,                      /*  3, Hard Fault                                              */
    App_Spurious_ISR,                   /*  4, Memory Management                                       */
    App_Spurious_ISR,                   /*  5, Bus Fault                                               */
    App_Spurious_ISR,                   /*  6, Usage Fault                                             */
    0,                                  /*  7, Reserved                                                */
    0,                                  /*  8, Reserved                                                */
    0,                                  /*  9, Reserved                                                */
    0,                                  /* 10, Reserved                                                */
    App_Spurious_ISR,                   /* 11, SVCall                                                  */
    App_Spurious_ISR,                   /* 12, Debug Monitor                                           */
    App_Spurious_ISR,                   /* 13, Reserved                                                */
    OSPendSV,                           /* 14, PendSV Handler                                          */
    Tmr_TickISR_Handler,                /* 15, uC/OS-II Tick ISR Handler                               */
    App_Spurious_ISR,                   /* 16, INTISR[  0], Window Watchdog                            */
    App_Spurious_ISR,                   /* 17, INTISR[  1]  PVD through EXTI Line Detection            */
    App_Spurious_ISR,                   /* 18, INTISR[  2]  Tamper Interrupt                           */
    App_Spurious_ISR,                   /* 19, INTISR[  3]  RTC Global Interrupt                       */
    App_Spurious_ISR,                   /* 20, INTISR[  4]  FLASH Global Interrupt                     */
    App_Spurious_ISR,                   /* 21, INTISR[  5]  RCC Global Interrupt                       */
    App_Spurious_ISR,                   /* 22, INTISR[  6]  EXTI Line0 Interrupt                       */
    App_Spurious_ISR,                   /* 23, INTISR[  7]  EXTI Line1 Interrupt                       */
    App_Spurious_ISR,                   /* 24, INTISR[  8]  EXTI Line2 Interrupt                       */
    App_Spurious_ISR,                   /* 25, INTISR[  9]  EXTI Line3 Interrupt                       */
    App_Spurious_ISR,                   /* 26, INTISR[ 10]  EXTI Line4 Interrupt                       */
    App_Spurious_ISR,                   /* 27, INTISR[ 11]  DMA Channel1 Global Interrupt              */
    App_Spurious_ISR,                   /* 28, INTISR[ 12]  DMA Channel2 Global Interrupt              */
    App_Spurious_ISR,                   /* 29, INTISR[ 13]  DMA Channel3 Global Interrupt              */
    App_Spurious_ISR,                   /* 30, INTISR[ 14]  DMA Channel4 Global Interrupt              */
    App_Spurious_ISR,                   /* 31, INTISR[ 15]  DMA Channel5 Global Interrupt              */
    App_Spurious_ISR,                   /* 32, INTISR[ 16]  DMA Channel6 Global Interrupt              */
    App_Spurious_ISR,                   /* 33, INTISR[ 17]  DMA Channel7 Global Interrupt              */
    App_Spurious_ISR,                   /* 34, INTISR[ 18]  ADC Global Interrupt                       */
    App_Spurious_ISR,                   /* 35, INTISR[ 19]  USB High Priority / CAN TX  Interrupts     */
    App_Spurious_ISR,                   /* 36, INTISR[ 20]  USB Low  Priority / CAN RX0 Interrupts     */
    App_Spurious_ISR,                   /* 37, INTISR[ 21]  CAN RX1 Interrupt                          */
    App_Spurious_ISR,                   /* 38, INTISR[ 22]  CAN SCE Interrupt                          */
    App_Spurious_ISR,                   /* 39, INTISR[ 23]  EXTI Line[9:5] Interrupt                   */
    App_Spurious_ISR,                   /* 40, INTISR[ 24]  TIM1 Break  Interrupt                      */
    App_Spurious_ISR,                   /* 41, INTISR[ 25]  TIM1 Update Interrupt                      */
    App_Spurious_ISR,                   /* 42, INTISR[ 26]  TIM1 Trigger & Commutation Interrupts      */
    App_Spurious_ISR,                   /* 43, INTISR[ 27]  TIM1 Capture Compare interrupt             */
    App_Spurious_ISR,                   /* 44, INTISR[ 28]  TIM2 Global Interrupt                      */
    App_Spurious_ISR,                   /* 45, INTISR[ 29]  TIM3 Global Interrupt                      */
    App_Spurious_ISR,                   /* 46, INTISR[ 30]  TIM4 Global Interrupt                      */
    App_Spurious_ISR,                   /* 47, INTISR[ 31]  I2C1 Event  Interrupt                      */
    App_Spurious_ISR,                   /* 48, INTISR[ 32]  I2C1 Error  Interrupt                      */
    App_Spurious_ISR,                   /* 49, INTISR[ 33]  I2C2 Event  Interrupt                      */
    App_Spurious_ISR,                   /* 50, INTISR[ 34]  I2C2 Error  Interrupt                      */
    App_Spurious_ISR,                   /* 51, INTISR[ 35]  SPI1 Global Interrupt                      */
    App_Spurious_ISR,                   /* 52, INTISR[ 36]  SPI2 Global Interrupt                      */
    ProbeRS232_RxTxISRHandler,          /* 53, INTISR[ 37]  USART1 Global Interrupt                    */
    App_Spurious_ISR,    //ProbeRS232_RxTxISRHandler,          /* 54, INTISR[ 38]  USART2 Global Interrupt                    */
    App_Spurious_ISR,                   /* 55, INTISR[ 39]  USART3 Global Interrupt                    */
    App_Spurious_ISR,                   /* 56, INTISR[ 40]  EXTI Line [15:10] Interrupts               */
    App_Spurious_ISR,                   /* 57, INTISR[ 41]  RTC Alarm through EXT Line Interrupt       */
    App_Spurious_ISR,                   /* 58, INTISR[ 42]  USB Wakeup from Suspend through EXTI Int.  */
};

/*
*********************************************************************************************************
*                                      Spurious Interrupt Handler
*********************************************************************************************************
*/

static  void  App_Spurious_ISR (void) @ "ICODE"
{
    while (1) {
        ;
    }
}

/*
*********************************************************************************************************
*                                        NMI Exception Handler
*********************************************************************************************************
*/

static  void  App_NMI_ISR (void) @ "ICODE"
{
    while (1) {
        ;
    }
}

/*
*********************************************************************************************************
*                                       FAULT Exception Handler
*********************************************************************************************************
*/

static  void  App_Fault_ISR (void) @ "ICODE"
{
    while (1) {
        ;
    }
}
