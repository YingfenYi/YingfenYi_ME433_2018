/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/
#define SLAVE_ADDR 0x6B	//0b 110 1011
#define OUT_TEMP_L 0x20 
#define WHO_AM_I 0x0F
#define HEIGHT 8
#define BarLength 100
#define ARRLEN 14

APP_DATA appData;

char data[ARRLEN];
char message[100];
//unsigned short tmp[]={0xFFFF,0x1111};
float Ax, Az, Ax1, Az1;

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
void LCD_drawBardata(float hor, float ver){
	float p1 = hor;
	float p2 = ver;
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

}

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    
	// some initialization function to set the right speed setting

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

}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {

            _CP0_SET_COUNT(0);  // Reset the timer
            LATAINV = 0x10;     // turn off/on LED

            I2C_read_multiple(SLAVE_ADDR, OUT_TEMP_L, data, ARRLEN);

            Ax1 = 100*(data[9]<<8 | data[8])*(2.0/32757.0);
            Az1 = 100*(data[13]<<8 | data[12])*(2.0/32757.0);
            // prevent fluctuation, otherwise not change Ax
            if(Ax1*Ax>=0 || abs(Ax)<=20)
                Ax = Ax1;
            else
                Ax = Ax-0.5;
            if(Az1*Az>=0 || abs(Az)<=20)
                Az = Az1;
            else
                Az = Az-0.5;

            sprintf(message,"Ax:%1.3f",Ax);
            LCD_drawString(5,10 + 2*HEIGHT,message,RED);
            sprintf(message,"Ay:%1.3f",Az);
            LCD_drawString(5,10 + 3*HEIGHT,message,RED);
            LCD_drawBardata(Ax,Az);

            while(_CP0_GET_COUNT() < 1200000){;}  // 24MHz/20Hz = 1200000
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
