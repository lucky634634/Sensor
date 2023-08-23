#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseESP8266.h>
#include <time.h>


// SSID for connecting to the WiFi network
// const char* ssid = "HoangPhung";
const char* ssid = "OPPO A9 2020";

// Password for connecting to the WiFi network
// const char* password = "03922526";
const char* password = "looky634";

// Insert Firebase project API Key
#define API_KEY "AIzaSyA3rxaWh38t8nyIhPy6o6f3LhYkWs5NKrU"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://testcloud-a615d-default-rtdb.asia-southeast1.firebasedatabase.app"

// Device ID
const String DEVICE_ID = "21127629";

// Initialize Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setupWifi();
void setupFirebase();
void GetData();

void setup()
{
    Serial.begin(115200);

    setupWifi();
    delay(500);

    setupFirebase();
}

void loop()
{
    GetData();
    delay(10000);
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
void GetData()
{
    Serial.println(Firebase.RTDB.getBool(&fbdo, "/control/state"));
}
