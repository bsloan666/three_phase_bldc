#include <Wire.h>
#include <AS5600.h>
#include <Arduino.h>
#include <soc/gpio_struct.h>

//const int coil_pins[6] = {3, 5, 6, 9, 10, 11};

const int pwmA = 13;
const int enA = 14;
const int pwmB = 18;
const int enB = 19;
const int pwmC = 25;
const int enC = 32;

const int hallA = 12;
const int hallB = 26;
const int hallC = 27;

volatile uint32_t last_hall_time = 0;
volatile uint32_t pulse_interval = 0;
volatile float rotor_velocity = 0; // Electrical rad/s
volatile int hall_state = 0;
int8_t hall_index = 0;
float fudge = 1.04719;
volatile long enc_angle;
long prev_enc_angle;
long target_angle;
long interval = 100000;
bool sensorChanged;
int TIMING_ADVANCE = 2;

unsigned long prev_time = 0;
volatile float curr_speed;
volatile float distance;
volatile float elapsed;

const int ss_fwd[6] = {1, 3, 2, 6, 4, 5};
const int ss_rev[6] = {5, 4, 6, 2, 3, 1};
int bridge = 0;
int polarity = 0;
volatile long direction  = 0;
long prev_direction  = 0;
int ss;

volatile int sa;
volatile int sb;
volatile int sc;

volatile int state;

// 1. Map direction to array index: 0 for Counter-Clockwise (negative), 1 for Clockwise (positive)
// If direction is 0, velocity is 0, so the state mapping choice does not matter.


// 2. Define the static lookup tables
// Dimensions: [Direction: 0=CCW, 1=CW][Sensor State: 0 to 7]

// Master Enable Pin State array: Bitmask format (Bit 0 = enA, Bit 1 = enB, Bit 2 = enC)
// 0 = LOW, 1 = HIGH
const uint8_t EN_MASK[2][8] = {
    // CCW (direction < 0)
    {0, 6, 3, 5, 5, 3, 6, 0}, 
    // CW  (direction > 0)
    {0, 6, 3, 5, 5, 3, 6, 0}  
};

void IRAM_ATTR onHallChange() {
    uint32_t gpio_state = GPIO.in;

    // Fast bit extraction from your active digital register bus
    bool ha = (gpio_state >> hallA) & 0x01;
    bool hb = (gpio_state >> hallB) & 0x01;
    bool hc = (gpio_state >> hallC) & 0x01;

    // Reconstruct your 3-bit sensor code instantly
    hall_state = (ha ? 1 : 0) | ((hb ? 1 : 0) << 1) | ((hc ? 1 : 0) << 2);
    sensorChanged = true;
}



// PWM Scaling Multiplier array: Multiplies velocity (0 = Off, 1 = On)
// Indices: [0]=pwmA, [1]=pwmB, [2]=pwmC
const uint8_t PWM_SCALE[2][8][3] = {
    // CCW State Lookups (ss 0 to 7)
    {
        {0, 0, 0}, // ss = 0 (Null)
        {0, 0, 1}, // ss = 1 (B Off, C Val) -> [enB, enC]
        {0, 1, 0}, // ss = 2 (A Off, B Val) -> [enA, enB]
        {0, 0, 1}, // ss = 3 (A Off, C Val) -> [enA, enC]
        {1, 0, 0}, // ss = 4 (A Val, C Off) -> [enA, enC]
        {1, 0, 0}, // ss = 5 (A Val, B Off) -> [enA, enB]
        {0, 1, 0}, // ss = 6 (B Val, C Off) -> [enB, enC]
        {0, 0, 0}  // ss = 7 (Null)
    },
    // CW State Lookups (ss 0 to 7)
    {
        {0, 0, 0}, // ss = 0 (Null)
        {0, 1, 0}, // ss = 1 (B Val, C Off) -> [enB, enC]
        {1, 0, 0}, // ss = 2 (A Val, B Off) -> [enA, enB]
        {1, 0, 0}, // ss = 3 (A Val, C Off) -> [enA, enC]
        {0, 0, 1}, // ss = 4 (A Off, C Val) -> [enA, enC]
        {0, 1, 0}, // ss = 5 (A Off, B Val) -> [enA, enB]
        {0, 0, 1}, // ss = 6 (B Off, C Val) -> [enB, enC]
        {0, 0, 0}  // ss = 7 (Null)
    }
};



