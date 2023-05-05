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
*/

void setup() 
{

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

}

int disabled()
{

}

int idle()
{
  
}

int Run() //CAPTAL R
{
  
}

int error() 
{
  
}
