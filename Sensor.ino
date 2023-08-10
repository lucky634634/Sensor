#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Firebase_ESP_Client.h>
#include <time.h>
const char* ssid = "Thanh Liem";
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

void setup()
{
	Serial.begin(115200);

	// analogReadResolution(10);
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
	int analogValue = analogRead(A0);

	int celsius = (analogValue / 1023.f) * 5.f * 100.f;

	Serial.printf("[%s] Temperature: %d\n", GetTime().c_str(), celsius);

	SendData(celsius, GetTime());

	delay(5000);
}


inline void setupWifi()
{
	WiFi.mode(WIFI_STA); //Optional
	Serial.println("\nConnecting");
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(500);
	}
	Serial.println("\nConnected to the WiFi network");
	Serial.print("Local ESP32 IP: ");
	Serial.println(WiFi.localIP());
}

void setupFirebase()
{
	config.api_key = API_KEY;
	config.database_url = DATABASE_URL;

	if (Firebase.signUp(&config, &auth, "", ""))
	{
		Serial.println("Signup successfully");
	}
	else
	{
		Serial.printf("%s\n", config.signer.signupError.message.c_str());
	}

	Serial.println("Connecting to Firebase:");

	Firebase.begin(&config, &auth);
	do
	{
		Firebase.reconnectWiFi(true);
		Serial.print(".");
		delay(500);
	} while (!Firebase.ready());

	Serial.println("Firebase connected");
}

inline void SendData(int temperature, String time)
{
	FirebaseJson json;
	json.add("time", time);
	json.add("temperature", temperature);

	if (Firebase.RTDB.pushJSON(&fbdo, "/sensors", &json))
	{
		Serial.println("Send data to Firebase successfully");
	}
	else
	{
		Serial.printf("%s\n", fbdo.errorReason().c_str());
	}
}

inline String GetTime()
{
	tm timeinfo;
	if (!getLocalTime(&timeinfo))
	{
		Serial.println("Failed to obtain time");
		return "";
	}
	String time = asctime(&timeinfo);
	if (time.endsWith("\n"))
		time = time.substring(0, time.length() - 1);
	return time;
}

