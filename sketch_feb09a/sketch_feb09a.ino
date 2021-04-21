// libs
#include <ESP8266WiFi.h>

//Initializing LED Pin
int led_pin = D5;
int led_pin1 = D6;
int buttonPin = D7;
char ssid[20] = "smart_light";
char psswd[20] = "123321123";
int device_is_host = 0;
WiFiServer server(80);

void setup() {

    pinMode(led_pin, OUTPUT);
    pinMode(led_pin1, OUTPUT);
    pinMode(buttonPin, INPUT);
    Serial.begin(115200);

}

void ap_active(){
    Serial.println("Setting soft-AP ... ");
    if (!device_is_host)
        device_is_host = WiFi.softAP(ssid, psswd) ? 1 : 0;
}

void ap_down(){
    if(device_is_host)
        device_is_host = WiFi.softAPdisconnect(true) ? 1 : 0;
}

void lights_active(){

     for(int i=0; i<1023; i++){
        analogWrite(led_pin, i);
        delay(1);
    }

    for(int i=1023; i>0; i--){
        analogWrite(led_pin, i);
        delay(1);
    }

}

void handle_app_requests() {

    WiFiClient client = server.available();

    if(!client) {
        return;
    }
    
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();
    String value;
    if(request.indexOf("/led/on") != -1) {
        digitalWrite(led_pin, HIGH);
        value = "on";
    } else if (request.indexOf("/led/off") != -1) {
        digitalWrite(led_pin, LOW);
        value = "off";
    }
    client.flush();

    String s = "HTTP/1.1 200 OK\r\n";
    s += "Content-Type: application/json\r\n\r\n";
    s += "{\"data\":{\"message\":\"success\",\"value\":\"";
    s += value;
    s += "\"}}\r\n";
    s += "\n";

    client.print(s);
    delay(1);
    Serial.println("Client disconnected");
}

bool device_is_active(){
    return digitalRead(buttonPin) == HIGH;
}

void loop() {

  if (device_is_active()) {
        ap_down();
        lights_active();
  } else {
        ap_active();
        handle_app_requests();
  }
  
}