AS5600 encoder;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(pwmC, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(enC, OUTPUT);

  pinMode(hallA, INPUT_PULLUP);
  pinMode(hallB, INPUT_PULLUP);
  pinMode(hallC, INPUT_PULLUP);

  // Attach ultra-fast hardware interrupts
  attachInterrupt(digitalPinToInterrupt(hallA), onHallChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(hallB), onHallChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(hallC), onHallChange, CHANGE);

  // Initialize old manual reading once to populate baseline global_ss at stall
  onHallChange();

  Wire.begin();
  if (!encoder.begin()) {
    Serial.println("AS5600 not detected. Check wiring!");
    while (1);
  }
  delay(2000);
  // Comment out below for trapezoidal
  // digitalWrite(enA, HIGH); digitalWrite(enB, HIGH); digitalWrite(enC, HIGH);
}

int get_sensor_state(){
  
  sa = digitalRead(hallA);
  sb = digitalRead(hallB);
  sc = digitalRead(hallC);
  
  state = sa << 2 | sb << 1 | sc;

  return state;
}
const int reverse[8][2] = {{0, 0}, {0, 1}, {1, 1}, {2, 0}, {2, 1}, {1, 0}, {0, 0}, {0, 0}};
const int forward[8][2] = {{0, 0}, {0, 0}, {1, 0}, {2, 1}, {2, 0}, {1, 1}, {0, 1}, {0, 0}};

const float sector_to_angle[8] = {
  0.0,             // 0: Invalid
  1.04719,         // 1: 60 degrees
  3.14159,         // 2: 180 degrees
  2.09439,         // 3: 120 degrees
  5.23598,         // 4: 300 degrees
  0.0,             // 5: 0 degrees
  4.18879,         // 6: 240 degrees
  0.0              // 7: Invalid
};

void poke(){
  digitalWrite(enA, HIGH); analogWrite(pwmA, 255);
  digitalWrite(enB, HIGH); analogWrite(pwmB, 0);
  delay(50);
  digitalWrite(enA, HIGH); analogWrite(pwmA, 0);
  digitalWrite(enC, HIGH); analogWrite(pwmC, 255);
  delay(50);
  digitalWrite(enC, HIGH); analogWrite(pwmC, 0);
  digitalWrite(enB, HIGH); analogWrite(pwmB, 255);
  delay(50);
}

const uint8_t saddle_table[120] = {
  128, 147, 166, 184, 201, 216, 229, 240, 248, 253, 255, 253, 249, 242, 233, 222,
  210, 198, 187, 177, 169, 164, 162, 163, 168, 175, 185, 197, 210, 223, 235, 245,
  251, 255, 254, 250, 242, 231, 218, 203, 186, 168, 150, 131, 112, 93, 75, 58,
  43, 31, 22, 14, 9, 6, 5, 8, 14, 23, 34, 48, 62, 77, 91, 104,
  116, 126, 133, 137, 138, 136, 131, 123, 113, 102, 89, 77, 65, 54, 46, 40,
  37, 37, 41, 47, 56, 68, 81, 95, 110, 125, 141, 156, 171, 186, 199, 211,
  222, 231, 239, 244, 248, 249, 248, 244, 238, 229, 218, 206, 192, 177, 162, 146,
  130, 114, 97, 82, 67, 52, 39, 27
};

