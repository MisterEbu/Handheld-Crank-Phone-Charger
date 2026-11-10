#include <TM1637Display.h>
#include <ACS712.h>

// --- Encoder pins ---
#define ENCODER_A 2
#define ENCODER_B 3

// --- Displays ---
#define CLK1 4   // RPM Display CLK
#define DIO1 5   // RPM Display DIO
#define CLK2 6   // Voltage Display CLK
#define DIO2 7   // Voltage Display DIO

TM1637Display dispRPM(CLK1, DIO1);
TM1637Display dispVolt(CLK2, DIO2);

ACS712  ACS(A0, 5.0, 1023, 100);
// --- Variables ---
volatile long pulseCount = 0;
unsigned long lastTime = 0;
float rpm = 0;

// FIT0277 spec: 334 pulses/rev per channel
const int pulsesPerRevolution = 720;

void countPulse() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);

  // Encoder input setup with internal pullups
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), countPulse, RISING);

  // Displays
  dispRPM.setBrightness(0x0f);
  dispVolt.setBrightness(0x0f);

  dispRPM.clear();
  dispVolt.clear();
}

void loop() {
  unsigned long now = millis();

  // Calculate every second
  if (now - lastTime >= 1000) {
    detachInterrupt(digitalPinToInterrupt(ENCODER_A));
    // compute RPM
    rpm = (pulseCount / (float)pulsesPerRevolution) * 60.0;
    pulseCount = 0;
    attachInterrupt(digitalPinToInterrupt(ENCODER_A), countPulse, RISING);
    lastTime = now;

    Serial.print("RPM: ");
    Serial.println(rpm);

    // Display RPM (capped to 9999)
    dispRPM.showNumberDec((int)rpm, true);
  }


  float current = ACS.mA_DC(1000);
  dispVolt.showNumberDec((current*5), true); // shows e.g. 512 for 5.12 V
  delay(250);
}
