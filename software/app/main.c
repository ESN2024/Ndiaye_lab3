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

Scale Factor at XOUT, YOUT, ZOUT All g-ranges, full resolution 
min: 3.5 type: 3.9 max: 4.3 mg/LSB 

The OFSX, OFSY, and OFSZ registers are each eight bits and
offer user-set offset adjustments in twos complement format
with a scale factor of 15.6 mg/LSB

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
#define x_offset 0x06
#define y_offset 0x06
#define z_offset 0x0C

#define scale_f_out 3.9
#define scale_f_off 15.6

#define SCL_speed 400000

volatile __uint8_t seg1[3]={0,0,0},seg2[3]={0,0,0},seg3[3]={0,0,0},seg4[3]={0,0,0},sign[3]={1,1,1},cnt=0;

volatile int x_data=0;
volatile int y_data=0;
volatile int z_data=0;

volatile int data[3]={0,0,0};//nombre à afficher


int extracted_data(alt_u32 id);
void seg_afficher();
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


int extracted_data(alt_u32 id)
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
	
	if(x_data & 0x8000) x_data= -(0xFFFF -x_data +1);
	if(y_data & 0x8000) y_data= -(0xFFFF -y_data +1);
	if(z_data & 0x8000) z_data= -(0xFFFF -z_data +1);
	
	x_data=(int)(x_data*scale_f_out);
	y_data=(int)(y_data*scale_f_out);
	z_data=(int)(z_data*scale_f_out);

	if((cnt%3)==0) alt_printf("x s'affiche\n\n\r");
	else if((cnt%3)==1) alt_printf("y s'affiche\n\n\r");
	else alt_printf("z s'affiche\n\n\r");
/*

	alt_printf("x: %x\t",(alt_u32)(x_data) );
	alt_printf("y: %x\t",(alt_u32)(y_data) );
	alt_printf("z: %x\n\n\r",(alt_u32)(z_data) );

*/
	seg_afficher();
	
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_1_BASE,seg1[cnt%3]);//seg1
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_2_BASE,seg2[cnt%3]);//seg2 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_3_BASE,seg3[cnt%3]);//seg3
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_4_BASE,seg4[cnt%3]);//seg4
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_5_BASE,sign[cnt%3]);//seg5 

	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE,0x01);
	
}

static void irq_button(void * context, alt_u32 id)
{
	cnt++;

    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_0_BASE, 0x01);
}


void seg_afficher()
{
	data[0]=x_data;
	data[1]=y_data;
	data[2]=z_data;

	if(data[cnt%3]>=0) 
	{
		sign[cnt%3]=1;
	}
	else 
	{
		sign[cnt%3]=0;
		data[cnt%3]=-data[cnt%3];
	}
	
	seg4[cnt%3]=data[cnt%3]/10000;
	seg3[cnt%3]=(data[cnt%3]%10000)/1000;
	seg2[cnt%3]=(data[cnt%3]%1000)/100;
	seg1[cnt%3]=(data[cnt%3]%100)/10;

}
