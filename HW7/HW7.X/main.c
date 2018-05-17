#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "config.h"
#include <stdio.h>
#include "i2c_master_noint.h"
#include"ST7735.h"


#define SLAVE_ADDR 0x6B  //0b 110 1011
#define OUT_TEMP_L 

void initIMU();
void I2C_read_multiple(unsigned char address, unsigned char register, unsigned char * data, int length);


int main() {
// some initialization function to set the right speed setting
  __builtin_disable_interrupts();
  /*
  TRISAbits.TRISA4 = 0; // set LED an output pin
  TRISBbits.TRISB4 = 1; // set push button an input pin
  LATAbits.LATA4 = 0; // turn LED off
  */
  initIMU();
  __builtin_enable_interrupts();
  char level;
  while(1) {

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
void I2C_read_multiple(unsigned char address, unsigned char register, unsigned char * data, int length){

}
