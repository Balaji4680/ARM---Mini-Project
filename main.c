// KITCHEN SAFETY PROJECT - main.c

#include <lpc21xx.h>
#include "types.h"
#include "delay.h"
#include "ADC.h"
#include "ADC_defines.h"
#include "LCD.h"
#include "LM35.h"
#include "LCD_defines.h"
#include "KPM.h"
#include "RTC.h"

// SECTION 1: SIMPLE SETTINGS

#define TEMP_LIMIT      40     // buzzer+LED turn ON above this many degrees C
#define GAS_LIMIT       300    // buzzer+LED turn ON above this gas reading (0-1023)

#define DEFAULT_PASSWORD 1234  // starting password (can be changed from the menu)

#define MENU_WAIT_TIME  30000  // 30 seconds to pick a menu option
#define POPUP_EVERY     10000  // show last event every 10 seconds
#define POPUP_FOR       3000   // show it for 3 seconds (fits the 2-3 sec ask)

#define SPLASH_HOLD_MS  2000   // how long screen 1 of the splash stays up

#define BUZZER_PIN      27     // P0.22 - kept away from P0.28, which the
                                 // ADC driver reserves as an analog pin (AIN1)
#define LED_PIN         25    // P0.30
#define SWITCH2_PIN     3      // P0.3 (silences buzzer, active-low)
// Switch1 always uses pin P0.1 - it is wired as a hardware interrupt

// SECTION 2: EVENT TYPE

typedef enum {
    EVENT_NONE = 0,   // nothing has happened yet
    EVENT_TEMP = 1,   // last danger was high temperature
    EVENT_GAS  = 2    // last danger was high gas
} event_type_t;

// SECTION 3: SHARED VARIABLES

volatile u32 open_menu_flag = 0;  // becomes 1 the moment Switch1 is pressed

u32 was_temp_high = 0;   // 1 = temperature is currently above the limit
u32 was_gas_high  = 0;   // 1 = gas reading is currently above the limit

s32 temp_limit_now = TEMP_LIMIT;  // can be changed later from the menu
u32 gas_limit_now  = GAS_LIMIT;   // can be changed later from the menu
u32 password_now   = DEFAULT_PASSWORD;  // can be changed later from the menu

// Remembers the most recent alarm event
event_type_t last_event_type = EVENT_NONE;
s32 last_event_value = 0;         // temp in C, or gas status 0/1
s32 last_event_hour, last_event_min, last_event_sec;

// SECTION 4: SIMPLE MILLISECOND CLOCK

void setup_clock(void)
{
    T0TCR = 1<<1;     // reset the timer
    T0PR  = 14999;    // makes it tick once every 1 millisecond
    T0TCR = 1<<0;     // start counting, and never stop
}

u32 time_now(void)
{
    return T0TC;
}


// SECTION 5: BUZZER, LED, SWITCH2

void setup_alarm_pins(void)
{
    IODIR0 |= 1<<BUZZER_PIN;        // buzzer pin = output
    IODIR0 |= 1<<LED_PIN;           // led pin = output
    IODIR0 &= ~(1<<SWITCH2_PIN);    // switch2 pin (P0.3) = input
}

void alarm_on(void)
{
    IOSET0 = 1<<BUZZER_PIN;   // buzzer ON
    IOSET0 = 1<<LED_PIN;      // led ON
}

void alarm_off(void)
{
    IOCLR0 = 1<<BUZZER_PIN;   // buzzer OFF
    IOCLR0 = 1<<LED_PIN;      // led OFF
}

// Switch2 is active-low: pressed = 0 (pin pulled to ground)
// Reads from PORT 0 because P0.3 lives on port 0.
u32 is_switch2_pressed(void)
{
    if (((IOPIN0 >> SWITCH2_PIN) & 1) == 0)
        return 1;
    return 0;
}

// SECTION 6: SWITCH1 INTERRUPT (opens the settings menu)

void switch1_interrupt(void) __irq
{
    open_menu_flag = 1;   // just raise the flag
    VICVectAddr = 0;      // tell the chip "done handling this interrupt"
    EXTINT = 1<<0;        // clear the interrupt flag
}

