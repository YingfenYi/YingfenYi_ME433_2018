#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "config.h"
#include <stdio.h>
#include"ST7735.h"


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
		    sprintf(message,"Hello World %03d! ",i);
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