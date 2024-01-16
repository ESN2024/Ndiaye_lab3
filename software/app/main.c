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

/*

Output Change in X-Axis 0.20 2.10 g
Output Change in Y-Axis −2.10 −0.20 g
Output Change in Z-Axis 0.30 3.40 g

*/

/*

0x1E 30 OFSX R/W 00000000 X-axis offset
0x1F 31 OFSY R/W 00000000 Y-axis offset
0x20 32 OFSZ R/W 00000000 Z-axis offset

*/

#define I2C_add  0x1D
#define X_add_0  0x32
#define X_add_1  0x33
#define Y_add_0  0x34
#define Y_add_1  0x35
#define Z_add_0  0x36
#define Z_add_1  0x37
#define SCL_speed 400000

volatile __uint8_t u=0,d=0,c=0;
volatile alt_u32 x_data=0;
volatile alt_u32 y_data=0;
volatile alt_u32 z_data=0;
volatile __uint16_t cnt=990;

static void irq_button(void * context, alt_u32 id)
{
	c=cnt/100;
	d=(cnt%100)/10;
	u=cnt%10;
	
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_1_BASE,u);//seg1
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_2_BASE,d);//seg2 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_3_BASE,c);//seg3
	
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_0_BASE, 0x01);
}


alt_u32 extracted_data(alt_u32 id)
{
	//write mode
	I2C_start(OPENCORES_I2C_0_BASE,I2C_add , 0);
	
	I2C_write(OPENCORES_I2C_0_BASE,id, 0);
	
	//read mode
	I2C_start(OPENCORES_I2C_0_BASE,I2C_add , 1);
	
	return I2C_read(OPENCORES_I2C_0_BASE,1);
	
}

void irq_timer()
{
	x_data= extracted_data(X_add_0);
	
	x_data = x_data + (extracted_data(X_add_1)<<8);

	y_data= extracted_data(Y_add_0);
	
	y_data = y_data + (extracted_data(Y_add_1)<<8);

	z_data= extracted_data(Z_add_0);
	
	z_data = z_data + (extracted_data(Z_add_1)<<8);
	
    //float floatValue = *(float*)&data;

	alt_printf("x data =%x\n\r",x_data);
	alt_printf("y data =%x\n\r",y_data);
	alt_printf("z data =%x\n\n\r",z_data);

	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE,0x01);
	
}

int main(void)
{
	// Init PIO 0 Irq
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_0_BASE, 0x1);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_0_BASE, 0x1);
	
	// Init Timer 

	IOWR_ALTERA_AVALON_TIMER_CONTROL (TIMER_0_BASE, 0x07 ); 
	
	// Init I2C
	
	I2C_init(OPENCORES_I2C_0_BASE,TIMER_0_FREQ,SCL_speed);
	
	// Register IRQ
	
	alt_irq_register(TIMER_0_IRQ,NULL,irq_timer);
	alt_irq_register( PIO_0_IRQ, NULL, (void*)irq_button );
	
	while(1);
	
	return 0;
}