void setup_switch1(void)
{
    PINSEL0 |= 3<<(1*2);            // make pin P0.1 act as an interrupt pin
    VICIntSelect &= ~(1<<14);       // use normal interrupt, not fast one
    VICIntEnable  = 1<<14;          // turn this interrupt ON
    VICVectAddr0  = (u32)switch1_interrupt;  // which function to call
    VICVectCntl0  = 1<<5 | 14;      // give it a priority slot
}


// SECTION 7: GAS SENSOR (MQ2)

void setup_gas_sensor(void)
{
    PINSEL1 |= AIN2;   // pin P0.29 will read an analog signal
}

u32 read_gas_raw(void)
{
    u32 raw_value;
    f32 not_used;

    Read_ADC(CH2, &raw_value, &not_used);
    return raw_value;
}

// SECTION 8: TEMPERATURE DISPLAY HELPER

// Prints a temperature value followed by a degree symbol and "C"
void print_temp_c(s32 value)
{
    S32LCD(value);
    WRITE_LCD_DATA(0xDF);   // built-in degree-like character on many LCDs
    WRITE_LCD_DATA('C');
}


// SECTION 9: STARTUP SPLASH SCREENS

// Scrolls one line of text across line 2, right to left, one full pass
void scroll_text_line2(u8* text)
{
    u32 text_len = 0;
    u32 offset, i;

    while (text[text_len] != '\0') text_len++;

    for (offset = 0; offset < text_len; offset++)
    {
        WRITE_LCD_CMD(GOTO_LINE2_POS0);
        for (i = 0; i < 16; i++)
        {
            u32 pos = (offset + i) % text_len;
            WRITE_LCD_DATA(text[pos]);
        }
        delay_ms(100);   // scroll speed - smaller number = faster
    }
}

void show_vector_id_screen(void)
{
    WRITE_LCD_CMD(CLEAR_LCD);
    WRITE_LCD_CMD(GOTO_LINE1_POS0);
    StrLCD((u8*)"VECTOR ID:");

    WRITE_LCD_CMD(GOTO_LINE2_POS0);
    StrLCD((u8*)"V25HE11B5");

    delay_ms(SPLASH_HOLD_MS);   // give the user time to read it
}

void show_project_title_screen(void)
{
    // extra spaces at the start/end make the scroll loop look smooth
    u8 title[] = "   KITCHEN SAFETY HEAT AND GAS MONITORING SYSTEM   ";

    WRITE_LCD_CMD(CLEAR_LCD);
    WRITE_LCD_CMD(GOTO_LINE1_POS0);
    StrLCD((u8*)"PROJECT TITLE:");

    scroll_text_line2(title);
}

void show_splash_screen(void)
{
    show_vector_id_screen();
    show_project_title_screen();
    WRITE_LCD_CMD(CLEAR_LCD);
}

// SECTION 10: SAVE AND SHOW THE LAST ALARM EVENT

void save_temp_event(s32 temp_c)
{
    s32 h, m, s;
    GetRTCTimeInfo(&h, &m, &s);

    last_event_type  = EVENT_TEMP;
    last_event_value = temp_c;
    last_event_hour  = h;
    last_event_min   = m;
    last_event_sec   = s;
}

void save_gas_event(u32 gas_status)   // gas_status is 0 or 1
{
    s32 h, m, s;
    GetRTCTimeInfo(&h, &m, &s);

    last_event_type  = EVENT_GAS;
    last_event_value = gas_status;
    last_event_hour  = h;
    last_event_min   = m;
    last_event_sec   = s;
}

// Prints the last saved event to the LCD.
// The caller (main loop) only calls this when last_event_type is
// NOT EVENT_NONE - see SECTION 14.
void show_last_event(void)
{
    WRITE_LCD_CMD(CLEAR_LCD);
    WRITE_LCD_CMD(GOTO_LINE1_POS0);

    // print the time the event happened
    S32LCD(last_event_hour); WRITE_LCD_DATA(':');
    S32LCD(last_event_min);  WRITE_LCD_DATA(':');
    S32LCD(last_event_sec);
    WRITE_LCD_DATA(' ');

    if (last_event_type == EVENT_TEMP)
    {
        StrLCD((u8*)"T:");
        print_temp_c(last_event_value);   // already prints value + deg + C
    }
    else if (last_event_type == EVENT_GAS)
    {
        StrLCD((u8*)"S:");
        U32LCD(last_event_value);   // prints 0 or 1
    }
}


