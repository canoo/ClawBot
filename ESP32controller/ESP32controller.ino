#include <Bluepad32.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Servos
Servo servo1; // Left joystick X
Servo servo2; // Left joystick Y
Servo servo3; // Right joystick X
Servo servo4; // Right joystick Y

int servoPin1 = 19;
int servoPin2 = 18;
int servoPin3 = 4;
int servoPin4 = 17;

// Servo parameters
const int SERVO_MIN_POS = 0;
const int SERVO_MAX_POS = 180;  // Using 180 degrees max for standard servos
const int INPUT_THRESHOLD = 10; // Slight threshold to avoid twitchiness

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

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
  display.println("Servo Control - 0 to 180 degrees");
  display.display();

  // Set all servos to 50Hz
  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  servo3.setPeriodHertz(50);
  servo4.setPeriodHertz(50);

  // Attach all servos
  servo1.attach(servoPin1, 1000, 2000); // typical SG90 range
  servo2.attach(servoPin2, 1000, 2000);
  servo3.attach(servoPin3, 1000, 2000);
  servo4.attach(servoPin4, 1000, 2000);

  // Initialize servos to 90° (center)
  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
  servo4.write(90);

  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.enableVirtualDevice(false);
}

void processGamepad(ControllerPtr ctl) {
  int axisX = ctl->axisX();    // Left joystick X
  int axisY = ctl->axisY();    // Left joystick Y
  int axisRX = ctl->axisRX();  // Right joystick X
  int axisRY = ctl->axisRY();  // Right joystick Y

  // Map function:
  // axis ∈ [-512, 511] -> angle ∈ [180, 0]
  // At -512 → 180°, at 511 → 0°, at 0 → ~90°

  if (abs(axisX) > INPUT_THRESHOLD) {
    int angle = map(axisX, -512, 511, 180, 0);
    servo1.write(constrain(angle, SERVO_MIN_POS, SERVO_MAX_POS));
  }

  if (abs(axisY) > INPUT_THRESHOLD) {
    int angle = map(axisY, -512, 511, 180, 0);
    servo2.write(constrain(angle, SERVO_MIN_POS, SERVO_MAX_POS));
  }

  if (abs(axisRX) > INPUT_THRESHOLD) {
    int angle = map(axisRX, -512, 511, 180, 0);
    servo3.write(constrain(angle, SERVO_MIN_POS, SERVO_MAX_POS));
  }

  if (abs(axisRY) > INPUT_THRESHOLD) {
    int angle = map(axisRY, -512, 511, 180, 0);
    servo4.write(constrain(angle, SERVO_MIN_POS, SERVO_MAX_POS));
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
  // Servos remain attached and hold their last written position.
}