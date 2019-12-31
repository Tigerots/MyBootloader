;******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
;* File Name          : stm32f10x_vector.s
;* Author             : MCD Application Team
;* Date First Issued  : 05/21/2007
;* Description        : This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == Reset_Handler,
;*                      - Set the vector table entries with the exceptions ISR address,
;*                      - Branches to __main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the CortexM3 processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;* <<< Use Configuration Wizard in Context Menu >>>   
;*******************************************************************************
; History:
; 05/21/2007: V0.3                     
;*******************************************************************************
; THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
; WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
; AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
; INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
; CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
; INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
;*******************************************************************************

; Amount of memory (in bytes) allocated for Stack and Heap
; Tailor those values to your application needs
;// <h> Stack Configuration
;//   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;// </h>

Stack_Size       EQU     0x00000200

                 AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem        SPACE   Stack_Size
__initial_sp


;// <h> Heap Configuration
;//   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
;// </h>

Heap_Size        EQU     0x00000200

                 AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem         SPACE   Heap_Size
__heap_limit

  
                 THUMB
                 PRESERVE8

;******************************************************************************
;                                  IMPORTS
;******************************************************************************

		         IMPORT	 Tmr_TickISR_Handler
      	         IMPORT	 OSPendSV
                 IMPORT  NMIException
                 IMPORT  HardFaultException
                 IMPORT  MemManageException
                 IMPORT  BusFaultException
                 IMPORT  UsageFaultException
                 IMPORT  SVCHandler
                 IMPORT  DebugMonitor
                 IMPORT  SysTickHandler
                 IMPORT  WWDG_IRQHandler
                 IMPORT  PVD_IRQHandler
                 IMPORT  TAMPER_IRQHandler
                 IMPORT  FLASH_IRQHandler
                 IMPORT  RCC_IRQHandler
                 IMPORT  EXTI0_IRQHandler
                 IMPORT  EXTI1_IRQHandler
                 IMPORT  EXTI2_IRQHandler
                 IMPORT  EXTI3_IRQHandler
                 IMPORT  EXTI4_IRQHandler
                 IMPORT  DMAChannel1_IRQHandler
                 IMPORT  DMAChannel2_IRQHandler
                 IMPORT  DMAChannel3_IRQHandler
                 IMPORT  DMAChannel4_IRQHandler
                 IMPORT  DMAChannel5_IRQHandler
                 IMPORT  DMAChannel6_IRQHandler
                 IMPORT  DMAChannel7_IRQHandler
                 IMPORT  ADC_IRQHandler
                 IMPORT  USB_HP_CAN_TX_IRQHandler
                 IMPORT  USB_LP_CAN_RX0_IRQHandler
                 IMPORT  CAN_RX1_IRQHandler
                 IMPORT  CAN_SCE_IRQHandler
                 IMPORT  EXTI9_5_IRQHandler
                 IMPORT  TIM1_BRK_IRQHandler
                 IMPORT  TIM1_UP_IRQHandler
                 IMPORT  TIM1_TRG_COM_IRQHandler
                 IMPORT  TIM1_CC_IRQHandler
                 IMPORT  TIM2_IRQHandler
                 IMPORT  TIM3_IRQHandler
                 IMPORT  TIM4_IRQHandler
                 IMPORT  I2C1_EV_IRQHandler
                 IMPORT  I2C1_ER_IRQHandler
                 IMPORT  I2C2_EV_IRQHandler
                 IMPORT  I2C2_ER_IRQHandler
                 IMPORT  SPI1_IRQHandler
                 IMPORT  SPI2_IRQHandler
				 IMPORT  USART1_IRQHandler
                 IMPORT  USART2_IRQHandler
                 IMPORT  USART3_IRQHandler
                 IMPORT  EXTI15_10_IRQHandler
                 IMPORT  RTCAlarm_IRQHandler
                 IMPORT  USBWakeUp_IRQHandler

;******************************************************************************
;                                  EXPORTS
;******************************************************************************


;******************************************************************************
;                                DEFINITIONS
;******************************************************************************


;*******************************************************************************
; Fill-up the Vector Table entries with the exceptions ISR address
;*******************************************************************************
                 AREA    RESET, DATA, READONLY
                 EXPORT  __Vectors
                      
