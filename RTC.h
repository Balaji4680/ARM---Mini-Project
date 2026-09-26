#ifndef RTC_H
#define RTC_H

#include <lpc214x.h>

#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)

#define PREINT_VAL  ((int)(PCLK/32768)-1)
#define PREFRAC_VAL (PCLK-((PREINT_VAL+1)*32768))

#define RTC_ENABLE (1<<0)
#define RTC_RESET  (1<<1)
#define RTC_CLKSRC (1<<4)

#define SUN 0
#define MON 1
#define TUE 2
#define WED 3
#define THU 4
#define FRI 5
#define SAT 6

void RTC_Init(void);

void SetRTCTimeInfo(unsigned int, unsigned int, unsigned int);
void GetRTCTimeInfo(int *, int *, int *);

void SetRTCDateInfo(unsigned int, unsigned int, unsigned int);
void GetRTCDateInfo(int *, int *, int *);

void SetRTCDay(unsigned int);
void GetRTCDay(int *);

void DisplayRTCTime(unsigned int, unsigned int, unsigned int);
void DisplayRTCDate(unsigned int, unsigned int, unsigned int);
void DisplayRTCDay(unsigned int);

#endif

