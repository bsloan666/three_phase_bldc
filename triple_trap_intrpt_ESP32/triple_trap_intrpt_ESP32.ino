#include <Wire.h>
#include <AS5600.h>
#include <Arduino.h>
#include <soc/gpio_struct.h>
#include <numeric>

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

//volatile uint32_t last_hall_time = 0;
//volatile uint32_t pulse_interval = 0;
//volatile float rotor_velocity = 0; // Electrical rad/s

volatile int hall_state = 0;
int8_t hall_index = 0;
float fudge = 1.04719;
volatile long enc_angle;
volatile long prev_enc_angle;
long target_angle;



//long interval = 100000;
bool sensorChanged;
int TIMING_ADVANCE = 4;

unsigned long prev_time = 0;
float speed_window[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
unsigned int speed_index = 0;
unsigned int speed_window_size = 5;
volatile float distance;
volatile float elapsed;


//const int ss_fwd[6] = {1, 3, 2, 6, 4, 5};
//const int ss_rev[6] = {5, 4, 6, 2, 3, 1};
//int bridge = 0;
//int polarity = 0;

volatile long direction  = 0;
long prev_direction  = 0;
int ss;

//volatile int sa;
//volatile int sb;
//volatile int sc;

//volatile int state;

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
    fast_coil_state(getShiftedState(hall_state));
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
  direction = constrain(direction * 1000, -255, 255);
  fast_coil_state(getShiftedState(hall_state));
}


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

void loop() {
  //direction = 255;  
  if(Serial.available()){ 
    int cmd = Serial.parseInt();
    if(cmd == 10){
      target_angle = Serial.parseInt();
      poke();
    } else if(cmd == 20){
      TIMING_ADVANCE = Serial.parseInt();
      poke();
    }
    
  }
  enc_angle = encoder.readAngle();
  unsigned long curr_time = micros();
  
  elapsed = curr_time - prev_time;

  prev_time = curr_time;
  
  distance = abs(enc_angle - prev_enc_angle);

  speed_window[speed_index] = (distance/elapsed);
  speed_index++;
  speed_index %=5;
  float curr_speed = 0.0;

  for(int i = 0; i < speed_window_size; i++){
    curr_speed += speed_window[i];
  }

  direction = constrain((target_angle - enc_angle), -255, 255);
    
  if(enc_angle != prev_enc_angle){
  //if (0){
    //Serial.print("angle: ");
    Serial.print(enc_angle);
    Serial.print(", ");
    Serial.print(target_angle);
    Serial.print(", ");
    Serial.print(direction);
    Serial.print(", ");
    Serial.print(curr_speed);
    Serial.print(", ");
    Serial.println(hall_state);
      //Serial.print("angle: ");
      //Serial.println(angle);
  }
  prev_enc_angle = enc_angle;
  
}