__Vectors        DCD  __initial_sp              ; Top of Stack
                 DCD  Reset_Handler
                 DCD  NMIException                          ;  2, NMI                                                     
        DCD     HardFaultException                          ;  3, Hard Fault                                              
        DCD     MemManageException                          ;  4, Memory Management                                      
        DCD     BusFaultException                           ;  5, Bus Fault                                               
        DCD     UsageFaultException                         ;  6, Usage Fault                                             
        DCD     0                                           ;  7, Reserved                                                
        DCD     0                                           ;  8, Reserved                                                
        DCD     0                                           ;  9, Reserved                                                
        DCD     0                                           ; 10, Reserved                                                
        DCD     SVCHandler                                  ; 11, SVCall                                                  
        DCD     DebugMonitor                                ; 12, Debug Monitor                                           
        DCD     0                                           ; 13, Reserved                                                
        DCD     OSPendSV                                    ; 14, PendSV Handler                                          
        DCD     Tmr_TickISR_Handler                         ; 15, uC/OS-II Tick ISR Handler                               
        DCD     WWDG_IRQHandler                             ; 16, INTISR[  0], Window Watchdog                            
        DCD     PVD_IRQHandler                              ; 17, INTISR[  1]  PVD through EXTI Line Detection            
        DCD     TAMPER_IRQHandler                           ; 18, INTISR[  2]  Tamper Interrupt                           
        DCD     0			                                ; 19, INTISR[  3]  RTC Global Interrupt                       
        DCD     FLASH_IRQHandler                            ; 20, INTISR[  4]  FLASH Global Interrupt                     
        DCD     RCC_IRQHandler                              ; 21, INTISR[  5]  RCC Global Interrupt                       
        DCD     EXTI0_IRQHandler                            ; 22, INTISR[  6]  EXTI Line0 Interrupt                       
        DCD     EXTI1_IRQHandler                            ; 23, INTISR[  7]  EXTI Line1 Interrupt                       
        DCD     EXTI2_IRQHandler                            ; 24, INTISR[  8]  EXTI Line2 Interrupt                       
        DCD     EXTI3_IRQHandler                            ; 25, INTISR[  9]  EXTI Line3 Interrupt                       
        DCD     EXTI4_IRQHandler                            ; 26, INTISR[ 10]  EXTI Line4 Interrupt                       
        DCD     DMAChannel1_IRQHandler                      ; 27, INTISR[ 11]  DMA Channel1 Global Interrupt              
        DCD     DMAChannel2_IRQHandler                      ; 28, INTISR[ 12]  DMA Channel2 Global Interrupt              
        DCD     DMAChannel3_IRQHandler                      ; 29, INTISR[ 13]  DMA Channel3 Global Interrupt              
        DCD     DMAChannel4_IRQHandler                      ; 30, INTISR[ 14]  DMA Channel4 Global Interrupt              
        DCD     DMAChannel5_IRQHandler                      ; 31, INTISR[ 15]  DMA Channel5 Global Interrupt              
        DCD     DMAChannel6_IRQHandler                      ; 32, INTISR[ 16]  DMA Channel6 Global Interrupt              
        DCD     DMAChannel7_IRQHandler                      ; 33, INTISR[ 17]  DMA Channel7 Global Interrupt              
        DCD     ADC_IRQHandler                              ; 34, INTISR[ 18]  ADC Global Interrupt                       
        DCD     USB_HP_CAN_TX_IRQHandler                    ; 35, INTISR[ 19]  USB High Priority / CAN TX  Interrupts     
        DCD     USB_LP_CAN_RX0_IRQHandler                   ; 36, INTISR[ 20]  USB Low  Priority / CAN RX0 Interrupts     
        DCD     CAN_RX1_IRQHandler                          ; 37, INTISR[ 21]  CAN RX1 Interrupt                          
        DCD     CAN_SCE_IRQHandler                          ; 38, INTISR[ 22]  CAN SCE Interrupt                          
        DCD     EXTI9_5_IRQHandler                          ; 39, INTISR[ 23]  EXTI Line[9:5] Interrupt                   
        DCD     TIM1_BRK_IRQHandler                         ; 40, INTISR[ 24]  TIM1 Break  Interrupt                      
        DCD     TIM1_UP_IRQHandler                          ; 41, INTISR[ 25]  TIM1 Update Interrupt                      
        DCD     TIM1_TRG_COM_IRQHandler                     ; 42, INTISR[ 26]  TIM1 Trigger & Commutation Interrupts
        DCD     0                                           ; 43, INTISR[ 27]  TIM1 CC Interrupts
        DCD     TIM2_IRQHandler                             ; 44, INTISR[ 28]  TIM2 Global Interrupt                      
        DCD     TIM3_IRQHandler                             ; 45, INTISR[ 29]  TIM3 Global Interrupt                      
        DCD     TIM4_IRQHandler                             ; 46, INTISR[ 30]  TIM4 Global Interrupt                      
        DCD     I2C1_EV_IRQHandler                          ; 47, INTISR[ 31]  I2C1 Event  Interrupt                      
        DCD     I2C1_ER_IRQHandler                          ; 48, INTISR[ 32]  I2C1 Error  Interrupt                      
        DCD     I2C2_EV_IRQHandler                          ; 49, INTISR[ 33]  I2C2 Event  Interrupt                      
        DCD     I2C2_ER_IRQHandler                          ; 50, INTISR[ 34]  I2C2 Error  Interrupt                      
        DCD     SPI1_IRQHandler                             ; 51, INTISR[ 35]  SPI1 Global Interrupt                      
        DCD     SPI2_IRQHandler                             ; 52, INTISR[ 36]  SPI2 Global Interrupt                      
        DCD     USART1_IRQHandler					                        ; 53, INTISR[ 37]  USART1 Global Interrupt                    
        DCD     USART2_IRQHandler                           ; 54, INTISR[ 38]  USART2 Global Interrupt                    
        DCD     USART3_IRQHandler                           ; 55, INTISR[ 39]  USART3 Global Interrupt                    
        DCD     EXTI15_10_IRQHandler                        ; 56, INTISR[ 40]  EXTI Line [15:10] Interrupts               
        DCD     RTCAlarm_IRQHandler                         ; 57, INTISR[ 41]  RTC Alarm through EXT Line Interrupt       
        DCD     USBWakeUp_IRQHandler                        ; 58, INTISR[ 42]  USB Wakeup from Suspend through EXTI Int.  
        
        


                 AREA    |.text|, CODE, READONLY
;******************************************************************************
;                          DEFAULT HANDLERS
;******************************************************************************

App_NMI_ISR         B       App_NMI_ISR

App_Fault_ISR       B       App_Fault_ISR

App_Spurious_ISR    B       App_Spurious_ISR

; Reset handler routine
Reset_Handler    PROC
                 EXPORT  Reset_Handler
                 IMPORT  __main
                 LDR     R0, =__main
                 BX      R0
                 ENDP

                 ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB
                
                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit
                
                 ELSE
                
                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap
__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END

;******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE*****
