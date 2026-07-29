#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h> // Hardware-specific library for ILI9341 2.4" TFT
#include <SPI.h>
#include "qrcode.h"          // Uses the ESP32 native internal library

// Define Pins for ESP32
#define TFT_CS   15
#define TFT_DC   27
#define TFT_RST  4

// Initialize Adafruit ILI9341 Display
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Declare the callback function ahead of time
void drawQRCallback(esp_qrcode_handle_t qrcode);

void setup() {
    Serial.begin(115200);
    
    // Initialize display
    tft.begin();
    tft.setRotation(1); // Set landscape mode to fit 320x240 layout
    tft.fillScreen(ILI9341_WHITE); // White background works best for QR scanning

    // Generate and display QR Code
    generateESP32QRCode("https://hackster.io");
}

void loop() {
    // Keep static
}

void generateESP32QRCode(const char* msg) {
    // Configure ESP32 native QR generator with matching properties
    esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
    cfg.display_func = drawQRCallback;   // Direct reference to whole-grid callback
    cfg.max_qrcode_version = 10;         // Correct parameter name for max version limit
    cfg.qrcode_ecc_level = ESP_QRCODE_ECC_LOW;

    // Generate and automatically output via the callback function
    esp_err_t ret = esp_qrcode_generate(&cfg, msg);
    if (ret == ESP_OK) {
        Serial.println("QR Code Rendered Successfully using ESP32 Core!");
    } else {
        Serial.println("Failed to generate QR code.");
    }
}

// Fixed Callback function parsing the complete matrix array onto the TFT screen
void drawQRCallback(esp_qrcode_handle_t qrcode) {
    int qr_size = esp_qrcode_get_size(qrcode); // Get the width/height of the QR array
    int scale = 4;                             // Pixel scaling multiplier for 2.4" screen
    
    // Center the custom-sized QR block perfectly on your 320x240 screen
    int x_offset = (320 - (qr_size * scale)) / 2; 
    int y_offset = (240 - (qr_size * scale)) / 2;

    // Process rows and columns across the matrix layout
    for (int y = 0; y < qr_size; y++) {
        for (int x = 0; x < qr_size; x++) {
            // Check if the individual bit module should be black or white
            if (esp_qrcode_get_module(qrcode, x, y)) {
                tft.fillRect(x_offset + (x * scale), y_offset + (y * scale), scale, scale, ILI9341_BLACK);
            } else {
                tft.fillRect(x_offset + (x * scale), y_offset + (y * scale), scale, scale, ILI9341_WHITE);
            }
        }
    }
}
