# CPE_301_Final
# Group members: Jack Garbe, Alfonso Custodio, Ryan Krysinski

#include <dht.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <Wire.h>

RTC_DS1307 rtc;
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
dht DHT;

volatile unsigned char* port_a = (unsigned char*) 0x22; 
volatile unsigned char* ddr_a = (unsigned char*)  0x21; 
volatile unsigned char* pin_a = (unsigned char*)  0x20; 
volatile unsigned char* port_b = (unsigned char*) 0x25; 
volatile unsigned char* ddr_b = (unsigned char*)  0x24; 
volatile unsigned char* pin_b = (unsigned char*)  0x23; 
volatile unsigned char* port_c = (unsigned char*) 0x28; 
volatile unsigned char* ddr_c = (unsigned char*)  0x27; 
volatile unsigned char* pin_c = (unsigned char*)  0x26; 
volatile unsigned char* port_d = (unsigned char*) 0x2B; 
volatile unsigned char* ddr_d = (unsigned char*)  0x2A; 
volatile unsigned char* pin_d = (unsigned char*)  0x29; 
volatile unsigned char* port_e = (unsigned char*) 0x2E; 
volatile unsigned char* ddr_e = (unsigned char*)  0x2D; 
volatile unsigned char* pin_e = (unsigned char*)  0x2C; 
volatile unsigned char* port_f = (unsigned char*) 0x31; 
volatile unsigned char* ddr_f = (unsigned char*)  0x30; 
volatile unsigned char* pin_f = (unsigned char*)  0x2F; 
volatile unsigned char* port_g = (unsigned char*) 0x34; 
volatile unsigned char* ddr_g = (unsigned char*)  0x33; 
volatile unsigned char* pin_g = (unsigned char*)  0x32; 
volatile unsigned char* port_h = (unsigned char*) 0x102; 
volatile unsigned char* ddr_h = (unsigned char*)  0x101; 
volatile unsigned char* pin_h = (unsigned char*)  0x100; 
volatile unsigned char* port_j = (unsigned char*) 0x105; 
volatile unsigned char* ddr_j = (unsigned char*)  0x104; 
volatile unsigned char* pin_j = (unsigned char*)  0x103; 
volatile unsigned char* port_k = (unsigned char*) 0x108; 
volatile unsigned char* ddr_k = (unsigned char*)  0x107; 
volatile unsigned char* pin_k = (unsigned char*)  0x106; 
volatile unsigned char* port_l = (unsigned char*) 0x10B; 
volatile unsigned char* ddr_l = (unsigned char*)  0x10A; 
volatile unsigned char* pin_l = (unsigned char*)  0x109; 

volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;
 
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

volatile unsigned char *myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1 = (unsigned char *) 0x6F;
volatile unsigned int  *myTCNT1  = (unsigned  int *) 0x84;
volatile unsigned char *myTIFR1 =  (unsigned char *) 0x36;

int tempC;
int tempF;
#define DHT11_PIN 51

#define RDA 0x80
#define TBE 0x20
  
unsigned int voltage = 0;
int waterthreshold = 300;
int tempthreshold = 80;

int state = 0;
int water = 0;

int dirStatus = 3;
int oldDir=3;

char t[32];
char Disabled[] = "Disabled ";
char Idle[] = "Idle ";
char Running[] = "Running ";
char Error[] = "Error ";
char To[] = "To ";
char FanOn[] = "Fan On ";
char FanOff[] = "Fan Off ";
char Counter[] = "Stepper Motor CCW Adjustment ";
char Clock[]= "Stepper Motor CW Adjustment ";

unsigned int currentTicks;
unsigned int timer_running;
/*states:
0-Disabled
1- Idle
2- Running
999-ERROR

Start button: D22 AKA PA0 INPUT
Stop button: D23 AKA PA1 INPUT
Reset button: D24 AKA PA2 INPUT
LED R: D25 AKA PA3 OUT
LED G: D26 AKA PA4 OUT
LED B: D27 AKA PA5 OUT
CW stepper button: D28 PA6 INPUT
CCW stepper button: D29 PA7 INPUT
FAN ENABLE: D53 PB0 OUT
*/

