# ARMduino Bootloader Source

This is the boot-up code for ARMduino MKL26Z128VFM4 programmer MCU.

If PTD5 (PROG_NRESET) is LOW after 10ms since boot the bootloader will initiate USB peripheral. Mounting a MSD virtual disk unit that user can drag & drop the programmer binaries and initiate FLASH reprogramming. Providing useful and easy firmware updates.

If PTD5 is kept HIGH (normal state) bootloader will proceed to boot the DAPLink-if code.
