#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <u8g2.h>

#include "sdkconfig.h"
#include "u8g2_esp32_hal.h"

#include "heltec_wifi_oled_esp32_SSD1306_i2c.h"

// SDA - GPIO04
#define PIN_SDA (4)

// SCL - GPIO15
#define PIN_SCL (15)

// RESET - GPIO16
#define PIN_RST (16)



//static const char *TAG = "ssd1306";

u8g2_esp32_hal_t u8g2_esp32_hal; // the esp32 hal
u8g2_t           u8g2;  // a structure which will contain all the data for one display

void gfx_init() {
    // initialise ESP32 hal
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    u8g2_esp32_hal.sda   = PIN_SDA;
    u8g2_esp32_hal.scl  = PIN_SCL;
    u8g2_esp32_hal.reset= PIN_RST;
    u8g2_esp32_hal_init(u8g2_esp32_hal);

    // initialise gfx subsystem
    u8g2_Setup_ssd1306_i2c_128x64_noname_f( // guess
        &u8g2,
        U8G2_R0, 
        // u8x8_byte_sw_i2c, 
        u8g2_esp32_i2c_byte_cb,
        u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure

    // initialise i2c
    u8x8_SetI2CAddress(&u8g2.u8x8,0x78);
    // send init sequence to the display, display is in sleep mode after this,
    u8g2_InitDisplay(&u8g2); 
    // clear and wake up display
    gfx_clear();
}

void gfx_clear()
{
    gfx_enable();
    u8g2_ClearBuffer(&u8g2); // clear buffer
}

void gfx_enable()
{
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
}

void gfx_disable()
{
    u8g2_SetPowerSave(&u8g2, 1); // put display to sleep
}

void gfx_flip_mode(uint8_t mode)
{
    u8g2_SetFlipMode(&u8g2, mode);
}

void gfx_contrast(uint8_t value)
{
    // u8g2_SetContrast(value);
}

/*
void gfx_display_rotation(const u8g2_cb_t *u8g2_cb)
{
    u8g2_SetDisplayRotation(&u8g2, u8g2_cb);
}
*/

uint16_t gfx_height()
{
    return u8g2_GetDisplayHeight(&u8g2);
}

uint16_t gfx_width()
{
    return u8g2_GetDisplayWidth(&u8g2);
}



void gfx_printf(char *format, ...) {
    va_list args;
    char *str;

    va_start (args, format);
    int n = vasprintf(&str, format, args);
    va_end (args);

    // error? then bail early
    if(n<=0)
        return;

    // now that we have a 'printf' let's get it out to the oled.
    u8g2_SetFont(&u8g2, u8g_font_6x13 );

    u8g2_DrawStr(&u8g2, 2,17, str);
    u8g2_SendBuffer(&u8g2);
    free(str);
}

void gfx_bargraph(int value, int max)
{
    float factor = ((float) max) / 100.0;
    float scaled = value / factor;
    uint16_t amount = 10 + (uint16_t) scaled;

    u8g2_DrawBox(  &u8g2, 0, 26, amount,6);
    u8g2_DrawFrame(&u8g2, 0, 26, 110   ,6);
}

