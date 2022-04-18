/******************************************************************************
* DISCLAIMER



* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.



* This software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.



* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY
* DISCLAIMED.



* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.



* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
* By using this software, you agree to the additional terms and
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved. */
/******************************************************************************
* File Name : intp_isr.c
* Version : 1.00
* Device(s) :
* Tool-Chain :
* H/W Platform :
* Description :
******************************************************************************
* History : DD.MM.YYYY Version Description
* : 01.07.2015 1.00 First Release
******************************************************************************/




/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include "r_macro.h" /* System macro and standard type definition */
#include "timer.h"
#include "lcd.h"



/******************************************************************************
Typedef definitions
******************************************************************************/
volatile int G_elapsedTime = 0; // Timer Counter
/******************************************************************************
Macro definitions
******************************************************************************/
#define NO_SCROLLDOWN 16
#define SCROLLDOWN 15
#define NO_SCROLLUP 14
#define SCROLLUP 13
#define LAST_RECORD 12
#define FIRST_RECORD 11
#define NO_RECORD 10
#define RUNNING 1
#define PAUSE 2
#define RECORDING 3



#define SW1 P7_bit.no6
#define SW2 P7_bit.no4
#define SW_ON (0)
/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/*Define arguments of SW3*/
uint8_t statusSW3 = 0;// status of SW3
uint8_t sttRecord_SW3 = 0; //STT record when click SW3
uint8_t sttRecord1_SW3 = 0;
uint8_t sttRecord2_SW3 = 0;
/*Define arguments of SW1*/
uint8_t statusRecord_SW1 = 0;
uint8_t statusSW1 = 0;
uint8_t sttRecord_SW1 =0; //STT record when click SW1
uint8_t sttRecord1_SW1 = 0;
uint8_t sttRecord2_SW1 = 0;
/*Define arguments of SW2*/
uint8_t statusRecord_SW2 = 0;
uint8_t statusSW2 = 0;
/*Define arguments nomal*/
uint8_t line = 8; //LCD_Line2
uint8_t g_time_chattering = 0;
uint8_t g_time_wait2s = 0;
uint8_t i = 0; //Index of Array
uint8_t j,l = 0; //Arguments of for
uint8_t k = 0; // New update of records
char * string_shown_on_lcd[10];
/*define second and minute of SW1*/
uint8_t second1_SW1 = 0;
uint8_t minute1_SW1 = 0;
uint8_t second2_SW1 = 0;
uint8_t minute2_SW1 = 0;
/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/******************************************************************************
Private global variables and functions
******************************************************************************/
uint8_t centiSecond[20];
uint8_t second[20];
uint8_t minute[20];
/******************************************************************************
* Function Name: intp8_isr
* Description : Interrupt service routine for INTP8 (SW2)
* : TODO: Complete the program
* Arguments : none
* Return Value : none
******************************************************************************/
__interrupt void intp8_isr(void)
{
EI();
/*When the stopwatch is running, if both SW1 and SW2 are pressed at the same time,
the stopwatch will be changed to pausing state. In this state, the time is not counted.*/
if(statusSW3 == RECORDING)
{
if (SW2 == SW_ON)
{
if(SW1 == SW_ON)
{
timer_stop();
statusSW3 = PAUSE;
DisplayLCD(0, (uint8_t *)"Pausing.....");
}
else
{
;
}
}
}
/*When the stopwatch is pausing, if both SW1 and SW2 are pressed at the same time,
the stopwatch will be reset and in pausing state.*/
else
{
if(statusSW3 == PAUSE)
{
if (SW1 == SW_ON)
{
if(SW2 == SW_ON)
{
statusSW3 = RUNNING;
statusSW2 = NO_SCROLLDOWN;
statusSW1 = NO_SCROLLUP;
g_time.minute1 = 0;
g_time.minute2 = 0;
g_time.second1 = 0;
g_time.second2 = 0;
g_time.centiSecond = 0;
g_time.minute = 0;
g_time.second = 0;
centiSecond[20]= 0;
second[20] = 0;
minute[20] = 0;
i = 0;
sttRecord_SW3 = 0;
line = 8;
sttRecord_SW1 = 0;
/* Clear LCD display */
ClearLCD();



/* Print message to the first line of LCD */
DisplayLCD(0, (uint8_t *)"Pausing.....");
sprintf(string_shown_on_lcd, "%d%d:%d%d:%d0",g_time.minute1,
g_time.minute2,g_time.second1,g_time.second2,g_time.centiSecond);
DisplayLCD(8, string_shown_on_lcd);
}
else
{
;
}
}

}
}
if(g_time_chattering > 4)
{
g_time_chattering = 0;
statusSW2 = SCROLLDOWN;
if(sttRecord_SW1 < 6)
{
statusRecord_SW2 = LAST_RECORD;
DisplayLCD(0, (uint8_t *)"Last record ");
g_time_wait2s = 0;
}
else
{
if(sttRecord_SW1 == sttRecord_SW3)
{
statusRecord_SW2 = LAST_RECORD;
DisplayLCD(0, (uint8_t *)"Last record ");
g_time_wait2s = 0;
}
else
{
line = 8;
sttRecord_SW1 = sttRecord_SW1 + 1;
for(l = sttRecord_SW1-5 ; l <= sttRecord_SW1; l++)
{
line = line + 8;
second1_SW1 = second[l] / 10;
second2_SW1 = second[l] % 10;
minute1_SW1 = minute[l] / 10;
minute2_SW1 = minute[l] % 10;
sttRecord1_SW1 = l / 10;
sttRecord2_SW1 = l % 10;
sprintf(string_shown_on_lcd, "#%d%d:%d%d:%d%d:%d0",
sttRecord1_SW1,sttRecord2_SW1,minute1_SW1,minute2_SW1,second1_SW1,
second2_SW1,centiSecond[l]);
DisplayLCD(line, string_shown_on_lcd);
}
}
}
if(sttRecord_SW1 == 0)
{
statusRecord_SW2 = NO_RECORD;
DisplayLCD(0, (uint8_t *)"No Record ");
g_time_wait2s = 0;
}

}
}



