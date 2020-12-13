#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include<stdio.h>
#include <math.h>
#include "stdio.h"
#include "string.h"
#include <time.h>
#define _XTAL_FREQ 4000000
#define TMR2PRESCALE    16
// using namespace std;

// TONES  ==========================================
#define  c     261
#define  d     293
#define  e     329
#define  f     349
#define  g     392
#define  a     440
#define  b     493
#define  C     523
#define  D     587
#define  E     659
#define  F     698
#define  G     784
// Define a special note, 'R', to represent a rest
#define  R     0

int flag = 0;
int digital = 0;
float voltage = 0.0;
int i=0;
void PWM1_Duty(unsigned int duty);

void __interrupt(high_priority) Hi_ISR(void)
{
    if(flag == 0 && PIR1bits.TMR1IF) 
    {
        PIR1bits.TMR1IF = 0; //clear FLAG BIT
        i = i+39;
        PWM1_Duty(i);
        if(i == 624)
            flag = 1;
    }
    else if(flag == 1 && PIR1bits.TMR1IF)
    {
        PIR1bits.TMR1IF = 0;
        i = i-39;
        PWM1_Duty(i);
        if(i == 0)
            flag = 0;
    }
    digital=ADC_Read(7);
    voltage = digital* ((float)5/1023); 
    TMR1 = 55000 - 3000 *voltage;
    return ;
}

char str[10];//things i type in

float v[4];//my v, calculate average
char auto_print[5];//the fourth v that i print , for mode2
char print[5];//for mode1
float average=0.0;
char average_print[5];

void PORT_Initialize()
{
    TRISDbits.RD0 = 0;
    TRISDbits.RD1 = 0;
    TRISDbits.RD2 = 0;
    TRISDbits.RD3 = 0;
    TRISDbits.RD4 = 0;
    TRISDbits.RD5 = 0;
    TRISDbits.RD6 = 0;
    TRISDbits.RD7 = 0;
    TRISBbits.RB0 = 0;
    TRISBbits.RB1 = 0;
    TRISBbits.RB2 = 0;
    TRISBbits.RB3 = 0;
    LATD = 0;
    LATB = 0;
}

void PWM1_Init(long setDuty)
{
  PR2 = setDuty;
}

void PWM1_Duty(unsigned int duty)
{
//set duty to CCPR1L , CCP1X(CCP1CON:5) and CCP1Y(CCP1CON:4)
    CCPR1L = duty >> 2;
    CCP1CONbits.DC1B1 = (duty >> 1) & 1; 
    CCP1CONbits.DC1B0 = duty & 1;
}

void PWM1_Start()
{

  //set CCP1CON
  CCP1CONbits.CCP1M2 = 1;
  CCP1CONbits.CCP1M3 = 1;
  
  //set timer2
  T2CONbits.T2CKPS1 = 1;//set tmr2 prescaler to 16
  T2CONbits.TMR2ON = 1;//set timer2 on
  TRISCbits.RC2 = 0;//set ccp1 output
}

void TMR1_Init()
{
   RCONbits.IPEN = 1; //Enable priority levels on interrupts
   INTCONbits.GIE = 1; //Enables all high priority interrupts
   PIE1bits.TMR1IE = 1; //Enables the TMR2 to PR2 match interrupt
    
   T1CONbits.T1CKPS1 = 1; //PRESCALER = 1:8
   T1CONbits.T1CKPS1 = 1;
    
   T1CONbits.TMR1CS = 0; //Internal clock (FOSC/4) = 500K/4 
   T1CONbits.RD16 = 1;//Enables register read/write of TImer1 in one 16-bit operation
   
   T1CONbits.TMR1ON = 1;//Timer1 On bit

   digital=ADC_Read(7);
    voltage = digital* ((float)5/1023); 
    TMR1 = 55000 - 3000 *voltage;
   
}

//int tempo = 2000;

int current_tone = 0; //current playing note
int current_tone_duration = 0; //current note duration

void delay_ms(unsigned int milliseconds) {
   while(milliseconds > 0) {
      milliseconds--;
       __delay_us(990);
   }
}