// SECTION 11: CHECK IF TEMPERATURE OR GAS IS DANGEROUS
void check_for_danger(s32 temp_c, u32 gas_raw)
{
    if (temp_c > temp_limit_now && was_temp_high == 0)
    {
        save_temp_event(temp_c);     // brand new danger, save it
        was_temp_high = 1;
        alarm_on();
        WRITE_LCD_CMD(CLEAR_LCD);
        WRITE_LCD_CMD(GOTO_LINE1_POS0);
        StrLCD((u8*)"ALERT!!");
        WRITE_LCD_CMD(GOTO_LINE2_POS0);
        StrLCD((u8*)"TEMP IS HIGH!");
        delay_ms(2500);
        WRITE_LCD_CMD(CLEAR_LCD);     // main() redraws the normal screen right after
    }
    if (temp_c <= temp_limit_now)
    {
        was_temp_high = 0;   // back to safe
    }

    // ---- gas ----
    if (gas_raw > gas_limit_now && was_gas_high == 0)
    {
        save_gas_event(1);           // 1 = gas unsafe
        was_gas_high = 1;

        // same flash for gas - buzzer/LED on for the same 2-3 sec
        alarm_on();
        WRITE_LCD_CMD(CLEAR_LCD);
        WRITE_LCD_CMD(GOTO_LINE1_POS0);
        StrLCD((u8*)"ALERT!!");
        WRITE_LCD_CMD(GOTO_LINE2_POS0);
        StrLCD((u8*)"GAS IS HIGH!");
        delay_ms(2500);
        WRITE_LCD_CMD(CLEAR_LCD);     // main() redraws the normal screen right after
    }
    if (gas_raw <= gas_limit_now)
    {
        was_gas_high = 0;
    }
}

// SECTION 12: NORMAL SCREEN
//   Line 1: HH:MM:SS T:<value><deg>C
//   Line 2: DD/MM/YY S:0/1

void show_normal_screen(s32 temp_c,s32 gas_raw)
{
    s32 h, m, s, dd, mo, yy;
    GetRTCTimeInfo(&h, &m, &s);
    GetRTCDateInfo(&dd, &mo, &yy);

    WRITE_LCD_CMD(GOTO_LINE1_POS0);
    if (h < 10)  WRITE_LCD_DATA('0'); S32LCD(h); WRITE_LCD_DATA(':');
    if (m < 10)  WRITE_LCD_DATA('0'); S32LCD(m); WRITE_LCD_DATA(':');
    if (s < 10)  WRITE_LCD_DATA('0'); S32LCD(s);
    StrLCD((u8*)" T:");
    print_temp_c(temp_c);
    StrLCD((u8*)"  ");   // pad - clears any leftover chars from the ALERT screen

    WRITE_LCD_CMD(GOTO_LINE2_POS0);
    if (dd < 10) WRITE_LCD_DATA('0'); S32LCD(dd); WRITE_LCD_DATA('/');
    if (mo < 10) WRITE_LCD_DATA('0'); S32LCD(mo); WRITE_LCD_DATA('/');
    S32LCD(yy);
    StrLCD((u8*)" S:");
    U32LCD(was_gas_high);   // 0 = safe, 1 = unsafe
    StrLCD((u8*)"  ");      // pad - clears any leftover chars from the ALERT screen
	  /*S32LCD(yy % 100);              // 2-digit year, to leave room for the gas value
    StrLCD((u8*)" G:");
    U32LCD(gas_raw);   */             // the actual MQ2 reading, 0-1023
	  
	
}

// SECTION 13: SETTINGS MENU (password + edit options)