/******************************************************************************
* Function Name: intp9_isr
* Description : Interrupt service routine for INTP9 (SW3)
* : TODO: Complete the program
* Arguments : none
* Return Value : none
******************************************************************************/
__interrupt void intp9_isr(void)
{
EI();
if(statusSW3 == RUNNING)
{
DisplayLCD(0, (uint8_t *)"Running.....");
statusSW3 = RECORDING;
timer_stop();
timer_start();
}
else
{

if(g_time_chattering > 4)
{
g_time_chattering = 0;
DisplayLCD(0, (uint8_t *)"Running.....");
if(statusSW1 == SCROLLUP)
{
i++;
centiSecond[i] = g_time.centiSecond;
second[i] = g_time.second;
minute[i] = g_time.minute;
sttRecord_SW3++;
}
else
{
if(statusSW1 == NO_SCROLLUP || statusSW2 == NO_SCROLLDOWN)
{
i++;
centiSecond[i] = g_time.centiSecond;
second[i] = g_time.second;
minute[i] = g_time.minute;
sttRecord_SW3++;
sttRecord_SW1 = sttRecord_SW3;
line = line + 8;
if(sttRecord_SW3 > 6)
{
k = sttRecord_SW3 - 6;
line = 8;
for(j = 1; j < 7; j++)
{
line = line + 8;
sttRecord_SW3 = j+k;
g_time.second1 = second[j+k] / 10;
g_time.second2 = second[j+k] % 10;
g_time.minute1 = minute[j+k] / 10;
g_time.minute2 = minute[j+k] % 10;
sttRecord1_SW3 = sttRecord_SW3 / 10;
sttRecord2_SW3 = sttRecord_SW3 % 10;
sprintf(string_shown_on_lcd, "#%d%d:%d%d:%d%d:%d0",
sttRecord1_SW3,sttRecord2_SW3,g_time.minute1,
g_time.minute2,g_time.second1,g_time.second2,centiSecond[j+k]);
DisplayLCD(line, string_shown_on_lcd);
}
}
else
{
g_time.second1 = second[i] / 10;
g_time.second2 = second[i] % 10;
g_time.minute1 = minute[i] / 10;
g_time.minute2 = minute[i] % 10;
sttRecord1_SW3 = sttRecord_SW3 / 10;
sttRecord2_SW3 = sttRecord_SW3 % 10;
sprintf(string_shown_on_lcd, "#%d%d:%d%d:%d%d:%d0",
sttRecord1_SW3,sttRecord2_SW3,g_time.minute1,g_time.minute2,g_time.second1,
g_time.second2,centiSecond[i]);
DisplayLCD(line, string_shown_on_lcd);
}
}
}
}
}
if(statusSW3 == PAUSE)
{
statusSW3 = RECORDING;
timer_start();
}






}