void SetupClock() {
    OSCCONbits.IRCF = 0b110; //set internal oscillator to 4 Mhz
    OSCCONbits.SCS = 0b10; //use oscillator defined by FOSC
}

void MusicPWM1_Init(long desired_frequency) {
    PR2 = (_XTAL_FREQ / (desired_frequency * 4 * TMR2PRESCALE)) - 1;
    current_tone = desired_frequency;
}

int MusicPWM_Max_Duty() {
    return (_XTAL_FREQ / (current_tone * TMR2PRESCALE));
}

void MusicPWM1_Duty(unsigned int duty) {
    if (duty < 1024) {
        duty = ((float) duty / 1023) * MusicPWM_Max_Duty();
        CCP1CONbits.DC1B1 = duty & 2;
        CCP1CONbits.DC1B0 = duty & 1;
        CCPR1L = duty >> 2;
    }
}

void MusicPWM1_Mode_On() {
    CCP1M3 = 1;
    CCP1M2 = 1;
}

void MusicPWM1_Start() {
    MusicPWM1_Mode_On();
    T2CKPS0 = 1;
    T2CKPS1 = 1;

    TMR2ON = 1;
}

void MusicPWM1_Stop() {
    CCP1M3 = 0;
    CCP1M2 = 0;
}

void playTone() {
    //TODO: The Tone R is not implemented yet.
    if (current_tone > 0) {
        MusicPWM1_Init(current_tone);
        MusicPWM1_Duty(511);
        
        delay_ms(current_tone_duration);
        MusicPWM1_Duty(0);
        LATD = 0;
        LATB = 0;
    }
    else if(current_tone == 0){
         delay_ms(current_tone_duration);
    }
}

