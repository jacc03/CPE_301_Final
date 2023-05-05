#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHT11PIN 7
#define DHTTYPE DHT11

LiquidCrystal_I2C lcd(0x3f, 16, 2);
DHT dht(DHT11PIN, DHTTYPE);

void setup(){
  dht.begin();
  lcd.init();
  lcd.backlight();
}

void loop(){
  delay(1000); // This allows us to wait between 1s measurements. 
  float humidity = dht.readHumidity();
  float temp = dht.readReadTemperature();
  lcd.clear();
  //This checks for the edge case 
  if (isnan(humidity) || isnan(temp)){
    lcd.setCursor(0,0);
    lcd.print("ERROR: Read failed, please check sensor")
  }
  else{
    lcd.setCursor(0,0);
    lcd.print("Current Temperature: ");
    lcd.print(temp);
    lcd.print((char)223);
    lcd.setCursor(0,1);
    lcd.print("Current Humidity:");
    lcd.print(humidity);
    lcd.print((char)223);
  }
}