u32 ReadNumClearable(void)
{
    u32 key;
    u32 sum = 0;
    u32 digit_count = 0;   // how many digits are currently on screen

    while (1)
    {
        key = KeyScan();

        if (key >= '0' && key <= '9')
        {
            sum = (sum * 10) + (key - '0');   // add the new digit
            WRITE_LCD_DATA(key);              // show it on the LCD
            digit_count++;
        }
        else if (key == 'C')
        {
            if (digit_count > 0)
            {
                sum = sum / 10;                 // drop the last digit numerically
                digit_count--;

                WRITE_LCD_CMD(GOTO_LINE2_POS0 + digit_count);
                WRITE_LCD_DATA(' ');
                WRITE_LCD_CMD(GOTO_LINE2_POS0 + digit_count);
            }
        }
        else
        {
            break;   // any other key (like '=') finishes the entry
        }
    }

    return sum;
}
u32 ReadPasswordMasked(void)
{
    u32 key;
    u32 sum = 0;
    u32 digit_count = 0;

    while (1)
    {
        key = KeyScan();

        if (key >= '0' && key <= '9')
        {
            sum = (sum * 10) + (key - '0');   // add the new digit

            WRITE_LCD_DATA(key);              // show the real digit briefly
            delay_ms(100);
            WRITE_LCD_CMD(GOTO_LINE2_POS0 + digit_count);   // back onto that character
            WRITE_LCD_DATA('*');              // cover it with a star

            digit_count++;
        }
        else if (key == 'C')
        {
            if (digit_count > 0)
            {
                sum = sum / 10;
                digit_count--;

                WRITE_LCD_CMD(GOTO_LINE2_POS0 + digit_count);
                WRITE_LCD_DATA(' ');
                WRITE_LCD_CMD(GOTO_LINE2_POS0 + digit_count);
            }
        }
        else
        {
            break;   // any other key (like '=') finishes the entry
        }
    }

    return sum;
}

u32 ask_for_password(void)
{
    u32 typed;

    WRITE_LCD_CMD(CLEAR_LCD);
    WRITE_LCD_CMD(GOTO_LINE1_POS0);
    StrLCD((u8*)"Enter Password:");
    WRITE_LCD_CMD(GOTO_LINE2_POS0);

    typed = ReadPasswordMasked();   // digits show as '*', '=' to confirm, 'C' to clear

    return (typed == password_now) ? 1 : 0;
}

// Asks for a number on line 2, under a label on line 1.
// Keeps asking again until the number is between min_val and max_val.
u32 ask_ranged_number(u8* label, u32 min_val, u32 max_val)
{
    u32 value;

    while (1)
    {
        WRITE_LCD_CMD(CLEAR_LCD);
        WRITE_LCD_CMD(GOTO_LINE1_POS0);
        StrLCD(label);
        WRITE_LCD_CMD(GOTO_LINE2_POS0);

        value = ReadNumClearable();

        if (value >= min_val && value <= max_val)
            return value;

        WRITE_LCD_CMD(CLEAR_LCD);
        WRITE_LCD_CMD(GOTO_LINE1_POS0);
        StrLCD((u8*)"Invalid! Retry");
        delay_ms(1000);
    }
}

void edit_rtc_info(void)
{
    u32 choice;
    s32 h, m, s, dd, mo, yy;

    while (1)
    {
        WRITE_LCD_CMD(CLEAR_LCD);
        WRITE_LCD_CMD(GOTO_LINE1_POS0);
        StrLCD((u8*)"1.H 2.M 3.S 4.D");
        WRITE_LCD_CMD(GOTO_LINE2_POS0);
        StrLCD((u8*)"5.M 6.Y 7.E");

        choice = KeyScan();

        if (choice == '1')                          // Hour
        {
            GetRTCTimeInfo(&h, &m, &s);
            h = ask_ranged_number((u8*)"HOUR (0-23):", 0, 23);
            SetRTCTimeInfo(h, m, s);
        }
        else if (choice == '2')                      // Minute
        {
            GetRTCTimeInfo(&h, &m, &s);
            m = ask_ranged_number((u8*)"MINUTE (0-59):", 0, 59);
            SetRTCTimeInfo(h, m, s);
        }
        else if (choice == '3')                      // Second
        {
            GetRTCTimeInfo(&h, &m, &s);
            s = ask_ranged_number((u8*)"SECOND (0-59):", 0, 59);
            SetRTCTimeInfo(h, m, s);
        }
        else if (choice == '4')                      // Date
        {
            GetRTCDateInfo(&dd, &mo, &yy);
            dd = ask_ranged_number((u8*)"DATE (1-31):", 1, 31);
            SetRTCDateInfo(dd, mo, yy);
        }
        else if (choice == '5')                      // Month
        {
            GetRTCDateInfo(&dd, &mo, &yy);
            mo = ask_ranged_number((u8*)"MONTH (1-12):", 1, 12);
            SetRTCDateInfo(dd, mo, yy);
        }
        else if (choice == '6')                      // Year
        {
            GetRTCDateInfo(&dd, &mo, &yy);
            yy = ask_ranged_number((u8*)"YEAR:", 2000, 2099);
            SetRTCDateInfo(dd, mo, yy);
        }
        else if (choice == '7')                      // Exit
        {
            return;
        }
        else
        {
            continue;   // key not recognized, just redraw the menu
        }

        WRITE_LCD_CMD(CLEAR_LCD);
        StrLCD((u8*)"Saved!");
        delay_ms(1000);
    }
}

