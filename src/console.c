#include <stdint.h>
#include "console.h"

#define VGA_MEM      ((volatile uint8_t*)0xB8000)
#define VGA_COLS     80
#define VGA_ROWS     25
#define ATTR_DEFAULT 0x07  // light grey on black

static int cursor = 0;     // 0..(80*25-1)

static void scroll(void){

    if (cursor < VGA_COLS * VGA_ROWS){
	return;
    }else 
	for(int row = 1; row < VGA_ROWS; row++){
		for (int cols = 0; cols < VGA_COLS; cols++){
	            VGA_MEM[(row - 1) * VGA_COLS * 2 + cols] = VGA_MEM[row * VGA_COLS * 2 + cols];
		}

	}
	for (int col = 0; col < VGA_COLS; col++) {
            int off = (VGA_ROWS - 1) * VGA_COLS * 2 + col * 2;
            VGA_MEM[off]     = ' ';
            VGA_MEM[off + 1] = ATTR_DEFAULT;
        }
        cursor = (VGA_ROWS - 1) * VGA_COLS;


}



static void newline(void) { 

    cursor = (cursor / VGA_COLS + 1) * VGA_COLS; 
    scroll(); 

}


void console_putc(int ch){


    if (ch == '\n'){
        newLine();
	   return;
    }
    if (ch == '\r'){
        return;
    }

    int off = cursor * 2;
    VGA_MEM[off] = (uint8_t)ch;
    VGA_MEM[off + 1] = ATTR_DEFAULT;
    cursor++;
    scroll();


}


int putc(int ch){

    console_putc(ch);
    return ch;
}


