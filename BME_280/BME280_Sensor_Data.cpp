#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Wi-Fi credentials
const char* ssid = "****";  // Network SSID
const char* password = "****";  // Network password

// Custom SDA and SCL pins for I2C
#define SDA_PIN 8  
#define SCL_PIN 18  

// Create a BME280 instance
Adafruit_BME280 bme;

// Google Apps Script Web App URL
const char* scriptURL = "******";

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  // Initialize I2C with custom SDA and SCL pins
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Check if the BME280 sensor is connected and initialize it
  if (!bme.begin(0x76)) {  
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
  Serial.println("BME280 sensor initialized successfully");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Read temperature, humidity, and pressure from the BME280 sensor
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;  // Convert Pa to hPa

    // Prepare the data to send in JSON format
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["temperature"] = temperature;
    jsonDoc["humidity"] = humidity;
    jsonDoc["pressure"] = pressure;
    
    String postData;
    serializeJson(jsonDoc, postData);
    
    // Initialize HTTP connection
    http.begin(scriptURL);
    
    // Set the content type to JSON
    http.addHeader("Content-Type", "application/json");

    // Send HTTP POST request
    int httpResponseCode = http.POST(postData);
    
    // Print HTTP response code
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response from server: " + response);
    } else {
      Serial.println("Error in sending POST request");
    }

    // Close HTTP connection
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  // Wait before sending new data
  delay(60000);  // Send data every 60 seconds
}
