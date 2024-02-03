#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

#define DHTPIN 2        // Digital pin connected to the DHT11 sensor
#define DHTTYPE DHT11   // DHT sensor type (DHT11 or DHT22)
#define MQ2PIN 34       // Analog pin connected to the MQ-2 sensor
#define MQ3PIN 35       // Analog pin connected to the MQ-3 sensor
#define ledPin 13        // Pin to which an LED is connected (for indication)

const char *ssid = "MAX";          // Replace with your Wi-Fi network SSID
const char *password = "12344321";  // Replace with your Wi-Fi network password
const char *thingSpeakApiKey = "BII0L17AN39ZK42E";  // Replace with your ThingSpeak API key
const unsigned long channelNumber = 2372036;  // Replace with your ThingSpeak channel number

WiFiClient client;  // Declare a WiFiClient object
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  ThingSpeak.begin(client);  // Initialize ThingSpeak with the WiFi client
  delay(100);
}

void loop() {
  float mq2Concentration = readGasConcentration(MQ2PIN);
  float mq3Concentration = readAlcoholConcentration(MQ3PIN);

  float temperature = dht.readTemperature();  // Read temperature in Celsius
  float humidity = dht.readHumidity();       // Read humidity

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Display readings on the serial monitor
  Serial.print("MQ-2 Gas Concentration: ");
  Serial.print(mq2Concentration);
  Serial.println(" ppm");

  Serial.print("MQ-3 Alcohol Concentration: ");
  Serial.print(mq3Concentration);
  Serial.println(" ppm");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Send data to ThingSpeak
  ThingSpeak.setField(1, mq2Concentration);
  ThingSpeak.setField(2, mq3Concentration);
  ThingSpeak.setField(3, temperature);
  ThingSpeak.setField(4, humidity);

  int status = ThingSpeak.writeFields(channelNumber, thingSpeakApiKey);
  if (status == 200) {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.println("Failed to send data to ThingSpeak");
  }

 

  delay(2000);  // Delay for 2 seconds before the next reading
}

float readGasConcentration(int pin) {
  int sensorValue = analogRead(pin);
  float voltage = sensorValue * (5.0 / 1023.0);
  float concentration = 100.0 * voltage;  // 

  return concentration;
}

float readAlcoholConcentration(int pin) {
  int sensorValue = analogRead(pin);
  float voltage = sensorValue * (5.0 / 1023.0);
  float concentration = 10.0 * voltage;  // a

  return concentration;
}
