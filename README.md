# CPE_301_Final
# Group members: Jack Garbe, Alfonso Custodio, Ryan Krysinski

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

int state = 0;
/*states:
0-Disabled
1- Idle
2- Running
999-ERROR

Start button: D22 AKA PA0 INPUT
Stop button: D23 AKA PA1 INPUT
Reset button: D24 AKA PA2 INPUT
LED R: D25 AKA PA3
LED G: D26 AKA PA4
LED B: D27 AKA PA5
*/

void setup() 
{
  *ddr_a &= 0xF8;   //Set DDRA0-DDRA2 to 0(input) using &= 1111 1000 which is 0xF8
  //D22-D24 Default State (not pressed) is HIGH
  *port_a |= 0x07;  //Set PORTA0-PORTA2 to 1(pullup resistor) using |= 0000 0111 which is 0x07
  *ddr_a |= 0x38;   //Set DDRA3-DDRA5 to 1(output) using |= 0011 1000 which is 0x38
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
  
  }
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
  
}

int error() 
{
  
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
