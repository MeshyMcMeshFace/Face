#include "ui.h"

#include <Arduino.h>
#include <U8g2lib.h>

ui_display_t ui_display_data;


//U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(UI_OLED_CLK, UI_OLED_DATA, UI_OLED_RESET);

//U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, 13, 11, 10, 8);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0,UI_OLED_CLK, UI_OLED_DATA, UI_OLED_RESET);

void ui_clear()
{
    for(int i=0;i<5;i++)
        memset(ui_display_data.cells[i],0,21);
    ui_display_data.battery.active =0;
    ui_display_data.bluetooth.active =0;
    ui_display_data.signal.active =0;
    ui_display_data.wifi.active =0;
    ui_display_data.line=0;
    ui_display_data.dirty=1;
}

void ui_diagnostics()
{
    int led =1;
    srand(millis());
    for(int i=0;i<5;i++)
        ui_println();

    for(int i=0;i<100;i++)
    {
        int q = rand() % 94;
        ui_display_data.cells[i / 20][i % 20] = q + 32;
        ui_display();
        digitalWrite(25,led); led = !led;
    }
    delay(250);
    ui_clear();
    ui_display();
    delay(250);
    for(int k=0;k<10;k++)
    {
        for(int i=0;i<5;i++)
            memset(ui_display_data.cells[i],57-k,20);
        ui_display();
        digitalWrite(25,led); led = !led;
        delay(500);
    }
    ui_clear();

    ui_icon_t* icons[] { 
        &ui_display_data.battery, 
        &ui_display_data.bluetooth, 
        &ui_display_data.signal,
        &ui_display_data.wifi 
        };
    for(int i = 0;i<4;i++)
    {
        ui_clear();
        icons[i]->w = 32;
        icons[i]->h = 16;
        icons[i]->x = 64 - (icons[i]->w/2);
        icons[i]->y = 32 - (icons[i]->h/2);
        icons[i]->active = true;
        for(int r=0;r<5;r++)
        {
            for(float k=0;k<=1;k+=0.1)
            {
                digitalWrite(25,led); led = !led;
                icons[i]->value = k;
                ui_display();
            }
            delay(100);
        }
    }
    delay(500);
}

void ui_init()
{
    Serial.println("UI init");
    u8g2.begin();

    //ui_diagnostics();
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

void ui_bluetooth(int x,int y, int w, int h, float value)
{
    if (value == 0)
        return;
    ui_display_data.dirty=true;
    for(int i=1;i <6;i++)
    {
        u8g2.drawLine(
            x + ui_bt_draw[i-1].x * (float)w,y + ui_bt_draw[i-1].y * (float)h,
            x + ui_bt_draw[i].x   * (float)w,y + ui_bt_draw[i].y   * (float)h);
    }
}
void ui_bluetooth()
{
    if(!ui_display_data.bluetooth.active)
        return;

    ui_bluetooth(
        ui_display_data.bluetooth.x,
        ui_display_data.bluetooth.y,
        ui_display_data.bluetooth.w,
        ui_display_data.bluetooth.h,
        ui_display_data.bluetooth.value
        );
}
void ui_draw_arc_calc(float x, float y, float r, float d, float *X, float *Y)
{
    *X=x+(r * cos(d));
    *Y=y+(r * sin(d));
}
void ui_draw_arc(float x, float y, float radius, float start, float end, float step = 1)
{
    bool wr = false;
    for(float deg = 0.1;deg<360;deg+=step)
    {
        float XX,YY;
        if(deg >=start && deg <=end)
        {
            float X,Y;
            ui_draw_arc_calc(x,y,radius, DEG_TO_RAD * deg, &X,&Y);
            if(wr)
            {
                u8g2.drawLine(XX,YY,X,Y);
            } else {
                wr = true;
            }
            XX = X;
            YY = Y;
        }
    }
}
void ui_wifi(int x, int y, int w, int h, float percent)
{
    ui_display_data.dirty=true;

    // 5 second filling animation
    if(percent <0 || percent > 1)
        percent = (millis() % 5000) / 5000.0;

    int cx = x+ w/2; 
    float r = max(w,h)/2;
    for(float i=2;i<r;i+=4)
    {
        if( (percent * r) >= (float)i )
            ui_draw_arc(cx,y, i,225, 315);
    }
    //Serial.printf("wiffy %f -> %f\n",percent,percent * H);
}
void ui_wifi()
{
    if(!ui_display_data.wifi.active)
        return;

    ui_wifi(
        ui_display_data.wifi.x,
        ui_display_data.wifi.y,
        ui_display_data.wifi.w,
        ui_display_data.wifi.h,
        ui_display_data.wifi.value
        );
}

void ui_signal(int x, int y, int w, int h, float percent)
{
    ui_display_data.dirty=true;

    // 5 second filling animation
    if(percent <0 || percent > 1)
        percent = (millis() % 5000) / 5000.0;

    float value = percent *h;
    for(int i=1; i<w;i+=2)
    {
        int xx = x+i;
        float pw = (float)i/(float)w;
        if(percent > pw ) {
           u8g2.drawLine(xx, y+h, xx, y+h - (h*pw) );
           //Serial.printf("%d: percent: %f percent-width: %f\n",i,percent,pw);
        }
        else {
          u8g2.drawLine(xx, y+h, xx+1, y+h);
        }
    }
}
void ui_signal()
{
    if(!ui_display_data.signal.active)
        return;

    ui_signal(
        ui_display_data.signal.x,
        ui_display_data.signal.y,
        ui_display_data.signal.w,
        ui_display_data.signal.h,
        ui_display_data.signal.value
        );
}

void ui_battery(int x,int y, int w, int h, float percent)
{
    ui_display_data.dirty=true;

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
void ui_battery()
{
    if(!ui_display_data.battery.active)
        return;

    ui_battery(
        ui_display_data.battery.x,
        ui_display_data.battery.y,
        ui_display_data.battery.w,
        ui_display_data.battery.h,
        ui_display_data.battery.value
        );
}

void ui_display()
{   
    if(!ui_display_data.dirty)
        return;
    
    ui_begin();
    u8g2.setFont(u8g2_font_profont11_mf);
    for(int i=0; i< 5; i++)
    {
        ui_display_data.cells[i][20] = 0;
        u8g2.drawStr(0,11*(i+1),ui_display_data.cells[i]);
    }

    //u8g2.setFont(u8g2_font_chroma48medium8_8r);
    //u8g2.drawStr(0,64,">ALL YOUR BASE!<");
    //possible status?...^^^^^^^^^^^^^^^^
 
    ui_signal();
    ui_battery();
    ui_bluetooth();
    ui_wifi();
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

void ui_println(const char *str,int pause, bool serial_dbg)
{
    ui_display_data.dirty=true;

    if(ui_display_data.line>=5)
    {
        for(int i=1;i<5;i++)
        {
            memcpy(ui_display_data.cells[i-1],ui_display_data.cells[i],20);
        }
        memset(ui_display_data.cells[4],0,21);
    }
    int line = ui_display_data.line;
    
    if(line > 4)
        line = 4;
    
    snprintf(ui_display_data.cells[line],20,"%s",str);
    
    ui_display_data.line ++;

    if(serial_dbg)
        Serial.println(str);
    
    if(pause)
        delay(pause);

    ui_display();
}

void ui_printf(const char *fmt, ...)
{
    va_list args;
    char p[21];
    memset(p,0,21);

    va_start(args,fmt);
    vsnprintf(p,20,fmt,args);
    va_end(args);
    
    ui_println(p);
}