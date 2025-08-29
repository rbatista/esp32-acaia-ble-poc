#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEScan.h>
#include <NimBLEAdvertisedDevice.h>

// Include the esp-arduino-ble-scales library
#include "remote_scales.h"
#include "remote_scales_plugin_registry.h"
#include "scales/acaia.h"

// Global variables
std::unique_ptr<RemoteScales> connectedScale = nullptr;
RemoteScalesScanner scanner;
bool isScanning = false;

// Weight callback function
void onWeightUpdate(float weight) {
    Serial.printf("Weight updated: %.1f g\n", weight);
}

// Log callback function
void onLogMessage(std::string message) {
    Serial.print("LOG: ");
    Serial.println(message.c_str());
}

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 Acaia Scale - Using esp-arduino-ble-scales Library");
    
    // Initialize BLE
    NimBLEDevice::init("ESP32-Acaia-Scale");
    
    // Register the Acaia plugin
    AcaiaScalesPlugin::apply();
    
    // Set up callbacks
    if (connectedScale) {
        connectedScale->setWeightUpdatedCallback(onWeightUpdate);
        connectedScale->setLogCallback(onLogMessage);
    }
    
    // Start scanning
    Serial.println("Starting BLE scan for Acaia scales...");
    scanner.initializeAsyncScan();
    isScanning = true;
}

void loop() {
    // If not connected, try to find and connect to a scale
    if (!connectedScale) {
        auto discoveredScales = scanner.getDiscoveredScales();
        
        for (const auto& device : discoveredScales) {
            Serial.printf("Found device: %s (%s)\n", 
                         device.getName().c_str(), 
                         device.getAddress().toString().c_str());
            
            // Check if it's an Acaia scale
            if (device.getName().find("ACAIA") != std::string::npos ||
                device.getName().find("PEARL") != std::string::npos ||
                device.getName().find("LUNAR") != std::string::npos ||
                device.getName().find("PYXIS") != std::string::npos ||
                device.getName().find("PROCH") != std::string::npos) {
                
                Serial.printf("Attempting to connect to %s...\n", device.getName().c_str());
                                // Create scale using the factory
                connectedScale = RemoteScalesFactory::getInstance()->create(device);
                
                if (connectedScale) {
                    // Set up callbacks
                    connectedScale->setWeightUpdatedCallback(onWeightUpdate);
                    connectedScale->setLogCallback(onLogMessage);
                    
                    // Try to connect
                    if (connectedScale->connect()) {
                        Serial.printf("Successfully connected to %s!\n", device.getName().c_str());
                        break;
                    } else {
                        Serial.printf("Failed to connect to %s\n", device.getName().c_str());
                        connectedScale.reset();
                    }
                }
            }
        }
        
        // If no scales found, restart scan
        if (discoveredScales.empty()) {
            static unsigned long lastScan = 0;
            if (millis() - lastScan > 10000) { // Scan every 10 seconds
                Serial.println("No scales found, restarting scan...");
                scanner.restartAsyncScan();
                lastScan = millis();
            }
        }
    } else {
        // Update connected scale
        connectedScale->update();
        
        // Check if still connected
        if (!connectedScale->isConnected()) {
            Serial.println("Scale disconnected");
            connectedScale->disconnect();
            connectedScale.reset();
            
            // Restart scanning
            scanner.restartAsyncScan();
            isScanning = true;
        } else {
            // Print current weight every 5 seconds
            static unsigned long lastWeightPrint = 0;
            if (millis() - lastWeightPrint > 5000) {
                float currentWeight = connectedScale->getWeight();
                Serial.printf("Current weight: %.1f g\n", currentWeight);
                lastWeightPrint = millis();
            }
            
        }
    }
    
    delay(100);
}

// ESP-IDF main function
extern "C" void app_main(void) {
    // Initialize Arduino
    initArduino();
    
    // Call Arduino setup
    setup();
    
    // Arduino main loop
    while (true) {
        loop();
        // Small delay to prevent watchdog issues
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