void setup() 
{
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  lcd.begin(16, 2);
  // setup the UART
  U0init(9600);
  // setup the ADC
  adc_init();
  
  //pinModes for STEPPER motor
  *ddr_b |= 0xF0;   //Set DDRB4-DDRB7 to 1(output) using |= 1111 0000 which is 0xF0

  //pinModes for fan motor
  *ddr_b |= 0x01;   //Set DDRB0 to 1(output) using |= 0000 0001 which is 0x01
  
  /*pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);  */
  
  *ddr_a &= 0xF8;   //Set DDRA0-DDRA2 to 0(input) using &= 1111 1000 which is 0xF8
  //D22-D24 Default State (not pressed) is HIGH
  *port_a |= 0x07;  //Set PORTA0-PORTA2 to 1(pullup resistor) using |= 0000 0111 which is 0x07
  *ddr_a |= 0x38;   //Set DDRA3-DDRA5 to 1(output) using |= 0011 1000 which is 0x38

  *ddr_a &= 0x3F; //Set DDRA6-DDRA7 to 0(input) using &= 0011 1111 which is 0x3F
  *port_a |= 0xC0; //Set PORTA6-PORTA7 to 1(pullup resistor) using |= 1100 0000 which is 0xC0

  *ddr_c &= 0x7F; //Set DDRC7 0(input) using &= 0111 1111 which is 0x7F
  *port_c |= 0x80; //Set PORTC7 to 1(pullup resistor) using |= 1000 0000 which is 0x80  
}

void loop() 
{
  switch (state)
  {
    case 0:
      state=disabled();
      break;
    case 1:
      state=idle();
      break;
    case 2:
      state=Run();
      break;
    case 999:
      state=error();
      break;
    default:
    break;
  }
  if(state != 0)
  {
    stepper();
    if (state !=999)
    {
      state=temphum();
    }
  }
  if (state == 2)
  {
  
    fanon();
  }
  else
  {
    fanoff();
  }
  //DateTime now = rtc.now();
  //sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());  
  //printTime();
  //delay(1000);
  //Serial.println(state);
 
}

void printTime()
{
  DateTime now = rtc.now();
  sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());  
  printString(t);
  U0putchar('\n');
}

void printString(char str[])
{
  int len = strlen(str);
  for(int i=0;i<len;i++)
  {
    U0putchar(str[i]);
  }
}
int temphum()
{
  int chk = DHT.read11(DHT11_PIN);
  tempC=DHT.temperature;
  tempF=(tempC*1.8) + 32;
  int tempFs=tempF;
  unsigned char d3 = tempFs/100 + '0';
  int d3a=tempFs/100;
  tempFs=tempFs-(d3a*100);
  unsigned char d2 = tempFs/10 + '0';
  int d2a=tempFs/10;
  tempFs=tempFs-(d2a*10);
  unsigned char d1 = tempFs/1 + '0';
  
  /*DISPLAY TEMP AND HIMIDITY TO 
  tempFs=tempF;
  U0putchar('T');
  U0putchar('E');
  U0putchar('M');
  U0putchar('P');
  U0putchar('E');
  U0putchar('R');
  U0putchar('A');
  U0putchar('T');
  U0putchar('U');
  U0putchar('R');
  U0putchar('E');
  U0putchar(':');
  U0putchar(' ');
  U0putchar(d3);
  U0putchar(d2);
  U0putchar(d1);
  U0putchar('\n');

  int humi=DHT.humidity;
  int humis=humi;
  d3 = humis/100 + '0';
  d3a = humis/100;
  humis=humis-(d3a*100);
  d2 = humis/10 + '0';
  d2a=humis/10;
  humis=humis-(d2a*10);
  d1 = humis/1 + '0';
  
  U0putchar('H');
  U0putchar('U');
  U0putchar('M');
  U0putchar('I');
  U0putchar('D');
  U0putchar('I');
  U0putchar('T');
  U0putchar('Y');
  U0putchar(':');
  U0putchar(' ');
  U0putchar(d3);
  U0putchar(d2);
  U0putchar(d1);
  U0putchar('\n');
  */
  
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  lcd.print(tempF);

  int humi=DHT.humidity;
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humi);


  //lcd.clear();
  
  if((state == 1) && (tempF>tempthreshold))
  {
    printString(Idle);
    printString(To);
    printString(Running);
    printTime();
    printString(FanOn);
    printTime();
    nocolor();
    return 2;
  }
  if((state==2) && (tempF <= tempthreshold))
  {
    printString(Running);
    printString(To);
    printString(Idle);
    printTime();
    printString(FanOff);
    printTime();
    nocolor();
    return 1;
  }
  return state;
}
int waterlevel()
{
  //1023
  voltage = adc_read(8);
  unsigned char d3 = voltage/1000 + '0'; //1
  int d3a = voltage/1000;
  
  voltage = voltage - (d3a*1000); //0023
  unsigned char d2 = voltage/100 + '0';//0
  int d2a = voltage/100;
  
  voltage = voltage - (d2a*100); //0023
  unsigned char d1 = voltage/10 + '0'; //2
  int d1a = voltage/10;
  
  unsigned char d0 = voltage - (d1a*10) + '0';
  int d0a=voltage;
  
  /*PRINT WATER LEVEL
  U0putchar(d3);     // echo character
  U0putchar(d2);
  U0putchar(d1);
  U0putchar(d0);
  U0putchar('\n');
  */

  return d3a*1000 + d2a*100 + d1a*10 +d0a;
}
int disabled() //0
{
  lcd.clear();
  yellow();
  //if A0(Start Button) HIGH(not pressed) 0000 0001
  if (*pin_a & 0x01)
  {
  }
  else //A0(Start Button) LOW(pressed)
  {
    printString(Disabled);
    printString(To);
    printString(Idle);
    printTime();
    nocolor();
    return 1;
  }
  return 0;
}

