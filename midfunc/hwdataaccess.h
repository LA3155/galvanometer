#pragma once

/***************************
 *  Hardware Define
 ***************************/
#define HW_DATAACCESS       1

#if HW_DATAACCESS
    #define HW_POWER        1
    #define HW_LCD          1
    #define HW_FLASH        1
    #define HW_ESP32        0
#endif

#if HW_POWER
  #include "ina226.h"
#endif

#if HW_FLASH
  #include "flash.h"
#endif

#if HW_LCD
  #include "lcd.h"
#endif

/***************************
 *  typedefs
 ***************************/
typedef struct
{
    void (*Init)(void);
}HW_POWER_Interfacetypedef;

typedef struct
{
    void (*Init)(void);
}HW_FLASH_Interfacetypedef;

typedef struct
{
    void (*Init)(void);
}HW_LCD_Interfacetypedef;

typedef struct
{
    HW_POWER_Interfacetypedef   ina226;
    HW_FLASH_Interfacetypedef   flash;
    HW_LCD_Interfacetypedef     lcd;
}HW_DATAACCESS_Interfacetypedef;

