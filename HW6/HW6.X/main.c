#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "config.h"
#include <stdio.h>
#include"ST7735.h"

#define BarLength 100
//#define FOREGROUND 0xFF0F//set foreground color
#define BACKGROUND 0x0000//set background color

void LCD_drawChar(unsigned short x, unsigned short y, unsigned char ch, unsigned short color);
void LCD_drawString(unsigned short x, unsigned short y, unsigned char ch[], unsigned short color);
void LCD_drawBar(unsigned short x, unsigned short y, unsigned char progress, unsigned short color);

int main() {
    //__builtin_disable_interrupts();
    SPI1_init();
    LCD_init();
    //__builtin_enable_interrupts();

    LCD_clearScreen(BACKGROUND);
    char message[100];
    int i;
    float FPS;
    while(1){
    	/*
    	//for test
        _CP0_SET_COUNT(0);  // Reset the timer
    	LCD_clearScreen(0xF800);
        while(_CP0_GET_COUNT() < 48000000){;}  // 24MHz/0.5Hz = 48000000
        _CP0_SET_COUNT(0);  // Reset the timer
    	LCD_clearScreen(0x7E0);
        while(_CP0_GET_COUNT() < 48000000){;}  // 24MHz/0.5Hz = 48000000
        _CP0_SET_COUNT(0);  // Reset the timer
    	LCD_clearScreen(0x1F);
        while(_CP0_GET_COUNT() < 48000000){;}  // 24MHz/0.5Hz = 48000000
        */
        for(i=0;i<=100;i++){
        	_CP0_SET_COUNT(0);  // Reset the timer
		    sprintf(message,"Hello World %d! ",i);
		    LCD_drawString(28,32,message,BLUE);
            LCD_drawBar(14,48,i,YELLOW);

            FPS = 24000000/_CP0_GET_COUNT(); //FPS
            sprintf(message,"FPS: %1.2f",FPS);
            LCD_drawString(14,80,message,BLUE);

        	while(_CP0_GET_COUNT() < 2400000){;}  // 24MHz/10Hz = 2400000
        }
        //LCD_clearScreen(BACKGROUND);
    }
    return 0;
}

void LCD_drawChar(unsigned short x, unsigned short y, unsigned char ch, unsigned short color){
// x, y is the top-left of the character
	int i,j;
	if((x<_GRAMWIDTH-5)&&(y<_GRAMWIDTH-8)){//no out of boundary
		for (i = 0; i < 5; i++){
			for (j = 0; j < 8; j++){
				if (((ASCII[ch-0x20][i]>>j) & 0x01))
					LCD_drawPixel(x+i,y+j,color);
                else
                    LCD_drawPixel(x+i,y+j,BACKGROUND);
			}
		}
	}
}

void LCD_drawString(unsigned short x, unsigned short y, unsigned char ch[], unsigned short color){
	int i=0;
	while(1){
		if(ch[i]=='\0'){
			break;
		}
		LCD_drawChar(x+6*i,y,ch[i],color);
		i++;
	}
}

void LCD_drawBar(unsigned short x, unsigned short y, unsigned char progress, unsigned short color){
    int i,j;
    for(i=0;i<progress;i++){
        for(j=0;j<4;j++){
            LCD_drawPixel(x+i,y+j,color);
        }
    }
    for(i=progress;i<BarLength;i++){
        for(j=0;j<4;j++){
            LCD_drawPixel(x+i,y+j,BACKGROUND);
        }
    }
}
