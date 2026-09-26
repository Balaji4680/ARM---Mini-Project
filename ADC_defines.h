//ADC_defines.h

//clk defines
#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)
#define ADCLK 3000000
#define CLKDIV_VALUE ((PCLK/ADCLK)-1)

//SFRs
//ADCR

#define CLKDIV 8
#define PDN_BIT 21
#define START_CONV 24

#define CH0 0
#define CH1 1
#define CH2 2
#define CH3 3

//ADDR
#define RESULT 6
#define DONE_BIT 31

#define AIN0 0x00400000
#define AIN1 0x01000000
#define AIN2 0x04000000
#define AIN3 0x10000000

