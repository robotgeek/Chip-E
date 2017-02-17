#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "YOUR SSID HERE";
const char* password = "YOUR PASSWORD HERE";

ESP8266WebServer server(82);

const int led = 5; //Pin 5 on ESP8266 Thing Dev, Pin 13 on others

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound(){
  //Serial.print( "Not found: " );
  Serial.println( server.uri().substring(1) );
  
  digitalWrite(led, 1);
  server.send(200, "text/plain", "command sent to robot");
  /*
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  //Serial.println( message );
  
  server.send(404, "text/plain", message);
  */
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(0, OUTPUT);
  digitalWrite(0, HIGH);
  
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  //Serial.println(""); 

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("");
  //Serial.print("Connected to ");
  //Serial.println(ssid);
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());

  if (MDNS.begin("chipe")) {
    //Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/dance", [](){
    digitalWrite(led, 1);
    Serial.println( "dance" );
    server.send(200, "text/plain", "dancing");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  //Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
