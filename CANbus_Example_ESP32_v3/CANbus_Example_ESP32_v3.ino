#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>

void setup() {
    Serial.begin(115200);
    
    // Forces the ESP32 to pause completely until you physically click 
    // "Serial Monitor" in the Arduino IDE. Eliminates missed startup strings.
    while(!Serial) {
        delay(10); 
    }
    
    Serial.println("\n--- ESP32 Boot Verification Start ---");

    // Reassigned to universally safe bidirectional pins 
    ESP32Can.setPins(4, 5); 

    // Initialize at 1Mbps
    if(ESP32Can.begin(ESP32Can.convertSpeed(1000))) {
        Serial.println("SUCCESS: CAN bus native controller is active!");
        Serial.println("Listening for incoming frames on Pins 4 & 5...");
    } else {
        Serial.println("FAILURE: Initialization failed. Check pin selection.");
    }
}

void loop() {
    CanFrame rxFrame;
    
    // Only print if a packet with real data bytes hits the register
    if(ESP32Can.readFrame(rxFrame, 0) && rxFrame.data_length_code > 0) { 
        Serial.printf("ID: 0x%03X | Length: %X | Data: ", rxFrame.identifier, rxFrame.data_length_code);
        for(int i = 0; i < rxFrame.data_length_code; i++) {
            Serial.printf("%02X ", rxFrame.data[i]);
        }
        Serial.println();
    }
    delay(1);
}