int idle()
{
  green();
  water=waterlevel();
  if(water<waterthreshold)
  {
    printString(Idle);
    printString(To);
    printString(Error);
    printTime();
    nocolor();
    lcd.clear();
    return 999;
  }
  //if A1(Stop Button) HIGH(not pressed) 0000 0010
  if (*pin_a & 0x02)
  {
  }
  else //A1(Stop Button) LOW(pressed)
  {
    nocolor();
    printString(Idle);
    printString(To);
    printString(Disabled);
    printTime();
    return 0;
  }
  return 1;
}

int Run() //CAPTAL R
{
  blue();
  water=waterlevel();
  if(water<waterthreshold)
  {
    printString(Running);
    printString(To);
    printString(Error);
    printTime();
    printString(FanOff);
    printTime();
    nocolor();
    lcd.clear();
    return 999;
  }
  //if A1(Stop Button) HIGH(not pressed) 0000 0010
  if (*pin_a & 0x02)
  {
  }
  else //A1(Stop Button) LOW(pressed)
  {
    printString(Running);
    printString(To);
    printString(Disabled);
    printTime();
    printString(FanOff);
    printTime();
    nocolor();
    return 0;
  }
  return 2;
}

int error() 
{
  lcd.setCursor(0, 0);
  lcd.print("ERROR: REFILL");
  red();
  //if A1(Stop Button) HIGH(not pressed) 0000 0010
  if (*pin_a & 0x02)
  {
  }
  else //A1(Stop Button) LOW(pressed)
  {
    printString(Error);
    printString(To);
    printString(Disabled);
    printTime();
    nocolor();
    return 0;
  }

  //if C7(Reset Button) HIGH(not pressed) 1000 0000
  if (*pin_c & 0x80)
  {
  }
  else //A1(RESET Button) LOW(pressed)
  {
    printString(Error);
    printString(To);
    printString(Idle);
    printTime();
    nocolor();
    return 1;
  }
  return 999;
}

void red()
{
  //DRIVE PORTA3(r) HIGH, use 0000 1000 = 08 and |
  *port_a |= 0x08;
}

void green()
{
  //DRIVE PORTA4(g) HIGH, use 0001 0000 = 10 and |
  *port_a |= 0x10;
}

void blue()
{
  //DRIVE PORTA5(B) HIGH, use 0010 0000 = 20 and |
  *port_a |= 0x20;
}

void yellow()
{
  red();
  green();
}

void nocolor() //Reset LED
{
  //DRIVE PORTA3-PORTA5 LOW (LED OFF), use 1100 0111 = C7 and &
  *port_a &= 0xC7;
}

void fanon()
{
  *port_b |= 0x01; //DRIVE PORTB0 HIGH, use 0000 0001 =  and |
}

void fanoff()
{
  *port_b &= 0xFE; //DRIVE PORTB0 LOW, use 1111 1110 =  and &
}

