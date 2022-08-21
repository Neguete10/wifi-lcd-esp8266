#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 2     // On the Thing's board
#define DHTTYPE DHT11   // DHT
//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiSSID[] = "Neguete-WIFI";
//const char WiFiSSID[] = "NEGUETE";
const char WiFiPSK[] = "youpoor123";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 5; // Thing's onboard, green LED

DHT dht(DHTPIN,DHTTYPE);
LiquidCrystal lcd(16, 12, 13, 4, 0, 5);
WiFiServer server(80);

void setup() 
{
  dht.begin();
  lcd.begin(16, 2);
  initHardware();
  connectWiFi();
  server.begin();
  setupMDNS();
  
}

void loop() 
{
  delay(2000);
  float h = dht.readHumidity();  
  float f = dht.readTemperature(true);

  if (isnan(h)|| isnan(f)) {
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


  
  // Check if a client has connected
  
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  if (req.indexOf("/led/0") != -1)
    val = 1; // Will write LED high
  else if (req.indexOf("/led/1") != -1)
    val = 0; // Will write LED low
  else if (req.indexOf("/read") != -1)
    val = -2; // Will print pin reads
  // Otherwise request will be invalid. We'll say as much in HTML

  // Set GPIO5 according to the request
  if (val >= 0)
    digitalWrite(LED_PIN, val);

  client.flush();  

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";

  /*Note: Uncomment the line below to refresh automatically
   *      for every 1 second. This is not ideal for large pages 
   *      but for a simple read out, it is useful for monitoring 
   *      your sensors and I/O pins. To adjust the fresh rate, 
   *      adjust the value for content. For 30 seconds, simply 
   *      change the value to 30.*/
  s += "<meta http-equiv='refresh' content='1'/>\r\n";//auto refresh page

  // If we're setting the LED, print out a message saying we did
  if (val >= 0)
  {
    s += "LED is now ";
    s += (val)?"off":"on";
  }
  else if (val == -2)
  {  
  
    s += "Temperature = ";
    s += String(f);
    s += "<br>";
    s += "Humidity = ";
    s += String(h);
    
  }
  else
  {
    s += "Invalid Request.<br> Try /led/1, /led/0, or /read.";
  }
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

void connectWiFi()
{
  byte ledStatus = LOW;
  Serial.println();
  Serial.println("Connecting to: " + String(WiFiSSID));
  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);

  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(100);
    // Potentially infinite loops are generally dangerous.
    // Add delays -- allowing the processor to perform other
    // tasks -- wherever possible.
  }
  /*Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  */
  lcd.setCursor(0,0);
  lcd.print("IP address:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(5000);
}

void setupMDNS()
{
  // Call MDNS.begin(<domain>) to set up mDNS to point to
  // "<domain>.local"
  if (!MDNS.begin("thing")) 
  {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

}

void initHardware()
{
  Serial.begin(9600);
//  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}
