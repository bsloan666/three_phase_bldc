
const int coil_pins[6] = {3, 5, 6, 9, 10, 11};

const int pwmA = 3;
const int enA = 5;
const int pwmB = 6;
const int enB = 9;
const int pwmC = 10;
const int enC = 11;
volatile uint32_t last_hall_time = 0;
volatile uint32_t pulse_interval = 0;
volatile float rotor_velocity = 0; // Electrical rad/s
int8_t hall_state = 0;
int8_t last_state = 0;
float fudge = 1.04719;
float angle;
unsigned long counter = 0;
const int ss_fwd[6] = {1, 3, 2, 6, 4, 5};
int bridge = 0;
int polarity = 0;
int direction  = 1;
int ss;
int sa;
int sb;
int sc;
int state;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for(int pin = 0; pin < 6; pin++) {
    pinMode(coil_pins[pin], OUTPUT);
  }
  digitalWrite(enA, HIGH); digitalWrite(enB, HIGH); digitalWrite(enC, HIGH);
}

int get_sensor_state(){
  sa = analogRead(A0) > 80? 1 : 0;
  sb = analogRead(A1) > 80? 1 : 0;
  sc = analogRead(A2) > 80? 1 : 0;

  state = sa << 2 | sb << 1 | sc;

  return state;
}
const int reverse[8][2] = {{0, 0}, {0, 1}, {1, 1}, {2, 0}, {2, 1}, {1, 0}, {0, 0}, {0, 0}};
const int forward[8][2] = {{0, 0}, {0, 0}, {1, 0}, {2, 1}, {2, 0}, {1, 1}, {0, 1}, {0, 0}};

  const uint8_t saddle_table[60] = {
    128, 141, 154, 167, 180, 191, 203, 213, 223, 231, 238, 244, 249, 253, 255, 256, 255, 
    253, 249, 244, 238, 231, 223, 213, 203, 192, 180, 167, 154, 141, 128, 115, 102, 
    89, 76, 65, 53, 43, 33, 25, 18, 12, 7, 3, 1, 0, 1, 3, 7, 
    12, 18, 25, 33, 43, 53, 64, 76, 89, 102, 115, 
  };


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


  


float get_smooth_angle(uint32_t time_since_flip) {
    

    // Start with the base angle of the current Hall sector
    float base_angle = sector_to_angle[hall_state];

    // Add the "guessed" progress based on velocity
    float interpolation = rotor_velocity * time_since_flip;
   // Serial.println(interpolation);
    // Cap interpolation at 60 degrees (PI/3) so it doesn't overshoot
    if (interpolation > M_PI / 3.0) interpolation = M_PI / 3.0;

    return base_angle + interpolation + fudge;
    //return base_angle + fudge;
}

void write_foc(unsigned long electrical_angle) {
  // electrical_angle is 0-119
  int angleA = electrical_angle % 60;
  int angleB = (electrical_angle + 20) % 60; // 120 degrees offset
  int angleC = (electrical_angle + 40) % 60; // 240 degrees offset
   
  analogWrite(pwmA, saddle_table[angleA]/4);
  analogWrite(pwmB, saddle_table[angleB]/4);
  analogWrite(pwmC, saddle_table[angleC]/4);

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
    if (ss == 1) { // A High, B Low, C Off
      digitalWrite(enA, HIGH); analogWrite(pwmA, velocity);
      digitalWrite(enB, HIGH); analogWrite(pwmB, 0);
    } else if (ss == 3) { // A High, C Low, B Off
      digitalWrite(enA, HIGH); analogWrite(pwmA, velocity);
      digitalWrite(enC, HIGH); analogWrite(pwmC, 0);
    } else if (ss == 2) { 
      digitalWrite(enB, HIGH); analogWrite(pwmB, velocity);
      digitalWrite(enC, HIGH); analogWrite(pwmC, 0);
    } else if (ss == 6) { 
      digitalWrite(enA, HIGH); analogWrite(pwmA, 0);
      digitalWrite(enB, HIGH); analogWrite(pwmB, velocity);
    } else if (ss == 4) { 
      digitalWrite(enA, HIGH); analogWrite(pwmA, 0);
      digitalWrite(enC, HIGH); analogWrite(pwmC, velocity);
    } else if (ss == 5) {
      digitalWrite(enB, HIGH); analogWrite(pwmB, 0);
      digitalWrite(enC, HIGH); analogWrite(pwmC, velocity);
    }
  } else if (direction < 0) {
    if (ss == 6) { // A High, B Low, C Off
      digitalWrite(enA, HIGH); analogWrite(pwmA, velocity);
      digitalWrite(enB, HIGH); analogWrite(pwmB, 0);
    } else if (ss == 4) { // A High, C Low, B Off
      digitalWrite(enA, HIGH); analogWrite(pwmA, velocity);
      digitalWrite(enC, HIGH); analogWrite(pwmC, 0);
    } else if (ss == 5) { 
      digitalWrite(enB, HIGH); analogWrite(pwmB, velocity);
      digitalWrite(enC, HIGH); analogWrite(pwmC, 0);
    } else if (ss == 1) { 
      digitalWrite(enA, HIGH); analogWrite(pwmA, 0);
      digitalWrite(enB, HIGH); analogWrite(pwmB, velocity);
    } else if (ss == 3) { 
      digitalWrite(enA, HIGH); analogWrite(pwmA, 0);
      digitalWrite(enC, HIGH); analogWrite(pwmC, velocity);
    } else if (ss == 2) {
      digitalWrite(enB, HIGH); analogWrite(pwmB, 0);
      digitalWrite(enC, HIGH); analogWrite(pwmC, velocity);
    }
  }
}

void loop() {
  //direction = 255;  
  
  int tick = millis() % 5000;
  if (tick < 2500){
    direction = 255;
  } else {
    direction = -255;
  }
  


  //hall_state = get_sensor_state();
  // next_coil_state(hall_state);
  

    //uint32_t now = micros();
    //pulse_interval = now - last_hall_time;
   // last_hall_time = now;
    
    // Serial.println(pulse_interval);

    // Calculate Velocity: 60 degrees (PI/3) per pulse_width
    //rotor_velocity = 1.04719 /pulse_interval; 
    //Serial.println(rotor_velocity);
    //angle = get_smooth_angle(pulse_interval);
    write_foc(counter);

    counter++;



  delay(1);
  //last_state = hall_state;

  // next_coil_state();
  

  if(0){
    Serial.println(hall_state);
     //Serial.print(", ");
     // Serial.print(bridge);
      //Serial.print("angle: ");
      //Serial.println(angle);
  }
}
