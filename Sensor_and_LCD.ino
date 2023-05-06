#include <LiquidCrystal.h>

uint8_t data[5];
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

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
  //int value = DHT.read11(7);
  //float humidity = DHT.humidity();
  //float temp = DHT.temperature();
  pinMode(7, INPUT_PULLUP);
  delay(1000); // This allows us to wait between 1s measurements. 
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  uint32_t tm = Signal(HIGH);  
  tm = Signal(LOW);
  tm = Signal(HIGH);
  uint8_t signals[80];
  for(uint8_t i = 0; i<80; i+=2){
    signals[i] = Signal(LOW);  // there are low signals also
    signals[i+1] = Signal(HIGH);  // our HIGH state signals
  }
  for (uint8_t a = 0; a<5; a++){
    for (uint8_t i=0; i<16; i+=2){
      data[a] <<= 1;
      if(signals[16 * a + i] < signals[16*a+i+1]) data[a] |= 1;
    }
  }
  //This checks for the edge case 
  if (isnan(data[0]) || isnan(data[2])){
    lcd.setCursor(0,0);
    lcd.print("ERROR: Read failed, please check sensor");
  }
  else{
    lcd.setCursor(0,0);
    lcd.print("Current Temperature: ");
    lcd.print(data[0]);
    lcd.print((char)223);
    lcd.setCursor(0,1);
    lcd.print("Current Humidity:");
    lcd.print(data[2]);
    lcd.print((char)223);
    lcd.clear();
  }
}
