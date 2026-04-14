
const int coil_pins[6] = {3, 5, 6, 9, 10, 11};

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

void loop() {  
  unsigned int msecs = millis() % 4000;

  if(msecs < 2000){
    direction = 255;
  } else {
    direction = -255;
  }

  ss = get_sensor_state();
  next_coil_state(ss);


  if(0){
    Serial.print(ss);
     Serial.print(", ");
      Serial.print(bridge);
       Serial.print(", ");
      Serial.println(msecs);
  }
}
