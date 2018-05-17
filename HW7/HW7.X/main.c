#include<xc.h>					 // processor SFR definitions
#include<sys/attribs.h>	// __ISR macro
#include "config.h"
#include <stdio.h>
#include "i2c_master_noint.h"
#include"ST7735.h"


#define SLAVE_ADDR 0x6B	//0b 110 1011
#define OUT_TEMP_L 0x20 

void initIMU();
void I2C_read_multiple(unsigned char address, unsigned char reg, unsigned char * data, int length);


int main() {
	// some initialization function to set the right speed setting
	int length = 14;
	char data[length-1];
	char message[100];
	__builtin_disable_interrupts();
	TRISAbits.TRISA4 = 0; // set LED an output pin
	TRISBbits.TRISB4 = 1; // set push button an input pin

	initIMU();
    SPI1_init();
    LCD_init();
	__builtin_enable_interrupts();
    LCD_clearScreen(BACKGROUND);
    
    sprintf(data,"Default");
	I2C_read_multiple(SLAVE_ADDR, OUT_TEMP_L, data, length);
	while(1) {
		sprintf(message,"Hello World! ");
		LCD_drawString(28,32,message,BLUE);
		LCD_drawString(28,48,data,RED);
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
	i2c_master_send(0x11); // write to CTRL2_G
	i2c_master_send(0x88); // 0b 1000 10 0 0: 1.66 kHz, with 1000 dps sensitivity.
	i2c_master_send(0x12); // write to CTRL3_C
	i2c_master_send(0x04); // 0b 0000 0100: Enable IF_INC
	i2c_master_stop(); // make the stop bit
}

void I2C_read_multiple(unsigned char address, unsigned char reg, unsigned char * data, int length){
	i2c_master_start(); // make the start bit
	i2c_master_send(address<<1|0); // 0 indicate writing
	i2c_master_send(reg); // send the register address of OUT_TEMP_L 
	i2c_master_restart(); // make the restart bit
	i2c_master_send(address<<1|1); // 1 indicate reading
    int i;
	for(i=0;i<length;i++){
	*data = i2c_master_recv();
	i2c_master_ack(0);
	data++;
	}
	i2c_master_ack(1); // make the ack so the slave knows we got it
	i2c_master_stop(); // make the stop bit
}
