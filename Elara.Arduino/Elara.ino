/*
* Elara Web Server: Hosts a web services that responds to incoming requests regarding:
*
*    1) The Current Airflow Status (via micro swtich)
*    2) The currnt Window OPEN/CLOSE Status (via REED switch)
*    3) A Ping request (with a 'ACK' response)
*    4) Alarm Status (a true if window is open and Airflow is detected
*
*
*    It also sets LEDs ON/OFF indicating airflow and Window OPEN/CLOSE Status.
*
*    NOTE: Since the input from the micro switch that detects the airflow flickers
*          when the air is flowing, we need to introduce some debouncing when reading
*          this input. The input is set to low, only if the swtich stays open for
*          longer than three seconds.
*
*
*   Fike Rehman
*   February 26, 2017
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

// WiFi Definitions 
const char WiFiSSID[] = "TPOCEANALB";
const char WiFiPSK[] = "FikeNETBB1972";


// Pin Definitions 
const int wifiErrorPin = 5;       // Thing's onboard LED
const int airflowInputPin = 15;   // the number airlfow input switch
const int airflowLEDPin = 13;     // the number of the output pin (the airflow LED pin)
const int reedInputPin = 4;       // the number of the REED switch input pin
const int reedLEDPin = 0;         // the number of the REED state LED pin
const int alarmLEDPin = 12;         // the number of the REED state LED pin

									// State Variables:
boolean WindowOpen = false;         // initial state of the window
boolean AirflowDetected = false;    // initially, no airflow detected

int airflowLEDState = HIGH;       // the current state of the airflow LED pin
int reading;                      // the current airlfow reading of airflow inputPin
int previous = LOW;               // the previous airlfow reading from the airflow input pin

								  // Following control the debouncing of the air flow switch
unsigned long previousmillis = 0;         // the last time the output pin was toggled
long debounce = 3000;                     // the debounce time, increase if the output flickers

										  // Follwoing control the Blink alaram LED
										  // unsigned long blinkpreviousmillis = 0;
										  // const int blinkinterval = 1000;

WiFiServer server(80);

void setup()
{
	initHardware();
	connectWiFi();
	server.begin();
	setupMDNS();
}

void loop()
{
	// We start with setting up the Airflow and windows status:
	DetectAirflow();
	DetectProximity();

	if (AirflowDetected && WindowOpen)
		blinkAlarmLED();
	else
		digitalWrite(alarmLEDPin, LOW);

	// Here we respond to incoming requests:
	// Check if a client has connected
	WiFiClient client = server.available();
	if (!client)
	{
		return;
	}

	// Prepare the response
	String s = "HTTP/1.1 200 OK\r\n";
	s += "Content-Type: text/html\r\n\r\n";
	s += "<!DOCTYPE HTML>\r\n<html>\r\n";

	// Read the first line of the request
	String request = client.readStringUntil('\r');
	Serial.println(request);
	client.flush();

	if (request.indexOf("/WindowOpen") != -1)
	{
		// client is looking for the Window Status:
		s += "Window Open: ";
		s += (WindowOpen) ? "TRUE" : "FALSE";
	}
	else if (request.indexOf("/AirflowDetected") != -1)
	{
		// client is looking for Airflow status
		s += "Airflow Detected: ";
		s += (AirflowDetected) ? "TRUE" : "FALSE";
	}
	else if (request.indexOf("/AlarmStatus") != -1)
	{
		// client is looking for Alarm status
		s += "Alarm Status: ";
		s += (AirflowDetected && WindowOpen) ? "ON" : "OFF";
	}
	else if (request.indexOf("/Ping") != -1)
	{
		// client is looking for Ping confirmation
		s += "Ping: ";
		s += "ACK";
	}
	else
	{
		// request is invalid 
		s += "Invalid Request.<br> Try /WindowOpen, or /AirflowDetected or/AlarmStatus or /Ping";
	}

	s += "</html>\n";

	// Send the response to the client
	client.print(s);
	delay(1);
	Serial.println("Client disonnected");
}

// Detects the current state of airflow and sets the state
// of the airflow LED accordingly. 
void DetectAirflow()
{
	reading = digitalRead(airflowInputPin);

	if (reading == LOW)
	{
		if (millis() - previousmillis > debounce)
		{
			// we can change the airflow state because we are passed the debounce period
			AirflowDetected = false;
			airflowLEDState = LOW;
		}
	}
	else
	{
		AirflowDetected = true;
		airflowLEDState = HIGH;

		// reset the previous millis:
		previousmillis = millis();
	}

	digitalWrite(airflowLEDPin, airflowLEDState);
}

void DetectProximity()
{
	int proximity = digitalRead(reedInputPin);

	if (proximity == LOW)
	{
		digitalWrite(reedLEDPin, LOW);
		WindowOpen = false;
	}
	else
	{
		digitalWrite(reedLEDPin, HIGH);
		WindowOpen = true;
	}
}

void initHardware()
{
	Serial.begin(9600);
	pinMode(wifiErrorPin, OUTPUT);
	digitalWrite(wifiErrorPin, HIGH);

	pinMode(airflowInputPin, INPUT);
	pinMode(airflowLEDPin, OUTPUT);

	pinMode(reedInputPin, INPUT_PULLUP);
	pinMode(reedLEDPin, OUTPUT);

	pinMode(alarmLEDPin, OUTPUT);
	digitalWrite(alarmLEDPin, LOW);
}

void blinkAlarmLED()
{
	digitalWrite(alarmLEDPin, HIGH);
	delay(100);
	digitalWrite(alarmLEDPin, LOW);
	delay(100);
}

void connectWiFi()
{
	byte ledStatus = LOW;
	Serial.println();
	Serial.println("Connecting to: " + String(WiFiSSID));
	// Set WiFi mode to station (as opposed to AP or AP_STA)
	WiFi.mode(WIFI_STA);

	// WiFI.begin([ssid], [passkey]) initiates a WiFI connection
	// to the stated [ssid], using the [passkey] as a WPA, WPA2,
	// or WEP passphrase.
	WiFi.begin(WiFiSSID, WiFiPSK);

	// Use the WiFi.status() function to check if the ESP8266
	// is connected to a WiFi network.
	while (WiFi.status() != WL_CONNECTED)
	{
		// Blink the LED
		digitalWrite(wifiErrorPin, ledStatus); // Write LED high/low
		ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

		// Delays allow the ESP8266 to perform critical tasks
		// defined outside of the sketch. These tasks include
		// setting up, and maintaining, a WiFi connection.
		delay(100);
		// Potentially infinite loops are generally dangerous.
		// Add delays -- allowing the processor to perform other
		// tasks -- wherever possible.
	}

	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

void setupMDNS()
{
	// Call MDNS.begin(<domain>) to set up mDNS to point to
	// "<domain>.local"
	if (!MDNS.begin("thing"))
	{
		Serial.println("Error setting up MDNS responder!");
		while (1) {
			delay(1000);
		}
	}
	Serial.println("mDNS responder started");
}