const uint8_t sine_table[120] = {
  128, 134, 141, 148, 154, 161, 167, 174, 180, 186, 192, 198, 203, 209, 214, 219,
  223, 228, 232, 236, 239, 242, 245, 248, 250, 252, 253, 254, 255, 255, 255, 255,
  255, 254, 253, 252, 250, 248, 245, 242, 239, 236, 232, 228, 223, 219, 214, 209,
  203, 198, 192, 186, 180, 174, 167, 161, 154, 148, 141, 134, 128, 121, 114, 107,
  101, 94, 88, 81, 75, 69, 63, 57, 52, 46, 41, 36, 32, 27, 23, 19,
  16, 13, 10, 7, 5, 3, 2, 1, 0, 0, 0, 0, 0, 1, 2, 3,
  5, 7, 10, 13, 16, 19, 23, 27, 32, 36, 41, 46, 52, 57, 63, 69,
  75, 81, 88, 94, 101, 107, 114, 121
};

float get_smooth_angle(uint32_t time_since_flip) {
    

    // Start with the base angle of the current Hall sector
    float base_angle = sector_to_angle[hall_state];

    // Add the "guessed" progress based on velocity
    float interpolation = rotor_velocity * time_since_flip;

    // Cap interpolation at 60 degrees (PI/3) so it doesn't overshoot
    if (interpolation > M_PI / 3.0) interpolation = M_PI / 3.0;

    return base_angle + interpolation + fudge;
    //return base_angle;
}

void write_foc(int electrical_angle) {
  // electrical_angle is 0-119
  int angleA = electrical_angle % 120;
  int angleB = (electrical_angle + 40) % 120; // 120 degrees offset
  int angleC = (electrical_angle + 80) % 120; // 240 degrees offset
   
  analogWrite(pwmA, sine_table[angleA]);
  analogWrite(pwmB, sine_table[angleB]);
  analogWrite(pwmC, sine_table[angleC]);

  //analogWrite(pwmA, saddle_table[angleA]);
  //analogWrite(pwmB, saddle_table[angleB]);
  //analogWrite(pwmC, saddle_table[angleC]);

}

// ============================================================================
// TUNING PARAMETER: Shift the pattern forward or backward.
// Change this integer to tune your motor's timing advance/retardation!
// ===========================================================================

// The true physical sequential clockwise pattern of your hall sensors
const uint8_t CW_SEQUENCE[6] = {4, 6, 2, 3, 1, 5};

/**
 * Returns the shifted sensor state based on the timing advance parameter.
 * Safely handles out-of-bounds null states (0 and 7).
 */

uint8_t getShiftedState(uint8_t raw_ss) {
    // Pass null states straight through to safely shut down pins
    if (raw_ss == 0 || raw_ss == 7) return raw_ss;

    // 1. Find the current index of the raw sensor state in our sequence
    int index = -1;
    for (int i = 0; i < 6; i++) {
        if (CW_SEQUENCE[i] == raw_ss) {
            index = i;
            break;
        }
    }

    // If a weird unmapped code somehow gets here, return it safely
    if (index == -1) return raw_ss;

    // 2. Apply the timing advance shift using modulo math
    // Adding 6 before modulo ensures negative shifts (e.g., -1) work correctly
    int shiftedIndex = (index + TIMING_ADVANCE) % 6;
    if (shiftedIndex < 0) shiftedIndex += 6;

    // 3. Return the newly mapped sensor code
    return CW_SEQUENCE[shiftedIndex];
}


void fast_coil_state(int ss) {
  int dirIdx = (direction > 0) ? 1 : 0; 
  uint8_t currentEnMask = EN_MASK[dirIdx][ss];
  int velocity = abs(direction);
  // Write Enable Pins using bitwise extraction
  digitalWrite(enA, (currentEnMask & 0x01) ? HIGH : LOW);
  digitalWrite(enB, (currentEnMask & 0x02) ? HIGH : LOW);
  digitalWrite(enC, (currentEnMask & 0x04) ? HIGH : LOW);

  // Write PWM Speeds safely scaling velocity
  analogWrite(pwmA, PWM_SCALE[dirIdx][ss][0] * velocity);
  analogWrite(pwmB, PWM_SCALE[dirIdx][ss][1] * velocity);
  analogWrite(pwmC, PWM_SCALE[dirIdx][ss][2] * velocity);
}


