#include <Wire.h>
#include <AS5600.h>
#include <Arduino.h>

// --- Hardware Setup ---
AS5600 encoder;
const int PIN_MOTOR_A = 5;
const int PIN_MOTOR_B = 6;

// --- LADRC Parameters (Tuning) ---
float wc = 10.0;       // Increased for smoother, more fluid motion
float wo = 50.0;       // Increased to match the faster controller
float b0 = 20000.0;    
const float dt = 0.002; 
const float deadband = 0.5; 
const int MIN_PWM = 110;    
const int KICK_PWM = 254;   // Near-max power (99.6% duty cycle) for driver stability
const int KICK_DURATION_MS = 400; // Longer sustained push

// --- State Tracking for Kicker & Fault Reset ---
unsigned long last_move_time = 0;
float last_y_kicker = 0;
bool is_kicking = false;
unsigned long kick_start_time = 0;
bool is_resetting_driver = false;
unsigned long reset_start_time = 0;

// --- Precalculated Gains ---
float beta1, beta2, beta3;
float kp, kd;

// --- LADRC Observer States ---
float z1 = 0; // Estimated position
float z2 = 0; // Estimated velocity
float z3 = 0; // Estimated total disturbance
float u = 0;  // Control signal

// --- Control Variables ---
long target_pos = 2048;
float current_setpoint = 2048; // The "moving" target
float max_velocity = 2000.0;   // Units per second
unsigned long prev_micros = 0;
long last_raw_pos = 0;
long unwrapped_pos = 0;

// --- Non-blocking Serial Buffer ---
String inputString = "";
bool stringComplete = false;

long get_unwrapped_pos() {
  long raw = encoder.readAngle();
  long diff = raw - last_raw_pos;

  if (diff > 2048) {
    diff -= 4096;
  } else if (diff < -2048) {
    diff += 4096;
  }

  unwrapped_pos += diff;
  last_raw_pos = raw;
  return unwrapped_pos;
}

float apply_motor_pwm(float control_u, float current_y) {
  static int last_dir = 0; 
  int current_dir = (control_u >= 0) ? 1 : -1;
  unsigned long now = millis();

  // 1. Stall Detection: If we want to move but haven't moved in 150ms
  if (abs(control_u * b0) > 5.0 && !is_kicking) {
    if (abs(current_y - last_y_kicker) > 1.0) {
      last_y_kicker = current_y;
      last_move_time = now;
    } else if (now - last_move_time > 150) {
      is_kicking = true;
      kick_start_time = now;
    }
  }

  int power = constrain(abs(control_u * b0), 0, 255);

  // 2. Handle Ramped Kicker
  if (is_kicking) {
    unsigned long elapsed = now - kick_start_time;
    const unsigned long ramp_ms = 150; // 150ms ramp for the 255 peak power
    
    if (elapsed < ramp_ms) {
      // Smoothly ramp from MIN_PWM to KICK_PWM
      power = map(elapsed, 0, ramp_ms, MIN_PWM, KICK_PWM);
    } else if (elapsed < KICK_DURATION_MS) {
      power = KICK_PWM;
    } else {
      is_kicking = false;
      last_move_time = now;
    }
  } 
  // 3. Normal Minimum PWM
  else if (power > 0 && power < MIN_PWM) {
    power = MIN_PWM;
  }

  // 4. Solid Direction Signal
  if (current_dir != last_dir) {
    pinMode(PIN_MOTOR_B, OUTPUT);
    if (current_dir == 1) {
      digitalWrite(PIN_MOTOR_B, LOW);
    } else {
      digitalWrite(PIN_MOTOR_B, HIGH);
    }
    last_dir = current_dir;
  }
  
  analogWrite(PIN_MOTOR_A, power);

  float actual_u = (float)power / b0;
  if (control_u < 0) actual_u = -actual_u;
  return actual_u;
}

void handle_serial() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }

  if (stringComplete) {
    inputString.trim();
    if (inputString.startsWith("T")) {
      target_pos = inputString.substring(1).toInt();
      Serial.print("New Target: ");
      Serial.println(target_pos);
    }
    inputString = "";
    stringComplete = false;
  }
}

void setup() {
  Serial.begin(115200); 
  Wire.begin();
  Wire.setClock(400000); 
  
  if (!encoder.begin()) {
    Serial.println("AS5600 not detected!");
    while (1);
  }

  pinMode(PIN_MOTOR_A, OUTPUT);
  pinMode(PIN_MOTOR_B, OUTPUT);

  // Precalculate Bandwidth-based Gains
  beta1 = 3.0 * wo;
  beta2 = 3.0 * wo * wo;
  beta3 = wo * wo * wo;
  kp = wc * wc;
  kd = 2.0 * wc;
  
  // Initialize unwrapped position
  last_raw_pos = encoder.readAngle();
  unwrapped_pos = last_raw_pos;
  
  // Initialize setpoint and observer state to current position
  current_setpoint = unwrapped_pos;
  target_pos = unwrapped_pos;
  
  z1 = unwrapped_pos;
  z2 = 0;
  z3 = 0;
  
  inputString.reserve(20);
  delay(1000);
  Serial.println("LADRC with Trajectory Ready.");
}

void loop() {
  unsigned long current_micros = micros();

  // Fixed Frequency Control Loop (500Hz)
  if (current_micros - prev_micros >= 2000) {
    prev_micros = current_micros;

    // 1. Read actual position (Unwrapped)
    float y = get_unwrapped_pos();

    // 2. Trajectory Generator
    float delta_limit = max_velocity * dt;
    float setpoint_error = (float)target_pos - current_setpoint;
    
    if (setpoint_error > delta_limit) {
      current_setpoint += delta_limit;
    } else if (setpoint_error < -delta_limit) {
      current_setpoint -= delta_limit;
    } else {
      current_setpoint = (float)target_pos;
    }

    // 3. Linear Extended State Observer (LESO)
    float error_o = y - z1;
    
    // Euler Integration
    z1 += (z2 + beta1 * error_o) * dt;
    z2 += (z3 + beta2 * error_o + b0 * u) * dt;
    z3 += (beta3 * error_o) * dt;

    // Anti-windup for z3 (limit the total disturbance estimate)
    z3 = constrain(z3, -1000.0, 1000.0); 

    // 4. Control Law
    float error_p = current_setpoint - z1;
    
    // Apply Deadband
    if (abs(error_p) < deadband) {
      error_p = 0;
    }

    float u0 = (kp * error_p) - (kd * z2);
    
    // 5. Disturbance Rejection
    u = (u0 - z3) / b0;
    
    // 6. Apply Output and Get Actual Applied U
    u = apply_motor_pwm(u, y);

    // 7. Telemetry
    static int count = 0;
    if (count++ % 10 == 0) {
       Serial.print("T:"); Serial.print(target_pos);

       Serial.print(" S:"); Serial.print(current_setpoint);
       Serial.print(" P:"); Serial.print(y);
       Serial.print(" U:"); Serial.println(u);
    }
  }

  handle_serial();
}
