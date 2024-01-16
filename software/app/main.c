#include "system.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_timer.h"
#include "altera_avalon_pio_regs.h"
#include "opencores_i2c_regs.h"
#include "opencores_i2c.h"
#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"
#include "alt_types.h"
#include "sys/alt_sys_init.h"
#include <stdio.h>
#include <unistd.h>

#define I2C_add  0x1D
#define X_add_0  0x32
#define X_add_1  0x33
#define Y_add  0x1D
#define Z_add  0x1D
#define SCL_speed 400000

volatile __uint8_t u=0,d=0,c=0;
volatile alt_u32 data=0;
volatile __uint16_t cnt=990;
/*
static void irq_button(void * context, alt_u32 id)
{
	c=cnt/100;
	d=(cnt%100)/10;
	u=cnt%10;
	
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE,u);//data 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_1_BASE,d);//data 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_2_BASE,c);//data
	
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_1_BASE, 0x01);
}*/

void irq_timer()
{
	//write mode
	if(I2C_start(OPENCORES_I2C_0_BASE,I2C_add , 0)==I2C_NOACK)
		alt_printf("Not Ack\n\r");
	else
		alt_printf("Ack\n\r");
	
	I2C_write(OPENCORES_I2C_0_BASE,X_add_0, 0);
	
	//read mode
	if(I2C_start(OPENCORES_I2C_0_BASE,I2C_add , 1)==I2C_NOACK)
		alt_printf("Not Ack\n\r");
	else
		alt_printf("Ack\n\r");
	
	data=I2C_read(OPENCORES_I2C_0_BASE,1);
	
	alt_printf("%x\n\r",data);

	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE,0x01);
	
}

int main(void)
{
	// Configurer le bouton pour générer des interruptions
   /* IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_0_BASE, 0x1);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_0_BASE, 0x1);*/
	
	// Init Timer 

	IOWR_ALTERA_AVALON_TIMER_CONTROL (TIMER_0_BASE, 0x07 ); 
	
	// Init I2C
	
	I2C_init(OPENCORES_I2C_0_BASE,TIMER_0_FREQ,SCL_speed);
	
	// Register IRQ
	
	alt_irq_register(TIMER_0_IRQ,NULL,irq_timer);
	//alt_irq_register( PIO_0_IRQ, NULL, (void*)irq_button );

	
	
	while(1);
	
	return 0;
}