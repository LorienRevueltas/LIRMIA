#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial GPS_Serial(10, 11); // RX, TX

int order;
Servo rudder;

void setup(){
    Serial.begin(4800);
    GPS_Serial.begin(4800);
    order = 0;
    rudder.attach(9); // Direction servo motor is on digital pin 9
    rudder.write(90);
}

void loop(){
    while(GPS_Serial.available()){
        Serial.write(GPS_Serial.read());
    }
    while(Serial.available()){
        order = 90 + ((int) Serial.read() * 90) / 127;
        if(order < 0){order = 0;}
        if(order > 180){order = 180;}
        rudder.write(order);
    }
    delay(200);
}
