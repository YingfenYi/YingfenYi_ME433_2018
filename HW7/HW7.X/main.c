#include<xc.h>					 // processor SFR definitions
#include<sys/attribs.h>	// __ISR macro
#include "config.h"
#include <stdio.h>
#include <math.h>
#include "i2c_master_noint.h"
#include"ST7735.h"


#define SLAVE_ADDR 0x6B	//0b 110 1011
#define OUT_TEMP_L 0x20 
#define WHO_AM_I 0x0F
#define HEIGHT 8

#define BarLength 100

void initIMU();
void I2C_read_multiple(unsigned char address, unsigned char reg, char * data, int length);
void Transform(char * data, short * dtrans, int length);
void LCD_drawBardata(short hor, short ver);

int main() {
	// some initialization function to set the right speed setting
	int length = 14;
	char data[length];
	short dtrans[length/2];//transformed data

	char message[100];
	__builtin_disable_interrupts();
	TRISAbits.TRISA4 = 0; // set LED an output pin
    LATAbits.LATA4 = 1;     // initialize LED high
	TRISBbits.TRISB4 = 1; // set push button an input pin

	initIMU();
    SPI1_init();
    LCD_init();
	__builtin_enable_interrupts();
    LCD_clearScreen(BACKGROUND);
    LCD_drawBarv(62,5,0,YELLOW,BLUE,150);//vertical bar background 
    LCD_drawBarh(5,78,0,YELLOW,BLUE,118);//horizontal bar background 

    //unsigned short tmp[]={0xFFFF,0x1111};
    signed short Ax, Az;
    
	while(1) {
    	_CP0_SET_COUNT(0);  // Reset the timer
        LATAINV = 0x10;     // turn off/on LED

		I2C_read_multiple(SLAVE_ADDR, OUT_TEMP_L, data, length);
		Transform(data, dtrans, length/2);

		sprintf(message,"Gx:%hd",dtrans[1]);
		LCD_drawString(5,10,message,RED);
		sprintf(message,"Gy:%hd",dtrans[3]);
		LCD_drawString(5,10 + HEIGHT,message,RED);

/*
		signed short Ax = dtrans[4];
		signed short Ay = dtrans[5];
		sprintf(message,"Ax:%d",Ax);
		LCD_drawString(5,10 + 2*HEIGHT,message,RED);
		sprintf(message,"Ay:%d",Ay);
		LCD_drawString(5,10 + 3*HEIGHT,message,RED);
		LCD_drawBardata(Ax,Ay);
		*/
		//if(dtrans[4]*Ax>=0 | abs(Ax)<500)
			Ax = dtrans[4];
		// prevent fluctuation, otherwise not change Ax
		//if(dtrans[6]*Az>=0 | abs(Az)<500)
			Az = dtrans[6];

		sprintf(message,"Ax:%d",Ax);
		LCD_drawString(5,10 + 2*HEIGHT,message,RED);
		sprintf(message,"Ay:%d",Az);
		LCD_drawString(5,10 + 3*HEIGHT,message,RED);
		LCD_drawBardata(Ax,Az);

		//sprintf(message,"Az:%d",p);
		//LCD_drawString(5,10 + 4*HEIGHT,message,RED);
		
        while(_CP0_GET_COUNT() < 4800000){;}  // 24MHz/5Hz = 4800000
	}
	return 0;
}

void initIMU(){
	ANSELBbits.ANSB2 = 0;
	ANSELBbits.ANSB3 = 0;
	i2c_master_setup();

	i2c_master_start(); // make the start bit
	i2c_master_send(SLAVE_ADDR<<1|0); // 0 indicate writing
	i2c_master_send(0x10); // write to CTRL1_XL
	i2c_master_send(0x82); // 0b 1000 00 10: 1.66 kHz, with 2g sensitivity, and 100 Hz filter.
	i2c_master_stop(); // make the stop bit

    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1);   // R/W = 0 = write
	i2c_master_send(0x11); // write to CTRL2_G
	i2c_master_send(0x88); // 0b 1000 10 0 0: 1.66 kHz, with 1000 dps sensitivity.
	i2c_master_stop(); // make the stop bit

    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1);   // R/W = 0 = write
	i2c_master_send(0x12); // write to CTRL3_C
	i2c_master_send(0x04); // 0b 0000 0100: Enable IF_INC
	i2c_master_stop(); // make the stop bit
}

void I2C_read_multiple(unsigned char address, unsigned char reg, char * data, int length){
	i2c_master_start(); // make the start bit
	i2c_master_send(address<<1|0); // 0 indicate writing
	i2c_master_send(reg); // send the register address of OUT_TEMP_L 
	i2c_master_restart(); // make the restart bit
	i2c_master_send(address<<1|1); // 1 indicate reading
    int i;
	for(i=0;i<length;i++){
		data[i] = i2c_master_recv();
		if(i!=length-1)
			i2c_master_ack(0);
	}
	i2c_master_ack(1); // make the ack so the slave knows we got it
	i2c_master_stop(); // make the stop bit
}
//8-bit char to 16-bit short
void Transform(char * data, short * dtrans, int length){
	int i;
	for(i=0;i<length;i++){
		//dtrans[i] = data[2*i+1]<<8 + data[2*i] & 0x00FF;

		dtrans[i] = data[2*i+1]<<8 | data[2*i];
	}
}

void LCD_drawBardata(short hor, short ver){
	float p1 = (float)hor/150;
	float p2 = (float)ver/150;
	if(hor>=0){
    	LCD_drawBarh(64,78,(int)p1,YELLOW,BLUE,59);//horizontal bar background 
    	LCD_drawBarh(5,78,0,YELLOW,BLUE,59);//clear another bar
	}
	else{
    	LCD_drawBarh(5,78,(int)(100+p1),BLUE,YELLOW,59);//horizontal bar background 
    	LCD_drawBarh(64,78,0,YELLOW,BLUE,59);//clear another bar
    }


	if(ver>=0){
    	LCD_drawBarv(62,5,(int)(100-p2),BLUE,YELLOW,75);//vertical bar background 
    	LCD_drawBarv(62,80,0,YELLOW,BLUE,75);//clear another bar
	}
	else{
    	LCD_drawBarv(62,80,(int)(-p2),YELLOW,BLUE,75);//vertical bar background 
    	LCD_drawBarv(62,5,0,YELLOW,BLUE,75);//clear another bar
    }

    //return (int)p1;
}