#include <WiFi.h>
#include <ESP32Servo.h>

const char* ssid     = "910";
const char* password = "Bl3ss3d@20";

WiFiServer server(80);
int ledpin = 2;
Servo servoMotor;

#define SERVO_PIN 26 // ESP32 pin GPIO26 connected to servo motor

void setup() {
    Serial.begin(115200);
    pinMode(ledpin, OUTPUT); // set the LED pin mode
    delay(10);

    servoMotor.attach(SERVO_PIN); // attach the servo on ESP32 pin

    // We start by connecting to a WiFi network
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
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
}

void loop() {
    WiFiClient client = server.available(); // listen for incoming clients

    if (client) { // if you get a client,
        Serial.println("New Client."); // print a message out the serial port
        String currentLine = ""; // make a String to hold incoming data from the client
        bool currentLineIsBlank = true; // flag to detect empty lines

        while (client.connected()) { // loop while the client's connected
            if (client.available()) { // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                Serial.write(c); // print it out the serial monitor

                if (c == '\n' && currentLineIsBlank) {
                    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                    // and a content-type so the client knows what's coming, then a blank line:
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type:text/html");
                    client.println();

                    // the content of the HTTP response follows the header:
                    client.print("Click <a href=\"/H\">here</a> to turn the servo to 0 degrees.<br>");
                    client.print("Click <a href=\"/L\">here</a> to turn the servo to 90 degrees.<br>");
                    
                    // The HTTP response ends with another blank line:
                    client.println();
                    break;
                } else if (c == '\n') { // if you got a newline, then clear currentLine
                    currentLineIsBlank = true;
                    currentLine = "";
                } else if (c != '\r') { // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                    currentLineIsBlank = false;
                }

                // Check to see if the client request was "GET /H" or "GET /L":
                if (currentLine.endsWith("GET /H")) {
                      for (int pos = 180; pos >= 0; pos--) 
                      {
                        servoMotor.write(pos);
                        delay(1); // waits 15ms to reach the position
                      } 
                    
                }
                if (currentLine.endsWith("GET /L")) {
                        for (int pos = 0; pos <= 180; pos++) 
                        {
                          servoMotor.write(pos);
                          delay(1); // waits 15ms to reach the position
                        }
                }

                // Handle favicon.ico request
                if (currentLine.endsWith("GET /favicon.ico")) {
                    client.println("HTTP/1.1 204 No Content");
                    client.println("Connection: close");
                    client.println();
                    break;
                }
            }
        }
        // close the connection:
        client.stop();
        Serial.println("Client Disconnected.");
    }
}