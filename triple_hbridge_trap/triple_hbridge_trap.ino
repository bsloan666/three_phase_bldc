
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

const int ss_fwd[6] = {1, 3, 2, 6, 4, 5};
int bridge = 0;
int polarity = 0;
int direction  = 0;
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
/*
void next_coil_state(int sensor_state){
  if(direction < 0) {
    bridge = reverse[sensor_state][0];
    polarity = reverse[sensor_state][1];
  } else if (direction > 0){
    bridge = forward[sensor_state][0];
    polarity = forward[sensor_state][1];
  } else {
    return;
  }
  if(direction != 0){
    for(int i =0; i < 6; i++){ 
      if( int(i/2) == bridge + polarity){
        analogWrite(coil_pins[i], abs(direction));
      } else {
        analogWrite(coil_pins[i], 0);
      }
    }
    bridge = (bridge + 1) % 3;
    polarity = (polarity + 1) % 2;
  }
}
*/
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
    direction = 92;
  } else {
    direction = -92;
  }
  


  hall_state = get_sensor_state();
  next_coil_state(hall_state);
  /*
  if (hall_state != last_state) {
    uint32_t now = micros();
    pulse_interval = now - last_hall_time;
    last_hall_time = now;
    last_state = hall_state;

    // Calculate Velocity: 60 degrees (PI/3) per pulse_width
    rotor_velocity = 1.04719 / pulse_interval; 
    
  }
  
  angle = get_smooth_angle(pulse_interval) * 180/3.14159;
  write_foc(int(angle));
  // next_coil_state();
  */

  if(0){
    Serial.println(hall_state);
     //Serial.print(", ");
     // Serial.print(bridge);
      //Serial.print("angle: ");
      //Serial.println(angle);
  }
}
