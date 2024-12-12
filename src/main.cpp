#include <Arduino.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//Pinouts using ATMega2560
//Motor A
int enA = 10;
int in1 = 22;
int in2 = 24;

//Motor B
int enB = 11;
int in3 = 26;
int in4 = 28;
int pos = 0;

//Servo1 13
Servo servo1;
int servoPin1 = 13;
//Servo2 12
Servo servo2;
int servoPin2 = 12;
//acting as arms of a claw
//servo1 will go from 180 - 90
//servo2 will go from 0 - 90


void setup() {
    Serial.begin(9600);
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    display.display();
    delay(200);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    delay(200);
    display.display();
    display.clearDisplay();
    display.println("Hello, world!");


pinMode(enA, OUTPUT);
pinMode(in1, OUTPUT);
pinMode(in2, OUTPUT);
pinMode(enB, OUTPUT);
pinMode(in3, OUTPUT);
pinMode(in4, OUTPUT);

servo1.attach(servoPin1);
servo2.attach(servoPin2);
servo1.write(0);
servo2.write(360);


}

void loop() {
    //write pos of servo1 and   servo2 to display while they spin 90deg every2 seconds
    for (pos = 0; pos <= 360; pos += 10) {
        servo1.write(pos);
        servo2.write(360 - pos);
        
        display.clearDisplay();
        display.setCursor(0,0);
        display.print("Servo1: ");
        display.println(servo1.read());
        display.print("Servo2: ");
        display.println(servo2.read());
        display.display();
        
        delay(20);
    }
    delay(2000);

    for (pos = 360; pos >= 0; pos -= 10) {
        servo1.write(pos);
        servo2.write(360 - pos);
        
        display.clearDisplay();
        display.setCursor(0,0);
        display.print("Servo1: ");
        display.println(servo1.read());
        display.print("Servo2: ");
        display.println(servo2.read());
        display.display();
        
        delay(20);
    }
}