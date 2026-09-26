//KPM.c
#include <lpc21xx.h>
#include "KPM_defines.h"
#include "lcd.h"
#include "types.h"
/*u32 KpmLUT[4][4]={{1,2,3,4},
									{5,6,7,8},
									{9,10,11,12},
									{13,14,15,16}};*/
u8 kpmLUT[4][4]={{'7','8','9','/'},
									{'4','5','6','*'},
									{'1','2','3','-'},
									{'C','0',':','+'}};

void Init_KPM(void)
{
	IODIR1|=15<<ROW0;
}
u32 ColScan(void)
{
	if(((IOPIN1>>COL0)&15)<15)
		return 0;
	else
		return 1;
}
u32 RowCheck(void)
{
	u32 rno;
	for(rno=0; rno<4; rno++)
	{
		IOPIN1=(IOPIN1&~(15<<ROW0))|((~(1<<rno))<<ROW0);
		if(ColScan()==0)
		break;
	}
	
	IOCLR1=15<<ROW0;
	return rno;
}
u32 ColCheck(void)
{
	u32 cno;
	for(cno=0; cno<4; cno++)
	{
		if(((IOPIN1>>(COL0+cno))&1)==0)
			break;
	}
	return cno;
}


u32 KeyScan(void)
{
	u32 rno, cno,key;
	//wait for switch press
	while(ColScan());
	//find the rno
	rno=RowCheck();
	//find the cno
	cno=ColCheck();
	//get the value from LUT
	key=kpmLUT[rno][cno];
	//wait for switch release
	while(!ColScan());
	return key;
}
u32 ReadNum(void)
{
	u8 key;
	u32 sum=0;
	while(1)
	{
		key=KeyScan();
		WRITE_LCD_DATA(key);
		if(key>='0' && key<='9')
		{
			sum=(sum*10)+(key-48);
		}
		
		else
			break;
	}
	return sum;
}