void Music()
{
    char *number = strtok(str,"X");
    int tempo = atoi(number);
    tempo = (tempo*15);
    
    ClearBuffer();
    int num;
    char MELODY_LENGTH;
    char str1[50];
    char str2[50];
    char str3[50];
    char str4[50];
    
    SetupClock();
    TRISBbits.RB3 = 0; //set Pin B3 as output
    TRISCbits.RC2 = 0;//set ccp1 output
    
    MusicPWM1_Init(500); //exact value doesn't matter at all, just not 0
    MusicPWM1_Duty(0);
    MusicPWM1_Start();
    
    /*while(1)
    {
        strcpy(str1, GetString());
        if(strstr(str1,"X"))    
            break;
    }
    ClearBuffer();
    char *number = strtok(str1,"X");
    //UART_Write_Text(number);
    int length = atoi(number);*/
    
    while(1)
    {
        strcpy(str1, GetString());
        if(strstr(str1,"X"))    
            break;
    }
    ClearBuffer();
    char *melody1 = strtok(str1,"X");
    
    while(1)
    {
        strcpy(str2, GetString());
        if(strstr(str2,"X"))    
            break;
    }
    ClearBuffer();
    char *melody2 = strtok(str2,"X");
    while(1)
    {
        strcpy(str3, GetString());
        if(strstr(str3,"X"))    
            break;
    }
    ClearBuffer();
    //UART_Write_Text(str2);
    char *melody3 = strtok(str3,"X");
    //UART_Write_Text(beat);
    while(1)
    {
        strcpy(str4, GetString());
        if(strstr(str4,"X"))    
            break;
    }
    ClearBuffer();
    char *melody4 = strtok(str4,"X");
    while(1){
        for(int i = 0; i < 8; i++) {
            ClearBuffer();
            if(melody1[i]=='c'){
                current_tone = 261;
                LATDbits.LATD0 = 1;
            }
            else if(melody1[i]=='d'){
                current_tone = 293;
                LATDbits.LATD1 = 1;
            }
            else if(melody1[i]=='e'){
                current_tone = 329;
                LATDbits.LATD2 = 1;
            }
            else if(melody1[i]=='f'){
                current_tone = 349;
                LATDbits.LATD3 = 1;
                LATDbits.LATD4 = 1;
            }
            else if(melody1[i]=='g'){
                current_tone = 392;
                LATDbits.LATD5 = 1;
            }
            else if(melody1[i]=='a'){
                current_tone = 440;
                 LATDbits.LATD6 = 1;
                 LATDbits.LATD7 = 1;
            }
            else if(melody1[i]=='b'){
                current_tone = 493;
                 LATBbits.LATB0 = 1;
                 LATBbits.LATB1 = 1;
            }
            else if(melody1[i]=='C'){
                current_tone = 523;
                LATBbits.LATB2 = 1;
            }
            else if(melody1[i]=='D'){
                current_tone = 587;
                //LATBbits.LATB0 = 1;
            }
            else if(melody1[i]=='E'){
                current_tone = 659;
               // LATBbits.LATB1 = 1;
            }
            else if(melody1[i]=='F'){
                current_tone = 698;
               // LATBbits.LATB2 = 1;
            }
            else if(melody1[i]=='G'){
                current_tone = 784;
               // LATBbits.LATB3 = 1;
            }
            else if(melody1[i]=='R'){
                current_tone = 0;
            }
            else if(melody4[i]=='Z'){
                current_tone = -1;
            }
            current_tone_duration = (tempo) / 4;
            playTone();
            delay_ms(tempo/200);
            strcpy(str, GetString());
            if(strstr (str,"S"))
            {
                while(1)
                {
                    strcpy(str, GetString());
                    if(strstr(str,"P"))
                        break;
                    else if(strstr(str, "Q")) {
                        ClearBuffer();
                        return;
                    }
                }
            }
            else if(strstr (str,"Q")){
                ClearBuffer();
                return;
          }
        }
        for (int i = 0; i < 8; i++) {
            ClearBuffer();
             if(melody2[i]=='c'){
                current_tone = 261;
                LATDbits.LATD0 = 1;
            }
            else if(melody2[i]=='d'){
                current_tone = 293;
                LATDbits.LATD1 = 1;
            }
            else if(melody2[i]=='e'){
                current_tone = 329;
                LATDbits.LATD2 = 1;
            }
            else if(melody2[i]=='f'){
                current_tone = 349;
                LATDbits.LATD3 = 1;
                LATDbits.LATD4 = 1;
            }
            else if(melody2[i]=='g'){
                current_tone = 392;
                LATDbits.LATD5 = 1;
            }
            else if(melody2[i]=='a'){
                current_tone = 440;
                 LATDbits.LATD6 = 1;
                 LATDbits.LATD7 = 1;
            }
            else if(melody2[i]=='b'){
                current_tone = 493;
                 LATBbits.LATB1 = 1;
                 LATBbits.LATB0 = 1;
            }
            else if(melody2[i]=='C'){
                current_tone = 523;
                LATBbits.LATB2 = 1;
            }
            else if(melody2[i]=='D'){
                current_tone = 587;
                //LATBbits.LATB0 = 1;
            }
            else if(melody2[i]=='E'){
                current_tone = 659;
              //  LATBbits.LATB1 = 1;
            }
            else if(melody2[i]=='F'){
                current_tone = 698;
               // LATBbits.LATB2 = 1;
            }
            else if(melody2[i]=='G'){
                current_tone = 784;
              //  LATBbits.LATB3 = 1;
            }
            else if(melody2[i]=='R'){
                current_tone = 0;
            }
            else if(melody2[i]=='Z'){
                current_tone = -1;
            }
            current_tone_duration = tempo / 4;
            playTone();
            delay_ms(tempo/200);
            strcpy(str, GetString());
            if(strstr (str,"S"))
            {
                while(1)
                {
                    strcpy(str, GetString());
                    if(strstr(str,"P"))
                        break;
                    else if(strstr(str, "Q")) {
                        ClearBuffer();
                        return;
                    }
                }
            }
            else if(strstr (str,"Q")){
                ClearBuffer();
                return;
          }
        }
        for (int i = 0; i < 8; i++) {
            ClearBuffer();
             if(melody3[i]=='c'){
                current_tone = 261;
                LATDbits.LATD0 = 1;
            }
            else if(melody3[i]=='d'){
                current_tone = 293;
                LATDbits.LATD1 = 1;
            }
            else if(melody3[i]=='e'){
                current_tone = 329;
                LATDbits.LATD2 = 1;
            }
            else if(melody3[i]=='f'){
                current_tone = 349;
                LATDbits.LATD3 = 1;
                LATDbits.LATD4 = 1;
            }
            else if(melody3[i]=='g'){
                current_tone = 392;
                LATDbits.LATD5 = 1;
            }
            else if(melody3[i]=='a'){
                current_tone = 440;
                 LATDbits.LATD6 = 1;
                 LATDbits.LATD7 = 1;
            }
            else if(melody3[i]=='b'){
                current_tone = 493;
                 LATBbits.LATB1 = 1;
                 LATBbits.LATB0 = 1;
            }
            else if(melody3[i]=='C'){
                current_tone = 523;
                LATBbits.LATB2 = 1;
            }
            else if(melody3[i]=='D'){
                current_tone = 587;
                //LATBbits.LATB0 = 1;
            }
            else if(melody3[i]=='E'){
                current_tone = 659;
               // LATBbits.LATB1 = 1;
            }
            else if(melody3[i]=='F'){
                current_tone = 698;
               // LATBbits.LATB2 = 1;
            }
            else if(melody3[i]=='G'){
                current_tone = 784;
                //LATBbits.LATB3 = 1;
            }
            else if(melody3[i]=='R'){
                current_tone = 0;
            }
            else if(melody4[i]=='Z'){
                current_tone = -1;
            }
            current_tone_duration = tempo / 4;
            playTone();
            delay_ms(tempo/200);
            strcpy(str, GetString());
            if(strstr (str,"S"))
            {
                while(1)
                {
                    strcpy(str, GetString());
                    if(strstr(str,"P"))
                        break;
                    else if(strstr(str, "Q")) {
                        ClearBuffer();
                        return;
                    }
                }
            }
            else if(strstr (str,"Q")){
                ClearBuffer();
                return;
          }
        }
        for (int i = 0; i < 8; i++) {
            ClearBuffer();
             if(melody4[i]=='c'){
                current_tone = 261;
                LATDbits.LATD0 = 1;
            }
            else if(melody4[i]=='d'){
                current_tone = 293;
                LATDbits.LATD1 = 1;
            }
            else if(melody4[i]=='e'){
                current_tone = 329;
                LATDbits.LATD2 = 1;
            }
            else if(melody4[i]=='f'){
                current_tone = 349;
                LATDbits.LATD3 = 1;
                LATDbits.LATD4 = 1;
            }
            else if(melody4[i]=='g'){
                current_tone = 392;
                LATDbits.LATD5 = 1;
            }
            else if(melody4[i]=='a'){
                current_tone = 440;
                 LATDbits.LATD6 = 1;
                 LATDbits.LATD7 = 1;
            }
            else if(melody4[i]=='b'){
                current_tone = 493;
                 LATBbits.LATB1 = 1;
                 LATBbits.LATB0 = 1;
            }
            else if(melody4[i]=='C'){
                current_tone = 523;
                LATBbits.LATB2 = 1;
            }
            else if(melody4[i]=='D'){
                current_tone = 587;
               // LATBbits.LATB0 = 1;
            }
            else if(melody4[i]=='E'){
                current_tone = 659;
                //LATBbits.LATB1 = 1;
            }
            else if(melody4[i]=='F'){
                current_tone = 698;
                //LATBbits.LATB2 = 1;
            }
            else if(melody4[i]=='G'){
                current_tone = 784;
               // LATBbits.LATB3 = 1;
            }
            else if(melody4[i]=='R'){
                current_tone = 0;
            }
            else if(melody4[i]=='Z'){
                current_tone = -1;
            }
            current_tone_duration = tempo / 4;
            playTone();
            delay_ms(tempo/200);
            strcpy(str, GetString());
            if(strstr (str,"S"))
            {
                while(1)
                {
                    strcpy(str, GetString());
                    if(strstr(str,"P"))
                        break;
                    else if(strstr(str, "Q")) {
                        ClearBuffer();
                        return;
                    }
                }
            }
            else if(strstr (str,"Q")){
                ClearBuffer();
                return;
            }
          } 
    }
    /*
    {
        int bee[] =      {G, E, E, R, F, D, D, R, C, D, E, F, G, G, G, R, G, 
        E, E, R, F, D, D, R, C, E, G, G, E, R};
        int bee_beats[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
        
        MELODY_LENGTH = sizeof (bee) / sizeof (bee[0]);
        
        while (1) {
        for (int i = 0; i < MELODY_LENGTH; i++) {
            ClearBuffer();
            current_tone = bee[i];
            current_tone_duration = tempo / bee_beats[i];

            playTone();
            delay_ms(tempo/200);
            strcpy(str, GetString());
            if(strstr (str,"s"))
            {
                while(1)
                {
                    strcpy(str, GetString());
                    if(strstr(str,"p"))
                        break;
                }
            }
            else if(strstr (str,"q"))
            {
                return;
            }
            else if(strstr (str,"clear")!= NULL){
                ClearBuffer();
            }
            else if(strstr (str,"blink")!= NULL){
            }
            else if(strstr (str,"breath")!= NULL){
            }
        }
        __delay_ms(2000);
    }
    }
    else if (num == 2)
    {
        int pirate[] =     {e, g, a, a, R, a, b, C, C, R, C, D, b, b, R, a, g, g, R};
        int pirate_beat[] = {4, 4, 8, 4, 4, 4, 4, 8, 4, 4, 4, 4, 8, 4, 4, 4, 4, 2, 4};
        MELODY_LENGTH = sizeof (pirate) / sizeof (pirate[0]);
        
        while (1) {
        for (int i = 0; i < MELODY_LENGTH; i++) {
            ClearBuffer();
            current_tone = pirate[i];
            current_tone_duration = tempo / pirate_beat[i];

            playTone();
            delay_ms(tempo/200);
            strcpy(str, GetString());
            if(strstr (str,"s"))
            {
                while(1)
                {
                    strcpy(str, GetString());
                    if(strstr(str,"p"))
                        break;
                }
            }
            else if(strstr (str,"q"))
            {
                return;
            }
            else if(strstr (str,"clear")!= NULL){
                ClearBuffer();
            }
            else if(strstr (str,"blink")!= NULL){
            }
            else if(strstr (str,"breath")!= NULL){
            }
        }
        __delay_ms(2000);
    }
    }
    else if (num == 3)
    {
        int melody[] = {g, g, a, g, C, b, g, g, a, g, D, C, g, g, G, E, C,
        b, a, F, F, E, C, D, C};
        int beats[] = {8, 8, 4, 4, 4, 2, 8, 8, 4, 4, 4, 2, 8, 8, 4, 4, 4,
        4, 4, 8, 8, 4, 4, 4, 2};
        MELODY_LENGTH = sizeof (melody) / sizeof (melody[0]);
        
        while (1) {
        for (int i = 0; i < MELODY_LENGTH; i++) {
            ClearBuffer();
            current_tone = melody[i];
            current_tone_duration = tempo / beats[i];

            playTone();
            delay_ms(tempo/200);
            strcpy(str, GetString());
            if(strstr (str,"s"))
            {
                while(1)
                {
                    strcpy(str, GetString());
                    if(strstr(str,"p"))
                        break;
                }
            }
            else if(strstr (str,"q"))
            {
                return;
            }
            else if(strstr (str,"clear")!= NULL){
                ClearBuffer();
            }
            else if(strstr (str,"blink")!= NULL){
            }
            else if(strstr (str,"breath")!= NULL){
            }
        }
        __delay_ms(2000);
    }
    }*/
}

void main(void) 
{
    SYSTEM_Initialize() ;
    OSCILLATOR_Initialize();
    PORT_Initialize();
   // TRISCbits.RC2 = 1;
    UART_Write_Text("---Main---");
    while(1) {
        strcpy(str, GetString());
        if(strstr (str,"Q")!= NULL){
            ClearBuffer();
        }
        else if(strstr (str,"X")!= NULL){
            Music();
        }
        // TODO 
        // "clear" > clear UART Buffer()
        // "mode1" > start Mode1 function as above
        // "mode2" > start Mode2 function as above
    }
    return; 
}