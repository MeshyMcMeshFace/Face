#ifndef FACE_UI_H
#define FACE_UI_H

#define UI_OLED_CLK   (15)
#define UI_OLED_DATA  (4)
#define UI_OLED_RESET (16)

typedef struct ui_icon_s {int x, y,w,h; bool active; float value;} ui_icon_t;
typedef struct ui_display_s {
    char cells[5][21]; // 5x20 (plus extra char for null);
    ui_icon_t battery;
    ui_icon_t bluetooth;
    ui_icon_t signal;
    ui_icon_t wifi;
    int line;
    bool dirty;
} ui_display_t;

extern ui_display_t ui_display_data;

void ui_diagnostics();
void ui_init();
void ui_display();
void ui_fault(const char *str);
void ui_println(const char *str="",int pause=1000,bool serial_dbg=true);
void ui_printf(const char *fmt, ...);


#endif 