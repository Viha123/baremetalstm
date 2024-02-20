# Board: STM-32F466

## Goal:

- Learn more about Baremetal programming and microcontrollers
- I'm following these tutorials to first get the LED to blink and then make a simple UART
- I want to try and implement the important peripherals on my own and learn as much as I can!

## Links to Tutorials:

- [Bare Metal Programming Guide Github](https://github.com/cpq/bare-metal-programming-guide?tab=readme-ov-file)
- [Low Byte Productions](https://www.youtube.com/watch?v=06ICtJjPKlA&list=PLP29wDx6QmW7HaCrRydOnxcy8QmW0SNdQ&index=2)

### Future note to self (Mistakes I'm making along the way and how I solved them):

- To run ST-link:
  - in the blink folder first run pyocd gdb (this starts the gdb server)
  - in a seperate terminal, type: `arm-none-eabi-gdb firmware.elf`
  - in the gdb prompt type:
    - `(gdb) target remote localhost:3333`
    - `(gdb) load`
    - `(gdb) monitor reset`
- Make sure the linker file(that may or may not be copied) is accurate in terms of Flash Memory length, and SRAM memory length. 
- It is probable that the Macros don't work as expected sometimes, turn macros into functions, they don't mess up stuff too much

