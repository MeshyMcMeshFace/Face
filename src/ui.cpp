#include "ui.h"

#include <U8g2lib.h>

void ui_bars(int x, int y, float percent);


//U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(UI_OLED_CLK, UI_OLED_DATA, UI_OLED_RESET);

//U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, 13, 11, 10, 8);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0,UI_OLED_CLK, UI_OLED_DATA, UI_OLED_RESET);

void ui_init()
{
    Serial.println("UI init");
    u8g2.begin();
}

void ui_begin()
{
    u8g2.clearBuffer();
}

void ui_end()
{
    u8g2.sendBuffer();
}

typedef struct point { float x, y; } Point;

static Point ui_bt_draw[] = { 
    { 0.25,0.75 }, // F
    { 0.75,0.25 }, // C
    { 0.5, 0.0 },  // A
    { 0.5, 1.0 },  // B
    { 0.75,0.75},  // D
    { 0.25,0.25},  // E
    }; 

void ui_btle(int x,int y, int w, int h, bool on)
{
    if (!on)
        return;
    for(int i=1;i <6;i++)
    {
        u8g2.drawLine(
            x + ui_bt_draw[i-1].x * (float)w,y + ui_bt_draw[i-1].y * (float)h,
            x + ui_bt_draw[i].x   * (float)w,y + ui_bt_draw[i].y   * (float)h);
    }
}
/* 128 x 64 */
void ui_signal(int x, int y, int w, int h, float percent)
{
    // 5 second filling animation
    if(percent <0 || percent > 1)
        percent = (millis() % 5000) / 5000.0;

    float value = percent *h;
    for(int i=1; i<w;i+=2)
    {
        int xx = x+i;
        float pw = (float)i/(float)w;
        if(percent > pw ) {
           u8g2.drawLine(xx, y, xx, y - (h*pw) );
           //Serial.printf("%d: percent: %f percent-width: %f\n",i,percent,pw);
        }
        else {
          u8g2.drawLine(xx, y, xx+1, y);
        }
    }
}


void ui_battery(int x,int y, int w, int h, float percent)
{
    // 5 second filling animation
    if(percent <0 || percent > 1)
        percent = (millis() % 5000) / 5000.0;
    // outline
    u8g2.drawFrame(x,y,w,h);
    // battery anode to the right
    int top = y + (h/4);
    u8g2.drawFrame(x+w,top,2,h/2);
    // filled in center
    float p = (w-4) * percent;
    for(int i=2;i<(h-2);i++) 
        u8g2.drawLine(x+2,y+i,x+2+p,y+i);
}


/* 8x8 char = 16 x 8 */

void ui_display()
{
    ui_begin();
    u8g2.setFont(u8g2_font_profont11_mf);
    //                 01234567890abcdef0123
    u8g2.drawStr(0,11," >Heltec OLED LoRa<");

    char buffer[32];
    memset(buffer,0,32);
    snprintf(buffer,32,"t=%04dms",millis()%10000);
    u8g2.drawStr(64,44,buffer);
    //Serial.println(buffer);
    //u8g2.drawStr(0,33,"ESP32 | Mesh Net2____");
    //u8g2.drawStr(0,44,"================3jyg~");
    //u8g2.drawStr(0,55,"0123456789abdcdefghij");

    //u8g2.setFont(u8g2_font_chroma48medium8_8r);
    //u8g2.drawStr(0,64,">ALL YOUR BASE!<");
// ....................^^^^^^^^^^^^^^^^
 
    ui_signal(0,60,16,12,-1);
    
    ui_battery(0,25,16,8, -1);

    ui_btle( 32, 25, 16,16, (millis() % 2500) < 1250);
    // 64-55 = 9
    ui_end();
}

void ui_fault(const char *str)
{
    Serial.println("Guru Meditation Error");
    Serial.println(str);
    Serial.printf("T: %d\n\n\n",millis());
    int x = 0;
    while(true)
    {
        ui_begin();

        char buffer[32];
        memset(buffer,0,32);
        snprintf(buffer,32,"%s",str);

        u8g2.setFont(u8g2_font_profont11_mf);
        //                 01234567890abcdef0123
        u8g2.drawStr(0,11,"Guru Meditation Error");
        u8g2.drawStr(0,22,buffer);

        if(!x)
            u8g2.drawStr(60,40,":-(");

        snprintf(buffer,32,"T: %d",millis());
        u8g2.drawStr(0,64,buffer);
        ui_end();
    
        digitalWrite(25,x);
        delay(1000);
        x = !x;
    }
}
