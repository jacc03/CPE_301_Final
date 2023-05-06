#include <LiquidCrystal.h>

LiquidCrystal lcd(13,12,11,10,9,8);

int humidity = 0;
int temperature = 0;
int temperaturecom = 0;
bool DHTFailure = false;

unsigned long ontime = 0;
unsigned long offtime = 0;

unsigned char datbyte[5];
unsigned int datpacket[40];
unsigned char bitdata = 0;
unsigned char checksum = 0;
int counter = 0;

void setup(){
  Serial.begin(9600);
  lcd.begin(16,2);
}

uint8_t Signal(bool pull){
  uint8_t count = 0;
  while(digitalRead(2) == pull){
    count++;
  }
  return count;
}

void loop(){
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  delay(18);
  digitalWrite(7, HIGH);
  pinMode(7, INPUT_PULLUP);
  offtime = pulseIn(7, LOW);
  if (offtime <= 84 && offtime >= 76) {
    while (true) {
      ontime = pulseIn(7, HIGH);
      if (ontime <= 28 && ontime >= 20) {
        bitdata = 0;
      }
      else if (ontime <= 74 && ontime >= 65) {
        bitdata = 1;
      }
      else if (counter == 40) {
        break;
      }
      datbyte[counter / 8] |= bitdata << (7 - (counter % 8));
      datpacket[counter] = bitdata;
      counter++;
    }
  }
  checksum = datbyte[0] + datbyte[1] + datbyte[2] + datbyte[3];
  if (checksum == datbyte[4] && checksum != 0) {
    lcd.setCursor(0,0);
    lcd.print("Humidity: ");
    lcd.print(datbyte[0]);
    lcd.print((char)223);
    lcd.setCursor(0,1);
    lcd.print("Temperature:");
    lcd.print(datbyte[2]);
    lcd.print((char)223);
    Serial.print(datbyte[1]);
    delay(60000);
  }
  counter = 0;
  datbyte[0] = datbyte[1] = datbyte[2] = datbyte[3] = datbyte[4] = 0;
  delay(1000);
  //Serial.print(data[0]);
}
