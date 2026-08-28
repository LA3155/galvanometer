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
    core/dma.c
    core/flash.c
    core/freertos.c
    core/gd32f30x_it.c
    core/gpio.c
    core/i2c.c
    core/main.c
    core/spi.c
    bsp/delay.c
    bsp/ina226.c
    bsp/key.c
    bsp/lcd.c
    midfunc/hwdataaccess.c

    cdc/cdc.c
    cdc/cdc_acm_core.c
    cdc/gd32f30x_usbd_hw.c
    cdc/usbd_core.c
    cdc/usbd_enum.c
    cdc/usbd_transc.c
    cdc/usbd_pwr.c
    cdc/usbd_lld_core.c
    cdc/usbd_lld_int.c

    app/event_detect.c
    app/log_store.c
    app/power_sample.c
    app/waveform_draw.c

    CMSIS/startup_gd32f30x_xd.S
    CMSIS/system_gd32f30x.c
    GD32F30x_Driver/src/syscalls.c
    GD32F30x_Driver/src/gd32f30x_rcu.c
    GD32F30x_Driver/src/gd32f30x_gpio.c
    GD32F30x_Driver/src/gd32f30x_exti.c
    GD32F30x_Driver/src/gd32f30x_spi.c
    GD32F30x_Driver/src/gd32f30x_i2c.c
    GD32F30x_Driver/src/gd32f30x_timer.c
    GD32F30x_Driver/src/gd32f30x_misc.c
    GD32F30x_Driver/src/gd32f30x_pmu.c
    GD32F30x_Driver/src/gd32f30x_dma.c
)

target_sources(galvanometer.elf PRIVATE
    FreeRTOS/src/tasks.c
    FreeRTOS/src/queue.c
    FreeRTOS/src/list.c
    FreeRTOS/src/timers.c
    FreeRTOS/src/event_groups.c
    FreeRTOS/src/stream_buffer.c
    FreeRTOS/port/GCC/ARM_CM4F/port.c
    FreeRTOS/port/MemMang/heap_4.c
    FreeRTOS/CMSIS_RTOS_V2/cmsis_os2.c
    Tasks/user_hardwareinit_Task.c
    Tasks/user_ina226_Task.c
    Tasks/user_key_Task.c
    Tasks/user_lcd_Task.c
    Tasks/user_messagesend_Task.c
    Tasks/user_TasksInit.c
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    app
    bsp
    cdc
    core
    midfunc
    Tasks
    FreeRTOS
    FreeRTOS/inc
    FreeRTOS/port/GCC/ARM_CM4F
    FreeRTOS/CMSIS_RTOS_V2
    CMSIS
    GD32F30x_Driver/inc
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    GD32F30X_XD
    HXTAL_VALUE=16000000U
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
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
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
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