void next_coil_state(int ss) {
  // ss is your sensor state (1-6)
  // enA, enB, enC are digital pins tied to (R_EN + L_EN) of each board
  // pwmA, pwmB, pwmC are PWM pins tied to RPWM of each board
  int velocity = abs(direction);
  // Step 1: Default to High-Z (All OFF)
  digitalWrite(enA, LOW); digitalWrite(enB, LOW); digitalWrite(enC, LOW);

  if(direction > 0) {
    // Step 2: Activate specific paths based on the Hall State
    if (ss == 2) { // A High, B Low, C Off
      digitalWrite(enA, HIGH); analogWrite(pwmA, velocity);
      digitalWrite(enB, HIGH); analogWrite(pwmB, 0);
    } else if (ss == 3) { // A High, C Low, B Off
      digitalWrite(enA, HIGH); analogWrite(pwmA, velocity);
      digitalWrite(enC, HIGH); analogWrite(pwmC, 0);
    } else if (ss == 1) { 
      digitalWrite(enB, HIGH); analogWrite(pwmB, velocity);
      digitalWrite(enC, HIGH); analogWrite(pwmC, 0);
    } else if (ss == 5) { 
      digitalWrite(enA, HIGH); analogWrite(pwmA, 0);
      digitalWrite(enB, HIGH); analogWrite(pwmB, velocity);
    } else if (ss == 4) { 
      digitalWrite(enA, HIGH); analogWrite(pwmA, 0);
      digitalWrite(enC, HIGH); analogWrite(pwmC, velocity);
    } else if (ss == 6) {
      digitalWrite(enB, HIGH); analogWrite(pwmB, 0);
      digitalWrite(enC, HIGH); analogWrite(pwmC, velocity);
    }
  } else if (direction < 0) {
    if (ss == 5) { // A High, B Low, C Off
      digitalWrite(enA, HIGH); analogWrite(pwmA, velocity);
      digitalWrite(enB, HIGH); analogWrite(pwmB, 0);
    } else if (ss == 4) { // A High, C Low, B Off
      digitalWrite(enA, HIGH); analogWrite(pwmA, velocity);
      digitalWrite(enC, HIGH); analogWrite(pwmC, 0);
    } else if (ss == 6) { 
      digitalWrite(enB, HIGH); analogWrite(pwmB, velocity);
      digitalWrite(enC, HIGH); analogWrite(pwmC, 0);
    } else if (ss == 2) { 
      digitalWrite(enA, HIGH); analogWrite(pwmA, 0);
      digitalWrite(enB, HIGH); analogWrite(pwmB, velocity);
    } else if (ss == 3) { 
      digitalWrite(enA, HIGH); analogWrite(pwmA, 0);
      digitalWrite(enC, HIGH); analogWrite(pwmC, velocity);
    } else if (ss == 1) {
      digitalWrite(enB, HIGH); analogWrite(pwmB, 0);
      digitalWrite(enC, HIGH); analogWrite(pwmC, velocity);
    }
  }
}

void loop() {
  //direction = 255;  
  if(Serial.available()){ 
    int cmd = Serial.parseInt();
    if(cmd == 10){
      target_angle = Serial.parseInt();
    } else if(cmd == 20){
      TIMING_ADVANCE = Serial.parseInt();
    }
    
  }
  enc_angle = encoder.readAngle();
  unsigned long curr_time = micros();
  
  elapsed = curr_time - prev_time;

  if(elapsed > 2500){
    
    hall_index++;
    hall_index %= 6;
  
    prev_time = curr_time;
  

    distance = fabsf(enc_angle - prev_enc_angle);

    curr_speed = (distance + 0.25/elapsed + 0.25);

    direction = constrain((target_angle - enc_angle), -255, 255);

    fast_coil_state(getShiftedState(hall_state));
  } 
  
  
  
  //if(enc_angle != prev_enc_angle){
  if (0){
    //Serial.print("angle: ");
    Serial.print(sa);
    Serial.print(", ");
    Serial.print(sb);
    Serial.print(", ");
    Serial.println(sc);
    //Serial.print(", ");
    //Serial.println(sc);
      //Serial.print("angle: ");
      //Serial.println(angle);
  }
  prev_enc_angle = enc_angle;
  
}
