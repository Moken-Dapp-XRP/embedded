#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#include "credentials.hpp"

#define LED_PIN 1
#define RELAY_PIN 0
#define ID 0

class WiFiManager {
public:
	void connect() {
		Serial.begin(115200);
		delay(10);
		Serial.println();
		Serial.print("Connecting to ");
		Serial.println(WIFI_SSID);

		WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

		while (WiFi.status() != WL_CONNECTED) {
			delay(500);
			Serial.print(".");
		}

		Serial.println("");
		Serial.println("Connected to WiFi network");
		Serial.println("IP Address: " + WiFi.localIP().toString());
	}
};

class MQTTManager {
private:
	WiFiClientSecure espClient;
	PubSubClient client;

public:
	MQTTManager() : client(espClient) { espClient.setInsecure(); }

	void connect() {
		while (!client.connected()) {
			Serial.println("Connecting to MQTT server...");
			if (client.connect("ESP8266Subscriber-" + ID, MQTT_USER, MQTT_PASSWORD)) {
				Serial.println("Connected to MQTT server");
				client.subscribe(MQTT_TOPIC);
			} else {
				Serial.print("MQTT connection failed, rc=");
				Serial.print(client.state());
				Serial.println(" Retrying in 5 seconds");
				delay(5000);
			}
		}
	}

	void setCallback(MQTT_CALLBACK_SIGNATURE) { client.setCallback(callback); }

	void loop() { client.loop(); }
};

class Application {
private:
	WiFiManager wifiManager;
	MQTTManager mqttManager;

public:
	static void callback(char *topic, byte *payload, unsigned int length) {
		Serial.print("Message arrived in topic: ");
		Serial.println(topic);

		Serial.print("Message:");
		String messageTemp;

		for (int i = 0; i < length; i++) {
			Serial.print((char)payload[i]);
			messageTemp += (char)payload[i];
		}

		Serial.println();
		Serial.println("-----------------------");

		if (messageTemp == "true") {
			Serial.println("LED and Relay ON");
			digitalWrite(LED_PIN, HIGH);
			digitalWrite(RELAY_PIN, LOW);
		} else if (messageTemp == "false") {
			Serial.println("LED and Relay OFF");
			digitalWrite(LED_PIN, LOW);
			digitalWrite(RELAY_PIN, HIGH);
		}
	}

	void setup() {
		Serial.begin(115200);

		pinMode(LED_PIN, OUTPUT);
		pinMode(RELAY_PIN, OUTPUT);

		digitalWrite(RELAY_PIN, HIGH);

		wifiManager.connect();

		mqttManager.setCallback(Application::callback);
		mqttManager.connect();
	}

	void loop() { mqttManager.loop(); }
};

Application app;

void setup() { app.setup(); }

void loop() { app.loop(); }
