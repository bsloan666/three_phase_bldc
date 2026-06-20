#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>

// GPIO definitions for the CAN transceiver
#define CAN_TX_PIN 17
#define CAN_RX_PIN 16

uint32_t myNodeID = 0;

// Placeholder function for your existing logic
uint32_t readAddressFromSwitches() {
    // Insert your 4-pin digitalRead logic here
    return 5; // Example address set by switches
}

void setup() {
    // Initialize Serial to print received messages to your PC monitor
    Serial.begin(115200);
    while(!Serial); // Wait for serial monitor to open

    // 1. Read your physical address from the switches
    myNodeID = readAddressFromSwitches();
    Serial.print("Node initialized with CAN ID: ");
    Serial.println(myNodeID);

    // 2. Initialize the TWAI configuration structures
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)CAN_TX_PIN, (gpio_num_t)CAN_RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config  = TWAI_TIMING_CONFIG_1MBITS(); // Match this to your PC USB-to-CAN app speed
    
    // 3. Set up the Hardware Acceptance Filter for your specific ID
    twai_filter_config_t f_config;
    f_config.acceptance_code = (myNodeID << 21);          // Shift left for standard 11-bit alignment
    f_config.acceptance_mask = ~(0x7FF << 21);            // Mask to strictly match this ID only
    f_config.single_filter = true;

    // 4. Start the driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        twai_start();
        Serial.println("CAN Controller successfully started. Listening...");
    } else {
        Serial.println("Failed to install TWAI driver.");
    }
}

void loop() {
    // Create a temporary frame to hold incoming data
    CanFrame rxFrame;

    // Check if a message is waiting in the hardware RX queue
    // zero timeout means it checks instantly and moves on without blocking the loop
    if (ESP32Can.readFrame(rxFrame, 0) == ESP_OK) {
        
        // Print message details to the Arduino Serial Monitor
        Serial.print("Received Frame! ID: ");
        Serial.print(rxFrame.identifier);
        Serial.print(" | Data Length: ");
        Serial.print(rxFrame.data_length_code);
        Serial.print(" | Data: ");

        // Print each byte of the payload in Hexadecimal format
        for (int i = 0; i < rxFrame.data_length_code; i++) {
            Serial.print("0x");
            if (rxFrame.data[i] < 0x10) Serial.print("0"); // Pad single digits
            Serial.print(rxFrame.data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    // Small delay to keep the background tasks happy
    delay(1); 
}
