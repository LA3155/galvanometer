cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(galvanometer C ASM)

set(GD32_LIB_ROOT "${CMAKE_CURRENT_LIST_DIR}/firmware_lib/GD32F30x_Firmware_Library_V3.0.3/GD32F30x_Firmware_Library_V3.0.3/GD32F30x_Firmware_Library_V3.0.3")
set(GD32_CMSIS "${GD32_LIB_ROOT}/Firmware/CMSIS")
set(GD32_DEVICE "${GD32_CMSIS}/GD/GD32F30x")
set(GD32_PERIPH "${GD32_LIB_ROOT}/Firmware/GD32F30x_standard_peripheral")
set(LINKER_SCRIPT "${GD32_DEVICE}/Source/GCC/Ld/gd32f303xE_flash.ld")

add_executable(${PROJECT_NAME}.elf
    app/main.c
    bsp/board.c
    bsp/bsp_lcd.c
    bsp/bsp_ina226.c
    bsp/system_gd32f30x.c
    cdc/cdc.c
    cdc/cdc_acm_core.c
    cdc/gd32f30x_usbd_hw.c
    cdc/usbd_core.c
    cdc/usbd_enum.c
    cdc/usbd_transc.c
    cdc/usbd_pwr.c
    cdc/usbd_lld_core.c
    cdc/usbd_lld_int.c
    ${GD32_DEVICE}/Source/GCC/newlib/syscalls.c
    ${GD32_DEVICE}/Source/GCC/startup_gd32f30x_xd.S
    ${GD32_PERIPH}/Source/gd32f30x_rcu.c
    ${GD32_PERIPH}/Source/gd32f30x_gpio.c
    ${GD32_PERIPH}/Source/gd32f30x_exti.c
    ${GD32_PERIPH}/Source/gd32f30x_spi.c
    ${GD32_PERIPH}/Source/gd32f30x_i2c.c
    ${GD32_PERIPH}/Source/gd32f30x_timer.c
    ${GD32_PERIPH}/Source/gd32f30x_misc.c
    ${GD32_PERIPH}/Source/gd32f30x_exti.c
    ${GD32_PERIPH}/Source/gd32f30x_pmu.c
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    app
    bsp
    cdc
    ${GD32_CMSIS}
    ${GD32_DEVICE}/Include
    ${GD32_PERIPH}/Include
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    GD32F30X_XD
    HXTAL_VALUE=16000000U
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=soft
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
    -Wno-unused-parameter
    -O0
    -g3
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=soft
    -T${LINKER_SCRIPT}
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--no-warn-rwx-segments
    --specs=nano.specs
    --specs=nosys.specs
)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
)
