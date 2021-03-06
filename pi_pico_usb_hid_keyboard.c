/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Brendon Tower
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "tusb.h"
#include "usb_descriptors.h"


//--------------------------------------------------------------------+
// KEY DEFINITIONS
//--------------------------------------------------------------------+

struct keyCombo{
    int m_gpio;
    uint8_t m_keycode;
};

#define maxKeypress 6
#define numGPIO 26
struct keyCombo buttons[numGPIO] = {{ 0,    HID_KEY_A},
                                    { 1,    HID_KEY_B},
                                    { 2,    HID_KEY_C},
                                    { 3,    HID_KEY_D},
                                    { 4,    HID_KEY_E},
                                    { 5,    HID_KEY_F},
                                    { 6,    HID_KEY_G},
                                    { 7,    HID_KEY_H},
                                    { 8,    HID_KEY_I},
                                    { 9,    HID_KEY_J},
                                    { 10,   HID_KEY_K},
                                    { 11,   HID_KEY_L},
                                    { 12,   HID_KEY_M},
                                    { 13,   HID_KEY_N},
                                    { 14,   HID_KEY_O},
                                    { 15,   HID_KEY_P}, // TODO - investigate note on adafruit site, replace with 25 and remove LED otherwise
                                    { 16,   HID_KEY_Q},
                                    { 17,   HID_KEY_R},
                                    { 18,   HID_KEY_S},
                                    { 19,   HID_KEY_T},
                                    { 20,   HID_KEY_U},
                                    { 21,   HID_KEY_V},
                                    { 22,   HID_KEY_W},
                                    { 26,   HID_KEY_X},
                                    { 27,   HID_KEY_Y},
                                    { 28,   HID_KEY_Z} };

//--------------------------------------------------------------------+
// PROTOTYPES
//--------------------------------------------------------------------+

void hid_task(void);
void input_task(void);


//--------------------------------------------------------------------+
// MAIN
//--------------------------------------------------------------------+

int main() {
    // initialize the board
    board_init();

    // initialize tiny usb
    tusb_init();

    // enable gpio/pullup resistors for all input, button will pull input to ground
    for(int i = 0;i < numGPIO;++i)
    {
        gpio_set_function(buttons[i].m_gpio, GPIO_FUNC_PIO0);
        gpio_pull_up(buttons[i].m_gpio);
    }

    // forever
    while (1)
    {
        // handle gpio input, sending keycodes
        input_task();

        // tinyusb device task
        tud_task();

        // handle USB specifics
        hid_task();
    }

    return 0;
}

//--------------------------------------------------------------------+
// INPUT TASK
//--------------------------------------------------------------------+

void input_task(void)
{
    // cap at 6 simultaneous keys
    // TODO - determine actual cap
    uint8_t keycode[maxKeypress] = {0};
    int keycodeIndex = 0;

    // check if any keys are depressed, stack as necessary
    for(int i = 0;i < numGPIO;++i)
    {
        if(!gpio_get(buttons[i].m_gpio)) {
            keycode[keycodeIndex++] = buttons[i].m_keycode;

            if(keycodeIndex >= maxKeypress) {
                break;
            }
        }
    }

    // if any keys are pressed, send them
    if(keycodeIndex > 0)
    {
        if(tud_hid_ready())
        {
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
            board_led_write(true);
        }
    }
    else
    {
        if(tud_hid_ready())
        {
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
            board_led_write(false);
        }
    }
}


//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

void hid_task(void) {
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    // Remote wakeup
    if (tud_suspended()) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }
}


//--------------------------------------------------------------------+
// DEVICE CALLBACKS
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    // TODO not Implemented
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    // TODO set LED based on CAPLOCK, NUMLOCK etc...
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;
}