void edit_setpoint(void)
{
    u32 choice;

    WRITE_LCD_CMD(CLEAR_LCD);
    StrLCD((u8*)"1.TEMP 2.GAS");
    choice = KeyScan();

    if (choice == '1')
        temp_limit_now = ask_ranged_number((u8*)"New Temp Limit:", 0, 200);
    else if (choice == '2')
        gas_limit_now = ask_ranged_number((u8*)"New Gas Limit:", 0, 1023);

    WRITE_LCD_CMD(CLEAR_LCD);
    StrLCD((u8*)"Saved!");
    delay_ms(1500);
}

void edit_password(void)
{
    u32 current_try, first_try, second_try;

    WRITE_LCD_CMD(CLEAR_LCD);
    WRITE_LCD_CMD(GOTO_LINE1_POS0);
    StrLCD((u8*)"Current Pass:");
    WRITE_LCD_CMD(GOTO_LINE2_POS0);
    current_try = ReadPasswordMasked();

    if (current_try != password_now)
    {
        WRITE_LCD_CMD(CLEAR_LCD);
        StrLCD((u8*)"Wrong Password");
        delay_ms(1500);
        return;   // stop here - never even asks for a new password
    }

    WRITE_LCD_CMD(CLEAR_LCD);
    WRITE_LCD_CMD(GOTO_LINE1_POS0);
    StrLCD((u8*)"New Password:");
    WRITE_LCD_CMD(GOTO_LINE2_POS0);
    first_try = ReadPasswordMasked();

    WRITE_LCD_CMD(CLEAR_LCD);
    WRITE_LCD_CMD(GOTO_LINE1_POS0);
    StrLCD((u8*)"Confirm:");
    WRITE_LCD_CMD(GOTO_LINE2_POS0);
    second_try = ReadPasswordMasked();

    WRITE_LCD_CMD(CLEAR_LCD);
    if (first_try == second_try)
    {
        password_now = first_try;
        StrLCD((u8*)"Password Saved!");
    }
    else
    {
        StrLCD((u8*)"Did Not Match!");
    }
    delay_ms(1500);
}

void show_menu(void)
{
    u32 start_time;
    u32 key;
    u32 elapsed, remaining_ms, seconds_left, last_shown_seconds;

    while (1)
    {
        WRITE_LCD_CMD(CLEAR_LCD);
        WRITE_LCD_CMD(GOTO_LINE2_POS0);
        StrLCD((u8*)"3.PASS 4.EXIT");

        start_time = time_now();
        key = 0;
        last_shown_seconds = 0xFFFFFFFF;   // force the first draw

        while (1)
        {
            elapsed = time_now() - start_time;
            remaining_ms = (elapsed < MENU_WAIT_TIME) ? (MENU_WAIT_TIME - elapsed) : 0;
            seconds_left = (remaining_ms + 999) / 1000;   // round up to whole seconds

            if (seconds_left != last_shown_seconds)
            {
                last_shown_seconds = seconds_left;
                // countdown lives on LINE 1, right after this label
                WRITE_LCD_CMD(GOTO_LINE1_POS0);
                StrLCD((u8*)"1.RTC 2.SETPT");
                U32LCD(seconds_left);
                WRITE_LCD_DATA(' ');   // erases a leftover digit when it drops from 2 digits to 1
            }

            if (ColScan() == 0)     // a key is being pressed right now
            {
                key = KeyScan();
                break;
            }
            if (elapsed > MENU_WAIT_TIME)
            {
                key = 0;            // countdown reached 0 with no key
                break;
            }
        }

        if (key == '1')
            edit_rtc_info();
        else if (key == '2')
            edit_setpoint();
        else if (key == '3')
            edit_password();
        else
            return;   // key '4', timeout, or anything else -> back to normal screen
    }
}

