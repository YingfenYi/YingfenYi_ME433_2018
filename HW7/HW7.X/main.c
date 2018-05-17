#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "config.h"
#include <stdio.h>
#include "i2c_master_noint.h"
#include"ST7735.h"


#define SLAVE_ADDR 0x6B  //0b 110 1011

void initIMU();
//write to the expander
void setIMU(char pin, char level);
//read from the expander
char getExpander();

int main() {
// some initialization function to set the right speed setting
  __builtin_disable_interrupts();
  TRISAbits.TRISA4 = 0; // set LED an output pin
  TRISBbits.TRISB4 = 1; // set push button an input pin
  LATAbits.LATA4 = 0; // turn LED off
  initExpander();
  setExpander(0,1);
  __builtin_enable_interrupts();
  char level;
  while(1) {
    //indicating LED
    while(_CP0_GET_COUNT() < 2400000){;}  // 24MHz/10Hz = 2400000
    LATAINV = 0x10;     // turn off/on LED
    _CP0_SET_COUNT(0);  // Reset the timer
    while(!PORTBbits.RB4){;} // if button is pushed, stop and wait
    //I2C
    level = (getExpander()>>7);
    setExpander(0,!level);
  }
  return 0;
}

void initExpander(){
  ANSELBbits.ANSB2 = 0;
  ANSELBbits.ANSB3 = 0;
  i2c_master_setup();
  i2c_master_start(); // make the start bit
  i2c_master_send(SLAVE_ADDR<<1|0); // 0 indicate writing
  i2c_master_send(0x00); // write to IODIR
  i2c_master_send(0xF0); // GP7-GP4:read; GP3-GP0:write
  i2c_master_stop(); // make the stop bit
}

void setExpander(char pin, char level){
  i2c_master_start(); // make the start bit
  i2c_master_send(SLAVE_ADDR<<1|0); // 0 indicate writing
  i2c_master_send(0x0A); // write to OLAT
  i2c_master_send(level<<pin); // the value to put in the register
  i2c_master_stop(); // make the stop bit
}

char getExpander(){
  i2c_master_start(); // make the start bit
  i2c_master_send(SLAVE_ADDR<<1|0); // 0 indicate writing
  i2c_master_send(0x09); // send the register address of GPIO
  i2c_master_restart(); // make the restart bit
  i2c_master_send(SLAVE_ADDR<<1|1); // 1 indicate reading
  char r = i2c_master_recv(); // save the value returned
  i2c_master_ack(1); // make the ack so the slave knows we got it
  i2c_master_stop(); // make the stop bit
  return r;
}
