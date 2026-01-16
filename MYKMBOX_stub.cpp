// MYKMBOX.lib stub implementation
// This is a stub implementation for missing MYKMBOX.lib library
// Provides dummy implementations for all functions used in KmBox

#include <iostream>
#include <string>
#include "KmBox.h"  // Include header for KmBoxBManager class definition

// Forward declarations for stub functions
void send_command(void* hSerial, const std::string& command);
std::string find_port(const std::string& targetDescription);
bool open_port(void*& hSerial, const char* portName, unsigned long baudRate);

// Global instance for MAKCU support (class definition moved to KmBox.h)
KmBoxBManager kmBoxBMgr;

// Utility functions (stub implementations for missing functions)
void send_command(void* hSerial, const std::string& command) {
    std::cout << "[STUB] send_command called with command: " << command << std::endl;

    // Special handling for MAKCU device check
    if (command == "km.version()\r") {
        std::cout << "[STUB] MAKCU responding with version info..." << std::endl;
        // Simulate MAKCU device response
        // In real implementation, this would be sent back through serial port
        // For now, we'll just log that MAKCU is responding
    }
}

std::string find_port(const std::string& targetDescription) {
    std::cout << "[STUB] find_port called with description: " << targetDescription << std::endl;
    return "COM1"; // Dummy port
}

bool open_port(void*& hSerial, const char* portName, unsigned long baudRate) {
    std::cout << "[STUB] open_port called with port: " << portName << ", baud: " << baudRate << std::endl;
    hSerial = (void*)1; // Dummy handle
    return true;
}