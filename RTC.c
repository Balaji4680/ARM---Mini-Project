#include "RTC.h"
#include "LCD.h"
#include "lcd_defines.h"
#include "types.h" 

u8 week[][4] = {
    "SUN","MON","TUE","WED",
    "THU","FRI","SAT"
};

void RTC_Init(void)
{
    
    CCR = RTC_RESET;

/*#ifndef CPU_LPC2148

    PREINT = PREINT_VAL;
    PREFRAC = PREFRAC_VAL;
    CCR = RTC_ENABLE;

#else

    CCR = RTC_ENABLE | RTC_CLKSRC;

#endif*/
	CCR = RTC_ENABLE | RTC_CLKSRC;

   
}

void SetRTCTimeInfo(u32 hour, u32 minute,u32 second)
{
    HOUR = hour;
    MIN = minute;
    SEC = second;
}

void GetRTCTimeInfo(s32 *hour, s32 *minute,s32 *second)
{
    *hour = HOUR;
    *minute = MIN;
    *second = SEC;
}

void SetRTCDateInfo(u32 date,u32 month,u32 year)
{
    DOM = date;
    MONTH = month;
    YEAR = year;
}

void GetRTCDateInfo(s32*date, s32 *month,s32 *year)
{
    *date = DOM;
    *month = MONTH;
    *year = YEAR;
}

void SetRTCDay(u32 dow)
{
    DOW = dow;
}

void GetRTCDay(s32 *dow)
{
    *dow = DOW;
}


/* LCD DISPLAY FUNCTIONS */

void DisplayRTCTime(u32 hour,u32 minute, u32 second)
{
    WRITE_LCD_CMD(GOTO_LINE1_POS0);

    WRITE_LCD_DATA(hour/10+48);
    WRITE_LCD_DATA(hour%10+48);
    WRITE_LCD_DATA(':');

    WRITE_LCD_DATA(minute/10+48);
    WRITE_LCD_DATA(minute%10+48);
    WRITE_LCD_DATA(':');

    WRITE_LCD_DATA(second/10+48);
    WRITE_LCD_DATA(second%10+48);
}

void DisplayRTCDate(u32 date,u32 month,u32 year)
{
    WRITE_LCD_CMD(GOTO_LINE2_POS0);

    WRITE_LCD_DATA(date/10+48);
    WRITE_LCD_DATA(date%10+48);
    WRITE_LCD_DATA('/');

    WRITE_LCD_DATA(month/10+48);
    WRITE_LCD_DATA(month%10+48);
    WRITE_LCD_DATA('/');

    U32LCD(year);
}

void DisplayRTCDay(u32 day)
{
    WRITE_LCD_CMD(GOTO_LINE1_POS0+10);
    StrLCD(week[day]);
}
