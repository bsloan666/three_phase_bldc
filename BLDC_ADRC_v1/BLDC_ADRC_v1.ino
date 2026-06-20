#include <Wire.h>
#include <AS5600.h>
#include <Arduino.h>

// --- Hardware Setup ---
AS5600 encoder;
const int PIN_MOTOR_A = 5;
const int PIN_MOTOR_B = 6;

// --- LADRC Parameters (Tuning) ---
float wc = 10.0;        // [THE SPRING] Higher = stiffer, Lower = smoother.
float wo = 50.0;       // [THE EYES] Keep this ~5x higher than wc.
float b0 = 20000.0;    // [THE POWER KNOB] Higher = gentler, Lower = stronger torque.

const float dt = 0.002; 
const float deadband = 1.0; 
const int MIN_PWM = 100;    
const int KICK_PWM = 240;   
const int KICK_RAMP_MS = 250; // Simple ramp duration for every start

// --- Simplified State Tracking ---
int last_direction = 0; 
bool is_ramping = false;
unsigned long ramp_start_time = 0;
float last_y_pos = 0;
unsigned long last_physical_move = 0; // Strictly tr/Dacks encoder changes
unsigned long last_stall_check = 0;   // Used for kicker timing
bool is_resetting_driver = false;
unsigned long driver_reset_start = 0;


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
float max_velocity = 6000.0;   // Units per second
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

float apply_motor_pwm(float control_u, float current_y, bool force_reset) {
  int target_dir = (control_u >= 0) ? 1 : -1;
  unsigned long now = millis();

  if (force_reset) {
    analogWrite(PIN_MOTOR_A, 0);
    pinMode(PIN_MOTOR_B, INPUT);
    last_direction = 0;
    return 0;
  }

  // 1. Physical Move Detection (Tracks if motor is actually spinning)
  if (abs(current_y - last_y_pos) > 2.0) { // Ignore < 2 units noise
    last_y_pos = current_y;
    last_physical_move = now;
    last_stall_check = now;
  }

  // 2. Stall Detection (For Kicker)
  if (now - last_stall_check > 150 && abs(control_u * b0) > 10.0 && !is_ramping) {
    is_ramping = true;
    ramp_start_time = now;
  }

  int power = constrain(abs(control_u * b0), 0, 255);

  // 3. Predictable Ramp
  if (is_ramping) {
    unsigned long elapsed = now - ramp_start_time;
    if (elapsed < KICK_RAMP_MS) {
      power = map(elapsed, 0, KICK_RAMP_MS, MIN_PWM, KICK_PWM);
    } else {
      is_ramping = false;
      last_stall_check = now; // Reset kicker timer only
    }
  } else if (power > 0 && power < MIN_PWM) {
    power = MIN_PWM;
  }


  // 3. Simple, Solid Direction
  if (target_dir != last_direction) {
    pinMode(PIN_MOTOR_B, OUTPUT);
    digitalWrite(PIN_MOTOR_B, (target_dir == 1) ? LOW : HIGH);
    last_direction = target_dir;
  }
  
  analogWrite(PIN_MOTOR_A, power);

  // 4. Actual feedback for observer
  float actual_u = (float)power / b0;
  if (target_dir < 0) actual_u = -actual_u;
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
      int temp = inputString.substring(1).toInt() + 270;

      target_pos = temp;
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

  // --- Driver Wake-up (Explicit Silence) ---
  pinMode(PIN_MOTOR_A, OUTPUT);
  pinMode(PIN_MOTOR_B, INPUT); // High-Z
  analogWrite(PIN_MOTOR_A, 0); 
  delay(500); // 500ms of "reboot silence" for the driver

  /* 
   * NOTE FOR ARDUINO NANO USERS:
   * To get 31.25kHz PWM (mimics a potentiometer), move PWM wire to PIN 9.
   * Then add this code here:
   * TCCR1B = (TCCR1B & 0b11111000) | 0x01; 
   */
  
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

  // Initialize stall and move detection
  last_y_pos = unwrapped_pos;
  last_physical_move = millis();
  last_stall_check = millis();
  
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
    z3 = constrain(z3, -600.0, 600.0); 

    // 4. Control Law
    float error_p = current_setpoint - z1;
    
    // Apply Deadband
    if (abs(error_p) < deadband) {
      error_p = 0;
    }

    float u0 = (kp * error_p) - (kd * z2);
    
    // 5. Disturbance Rejection
    u = (u0 - z3) / b0;
    
    // 6. Idle Soft-Reboot Logic (Based on physical movement)
    unsigned long now_ms = millis();
    
    // If we haven't moved > 2 units in 5 seconds, perform a refresh
    // This will now trigger even if the kicker is firing (since kicker doesn't move motor)
    if (now_ms - last_physical_move > 5000) {
      if (!is_resetting_driver) {
        is_resetting_driver = true;
        driver_reset_start = now_ms;
        Serial.println("Idle/Stall Refreshing Driver...");
      }
    }

    if (is_resetting_driver) {
      if (now_ms - driver_reset_start < 150) { // Increased to 150ms
        u = apply_motor_pwm(0, y, true);
        z3 = 0; // Clear the observer's push
      } else {
        is_resetting_driver = false;
        last_physical_move = now_ms; // Reset the 5s timer
      }
    } else {
      u = apply_motor_pwm(u, y, false);
    }

    // 7. Telemetry
    static int count = 0;
    if (count++ % 100 == 0) {
       Serial.print("T:"); Serial.print(target_pos);

       Serial.print(" S:"); Serial.print(current_setpoint);
       Serial.print(" P:"); Serial.print(y);
       Serial.print(" U:"); Serial.println(u);
    }
  }

  handle_serial();
}