void stepper()
{
  //if A7(CCW Button) HIGH(not pressed) 1000 0000
  if (*pin_a & 0x80)
  {
  }
  else //A7(CCW Button) LOW(pressed)
  {
    dirStatus=1;
  }
  
  //if A6(CW Button) HIGH(not pressed) 0100 0000
  if (*pin_a & 0x40)
  {
  }
  else//A6(CW Button) LOW(pressed)
  {
    dirStatus=2;
  }
  
  //If both are HIGH(not pressed)
  if((*pin_a & 0x40) && (*pin_a & 0x80))
  {
    dirStatus = 3;
  }
  //If both are low(pressed)
  if(!(*pin_a & 0x40) && !(*pin_a & 0x80))
  {
    dirStatus = 3;
  }

  if((oldDir == 1) && (dirStatus == 3))
  {
    printString(Counter);
    printTime();
  }
  if((oldDir == 2) && (dirStatus==3))
  {
    printString(Clock);
    printTime();    
  }
  oldDir=dirStatus;

  switch(dirStatus)
  {
    case 1:
      CCW();
      break;
    case 2:
      CW();
      break;
    case 3:
      break;
    default:
      break;
  }
}
void CCW()
{
  *port_b |= 0x80; //DRIVE PORTB7 HIGH, use 1000 0000 =  and |
  delay1ms(3);
  resetstep();
  
  *port_b |= 0x40; //DRIVE PORTB6 HIGH, use 0100 0000 =  and |
  delay1ms(3);
  resetstep();
  
  *port_b |= 0x20; //DRIVE PORTB5 HIGH, use 0010 0000 =  and |
  delay1ms(3);
  resetstep();

  *port_b |= 0x10; //DRIVE PORTB4 HIGH, use 0001 0000 =  and |
  delay1ms(3);
  resetstep();

}

void CW()
{
  *port_b |= 0x10; //DRIVE PORTB4 HIGH, use 0001 0000 =  and |
  delay1ms(3);
  resetstep(); 
  *port_b |= 0x20; //DRIVE PORTB5 HIGH, use 0010 0000 =  and |
  delay1ms(3);
  resetstep();
  *port_b |= 0x40; //DRIVE PORTB6 HIGH, use 0100 0000 =  and |
  delay1ms(3);
  resetstep();
  *port_b |= 0x80; //DRIVE PORTB7 HIGH, use 1000 0000 =  and |
  delay1ms(3);
  resetstep();
}

void resetstep()
{
  //DRIVE PORTB4-PORTB7 LOW (LEDs OFF), use 0000 1111 = 0F and &
      *port_b &= 0x0F;
}
void adc_init()
{
  // setup the A register
  *my_ADCSRA |= 0b10000000; // set bit   7 to 1 to enable the ADC
  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0b11110111; // clear bit 4 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0b11111000; // clear bit 0-2 to 0 to set prescaler selection to slow reading
  // setup the B register
  *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0b11111000; // clear bit 2-0 to 0 to set free running mode
  // setup the MUX Register
  *my_ADMUX  &= 0b01111111; // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
}

unsigned int adc_read(unsigned char adc_channel_num)
{
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX  &= 0b11100000;
  // clear the channel selection bits (MUX 5)
  *my_ADCSRB &= 0b11110111;
  // set the channel number
  if(adc_channel_num > 7)
  {
    // set the channel selection bits, but remove the most significant bit (bit 3)
    adc_channel_num -= 8;
    // set MUX bit 5
    *my_ADCSRB |= 0b00001000;
  }
  // set the channel selection bits
  *my_ADMUX  += adc_channel_num;
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register
  return *my_ADC_DATA;
}

void U0init(int U0baud)
{
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}
unsigned char U0kbhit()
{
  return *myUCSR0A & RDA;
}
unsigned char U0getchar()
{
  return *myUDR0;
}
void U0putchar(unsigned char U0pdata)
{
  while((*myUCSR0A & TBE)==0);
  *myUDR0 = U0pdata;
}

void setup_timer_regs()
{
  // setup the timer control registers
  *myTCCR1A= 0x00;
  *myTCCR1B= 0X00;
  *myTCCR1C= 0x00;
  
  // reset the TOV flag
  *myTIFR1 |= 0x01;
  
  // enable the TOV interrupt
  *myTIMSK1 |= 0x01;
}


// TIMER OVERFLOW ISR
ISR(TIMER1_OVF_vect)
{
  // Stop the Timer
  *myTCCR1B &=0xF8;
  // Load the Count
  *myTCNT1 =  (unsigned int) (65535 -  (unsigned long) (currentTicks));
  // Start the Timer
  *myTCCR1B |=0x01;
  // if it's not the STOP amount
  if(currentTicks != 65535)
  {
    // XOR to toggle PB6
    *port_b ^= 0x40;
  }
}

void delay1ms(unsigned int numOfms) //delay in ms (like delay)
{
  for(int i=0;i<numOfms;i++)
  {
    unsigned int ticks = 16000;
    // stop the timer
    *myTCCR1A = 0x00;
    *myTCCR1B &= 0xF8;
    // set the counts
    *myTCNT1 = (unsigned int) (65536 - ticks);
    // start the timer
    * myTCCR1B |= 0b00000001;
    // wait for overflow
    while((*myTIFR1 & 0x01)==0); 
    // stop the timer
    *myTCCR1B &= 0xF8;  
    // reset TOV           
    *myTIFR1 |= 0x01;
  }
}
