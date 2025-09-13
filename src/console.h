#ifndef CONSOLE_H
#define CONSOLE_H
void console_clear(void);
void console_putc(int ch);
int  putc(int ch);  // adapter for esp_printf (int (*)(int))
#endif
