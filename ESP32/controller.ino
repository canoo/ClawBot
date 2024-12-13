#include <Bluepad32.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//

// Display settings (if still needed)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Servos
Servo servo1; // Controlled by left joystick Y
Servo servo2; // Controlled by right joystick Y
Servo servo3; // Controlled by brake
Servo servo4; // Controlled by throttle

int servoPin1 = 19;  
int servoPin2 = 18;  
int servoPin3 = 4;   
int servoPin4 = 17;  

// Servo parameters
const int SERVO_MIN_POS = 0;
const int SERVO_MAX_POS = 180;
const int SERVO_START_POS = 90;
const int INPUT_THRESHOLD = 100; // For joystick deadzone checking
// No input timeout used now, since we're not detaching.

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// Track last positions
int lastServo1Pos = SERVO_START_POS;
int lastServo2Pos = SERVO_START_POS;
int lastServo3Pos = SERVO_START_POS;
int lastServo4Pos = SERVO_START_POS;

//

void setServoPositionIfChanged(Servo &s, int &lastPos, int newPos) {
    newPos = constrain(newPos, SERVO_MIN_POS, SERVO_MAX_POS);
    if (abs(newPos - lastPos) > 2) { 
        s.write(newPos);
        lastPos = newPos;
    }
}

void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            myControllers[i] = ctl;
            break;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            myControllers[i] = nullptr;
            break;
        }
    }
}

void setup() {
    Serial.begin(115200);
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("SSD1306 allocation failed");
        for (;;);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println("Servo Control - Always Attached");
    display.display();

    // Setup and attach all servos at startup.
    // All servos remain attached at all times to avoid jitter from attach/detach.
    servo1.setPeriodHertz(50);
    servo2.setPeriodHertz(50);
    servo3.setPeriodHertz(50);
    servo4.setPeriodHertz(50);

    servo1.attach(servoPin1, 1000, 2000); // Assuming SG90 servo range
    servo2.attach(servoPin2, 1000, 2000);
    servo3.attach(servoPin3, 1000, 2000);
    servo4.attach(servoPin4, 1000, 2000);

    // Initialize servos to start position
    servo1.write(SERVO_START_POS);
    servo2.write(SERVO_START_POS);
    servo3.write(SERVO_START_POS);
    servo4.write(SERVO_START_POS);

    lastServo1Pos = SERVO_START_POS;
    lastServo2Pos = SERVO_START_POS;
    lastServo3Pos = SERVO_START_POS;
    lastServo4Pos = SERVO_START_POS;

    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.enableVirtualDevice(false);
}

void processGamepad(ControllerPtr ctl) {
    int axisX = ctl->axisX();
    int axisY = ctl->axisY();
    int axisRX = ctl->axisRX();
    int axisRY = ctl->axisRY();
    int brakeVal = ctl->brake();     
    int throttleVal = ctl->throttle();

    // Move servo1 with left joystick Y if outside deadzone
    if (abs(axisY) > INPUT_THRESHOLD) {
        int s1Pos = map(constrain(axisY, -512, 511), -512, 511, SERVO_MIN_POS, SERVO_MAX_POS);
        setServoPositionIfChanged(servo1, lastServo1Pos, s1Pos);
    }

    // Move servo2 with right joystick Y if outside deadzone
    if (abs(axisRY) > INPUT_THRESHOLD) {
        int s2Pos = map(constrain(axisRY, -512, 511), -512, 511, SERVO_MIN_POS, SERVO_MAX_POS);
        setServoPositionIfChanged(servo2, lastServo2Pos, s2Pos);
    }

    // Move servo3 with brake value if above a small threshold (like >10)
    if (brakeVal > 10) {
        int s3Pos = map(brakeVal, 0, 1023, SERVO_MIN_POS, SERVO_MAX_POS);
        setServoPositionIfChanged(servo3, lastServo3Pos, s3Pos);
    }

    // Move servo4 with throttle value if above a small threshold
    if (throttleVal > 10) {
        int s4Pos = map(throttleVal, 0, 1023, SERVO_MIN_POS, SERVO_MAX_POS);
        setServoPositionIfChanged(servo4, lastServo4Pos, s4Pos);
    }
}

void processController() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

void loop() {
    bool dataUpdated = BP32.update();
    if (dataUpdated) {
        processController();
    }
    // No detach calls, no timeouts. Servos remain at last commanded position.
}
