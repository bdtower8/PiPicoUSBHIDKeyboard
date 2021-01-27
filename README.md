# PiPicoUSBHIDKeyboard

A bare minimum example of how to use a Raspberry Pi Pico as a USB-HID keyboard.

Tests were performed with an irq callback approach as well as incorporating debounce timers. There's probably plenty of other ways to approach this problem as well. Based on experiments this path produces the expected behavior with the easiest-to-read code. 
