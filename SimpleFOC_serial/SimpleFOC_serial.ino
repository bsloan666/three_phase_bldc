/**
 * Comprehensive BLDC motor control example using encoder
 *
 * Using serial terminal user can send motor commands and configure the motor and FOC in real-time:
 * - configure PID controller constants
 * - change motion control loops
 * - monitor motor variabels
 * - set target values
 * - check all the configuration values
 *
 * See more info in docs.simplefoc.com/commander_interface
 */
#include <SimpleFOC.h>
float output_target = 0.0;
// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(6);
BLDCDriver3PWM driver = BLDCDriver3PWM(13, 18, 25, 14, 19, 32);

// Stepper motor & driver instance
//StepperMotor motor = StepperMotor(50);
//StepperDriver4PWM driver = StepperDriver4PWM(9, 5, 10, 6,  8);

// encoder instance
MagneticSensorI2C output_sensor = MagneticSensorI2C(0x36, 12, 0x0E, 4); 
//Encoder encoder = Encoder(2, 3, 8192);
HallSensor electrical_sensor = HallSensor(12, 26, 27, 6);
// With shuffled values

void doA(){electrical_sensor.handleA();}
void doB(){electrical_sensor.handleB();}
void doC(){electrical_sensor.handleC();}



// commander interface
Commander command = Commander(Serial);
void onMotor(char* cmd){ command.motor(&motor, cmd); }

void doHall(char* cmd) {
  // Force pullups again in case something reset them
  pinMode(12, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);
  
  Serial.print("Hall Pins [12, 26, 27]: ");
  Serial.print(digitalRead(12));
  Serial.print(digitalRead(26));
  Serial.println(digitalRead(27));
}

PIDController pid_output_position = 
    PIDController(2.0, 0.0, 0.05, 100.0, 20.0); // P, I, D, ramp, limit

void doOffset(char* cmd) {
  if (cmd[1] == '+') {
    motor.zero_electric_angle += 0.05; // Fine-tune up
  } else if (cmd[1] == '-') {
    motor.zero_electric_angle -= 0.05; // Fine-tune down
  } else {
    int step = atoi(&cmd[1]);
    // Each step is exactly 60 electrical degrees (1.0472 Radians)
    motor.zero_electric_angle = step * 1.0472;
    Serial.print("Testing Offset Step: ");
    Serial.println(step);
  }
  
  Serial.print("Set zero_electric_angle to: ");
  Serial.println(motor.zero_electric_angle);
}

bool emergency_stop = false;

void doReset(char* cmd) {
  emergency_stop = false;
  driver.enable();
  Serial.println("Emergency stop cleared. Driver re-enabled.");
}

void doTarget(char* cmd) {
  // 1. Parse the incoming serial string and save it to our global position target
   
  command.scalar(&output_target, cmd);
  
  // 2. Print confirmation back to the user
  Serial.print("Target updated! Output shaft moving to: ");
  Serial.print(output_target);
  Serial.println(" radians.");
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Give serial monitor time to open
  Serial.println("Initializing system...");
  
  // 1. Initialize Hall Sensors FIRST with pullups
  pinMode(12, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);
  delay(10); // Stability delay
  
  electrical_sensor.init();
  electrical_sensor.enableInterrupts(doA, doB, doC);
  
  // Reinforce pullups AFTER init
  pinMode(12, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);

  // 2. Initialize I2C for AS5600
  Wire.begin(21, 22);
  Wire.setClock(400000);
  output_sensor.init();

  // 1. Link the Hall Sensor to the motor for FOC commutation
  motor.linkSensor(&electrical_sensor);

  motor.useMonitoring(Serial);
  motor.monitor_variables = _MON_TARGET | _MON_VEL | _MON_ANGLE | _MON_VOLT_Q;
  command.add('T', doTarget, "target position");
  command.add('A', doOffset, "change hall angle");
  command.add('M', onMotor, "motor");
  command.add('H', doHall, "hall debug");
  command.add('R', doReset, "reset emergency stop");

  driver.pwm_frequency = 10000;
  driver.voltage_power_supply = 24;
    
  driver.init();
  motor.linkDriver(&driver);

  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;
  //motor.foc_modulation = FOCModulationType::SinePWM;
  // 2. Set motor to VELOCITY mode. The outer loop will feed it velocity targets.
  
  motor.controller = MotionControlType::velocity; 
  // Inner loop velocity tuning (Using Hall Sensors)
  motor.PID_velocity.P = 0.2; 
  motor.PID_velocity.I = 1.0;     // Lowered from 2.0 to reduce overshoot
  motor.PID_velocity.D = 0.005;   // Added small D for high-frequency damping
  motor.PID_velocity.output_ramp = 1000; // Smoothen voltage application
  motor.voltage_limit = 20;   // Increased to 20V for more starting power

  pid_output_position.P = 2.0;    // Increased from 1.0 for more "pull"
  pid_output_position.I = 0.02; 
  pid_output_position.D = 0.2;    // Increased from 0.1 for more damping

  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);
  pinMode(32, OUTPUT);
  digitalWrite(32, HIGH);
  motor.init();
  motor.sensor_direction = Direction::CCW; // Flipped from CW to see if it fixes runaway
  motor.zero_electric_angle = 2.0944;      // Pre-set to A2 step (2 * 1.0472)

  motor.initFOC();
  Serial.println("Begin...");
}
void loop() {
  // 1. Run core FOC commutation (Hall sensors)
  motor.loopFOC();

  // 2. Read the absolute physical position of the AS5600 output shaft
  output_sensor.update();
  float current_output_angle = output_sensor.getAngle();

  // 3. Cascade Calculation
  float error = output_target - current_output_angle;
  float target_motor_velocity = 0;
  
  static uint32_t stall_timer = 0;
  static float last_angle_check = 0;

  if (emergency_stop) {
    target_motor_velocity = 0;
    driver.disable();
  } else if (abs(error) > 0.05) { 
    float required_output_velocity = pid_output_position(error);
    target_motor_velocity = required_output_velocity * 50.0;

    // Stiction Compensation (The "Shove")
    if (target_motor_velocity > 0) target_motor_velocity += 20.0;
    else if (target_motor_velocity < 0) target_motor_velocity -= 20.0;

    // Stall Protection Logic
    if (abs(current_output_angle - last_angle_check) < 0.005) {
      if (millis() - stall_timer > 3000) { // 3 seconds of load without motion
        emergency_stop = true;
        driver.disable();
        Serial.println("\n!!! STALL DETECTED - EMERGENCY SHUTDOWN !!!");
        Serial.println("Coils disabled to prevent melting. Check mechanics and send 'R' to reset.");
      }
    } else {
      stall_timer = millis();
      last_angle_check = current_output_angle;
    }
  } else {
    stall_timer = millis(); // Reset timer when target reached
  }

  // 4. Update motor target
  motor.move(target_motor_velocity);

  // 5. Run serial commander monitoring and parsing
  static uint32_t last_monitor = 0;
  if (millis() - last_monitor > 100) { // Throttle monitoring to 10Hz
    motor.monitor();
    last_monitor = millis();
  }
  command.run();

  // 6. Diagnostic Heartbeat (Prints every 1 second)
  static uint32_t last_heartbeat = 0;
  if (millis() - last_heartbeat > 1000) {
    Serial.print("DIAG >> ");
    Serial.printf("Target: %.2f | AS5600: %.2f | Hall Angle: %.2f | Motor Target: %.2f\n", 
                  output_target, current_output_angle, motor.shaft_angle, target_motor_velocity);
    last_heartbeat = millis();
  }
}
