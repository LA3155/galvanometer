#include "hwdataaccess.h"

void HW_ina226_Init(void)
{
	#if HW_POWER
		return ina226_init();
	#endif
}

void HW_flash_init(void)
{
	#if HW_FLASH
		return ;
	#endif
}

void HW_lcd_init(void)
{
	#if HW_LCD
		return lcd_init();
	#endif
}

HW_DATAACCESS_Interfacetypedef hw_dataaccess = 
{
    .ina226 = 
    {
        .Init = ina226_init,
    },

    .lcd = 
    {
        .Init = lcd_init,
    },
};