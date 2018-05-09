#ifndef SPI1__H__
#define SPI1__H__

#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro

// Demonstrates spi by accessing external ram
// PIC is the master, ram is the slave
// Uses microchip 23K256 ram chip (see the data sheet for protocol details)
// SDO1 -> SI (pin A1 -> pin 5)
// SDI1 -> SO (pin B8 -> /)(we donot use it)
// SCK1 -> SCK (pin B14 -> pin 4)
// SS1 -> CS (pin A0 -> pin 3)
// Additional MCP4912 connections
// Vdd (Pin 1), VrefA(13), VrefB(11) -> 3.3V
// LDAC (Pin 8), Vss (Pin 12) -> ground
// VoutA(14), VoutB(10) -> output 
#define CS LATAbits.LATA0       // chip select pin
#define CHANNELA 0         
#define CHANNELB 1
#define PI 3.1415926535898

unsigned char SPI1_IO(unsigned char ch); // send a byte via spi and return the response
void SPI1_init(); // initialize SPI1
void setVoltage(char channel, unsigned short voltage); // set channel and voltage
    
#endif