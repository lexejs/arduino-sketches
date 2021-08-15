#include <TM1637.h>

/*
 * TM1637.cpp
 * A library for the 4 digit display
 *
 * Copyright (c) 2012 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : Frankie.Chu
 * Create Time: 9 April,2012
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "TM1637.h"
#define CLK 2//pins definitions for TM1637 and can be changed to other ports
#define DIO 3
TM1637 tm1637(CLK,DIO);
 uint8_t Clkpin;
    uint8_t Datapin;
    
    uint8_t Cmd_SetData;
    uint8_t Cmd_SetAddr;
    uint8_t Cmd_DispCtrl;
    boolean _PointFlag;   
    int v1 = 600,v2 =600,v3 =600, val=0;
void setup()
{
  // Serial.begin(9600);
  tm1637.init();
  tm1637.set(3);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
}
void loop()
{
  int sensorValue = analogRead(A0);
v1=v2;
v2=v3;

v3=v2+0.2*(sensorValue-v2);

//val = (int)( v1/(float)2+v2/(float)4+v3/(float)4);
val =v3;// v2+0.1*(v3-v2);

//Serial.print(sensorValue);

//Serial.print("  ~");
//Serial.print(val);
//Serial.println("  ");  
  DigitDisplayWrite(CLK, DIO, val);
  delay(100);
}

 void display(uint8_t BitAddr,int8_t DispData){
   tm1637.display(BitAddr,DispData);
 }
  
void DigitDisplayWrite(int pinClk, int pinDta, int num)
{
        if(num>9999 || num<0)return;
        
        static int num_buf = 0;
        if(num == num_buf)return;
        num_buf = num;

        // init io
        Clkpin  = pinClk;
        Datapin = pinDta;
        pinMode(Clkpin, OUTPUT);
        pinMode(Datapin, OUTPUT);
  
        
        
        if(num<10)
        {
            display(3, num);
            display(2, 0x7f);
            display(1, 0x7f);
            display(0, 0x7f);
        }
        else if(num<100)
        {
            display(3, num%10);
            display(2, (num/10)%10);
            display(1, 0x7f);
            display(0, 0x7f);
        }
        else if(num<1000)
        {
            display(3, num%10);
            display(2, (num/10)%10);
            display(1, (num/100)%10);
            display(0, 0x7f);
        }
        else
        {
            display(3, num%10);
            display(2, (num/10)%10);
            display(1, (num/100)%10);
            display(0, (num/1000)%10);
        }
    }
