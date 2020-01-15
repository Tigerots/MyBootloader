

/**************************** (C) COPYRIGHT 2019 ****************************
* 设计师:   Tigerots
* 创建时间: 2019.12.28
* 功能描述: Flash基本操作
*       
*****************************************************************************
* ********************************修改历史记录********************************
* 修改时间: 
* 版本号:
* 修改内容:
*****************************************************************************/
#define __MY_FLASH

#include <stdio.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "my_iap.h"
#include "my_type.h"
#include "my_flash.h"


/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 得到设备唯一ID号,
            96位的独特ID位于地址 0x1FFFF7E8 ~ 0x1FFFF7F4 的系统存储区 
            由ST在工厂中写入(用户不能修改) 用户可以以字节、半字、
            或字的方式单独读取其间的任一地址
* 入口参数: 
* 函数返回: 12字节ID号存储缓存
*****************************************************************************/
void my_get_device_id(u8 *ID_Buffer)
{
	u8 i;
    u32 FlashDestination; 

	//Flash上IC虚拟目标地址
	FlashDestination = 0x1FFFF7E8;
	//密钥占4字节
	for(i=0;i<12;i++)
	{
		*ID_Buffer= *(u8*)FlashDestination;
		ID_Buffer++;
		FlashDestination ++;
	}
}

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.29
* 功能描述: 计算数据占用的扇区个数
* 入口参数: 起始地址, 数据长度
* 函数返回: 扇区个数
*****************************************************************************/
uint16_t my_calculate_page_num(uint32_t Eaddr, uint16_t Elen)
{
    uint16_t page_total_num = 0; 
    uint32_t page_start_num = 0;
    uint32_t page_end_num = 0;
    
    uint32_t temp = 0;
    
    page_start_num = (Eaddr - STM_FLASH_BASE) / STM_PAGE_SIZE;
    temp = (Eaddr - STM_FLASH_BASE) % STM_PAGE_SIZE;
    if(temp > 0)
    {
        page_start_num = page_start_num+1;
    }
    
    page_end_num = (Eaddr + Elen - STM_FLASH_BASE) / STM_PAGE_SIZE;
    temp = (Eaddr + Elen - STM_FLASH_BASE) % STM_PAGE_SIZE;
    if(temp > 0)
    {
        page_end_num = page_end_num+1;
    }
    
    page_total_num = page_end_num - page_start_num + 1;
    
    return page_total_num;
}

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.29
* 功能描述: 擦除目标扇区
* 入口参数: 起始地址, 数据长度
* 函数返回: 如果出现错误这个变量会被设置为出错的FLASH地址
*****************************************************************************/
uint32_t my_erase_pages(uint32_t Eaddr, uint16_t Elen)
{
    
    uint32_t PageError = 0; 
    FLASH_EraseInitTypeDef My_Flash; //Flash擦除结构
    
    //写入的地址范围应该小于
    if( Eaddr < STM_FLASH_BASE || (Eaddr >= (STM_FLASH_BASE+STM_FLASH_SIZE)))
    {
        return 1;//非法地址
    }
    
    My_Flash.TypeErase = FLASH_TYPEERASE_PAGES; //标明Flash执行页面只做擦除操作
    My_Flash.PageAddress = Eaddr;   //声明要擦除的地址
    uint16_t page_num = my_calculate_page_num(Eaddr, Elen);//计算使用的扇区数
    My_Flash.NbPages = page_num;   //说明要擦除的页数
    
    HAL_FLASH_Unlock();//解锁程序Flash
    HAL_FLASHEx_Erase(&My_Flash, &PageError);//擦除使用的扇区
    HAL_FLASH_Lock(); //锁住Flash
    
    return PageError;
}

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 读出-修改-擦除-重写, 适用于一个扇区内存多组数据的情况
* 入口参数: 
* 函数返回: 
*****************************************************************************/
uint8_t my_read_erase_write_to_flash(uint32_t Eaddr, uint8_t *Edat, uint16_t Elen)
{//TODO: 还没完成
	uint32_t j;
    uint32_t Write_Flash_Data;
    uint32_t FlashDestination;
	
    //写入的地址范围应该小于
    if( Eaddr < STM_FLASH_BASE || (Eaddr >= (STM_FLASH_BASE+STM_FLASH_SIZE)))
    {
        return 1;//非法地址
    }
    
    //擦除扇区
    my_erase_pages(Eaddr, Elen);
    
    HAL_FLASH_Unlock();//解锁程序Flash
    FlashDestination = Eaddr;//首地址
    Write_Flash_Data = (uint32_t)(&Edat[0]);//读取数据
    for (j = 0; j<Elen; j += 4)
	{
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FlashDestination, *(uint32_t*)Write_Flash_Data);
		FlashDestination += 4;
		Write_Flash_Data += 4;
	}
	HAL_FLASH_Lock(); //锁住Flash
    
    return 0;
}

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 写数据到Flash, 写之前先擦除, 无法保护扇区内其他数据
            1. 要求写入数据独占该存储区域
