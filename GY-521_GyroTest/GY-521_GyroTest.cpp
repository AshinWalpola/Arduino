#For use with GY-521 Module
#Arduino UNO R3
#Author: Ashin Walpola
#include <Wire.h>

const int MPU = 0x68;  // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ;
bool collectData = false;

const int led1 = 5;  // LED for top-left corner
const int led2 = 4;  // LED for top-right corner
const int led3 = 3;  // LED for bottom-left corner
const int led4 = 2;  // LED for bottom-right corner
const int LEDInt = 200;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Send 'start' to begin data collection.");
  Serial.println("Send 'stop' to end data collection.");

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  startMPU6050();
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();  // Remove any leading or trailing whitespace

    if (command.equalsIgnoreCase("start")) {
      collectData = true;
      Serial.println("Data collection started.");
    } else if (command.equalsIgnoreCase("stop")) {
      collectData = false;
      Serial.println("Data collection stopped.");
      turnOffAllLEDs();
    }
  }

  if (collectData) {
    readMPU6050Data();
    lightUpLEDs();
    delay(100);
  }
}

void startMPU6050() {
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void readMPU6050Data() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true);  // request a total of 14 registers

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
}

void lightUpLEDs() {
  // Define thresholds for detecting tilt
  const int threshold = 2250;

  // Turn off all LEDs
  turnOffAllLEDs();

  if (AcX < -threshold && AcY > threshold) {
    digitalWrite(led1, HIGH);  // Top-left corner
  } else if (AcX > threshold && AcY > threshold) {
    digitalWrite(led2, HIGH);  // Top-right corner
  } else if (AcX < -threshold && AcY < -threshold) {
    digitalWrite(led3, HIGH);  // Bottom-left corner
  } else if (AcX > threshold && AcY < -threshold) {
    digitalWrite(led4, HIGH);  // Bottom-right corner
  } else if (AcY < -threshold) {
    // Tilted down, light up both bottom LEDs
    digitalWrite(led3, HIGH);  // Bottom-left corner
    digitalWrite(led4, HIGH);  // Bottom-right corner
  } else if (AcX < -threshold) {
    // Tilted left, light up both bottom LEDs
    digitalWrite(led1, HIGH);
    digitalWrite(led3, HIGH);
  }
}

void turnOffAllLEDs() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
}
