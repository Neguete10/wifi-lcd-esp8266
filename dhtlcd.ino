#include "DHT.h"
#include <LiquidCrystal.h>

#define DHTPIN 2    
#define ESP8266 9 
#define DHTTYPE DHT11   // DHT

DHT dht(DHTPIN,DHTTYPE);

LiquidCrystal lcd(16, 12, 13, 4, 0, 5);

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16, 2);
 }

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  //float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
}
 

  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(f);
  lcd.print((char)223);
  lcd.print("F");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(h);
  lcd.print("%");
  delay(1000);
  
}