* 入口参数: 
* 函数返回: 
*****************************************************************************/
uint8_t my_erase_write_to_flash(uint32_t Eaddr, uint8_t *Edat, uint16_t Elen)
{
	uint32_t j;
    uint32_t Write_Flash_Data;
    uint32_t FlashDestination;
	
    //写入的地址范围应该小于
    if( Eaddr < STM_FLASH_BASE || (Eaddr >= (STM_FLASH_BASE+STM_FLASH_SIZE)))
    {
        return 1;//非法地址
    }
    
    //擦除扇区
    my_erase_pages(Eaddr, Elen);
    
    HAL_FLASH_Unlock();//解锁程序Flash
    FlashDestination = Eaddr;//首地址
    Write_Flash_Data = (uint32_t)(&Edat[0]);//读取数据
    for (j = 0; j<Elen; j += 4)
	{
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FlashDestination, *(uint32_t*)Write_Flash_Data);
		FlashDestination += 4;
		Write_Flash_Data += 4;
	}
	HAL_FLASH_Lock(); //锁住Flash
    
    return 0;
}

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 直接写数据到Flash, 无擦除操作, 需要调用之前单独擦除
* 入口参数: 
* 函数返回: 
*****************************************************************************/
uint8_t my_write_to_flash(uint32_t Eaddr, uint8_t *Edat, uint16_t Elen)
{
	uint32_t j;
    uint32_t Write_Flash_Data;
    uint32_t FlashDestination;
	
    //写入的地址范围应该小于
    if( Eaddr < STM_FLASH_BASE || (Eaddr >= (STM_FLASH_BASE+STM_FLASH_SIZE)))
    {
        return 1;//非法地址
    }
    
	HAL_FLASH_Unlock();//解锁程序Flash
    FlashDestination = Eaddr;//写入首地址
    Write_Flash_Data = (uint32_t)(&Edat[0]);//读取数据
    for (j = 0; j<Elen; j += 4)
	{
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FlashDestination, *(uint32_t*)Write_Flash_Data);
		FlashDestination += 4;
		Write_Flash_Data += 4;
	}
	HAL_FLASH_Lock(); //锁住Flash
    
    return 0;
}


/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 读取Flash内容到缓存
* 入口参数: 
* 函数返回: 
*****************************************************************************/
void my_read_from_flash(uint32_t Eaddr, uint8_t *Edat, uint16_t Elen)
{
	uint16_t i;
    uint32_t FlashDestination; 
	
	FlashDestination = Eaddr;
	for(i=0; i<Elen; i++)
	{
		Edat[i]= *(uint8_t*)FlashDestination;
		FlashDestination ++;
	}
}

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: iap从buff区转存升级代码到user区, 
            过程:擦除user区flash -> 循环读取buff数据 -> 循环写入到uwer区
* 入口参数: 
            MyIapMap, MyIapFlag 
* 函数返回: 
*****************************************************************************/
uint8_t my_buffer[STM_PAGE_SIZE];
uint8_t iap_re_write_user_code(void)
{
    uint32_t j;
    uint32_t Flash_des, Flash_obj;
    uint32_t PageError = 0; //如果出现错误这个变量会被设置为出错的FLASH地址
    FLASH_EraseInitTypeDef My_Flash; //Flash擦除结构
	
    
	HAL_FLASH_Unlock();//解锁程序Flash

    My_Flash.TypeErase = FLASH_TYPEERASE_PAGES; //标明Flash执行页面只做擦除操作
    My_Flash.PageAddress = MyIapMap.UserAddr;   //声明要擦除的地址
    My_Flash.NbPages = (MyIapFlag.FileSize / STM_PAGE_SIZE);   //说明要擦除的页数
    if(MyIapFlag.FileSize % STM_PAGE_SIZE > 0)
    {
        My_Flash.NbPages = My_Flash.NbPages+1;
    }
    HAL_FLASHEx_Erase(&My_Flash, &PageError);//擦除所用扇区

    Flash_des = MyIapMap.BuffAddr;//原地址
    Flash_obj = MyIapMap.UserAddr;//目标地址
    for (j = 0; j<My_Flash.NbPages; j += 1)
	{
        my_read_from_flash(Flash_des, my_buffer, STM_PAGE_SIZE);
        my_write_to_flash(Flash_obj, my_buffer, STM_PAGE_SIZE);
		Flash_des += STM_PAGE_SIZE;
		Flash_obj += STM_PAGE_SIZE;
	}
	HAL_FLASH_Lock(); //锁住Flash
    
    return 0;
}




