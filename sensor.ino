#include <DallasTemperature.h>
#include <OneWire.h>

/*  Connects to the home WiFi network
 *  Asks some network parameters
 *  Starts WiFi server with fix IP and listens
 *  Receives and sends messages to the client
 *  Communicates: wifi_client_01.ino
 */
#include <SPI.h>
#include <ESP8266WiFi.h>


const int oneWireBus = 4; 
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

double temp;
double currtemp;
String message;
byte ledPin = 2;
char ssid[] = "NLT063859NU 7818";               // SSID of your home WiFi
char pass[] = "12345678";               // password of your home WiFi
WiFiServer server(5560);  

String request = "";
//https://randomnerdtutorials.com/esp8266-ds18b20-temperature-sensor-web-server-with-arduino-ide/

//IPAddress ip(192, 168, 137, 56);            // IP address of the server
//IPAddress gateway(192,168,137,1);           // gateway of your network
//IPAddress subnet(255,255,255,0);          // subnet mask of your network

void setup() {
  Serial.begin(115200);                   // only for debug
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, pass);                 // connects to the WiFi router
 // WiFi.config(ip, gateway, subnet);       // forces to use the fix IP
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  server.begin();                         // starts the server
  sensors.begin(); 
  Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());  // some parameters from the network
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());
  Serial.print("Networks: "); Serial.println(WiFi.scanNetworks());
  pinMode(ledPin, OUTPUT);
  
}


void loop () {
  WiFiClient client = server.available();
  request = "";
  if (client) {
    if (client.connected()) {
      request = client.readStringUntil('\r');    // receives the message from the client
      Serial.print("From client: "); Serial.println(request);
      client.flush();
      currtemp = getTemp();
      message = String(currtemp)+"\r";
      Serial.println(message);
      client.println(message); // sends the answer to the client
    }
    client.stop();                // tarminates the connection with the client
  }
}

double getTemp(){
  sensors.requestTemperatures(); 
  double temperatureC = sensors.getTempCByIndex(0);
  return temperatureC;
}
