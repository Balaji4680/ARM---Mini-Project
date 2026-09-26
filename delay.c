//delay.c
#include "types.h"
void delay_us(u32 dlyus)
{
	for(dlyus*=12; dlyus>0; dlyus--);
}

void delay_ms(u32 dlyms)
{
	for(dlyms*=12000; dlyms>0; dlyms--);
}

void delay_s(u32 dlys)
{
	for(dlys*=12000000; dlys>0; dlys--);
}