void open_settings_menu(void)
{
    u32 wrong_tries = 0;
    alarm_off();

    while (1)
    {
        if (ask_for_password())
        {
            show_menu();
            break;   // menu finished (Exit or timeout) - back to normal screen
        }

        wrong_tries++;

        WRITE_LCD_CMD(CLEAR_LCD);
        StrLCD((u8*)"Access Denied");   // exact wording from the project spec
        alarm_on();
        delay_ms(500);
        alarm_off();

        if (wrong_tries >= 3)
        {
            u32 seconds_left;

            WRITE_LCD_CMD(CLEAR_LCD);           // clear ONCE, not every second
            WRITE_LCD_CMD(GOTO_LINE1_POS0);
            StrLCD((u8*)"System Locked");       // exact wording from the project spec,
                                                  // fixed text, drawn once

            for (seconds_left = 10; seconds_left >= 1; seconds_left--)
            {
                WRITE_LCD_CMD(GOTO_LINE2_POS0); // only this number gets redrawn
                StrLCD((u8*)"Wait:");
                U32LCD(seconds_left);
                StrLCD((u8*)"  ");               // pad - erases a leftover digit (10 -> 9, etc.)
                delay_ms(1000);
            }

            wrong_tries = 0;   // fresh 3 attempts next time
            // loop back around to ask_for_password() again - does NOT
            // return to the normal screen
        }
        // otherwise loop back and ask for the password again
    }

    WRITE_LCD_CMD(CLEAR_LCD);
}

// SECTION 14: MAIN PROGRAM

int main(void)
{
    s32 temp_c;
    u32 gas_raw;
    //u32 rtc_was_reset;

    u32 showing_popup;
    u32 popup_started_at;
    u32 last_popup_check;

    // ---- turn everything on, one line at a time ----
    Init_LCD();
    Init_ADC();
    setup_gas_sensor();
    RTC_Init();   // 1 = first ever start, 0 = kept running through power-off
    Init_KPM();
    setup_alarm_pins();
    setup_clock();
    setup_switch1();
        SetRTCTimeInfo(9, 37, 0);
        SetRTCDateInfo(24, 9, 2026);

    // show the two-part splash screen once at power-up
    show_splash_screen();

    showing_popup = 0;
    last_popup_check = time_now();

    while (1)
    {
        // ---- Switch1 pressed? handle settings menu first ----
        if (open_menu_flag)
        {
            open_settings_menu();
            open_menu_flag = 0;
            last_popup_check = time_now();
            continue;
        }

        // ---- read the sensors ----
        temp_c   = (s32)LM35tC();
        gas_raw  = read_gas_raw();

        // ---- check danger and log new events (does not touch the alarm) ----
        check_for_danger(temp_c, gas_raw);

        // ---- decide what to show on the screen ----
        // showing_popup == 0 -> normal/alert screen
        // showing_popup == 1 -> last-event screen, shown for POPUP_FOR ms
        if (showing_popup)
        {
            alarm_off();

            if ((time_now() - popup_started_at) > POPUP_FOR)
            {
                showing_popup = 0;              // 2-3 sec is up, go back to normal
                last_popup_check = time_now();  // restart the 10-sec countdown
            }
        }
        else
        {
            // Normal mode: the alarm follows the danger flags, unless
            // Switch2 is silencing it right now.
            if (was_temp_high || was_gas_high)
                alarm_on();
            else
                alarm_off();

            if (is_switch2_pressed())
            {
                alarm_off();
            }

            show_normal_screen(temp_c,gas_raw);

            // Only pop up if a REAL event has happened at least once.
            if (last_event_type != EVENT_NONE &&
                (time_now() - last_popup_check) > POPUP_EVERY)
            {
                showing_popup = 1;               // 10 sec passed, show the popup
                popup_started_at = time_now();
                alarm_off();                     // silence right as the popup begins
                show_last_event();
            }
        }
    }
}

