#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Firebase_ESP_Client.h>
#include <time.h>


// SSID for connecting to the WiFi network
const char* ssid = "Thanh Liem";

// Password for connecting to the WiFi network
const char* password = "Machao1510";


// const float BETA = 3950; // should match the Beta Coefficient of the thermistor
// const char* serverName = "https://api.telegram.org/bot6266365592:AAEGRWCHLU6Jq14fCv8IpNzKCxEAncwLMww/sendMessage";
// const char* serverName = "https://testcloud-a615d-default-rtdb.asia-southeast1.firebasedatabase.app/sensors.json";

// Insert Firebase project API Key
#define API_KEY "AIzaSyA3rxaWh38t8nyIhPy6o6f3LhYkWs5NKrU"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://testcloud-a615d-default-rtdb.asia-southeast1.firebasedatabase.app"

// Device ID
#define DEVICE_ID "5935177714"

// Sensor pin
#define SENSOR_PIN A0

// Time server
#define ntpServer "pool.ntp.org"
#define gmtOffset_sec 7 * 3600
#define daylightOffset_sec 0

// Initialize Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setupWifi();
void setupFirebase();
void SendData(int temperature, String time);
String GetTime();
int ReadTemperatureInCelsius();

void setup()
{
	Serial.begin(115200);

	setupWifi();
	delay(500);

	setupFirebase();

	// Init and get the time
	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

	// Set sensor pin
	pinMode(SENSOR_PIN, INPUT);
}

void loop()
{
	int celsius = ReadTemperatureInCelsius();
	Serial.printf("[%s] Temperature: %d\n", GetTime().c_str(), celsius);

	SendData(celsius, GetTime());

	delay(5000);
}

// Set up WiFi connection
void setupWifi()
{
	WiFi.mode(WIFI_STA); // Set the WiFi mode to station mode
	Serial.println("\nConnecting"); // Print message to indicate connection process
	WiFi.begin(ssid, password); // Connect to WiFi network using provided SSID and password
	while (WiFi.status() != WL_CONNECTED) // Wait until WiFi is connected
	{
		Serial.print("."); // Print a dot to indicate waiting
		delay(500); // Delay for 500 milliseconds
	}
	Serial.println("\nConnected to the WiFi network"); // Print message to indicate successful connection
	Serial.print("Local ESP32 IP: "); // Print message to indicate local IP address
	Serial.println(WiFi.localIP()); // Print the local IP address of the ESP32
}

/**
 * @brief Set up the Firebase configuration and connect to the Firebase server
 */
void setupFirebase()
{
	// Set the API key and database URL
	config.api_key = API_KEY;
	config.database_url = DATABASE_URL;

	// Sign up with the Firebase configuration
	if (Firebase.signUp(&config, &auth, "", ""))
	{
		Serial.println("Signup successfully");
	}
	else
	{
		Serial.printf("%s\n", config.signer.signupError.message.c_str());
	}

	Serial.println("Connecting to Firebase:");

	// Begin the Firebase connection
	Firebase.begin(&config, &auth);

	// Wait for the Firebase connection to be ready
	do
	{
		Firebase.reconnectWiFi(true);
		Serial.print(".");
		delay(500);
	} while (!Firebase.ready());

	Serial.println("Firebase connected");
}

/**
 * @brief Sends temperature data and time to Firebase database.
 *
 * @param temperature The temperature data to send.
 * @param time The time data to send.
 */
inline void SendData(int temperature, String time)
{
	FirebaseJson json;
	json.add("time", time);
	json.add("temperature", temperature);

	if (Firebase.RTDB.pushJSON(&fbdo, "/sensors", &json)) // Push JSON data to Firebase
	{
		Serial.println("Send data to Firebase successfully");
	}
	else
	{
		Serial.printf("%s\n", fbdo.errorReason().c_str()); // Print error message if data sending fails
	}
}

// This function returns the current time as a string.
// If the time cannot be obtained, it returns an empty string.
String GetTime()
{
	tm timeinfo;

	// Attempt to obtain the local time
	if (!getLocalTime(&timeinfo))
	{
		Serial.println("Failed to obtain time");
		return "";
	}

	// Convert the time to a string
	String time = asctime(&timeinfo);

	// Remove the trailing newline character, if present
	if (time.endsWith("\n"))
	{
		time = time.substring(0, time.length() - 1);
	}

	return time;
}
/**
 * Read the temperature in Celsius from a sensor.
 *
 * @return The temperature in Celsius.
 */
inline int readTemperatureInCelsius() {
	// Read the analog value from the sensor
	int analogValue = analogRead(SENSOR_PIN);

	// Convert the analog value to temperature in Celsius
	float voltage = analogValue / 1023.f * 5.f;
	int temperature = voltage * 100.f;

	return temperature;
}
