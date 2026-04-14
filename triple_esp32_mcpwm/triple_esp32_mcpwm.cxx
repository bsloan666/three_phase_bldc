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

