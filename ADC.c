//ADC.c
#include <lpc21xx.h>
#include "ADC_defines.h"
#include "pin_connect_block.h"
#include "delay.h"
void Init_ADC(void)
{
	//make p0.27 to p0.30 as GPIO
	PINSEL1&=~(255<<((27-16)*2));
	//cfg p0.27 as AIN0(analog input pin)
//	PINSEL1|=AIN0;
	//cfgportpin(0,27,1);
	//PINSEL1|=0x15400000;//p0.27 to p0.30 as AIN
	//cfg p0.28 as AIN1
	PINSEL1|=AIN1;
	ADCR=1<<PDN_BIT|CLKDIV_VALUE<<CLKDIV;
}
void Read_ADC(u32 chno,u32* dval, f32* eAR)
{
	//clear previous channel values
	ADCR&=~(255<<0);
	//select channel & start conversion
	ADCR|=1<<chno|1<<START_CONV;
	//wait for 3usec
	delay_us(3);
	//check the done bit status
	while(((ADDR>>DONE_BIT)&1)==0);
	//stop conversion
	ADCR&=~(1<<START_CONV);
	//extract 10bit digital output
	*dval= ((ADDR>>RESULT)&1023);
	//find eAR value
	*eAR=(3.3/1023)*(*dval);
}

