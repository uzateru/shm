// Данный кусочек кода отвечает за внедрения "Шапки", в которой прописаны детали библиотеки
#include "qmobot.h"

// Load Wi-Fi library
#include <WiFi.h>
// Replace with your network credentials
const char* ssid     = "NLT063859NU 7818";
const char* password = "12345678";
#define relay 13
// Set web server port number to 80
WiFiServer server(80);

WiFiClient client1;
IPAddress server1(192,168,137,172);       // the fix IP address of the server

//IPAddress server1(10,101,11,240);
// Variable to store the HTTP request
String header;
String ss = "";
String mess = "";
// Auxiliar variables to store the current output state
String output = "off";
double setTemp;

void setup() {
  Qchip.begin(true, true);
  
  // Initialize the output variables as outputs
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(relay, OUTPUT);
  // Set outputs to LOW
  digitalWrite(BUILTIN_LED, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  
  setTemp = ss.toDouble();
  
  if(!server.available()) {
    client1.connect(server1, 5560);   // Connection to the server
    client1.println("Give the data");  // sends the message to the server 
    String answer = client1.readStringUntil('\r');   // receives the answer from the sever
    double currtemp = answer.toDouble();
    Serial.println("from sensor: " + answer);
    
    if(setTemp == 0) {
    } else if(currtemp < setTemp) {
      mess = "ValveOpen, temp= " + answer + '\n'+String(setTemp);
      digitalWrite(relay, LOW);
    } else {
      mess = "ValveClose, temp= " + answer+'\n'+String(setTemp);
      digitalWrite(relay, HIGH);  
    }
    Serial.println(mess);
    Qchip.display->clear(); // Очистить дисплей от надписей
                          // Пиксели по x,      пиксели по y,     текст
    Qchip.display->drawString( 0 /*от 0 до 128*/, 0 /*от 0 до 64*/, mess); // Написать текст на дисплее
    Qchip.display->drawString(0, 63, "y");
    Qchip.display->drawString(127, 0, "x");
    
    
    Qchip.display->display();
    delay(2000);
  } else {
    WiFiClient client = server.available();   // Listen for incoming clients
    if (client) {                             // If a new client connects,
      Serial.println("New Client.");          // print a message out in the serial port
      String currentLine = "";                // make a String to hold incoming data from the client
      while (client.connected()) {            // loop while the client's connected
        if (client.available()) {             // if there's bytes to read from the client,
          char c = client.read();             // read a byte, then
          Serial.write(c);                    // print it out the serial monitor
          header += c;
          if (c == '\n') {                    // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");

              // // Web Page Heading
              client.println("<body><h1>Smart Heat Managenent</h1>");
              
              client.println("<form action=\"/temp\">");
              client.println("Set temperature: <input type=\"number\" name=\"temperature\" min=\"-20\" max=\"35\">");
              client.println("<input type=\"submit\" name = \"OK\">");
              client.println("</form>");
              client.println("</body></html>");
              //int a = header.indexOf("temperature=");
              ss = header.substring(22, 24);
              Serial.println(ss);
              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            } else { // if you got a newline, then clear currentLine
              currentLine = "";
            }
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
          }
        } 
        }
      // Clear the header variable
      header = "";
      // Close the connection
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
  }
  
}