/******************************************************************************
* Function Name: intp10_isr
* Description : Interrupt service routine for INTP10 (SW1)
* : TODO: Complete the program
* Arguments : none
* Return Value : none
******************************************************************************/
__interrupt void intp10_isr(void)
{
EI();
/*When the stopwatch is running, if both SW1 and SW2 are pressed at the same time,
the stopwatch will be changed to pausing state. In this state, the time is not counted.*/
if(statusSW3 == RECORDING)
{
if (SW1 == SW_ON)
{
if(SW2 == SW_ON)
{
timer_stop();
statusSW3 = PAUSE;
DisplayLCD(0, (uint8_t *)"Pausing.....");
}
else
{
;
}
}
}
/*When the stopwatch is pausing, if both SW1 and SW2 are pressed at the same time,
the stopwatch will be reset and in pausing state.*/
else
{
if(statusSW3 == PAUSE)
{
if (SW1 == SW_ON)
{
if(SW2 == SW_ON)
{
statusSW3 = RUNNING;
statusSW2 = NO_SCROLLDOWN;
statusSW1 = NO_SCROLLUP;
g_time.minute1 = 0;
g_time.minute2 = 0;
g_time.second1 = 0;
g_time.second2 = 0;
g_time.centiSecond = 0;
g_time.minute = 0;
g_time.second = 0;
centiSecond[20]= 0;
second[20] = 0;
minute[20] = 0;
i = 0;
sttRecord_SW3 = 0;
line = 8;
sttRecord_SW1 = 0;

/* Clear LCD display */
ClearLCD();



/* Print message to the first line of LCD */
DisplayLCD(0, (uint8_t *)"Pausing.....");
sprintf(string_shown_on_lcd, "%d%d:%d%d:%d0",g_time.minute1,
g_time.minute2,g_time.second1,g_time.second2,g_time.centiSecond);
DisplayLCD(8, string_shown_on_lcd);
}
else
{
;
}
}

}
}
if(g_time_chattering > 4)
{
g_time_chattering = 0;
statusSW1 = SCROLLUP;
if(sttRecord_SW3 > 20)
{
if(sttRecord_SW1 < (sttRecord_SW3 - 13))
{
sttRecord_SW1 = sttRecord_SW1 + 1;
statusRecord_SW1 = FIRST_RECORD;
DisplayLCD(0, (uint8_t *)"First record");
g_time_wait2s = 0;
line = 8;
for(l = sttRecord_SW3 - 19 ; l < sttRecord_SW3 - 13 ; l++)
{
line = line + 8;
second1_SW1 = second[l] / 10;
second2_SW1 = second[l] % 10;
minute1_SW1 = minute[l] / 10;
minute2_SW1 = minute[l] % 10;
sttRecord1_SW1 = l / 10;
sttRecord2_SW1 = l % 10;
sprintf(string_shown_on_lcd, "#%d%d:%d%d:%d%d:%d0",
sttRecord1_SW1,sttRecord2_SW1,minute1_SW1,minute2_SW1,second1_SW1,
second2_SW1,centiSecond[l]);
DisplayLCD(line, string_shown_on_lcd);
}

}
else
{
line = 8;
sttRecord_SW1 = sttRecord_SW1 - 1;
for(l = sttRecord_SW1-5 ; l <= sttRecord_SW1; l++)
{
line = line + 8;
second1_SW1 = second[l] / 10;
second2_SW1 = second[l] % 10;
minute1_SW1 = minute[l] / 10;
minute2_SW1 = minute[l] % 10;
sttRecord1_SW1 = l / 10;
sttRecord2_SW1 = l % 10;
sprintf(string_shown_on_lcd, "#%d%d:%d%d:%d%d:%d0",
sttRecord1_SW1,sttRecord2_SW1,minute1_SW1,minute2_SW1,second1_SW1,
second2_SW1,centiSecond[l]);
DisplayLCD(line, string_shown_on_lcd);
}
}
}
else
{
if(sttRecord_SW1 <= 6)
{
statusRecord_SW1 = FIRST_RECORD;
DisplayLCD(0, (uint8_t *)"First record");
g_time_wait2s = 0;
}
else
{
line = 8;
sttRecord_SW1 = sttRecord_SW1 - 1;
for(l = sttRecord_SW1-5 ; l <= sttRecord_SW1; l++)
{
line = line + 8;
second1_SW1 = second[l] / 10;
second2_SW1 = second[l] % 10;
minute1_SW1 = minute[l] / 10;
minute2_SW1 = minute[l] % 10;
sttRecord1_SW1 = l / 10;
sttRecord2_SW1 = l % 10;
sprintf(string_shown_on_lcd, "#%d%d:%d%d:%d%d:%d0",
sttRecord1_SW1,sttRecord2_SW1,minute1_SW1,minute2_SW1,second1_SW1,
second2_SW1,centiSecond[l]);
DisplayLCD(line, string_shown_on_lcd);
}
}

}
if(sttRecord_SW1 == 0)
{
statusRecord_SW1 = NO_RECORD;
DisplayLCD(0, (uint8_t *)"No Record ");
g_time_wait2s = 0;
}
}


}



/******************************************************************************
End of file
******************************************************************************/