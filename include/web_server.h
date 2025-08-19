#pragma once
#include <Arduino.h>

// Initialize and start the web server
void webServerSetup();

// Handle client requests (call in loop)
void webServerLoop();
