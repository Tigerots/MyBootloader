/**
  ******************************************************************************
  * @file    stm32l152d_eval_spi_ee.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    24-January-2012
  * @brief   This file provides a set of functions needed to manage the SPI M95xxx
  *          EEPROM memory mounted on STM32L152D-EVAL board (refer to stm32l152d_eval.h
  *          to know about the boards supporting this memory). 
  *          It implements a high level communication layer for read and write 
  *          from/to this memory. The needed STM32 hardware resources (SPI and 
  *          GPIO) are defined in stm32l152d_eval.h file, and the initialization is 
  *          performed in SPI_sEE_LowLevel_Init() function declared in stm32l152d_eval.c 
  *          file.
  *          You can easily tailor this driver to any other development board, 
  *          by just adapting the defines for hardware resources and 
  *          SPI_sEE_LowLevel_Init() function.
  *            
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+---------------+-------------+
  *          |  STM32 SPI Pins             |      sEE      |    Pin      |
  *          +-----------------------------+---------------+-------------+
  *          | sEE_CS_PIN                  | ChipSelect    |    1 (/S)   |
  *          | sEE_MISO_PIN / MISO         |   DataOut     |    2 (Q)    |
  *          | sEE_WP_PIN                  | WriteProtect  |    3 (/W)   |
  *          |                             |   GND         |    4 (0 V)  |
  *          | sEE_MOSI_PIN / MOSI         |   DataIn      |    5 (D)    |
  *          | sEE_SCK_PIN / SCLK          |   Clock       |    6 (C)    |
  *          |                             |   Hold        |    7 (/HOLD)|
  *          |                             |   VCC         |    8 (3.3 V)|  
  *          +-----------------------------+---------------+-------------+  
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
  * LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l152d_eval_spi_ee.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup STM32L152D_EVAL
  * @{
  */
  
/** @addtogroup STM32L152D_EVAL_SPI_EEPROM
  * @brief      This file includes the M95xxx SPI EEPROM driver of STM32-EVAL boards.
  * @{
  */  

/** @defgroup STM32L152D_EVAL_SPI_EEPROM_Private_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32L152D_EVAL_SPI_EEPROM_Private_Defines
  * @{
  */  
/**
  * @}
  */ 

/** @defgroup STM32L152D_EVAL_SPI_EEPROM_Private_Macros
  * @{
  */
/**
  * @}
  */ 
  

/** @defgroup STM32L152D_EVAL_SPI_EEPROM_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32L152D_EVAL_SPI_EEPROM_Private_Function_Prototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32L152D_EVAL_SPI_EEPROM_Private_Functions
  * @{
  */ 

/**
  * @brief  DeInitializes the peripherals used by the SPI EEPROM driver.
  * @param  None
  * @retval None
  */
void sEE_DeInit(void)
{
  sEE_SPI_LowLevel_DeInit();
}

/**
  * @brief  Initializes the peripherals used by the SPI EEPROM driver.
  * @param  None
  * @retval None
  */
void sEE_Init(void)
{
  sEE_SPI_LowLevel_Init();
    
  /*!< Deselect the EEPROM: Chip Select high */
  sEE_CS_HIGH();
}

/**
  * @brief  Writes more than one byte to the EEPROM with a single WRITE cycle 
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the EEPROM page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the EEPROM.
  * @param  WriteAddr: EEPROM's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the EEPROM, must be equal
  *         or less than "sEE_PAGESIZE" value.
  * @retval None
  */
uint32_t sEE_WritePage(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t* NumByteToWrite)
{
  /*!< Enable the write access to the EEPROM */
  sEE_WriteEnable();
  
  /*!< Select the EEPROM: Chip Select low */
  sEE_CS_LOW();

  /*!< Send "Write to Memory" instruction and MSB of WriteAddr  */
  sEE_SendByte(sEE_CMD_WRITE | (uint8_t)((WriteAddr & 0x0100)>>5));
  
  /*!< Send WriteAddr address byte to write to */
  sEE_SendByte(WriteAddr & 0xFF);
  
  /*!< while there is data to be written on the EEPROM */
  while ((*NumByteToWrite)--)
  {
    /*!< Send the current byte */
    sEE_SendByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }
  
  /*!< Deselect the EEPROM: Chip Select high */
  sEE_CS_HIGH();
  
  /*!< Wait the end of EEPROM writing */
  sEE_WaitEepromStandbyState();
  
  /*!< Disable the write access to the EEROM */
  sEE_WriteDisable();
  
  return 0;
}

/**
  * @brief  Writes block of data to the EEPROM. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the EEPROM.
  * @param  WriteAddr: EEPROM's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the EEPROM.
  * @retval None
  */
