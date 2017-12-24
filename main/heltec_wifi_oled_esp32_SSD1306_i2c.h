void gfx_init();
void gfx_clear();
void gfx_enable();
void gfx_disable();
void gfx_flip_mode(uint8_t mode);
void gfx_contrast(uint8_t value);
//void gfx_display_rotation(const u8g2_cb_t *u8g2_cb);
void gfx_printf(char *format, ...);
void gfx_bargraph(int value, int max);

