//LCD_defines.h

//LCD pin defines
#define LCD_DATA 8 //p0.8 to p0.15
#define LCD_RS 16 //p0.16
#define LCD_RW 17 //p0.17
#define LCD_EN 18 //p0.18

//LCD_commands
#define CLEAR_LCD 0x01
#define RET_CUR_HOME 0x02
#define MODE_8BIT_1LINE 0x30
#define MODE_8BIT_2LINE 0x38
#define MODE_4BIT_1LINE 0x20
#define MODE_4BIT_2LINE 0x28
#define DISP_OFF 0x08
#define DISP_ON_CUR_OFF 0x0C
#define DISP_ON_CUR_ON 0x0E
#define DISP_ON_CUR_BLINK 0x0F
#define SHIFT_CUR_RIGHT 0x06
#define GOTO_LINE1_POS0 0x80
#define GOTO_LINE2_POS0 0xC0
#define GOTO_LINE3_POS0 0x94
#define GOTO_LINE4_POS0 0xD4
#define GOTO_CGRAM 0x40










