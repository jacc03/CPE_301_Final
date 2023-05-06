# CPE_301_Final
# Group members: Jack Garbe, Alfonso Custodio, Ryan Krysinski

#include <dht.h>
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

int tempC;
int tempF;
#define DHT11_PIN 8

#define RDA 0x80
#define TBE 0x20
  
unsigned int voltage = 0;
int waterthreshold = 300;
int tempthreshold = 75;

int state = 0;
int water = 0;

int pin1 = 10; //IN1 D10 PB4
int pin2 = 11; //    D11
int pin3 = 12; //    D12
int pin4 = 13; //IN4 D13 PB7

int stepMotor = 0;
int dirStatus = 3;
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

*/

void setup() 
{
  // setup the UART
  U0init(9600);
  // setup the ADC
  adc_init();
  
  //pinModes for STEPPER motor
  *ddr_b |= 0xF0;   //Set DDRB4-DDRB7 to 1(output) using |= 1111 0000 which is 0xF0
  
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
  Serial.begin(9600);
  
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
  Serial.println(state);
}

int temphum()
{
  int chk = DHT.read11(DHT11_PIN);
  tempC=DHT.temperature;
  tempF=(tempC*1.8) + 32;
  Serial.print("Temperature = ");
  Serial.println(tempF);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  if((state == 1) && (tempF>tempthreshold))
  {
    nocolor();
    return 2;
  }
  if((state==2) && (tempF <= tempthreshold))
  {
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
  
  
  U0putchar(d3);     // echo character
  U0putchar(d2);
  U0putchar(d1);
  U0putchar(d0);
  U0putchar('\n');

  return d3a*1000 + d2a*100 + d1a*10 +d0a;
}
int disabled()
{
  yellow();
  //if A0(Start Button) HIGH(not pressed) 0000 0001
  if (*pin_a & 0x01)
  {
  }
  else //A0(Start Button) LOW(pressed)
  {
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
    nocolor();
    return 999;
  }
  //if A1(Stop Button) HIGH(not pressed) 0000 0010
  if (*pin_a & 0x02)
  {
  }
  else //A1(Stop Button) LOW(pressed)
  {
    nocolor();
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
    nocolor();
    return 999;
  }
  //if A1(Stop Button) HIGH(not pressed) 0000 0010
  if (*pin_a & 0x02)
  {
  }
  else //A1(Stop Button) LOW(pressed)
  {
    nocolor();
    return 0;
  }
  return 2;
}

int error() 
{
  red();
  //if A1(Stop Button) HIGH(not pressed) 0000 0010
  if (*pin_a & 0x02)
  {
  }
  else //A1(Stop Button) LOW(pressed)
  {
    nocolor();
    return 0;
  }

  //if C7(Reset Button) HIGH(not pressed) 1000 0000
  if (*pin_c & 0x80)
  {
  }
  else //A1(Stop Button) LOW(pressed)
  {
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
  
}

void fanoff()
{
  
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
  delay(5);
  resetstep();
  
  *port_b |= 0x40; //DRIVE PORTB6 HIGH, use 0100 0000 =  and |
  delay(5);
  resetstep();
  
  *port_b |= 0x20; //DRIVE PORTB5 HIGH, use 0010 0000 =  and |
  delay(5);
  resetstep();

  *port_b |= 0x10; //DRIVE PORTB4 HIGH, use 0001 0000 =  and |
  delay(5);
  resetstep();

}

void CW()
{
  *port_b |= 0x10; //DRIVE PORTB4 HIGH, use 0001 0000 =  and |
  delay(5);
  resetstep(); 
  *port_b |= 0x20; //DRIVE PORTB5 HIGH, use 0010 0000 =  and |
  delay(5);
  resetstep();
  *port_b |= 0x40; //DRIVE PORTB6 HIGH, use 0100 0000 =  and |
  delay(5);
  resetstep();
  *port_b |= 0x80; //DRIVE PORTB7 HIGH, use 1000 0000 =  and |
  delay(5);
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
