



#ifndef  __MY_FLASH_H__
#define  __MY_FLASH_H__
 
 
 
#ifndef  __MY_FLASH
#define	 __MY_FLASH_EXT  extern
#else
#define	 __MY_FLASH_EXT 
#endif

#include "my_type.h"
#include "stdint.h"

__MY_FLASH_EXT uint32_t my_erase_pages(uint32_t Eaddr, uint16_t Elen);
__MY_FLASH_EXT uint8_t iap_re_write_user_code(void);
__MY_FLASH_EXT void my_get_device_id(u8 *ID_Buffer);
__MY_FLASH_EXT uint8_t my_write_to_flash(uint32_t Eaddr, uint8_t *Edat, uint16_t Elen);
__MY_FLASH_EXT void my_read_from_flash(uint32_t Eaddr, uint8_t *Edat, uint16_t Elen);
__MY_FLASH_EXT uint8_t my_erase_write_to_flash(uint32_t Eaddr, uint8_t *Edat, uint16_t Elen);
__MY_FLASH_EXT uint8_t my_read_erase_write_to_flash(uint32_t Eaddr, uint8_t *Edat, uint16_t Elen);

#endif