void sEE_WriteBuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
  uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
  uint16_t   sEE_DataNum = 0;
  
  Addr = WriteAddr % sEE_PAGESIZE;
  count = sEE_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / sEE_PAGESIZE;
  NumOfSingle = NumByteToWrite % sEE_PAGESIZE;

  if (Addr == 0) /*!< WriteAddr is sEE_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sEE_PAGESIZE */
    {
      sEE_DataNum = NumByteToWrite;      
      sEE_WritePage(pBuffer, WriteAddr, &sEE_DataNum);
    }
    else /*!< NumByteToWrite > sEE_PAGESIZE */
    {
      while (NumOfPage--)
      {
        sEE_DataNum = sEE_PAGESIZE;
        sEE_WritePage(pBuffer, WriteAddr, &sEE_DataNum);
        WriteAddr +=  sEE_PAGESIZE;
        pBuffer += sEE_PAGESIZE;
      }
      
      sEE_DataNum = NumOfSingle;
      sEE_WritePage(pBuffer, WriteAddr, &sEE_DataNum);
    }
  }
  else /*!< WriteAddr is not sEE_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sEE_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sEE_PAGESIZE */
      {
        temp = NumOfSingle - count;
        sEE_DataNum = count;
        sEE_WritePage(pBuffer, WriteAddr, &sEE_DataNum);
        WriteAddr +=  count;
        pBuffer += count;
        
        sEE_DataNum = temp;
        sEE_WritePage(pBuffer, WriteAddr, &sEE_DataNum);
      }
      else
      {
        sEE_DataNum = NumByteToWrite; 
        sEE_WritePage(pBuffer, WriteAddr, &sEE_DataNum);
      }
    }
    else /*!< NumByteToWrite > sEE_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / sEE_PAGESIZE;
      NumOfSingle = NumByteToWrite % sEE_PAGESIZE;

      sEE_DataNum = count;
        
      sEE_WritePage(pBuffer, WriteAddr, &sEE_DataNum);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        sEE_DataNum = sEE_PAGESIZE;
        
        sEE_WritePage(pBuffer, WriteAddr, &sEE_DataNum);
        WriteAddr +=  sEE_PAGESIZE;
        pBuffer += sEE_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        sEE_DataNum = NumOfSingle;
        
        sEE_WritePage(pBuffer, WriteAddr, &sEE_DataNum);
      }
    }
  }
}

/**
  * @brief  Reads a block of data from the EEPROM.
  * @param  pBuffer: pointer to the buffer that receives the data read from the EEPROM.
  * @param  ReadAddr: EEPROM's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the EEPROM.
  * @retval None
  */
uint32_t sEE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t* NumByteToRead)
{
  /*!< Select the EEPROM: Chip Select low */
  sEE_CS_LOW();
  
  /*!< Send "Read from Memory" instruction and MSB of WriteAddr  */
  sEE_SendByte(sEE_CMD_READ | (uint8_t)((ReadAddr & 0x100)>>5));

  /*!< Send WriteAddr address byte to write to */
  sEE_SendByte(ReadAddr & 0xFF);
  
  while ((*NumByteToRead)--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the EEPROM */
    *pBuffer = sEE_SendByte(sEE_DUMMY_BYTE);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }
  
  /*!< Deselect the EEPROM: Chip Select high */
  sEE_CS_HIGH();
  
  return 0;
}


/**
  * @brief  Reads a byte from the SPI EEPROM.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI EEPROM.
  */
uint8_t sEE_ReadByte(void)
{
  return (sEE_SendByte(sEE_DUMMY_BYTE));
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t sEE_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(sEE_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI peripheral */
  SPI_SendData(sEE_SPI, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(sEE_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return (uint8_t)SPI_ReceiveData(sEE_SPI);
}
/**
  * @brief  Enables the write access to the EEPROM.
  * @param  None
  * @retval None
  */
void sEE_WriteEnable(void)
{
  /*!< Select the EEPROM: Chip Select low */
  sEE_CS_LOW();

  /*!< Send "Write Enable" instruction */
  sEE_SendByte(sEE_CMD_WREN);

  /*!< Deselect the EEPROM: Chip Select high */
  sEE_CS_HIGH();
}

/**
  * @brief  Disables the write access to the EEPROM.
  * @param  None
  * @retval None
  */
void sEE_WriteDisable(void)
{
  /*!< Select the EEPROM: Chip Select low */
  sEE_CS_LOW();

  /*!< Send "Write Disable" instruction */
  sEE_SendByte(sEE_CMD_WRDI);

  /*!< Deselect the EEPROM: Chip Select high */
  sEE_CS_HIGH();
}

/**
  * @brief  Write new value in EEPROM Status Register.
  * @param  regval : new value of register
  * @retval None
  */
void sEE_WriteStatusRegister(uint8_t regval)
{
  /*!< Select the EEPROM: Chip Select low */
  sEE_CS_LOW();

  /*!< Enable the write access to the EEPROM */
  sEE_WriteEnable();
  
  /*!< Send "Write Status Register" instruction */
  sEE_SendByte(sEE_CMD_WRSR);
  
  /*!< Write regval in status register */
  sEE_SendByte(regval);  

  /*!< Deselect the EEPROM: Chip Select high */
  sEE_CS_HIGH();
}

/**
  * @brief  Read EEPROM Status Register.
  * @param  None
  * @retval The value of the Status register.
  */
uint8_t sEE_ReadStatusRegister(void)
{
  uint8_t sEEstatus = 0;
  
  /*!< Select the EEPROM: Chip Select low */
  sEE_CS_LOW();
  
  /*!< Send "Read Status Register" instruction */
  sEE_SendByte(sEE_CMD_RDSR);
  
  /*!< Send a dummy byte to generate the clock needed by the EEPROM
  and put the value of the status register in EEPROM Status variable */
  sEEstatus = sEE_SendByte(sEE_DUMMY_BYTE);
  
  /*!< Deselect the EEPROM: Chip Select high */
  sEE_CS_HIGH();
  
  return sEEstatus;
}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the EEPROM's
  *         status register and loop until write operation has completed.
  * @param  None
  * @retval None
  */
uint32_t sEE_WaitEepromStandbyState(void)      
{
  uint8_t sEEstatus = 0;

  /*!< Select the EEPROM: Chip Select low */
  sEE_CS_LOW();

  /*!< Send "Read Status Register" instruction */
  sEE_SendByte(sEE_CMD_RDSR);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the EEPROM
    and put the value of the status register in EEPROM Status variable */
    sEEstatus = sEE_SendByte(sEE_DUMMY_BYTE);

  }
  while ((sEEstatus & sEE_WIP_FLAG) == SET); /* Write in progress */

  /*!< Deselect the EEPROM: Chip Select high */
  sEE_CS_HIGH();
  
  return 0;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */  

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/