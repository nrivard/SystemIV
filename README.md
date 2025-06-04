# System IV

```ascii
   _____            __                     ___    __
  / ___/__  _______/ /____  ____ ___      /=/ |  / /
  \__ \/ / / / ___/ __/ _ \/ __ `__ \    /=/| | / /
 ___/ / /_/ (__  ) /_/  __/ / / / / /   /=/ | |/ /
/____/\__, /____/\__/\___/_/ /_/ /_/   /=/  |___/
     \____/
```

System IV (aka System N) is a Unix-y operating system for retro and homebrew computers.
It is intended to itself be retro with the look and feel of older systems.
This means there is a ROM that looks for a bootloader on a "floppy disk", the bootloader is pure assembly code in the first sector of this disk, and the rest of the OS is loaded by the bootloader and executed.

There are 3 major components to System IV:

* a ROM that exists in dedicated hardware and is executed on first boot of the machine
* a bootloader in the MBR record of a "floppy disk" (I use an SD card)
* an operating system that is loaded from the disk

There is also some dedicated tooling created to aid in the installation of these parts, specifically for burning the ROM and installing the bootloader onto the MBR of a FAT32 SD card.

For more details on each part, see the README in the appropriate subfolder.
