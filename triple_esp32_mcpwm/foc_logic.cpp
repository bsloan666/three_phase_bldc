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

