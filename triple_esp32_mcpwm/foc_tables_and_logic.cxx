#include "driver/mcpwm.h"

void setup_motor_hardware() {
    // 1. Initialize GPIOs for MCPWM
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, 15);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, 16);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, 17);

    // 2. Configure PWM frequency and counter mode
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 20000;    // 20kHz (above human hearing)
    pwm_config.cmpr_a = 0;           // Duty cycle start at 0%
    pwm_config.cmpr_b = 0;
    pwm_config.counter_mode = MCPWM_UP_DOWN_COUNTER; // Center-aligned
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

    // 3. Apply config to all 3 phases
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);
}

// To update the motor field in your FOC loop:
void update_phases(float a, float b, float c) {
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, a);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, b);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, c);
}


// Pre-calculated Sine Table (Offset to be all positive: 0 to 255)
// This represents one full electrical cycle (360 degrees)
const uint8_t SINE_LUT[256] = {
  128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167, 170, 173, 
  176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 206, 208, 211, 213, 215, 
  218, 220, 222, 224, 226, 228, 230, 232, 234, 235, 237, 238, 240, 241, 243, 244, 
  245, 246, 247, 248, 249, 250, 251, 252, 252, 253, 253, 254, 254, 254, 255, 255, 
  255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 252, 251, 250, 249, 248, 247, 
  246, 245, 244, 243, 241, 240, 238, 237, 235, 234, 232, 230, 228, 226, 224, 222, 
  220, 218, 215, 213, 211, 208, 206, 203, 201, 198, 196, 193, 190, 188, 185, 182, 
  179, 176, 173, 170, 167, 165, 162, 158, 155, 152, 149, 146, 143, 140, 137, 134, 
  131, 128, 124, 121, 118, 115, 112, 109, 106, 103, 100, 97, 93, 90, 88, 85, 
  82, 79, 76, 73, 70, 67, 65, 62, 59, 57, 54, 52, 49, 47, 44, 42, 
  40, 37, 35, 33, 31, 29, 27, 25, 23, 21, 20, 18, 17, 15, 14, 12, 
  11, 10, 9, 8, 7, 6, 5, 4, 3, 3, 2, 2, 1, 1, 1, 0, 
  0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 5, 6, 7, 
  8, 9, 10, 11, 12, 14, 15, 17, 18, 20, 21, 23, 25, 27, 29, 31, 
  33, 35, 37, 40, 42, 44, 47, 49, 52, 54, 57, 59, 62, 65, 67, 70, 
  73, 76, 79, 82, 85, 88, 90, 93, 97, 100, 103, 106, 109, 112, 115, 118, 121, 124
};


void update_motor_position(uint8_t angle) {
  // Phase A is at the current angle
  uint8_t dutyA = SINE_LUT[angle];
  
  // Phase B is offset by 120 degrees (85 steps in a 256-step circle)
  uint8_t dutyB = SINE_LUT[(angle + 85) % 256];
  
  // Phase C is offset by 240 degrees (171 steps)
  uint8_t dutyC = SINE_LUT[(angle + 171) % 256];

  // Set the PWM to your 3 IBT-2 RPWM pins
  analogWrite(PIN_A, dutyA);
  analogWrite(PIN_B, dutyB);
  analogWrite(PIN_C, dutyC);
}

// Variables for Interpolation
volatile uint32_t last_hall_time = 0;
volatile uint32_t pulse_interval = 0;
volatile float rotor_velocity = 0; // Electrical rad/s
int8_t hall_state = 0;

// This interrupt runs every time a Hall sensor flips
void IRAM_ATTR onHallChange() {
    uint32_t now = esp_timer_get_time(); // Microseconds
    pulse_interval = now - last_hall_time;
    last_hall_time = now;

    // Read current state (32, 33, 34)
    int A = digitalRead(32);
    int B = digitalRead(33);
    int C = digitalRead(34);
    hall_state = (A << 2) | (B << 1) | C;

    // Calculate current velocity in rad/us
    // 60 degrees = PI/3 radians
    rotor_velocity = (M_PI / 3.0) / (float)pulse_interval;
}

// This function gives you the "Smooth Angle" for your Sine Table
float get_smooth_angle() {
    uint32_t time_since_flip = esp_timer_get_time() - last_hall_time;
    
    // Start with the base angle of the current Hall sector
    float base_angle = sector_to_angle[hall_state]; 
    
    // Add the "guessed" progress based on velocity
    float interpolation = rotor_velocity * time_since_flip;
    
    // Cap interpolation at 60 degrees (PI/3) so it doesn't overshoot
    if (interpolation > M_PI / 3.0) interpolation = M_PI / 3.0;

    return base_angle + interpolation;
}


void loop() {
  float angle = get_interpolated_angle(); // Uses Hall sensors + time
  
  // FOC "Sine" Math
  float duty_A = sin(angle) * power;
  float duty_B = sin(angle + 2.0*PI/3.0) * power;
  float duty_C = sin(angle + 4.0*PI/3.0) * power;

  // Send to MCPWM hardware
  update_mcpwm(duty_A, duty_B, duty_C);
}
