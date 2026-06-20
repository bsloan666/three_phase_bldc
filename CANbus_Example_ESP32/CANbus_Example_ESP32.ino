#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>

// GPIO definitions for the CAN transceiver
#define CAN_TX_PIN 17
#define CAN_RX_PIN 16

uint32_t myNodeID = 0;

// Placeholder function for your existing logic
uint32_t readAddressFromSwitches() {
    // Insert your 4-pin digitalRead logic here
    // Example returns a 4-bit integer (0 to 15)
    return 5; 
}

void setup() {
    // 1. Read your physical address from the switches
    myNodeID = readAddressFromSwitches();

    // 2. Initialize the TWAI configuration structures
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)CAN_TX_PIN, (gpio_num_t)CAN_RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config  = TWAI_TIMING_CONFIG_1MBPS(); // Standard speed for robotic servos
    
    // 3. Set up the Hardware Acceptance Filter for your specific ID
    // This filters out traffic meant for the other 5 robotic servomotors
    twai_filter_config_t f_config;
    f_config.acceptance_code = (myNodeID << 21);          // Shift left for standard 11-bit identifier alignment
    f_config.acceptance_mask = ~(0x7FF << 21);            // Mask to strictly match this ID only
    f_config.single_filter = true;

    // 4. Start the driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        twai_start();
    }
}

void loop() {
    // Constructing an outgoing message using your dynamic node ID
    CanFrame txFrame;
    txFrame.identifier = myNodeID; // The dynamic 4-bit ID set by the physical switches
    txFrame.extd = 0;              // Standard 11-bit ID
    txFrame.data_length_code = 2;
    txFrame.data[0] = 0xAA;        // Example servo command data
    txFrame.data[1] = 0xBB;

    // Send the packet out to the bus
    ESP32Can.writeFrame(txFrame);
    delay(100);
}
