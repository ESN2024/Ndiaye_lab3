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

Placez la carte dans une position initiale bien définie.

Enregistrez les valeurs de l'accélération dans cette position. 
Ces valeurs représentent l'offset initial.

Lors de l'utilisation normale de l'accéléromètre, soustrayez l'offset 
initial des mesures d'accélération pour compenser l'effet de la gravité.

Top
x_0 = 0xf8  
y_0 = 0xef  
z_0 = 0xd9

Resultat attendu pour top
XOUT = 0g
YOUT = 0g	
ZOUT = 1g

0x7F = 2 g ==> 0x3F ~= 1g

*/

#define I2C_add  0x1D

#define X_add_0  0x32
#define X_add_1  0x33
#define X_Offset_add 0x1E

#define Y_add_0  0x34
#define Y_add_1  0x35
#define Y_Offset_add 0x1F

#define Z_add_0  0x36
#define Z_add_1  0x37
#define Z_Offset_add 0x20

//A calculer à la main
#define x_offset 0x01
#define y_offset 0x04
#define z_offset 0x3D

#define SCL_speed 400000

volatile __uint8_t u=0,d=0,c=0,m=0,sign=1;

volatile __uint16_t cnt=1023;//nombre à afficher

volatile alt_u32 x_data=0;
volatile alt_u32 y_data=0;
volatile alt_u32 z_data=0;




__int32_t extracted_data(alt_u32 id);
void send_offset(alt_u32 id,alt_u32 data);
void irq_timer();
static void irq_button(void * context, alt_u32 id);

int main(void)
{
	// Init PIO 0 Irq
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_0_BASE, 0x1);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_0_BASE, 0x1);
	
	// Init Timer 
	IOWR_ALTERA_AVALON_TIMER_CONTROL (TIMER_0_BASE, 0x07 ); 
	
	// Init I2C
	I2C_init(OPENCORES_I2C_0_BASE,TIMER_0_FREQ,SCL_speed);

	//Full resolution
	/*
	alt_printf("%x\n\r",extracted_data(0x31));

	I2C_start(OPENCORES_I2C_0_BASE,I2C_add , 0);
	
	I2C_write(OPENCORES_I2C_0_BASE,0x31, 0);

	I2C_write(OPENCORES_I2C_0_BASE,extracted_data(0x31) | 0x08, 1);

	alt_printf("%x\n\r",extracted_data(0x31));
	*/

	// Reset offset
	send_offset(X_Offset_add,0);
	send_offset(Y_Offset_add,0);
	send_offset(Z_Offset_add,0);

	// Send offset
	send_offset(X_Offset_add,x_offset);
	send_offset(Y_Offset_add,y_offset);
	send_offset(Z_Offset_add,z_offset);
	
	
	// Register IRQ
	alt_irq_register(TIMER_0_IRQ,NULL,irq_timer);
	alt_irq_register( PIO_0_IRQ, NULL, (void*)irq_button );
	
	while(1);
	
	return 0;
}


__int32_t extracted_data(alt_u32 id)
{
	//write mode
	I2C_start(OPENCORES_I2C_0_BASE,I2C_add , 0);
	
	I2C_write(OPENCORES_I2C_0_BASE,id, 0);
	
	//read mode
	I2C_start(OPENCORES_I2C_0_BASE,I2C_add , 1);
	
	return I2C_read(OPENCORES_I2C_0_BASE,1);
	
}

void send_offset(alt_u32 id,alt_u32 data)
{
	//write mode
	I2C_start(OPENCORES_I2C_0_BASE,I2C_add , 0);
	
	I2C_write(OPENCORES_I2C_0_BASE,id, 0);

	I2C_write(OPENCORES_I2C_0_BASE,data, 1);
}

void irq_timer()
{

	x_data= extracted_data(X_add_0);
	x_data = x_data | (extracted_data(X_add_1)<<8);

	y_data= extracted_data(Y_add_0);
	y_data = y_data | (extracted_data(Y_add_1)<<8);

	z_data= extracted_data(Z_add_0);
	z_data = z_data | (extracted_data(Z_add_1)<<8);

	// Complément à deux
	/*
	
	if(x_data & 0x8000) x_data= -(0xFFFF -x_data +1);
	if(y_data & 0x8000) y_data= -(0xFFFF -y_data +1);
	if(z_data & 0x8000) z_data= -(0xFFFF -z_data +1);

	x_data=(x_data*3.9);
	y_data=(y_data*3.9);
	z_data=(z_data*3.9);

	*/

	
	

	alt_printf("x data corrected =%x\n\r",x_data );
	alt_printf("y data corrected =%x\n\r",y_data );
	alt_printf("z data corrected =%x\n\n\r",z_data );

	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE,0x01);
	
}

static void irq_button(void * context, alt_u32 id)
{
	m=cnt/1000;
	c=(cnt%1000)/100;
	d=(cnt%100)/10;
	u=cnt%10;
	
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_1_BASE,u);//seg1
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_2_BASE,d);//seg2 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_3_BASE,c);//seg3
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_4_BASE,m);//seg4
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_5_BASE,sign);//seg5 
	
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_0_BASE, 0x01);
}