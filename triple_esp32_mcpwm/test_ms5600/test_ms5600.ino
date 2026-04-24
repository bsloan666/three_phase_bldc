#include <Wire.h>
#include <AS5600.h>
#include "TM1637Display.h"

AS5600 encoder;

unsigned long prev_angle;
TM1637Display display = TM1637Display(11, 10);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!encoder.begin()) {
    Serial.println("AS5600 not detected. Check wiring!");
    while (1);
  }
  Serial.println("AS5600 Initialized");
  // initialize digital pin LED_BUILTIN as an output.
  display.clear();
  pinMode(2, OUTPUT);
  display.setBrightness(7);
  // Serial.begin(9600);

}

void loop() {
  // Read absolute angle in degrees (0.0 to 360.0)
  unsigned long angle = encoder.readAngle();
  float factor = 360.0f/4096.0f;
  if (angle != prev_angle){
    unsigned int ord = int(angle * factor);
    
    display.showNumberDecEx(ord, 0, false, 4, 0);
    Serial.println(float(angle) * factor);
   
  }
  prev_angle = angle;
  delay(100); // 10Hz update rate
}
