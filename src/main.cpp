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

// Pinouts using ATMega2560
// Motor A
int enA = 10;
int in1 = 22;
int in2 = 24;

// Motor B
int enB = 11;
int in3 = 26;
int in4 = 28;
int pos = 0;

// Servo objects and pin definitions
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

int servoPin1 = 3;
int servoPin2 = 4;
int servoPin3 = 5;
int servoPin4 = 6;
int servoPin5 = 7;

void setup() {
    Serial.begin(9600);
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Don't proceed, loop forever
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
        servo3.attach(servoPin3);
        servo4.attach(servoPin4);
        servo5.attach(servoPin5);
}

String receivedMessage = "";
int angle = 0;

void loop() {
    for (int angle = 0; angle <= 90; angle += 10) {
        servo1.write(angle);
        servo2.write(angle);
        servo3.write(angle);
        servo4.write(angle);
        servo5.write(angle);
        delay(100); // Adjust delay as needed
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Servo1: ");
        display.println(angle);
        display.print("Servo2: ");
        display.println(angle);
        display.print("Servo3: ");
        display.println(angle);
        display.print("Servo4: ");
        display.println(angle);
        display.print("Servo5: ");
        display.println(angle);
        display.display();
    }
}