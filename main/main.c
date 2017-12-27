#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "heltec_wifi_oled_esp32_SSD1306_i2c.h"

#include "lora.h"

void app_main()
{
    gfx_init();
    //gfx_printf("Hello World!");

    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Initialising LoRa...\n");
    lora_init();
    printf("Begin Lora.... 869.500MHz\n");
    int result = lora_begin(8695E5);
    if(result == 0)
    {
        while(true)
        {
            printf("FAILED TO INITIALISE RADIO\n");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
    printf("Dump registers.\n");
    lora_dumpRegisters();
    printf("\n\n\n");
    printf("Sending First Packet\n");
    lora_beginPacket_default();
    lora_printf("HELLO");
    lora_endPacket();

    //printf("Send initial message:\n");
    //lora_printf("This is a test. This is a test.\n");

    for (int i = 100; i >= 0; i--) {
        printf("Restarting in %d ...\n", i);
        gfx_clear();
        gfx_printf("Rebooting in: %04d.\n",i);
        lora_beginPacket_default();
        lora_printf("Countdown %04d.\n");
        lora_endPacket();
        gfx_bargraph(10-i,10);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    lora_end();
    esp_restart();
}
