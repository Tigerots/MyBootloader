

/**************************** (C) COPYRIGHT 2019 ****************************
* ���ʦ:   Tigerots
* ����ʱ��: 2019.12.28
* ��������: Flash��������
*       
*****************************************************************************
* ********************************�޸���ʷ��¼********************************
* �޸�ʱ��: 
* �汾��:
* �޸�����:
*****************************************************************************/
#define __MY_FLASH

#include <stdio.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "my_iap.h"
#include "my_type.h"
#include "my_flash.h"


/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: �õ��豸ΨһID��,
            96λ�Ķ���IDλ�ڵ�ַ 0x1FFFF7E8 ~ 0x1FFFF7F4 ��ϵͳ�洢�� 
            ��ST�ڹ�����д��(�û������޸�) �û��������ֽڡ����֡�
            ���ֵķ�ʽ������ȡ������һ��ַ
* ��ڲ���: 
* ��������: 12�ֽ�ID�Ŵ洢����
*****************************************************************************/
void my_get_device_id(u8 *ID_Buffer)
{
	u8 i;
    u32 FlashDestination; 

	//Flash��IC����Ŀ���ַ
	FlashDestination = 0x1FFFF7E8;
	//��Կռ4�ֽ�
	for(i=0;i<12;i++)
	{
		*ID_Buffer= *(u8*)FlashDestination;
		ID_Buffer++;
		FlashDestination ++;
	}
}

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.29
* ��������: ��������ռ�õ���������
* ��ڲ���: ��ʼ��ַ, ���ݳ���
* ��������: ��������
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

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.29
* ��������: ����Ŀ������
* ��ڲ���: ��ʼ��ַ, ���ݳ���
* ��������: ������ִ�����������ᱻ����Ϊ�����FLASH��ַ
*****************************************************************************/
uint32_t my_erase_pages(uint32_t Eaddr, uint16_t Elen)
{
    
    uint32_t PageError = 0; 
    FLASH_EraseInitTypeDef My_Flash; //Flash�����ṹ
    
    //д��ĵ�ַ��ΧӦ��С��
    if( Eaddr < STM_FLASH_BASE || (Eaddr >= (STM_FLASH_BASE+STM_FLASH_SIZE)))
    {
        return 1;//�Ƿ���ַ
    }
    
    My_Flash.TypeErase = FLASH_TYPEERASE_PAGES; //����Flashִ��ҳ��ֻ����������
    My_Flash.PageAddress = Eaddr;   //����Ҫ�����ĵ�ַ
    uint16_t page_num = my_calculate_page_num(Eaddr, Elen);//����ʹ�õ�������
    My_Flash.NbPages = page_num;   //˵��Ҫ������ҳ��
    
    HAL_FLASH_Unlock();//��������Flash
    HAL_FLASHEx_Erase(&My_Flash, &PageError);//����ʹ�õ�����
    HAL_FLASH_Lock(); //��סFlash
    
    return PageError;
}

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: ����-�޸�-����-��д, ������һ�������ڴ�������ݵ����
* ��ڲ���: 
* ��������: 
*****************************************************************************/
uint8_t my_read_erase_write_to_flash(uint32_t Eaddr, uint8_t *Edat, uint16_t Elen)
{//TODO: ��û���
	uint32_t j;
    uint32_t Write_Flash_Data;
    uint32_t FlashDestination;
	
    //д��ĵ�ַ��ΧӦ��С��
    if( Eaddr < STM_FLASH_BASE || (Eaddr >= (STM_FLASH_BASE+STM_FLASH_SIZE)))
    {
        return 1;//�Ƿ���ַ
    }
    
    //��������
    my_erase_pages(Eaddr, Elen);
    
    HAL_FLASH_Unlock();//��������Flash
    FlashDestination = Eaddr;//�׵�ַ
    Write_Flash_Data = (uint32_t)(&Edat[0]);//��ȡ����
    for (j = 0; j<Elen; j += 4)
	{
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FlashDestination, *(uint32_t*)Write_Flash_Data);
		FlashDestination += 4;
		Write_Flash_Data += 4;
	}
	HAL_FLASH_Lock(); //��סFlash
    
    return 0;
}

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: д���ݵ�Flash, д֮ǰ�Ȳ���, �޷�������������������
            1. Ҫ��д�����ݶ�ռ�ô洢����
* ��ڲ���: 
* ��������: 
*****************************************************************************/
uint8_t my_erase_write_to_flash(uint32_t Eaddr, uint8_t *Edat, uint16_t Elen)
{
	uint32_t j;
    uint32_t Write_Flash_Data;
    uint32_t FlashDestination;
	
    //д��ĵ�ַ��ΧӦ��С��
    if( Eaddr < STM_FLASH_BASE || (Eaddr >= (STM_FLASH_BASE+STM_FLASH_SIZE)))
    {
        return 1;//�Ƿ���ַ
    }
    
    //��������
    my_erase_pages(Eaddr, Elen);
    
    HAL_FLASH_Unlock();//��������Flash
    FlashDestination = Eaddr;//�׵�ַ
    Write_Flash_Data = (uint32_t)(&Edat[0]);//��ȡ����
    for (j = 0; j<Elen; j += 4)
	{
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FlashDestination, *(uint32_t*)Write_Flash_Data);
		FlashDestination += 4;
		Write_Flash_Data += 4;
	}
	HAL_FLASH_Lock(); //��סFlash
    
    return 0;
}

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: ֱ��д���ݵ�Flash, �޲�������, ��Ҫ����֮ǰ��������
* ��ڲ���: 
* ��������: 
*****************************************************************************/
uint8_t my_write_to_flash(uint32_t Eaddr, uint8_t *Edat, uint16_t Elen)
{
	uint32_t j;
    uint32_t Write_Flash_Data;
    uint32_t FlashDestination;
	
    //д��ĵ�ַ��ΧӦ��С��
    if( Eaddr < STM_FLASH_BASE || (Eaddr >= (STM_FLASH_BASE+STM_FLASH_SIZE)))
    {
        return 1;//�Ƿ���ַ
    }
    
	HAL_FLASH_Unlock();//��������Flash
    FlashDestination = Eaddr;//д���׵�ַ
    Write_Flash_Data = (uint32_t)(&Edat[0]);//��ȡ����
    for (j = 0; j<Elen; j += 4)
	{
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FlashDestination, *(uint32_t*)Write_Flash_Data);
		FlashDestination += 4;
		Write_Flash_Data += 4;
	}
	HAL_FLASH_Lock(); //��סFlash
    
    return 0;
}


/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: ��ȡFlash���ݵ�����
* ��ڲ���: 
* ��������: 
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

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: iap��buff��ת���������뵽user��, 
            ����:����user��flash -> ѭ����ȡbuff���� -> ѭ��д�뵽uwer��
* ��ڲ���: 
            MyIapMap, MyIapFlag 
* ��������: 
*****************************************************************************/
uint8_t my_buffer[STM_PAGE_SIZE];
uint8_t iap_re_write_user_code(void)
{
    uint32_t j;
    uint32_t Flash_des, Flash_obj;
    uint32_t PageError = 0; //������ִ�����������ᱻ����Ϊ�����FLASH��ַ
    FLASH_EraseInitTypeDef My_Flash; //Flash�����ṹ
	
    
	HAL_FLASH_Unlock();//��������Flash

    My_Flash.TypeErase = FLASH_TYPEERASE_PAGES; //����Flashִ��ҳ��ֻ����������
    My_Flash.PageAddress = MyIapMap.UserAddr;   //����Ҫ�����ĵ�ַ
    My_Flash.NbPages = (MyIapFlag.FileSize / STM_PAGE_SIZE);   //˵��Ҫ������ҳ��
    if(MyIapFlag.FileSize % STM_PAGE_SIZE > 0)
    {
        My_Flash.NbPages = My_Flash.NbPages+1;
    }
    HAL_FLASHEx_Erase(&My_Flash, &PageError);//������������

    Flash_des = MyIapMap.BuffAddr;//ԭ��ַ
    Flash_obj = MyIapMap.UserAddr;//Ŀ���ַ
    for (j = 0; j<My_Flash.NbPages; j += 1)
	{
        my_read_from_flash(Flash_des, my_buffer, STM_PAGE_SIZE);
        my_write_to_flash(Flash_obj, my_buffer, STM_PAGE_SIZE);
		Flash_des += STM_PAGE_SIZE;
		Flash_obj += STM_PAGE_SIZE;
	}
	HAL_FLASH_Lock(); //��סFlash
    
    return 0;
}




