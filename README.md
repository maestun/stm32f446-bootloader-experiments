STM32Cube IDE projects

# actuator-driver-bootloader

The bootloader, will be flashed at 0x8000000 (sector 0), binary shouldn't be more than 16kB in size: see [linker file](actuator-driver-bootloader/STM32F446RETX_FLASH.ld).

## TODO

- In linker file, set
`FLASH    (rx)    : ORIGIN = 0x8000000,   LENGTH = 16K`
to 
`FLASH    (rx)    : ORIGIN = 0x8000000,   LENGTH = 8K`
and see what happens

- Load value at sector 7 offset zero, set it at jump address (create a python script to flash this addr)

- integrate bootloader into firmware: start with bl_init(), if no address to jump to, just continue executing (and if addr to jump to equals my own addr, ignore)


# actuator-driver-firmware

Fast blinker, flashed after bootloader, at 0x8004000 (sectors 1...4)

## TODO

See [this file](actuator-driver-firmware/Core/Src/system_stm32f4xx.c), symbol `VECT_TAB_OFFSET` is hardcoded to `0x00004000U`, set in in startup code ?


# actuator-driver-firmware-copy

Slow blinker, flashed at 0x8020000 (sector 5)

## TODO

See [this file](actuator-driver-firmware/Core/Src/system_stm32f4xx.c), symbol `VECT_TAB_OFFSET` is hardcoded to `0x00004000U`, set in in startup code ?