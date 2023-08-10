#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Firebase_ESP_Client.h>

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

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

void setupWifi();
void setupFirebase();
void SendTemperature(int temp);

void setup()
{
	Serial.begin(9600);

	// analogReadResolution(10);
	setupWifi();
	delay(500);

	setupFirebase();

	pinMode(A0, INPUT);
}

void loop()
{
	// analogWriteResolution(10);
	int analogValue = analogRead(A0);
	// float celsius = 1 / (log(1 / (1023. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;
	// Serial.println(analogValue);
	float celsius = ((analogValue / 1023.0) * 5.0 - 0.5) * 100;
	// float celsius = (analogValue / 1023.f) * 3.3f * 100.f;
	Serial.print("Temperature: ");
	Serial.println(celsius);

	SendTemperature(celsius);

	delay(1000);
}


inline void setupWifi()
{
	WiFi.mode(WIFI_STA); //Optional
	WiFi.begin(ssid, password);
	Serial.println("\nConnecting");
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(100);
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

	do
	{
		Firebase.begin(&config, &auth);
		Firebase.reconnectWiFi(true);
		Serial.print(".");
	} while (!Firebase.ready());

	Serial.println("Firebase connected");
}

inline void SendTemperature(int temp)
{
	FirebaseJson json;
	json.add("id", DEVICE_ID);
	json.add("temperature", temp);

	if (Firebase.RTDB.pushJSON(&fbdo, "/test", &json))
	{
		Serial.println("Send data to Firebase successfully");
	}
	else
	{
		Serial.printf("%s\n", fbdo.errorReason().c_str());
	}
}
