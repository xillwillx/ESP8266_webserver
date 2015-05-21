/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request from jquery
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */

#include <ESP8266WiFi.h>

const int led = 13;
const char* ssid = "WIFI_SSID";
const char* password = "yourwirelesspassword";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO13
  pinMode(led, OUTPUT);
  digitalWrite(2, 0);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  //
  if (req.indexOf("/off") != -1)
    val = 0;
  else if (req.indexOf("/on") != -1)
    val = 1;
  else {
    if (req[req.indexOf("/") + 1] > ' '){
      Serial.println("Browser sending invalid request.");
      client.stop();
      return;
    }else{
      Serial.println("Browser Requesting / index page.");
      val = -1;
    }
  }

  // Set GPIO13 according to the request
  if (val != -1) digitalWrite(led, val);
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>";
  
  s += "<head>\r\n<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>\r\n";
  s += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
  s += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">\r\n";
  s += "</head>\r\n<div class=\"container\">\r\n<h1>ESP8266 LED Control</h1>\r\n<div class=\"row\">\r\n";
  s += "<div class=\"col-md-2\"><input class=\"btn btn-block btn-lg btn-primary\" type=\"button\" value=\"On\" onclick=\"on()\"></div>\r\n";
  s += "<div class=\"col-md-2\"><input class=\"btn btn-block btn-lg btn-danger\" type=\"button\" value=\"Off\" onclick=\"off()\"></div>\r\n";
  s += "</div></div>\r\n";
  s += "<script>function on() {$.get(\"/on\");}</script><script>function off() {$.get(\"/off\");}</script>\r\n";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disconnected");
}
