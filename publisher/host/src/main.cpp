#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "credentials.hpp"

#define LED_PIN 2
#define RXD2 16
#define TXD2 17
#define ID 0

class UARTManager {
public:
	String waitForData() {
		String data = "";
		while (Serial2.available()) {
			char c = Serial2.read();
			data += c;
		}
		return data;
	}
};

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

	void connect(int id) {
		while (!client.connected()) {
			Serial.println("Connecting to MQTT server...");
			if (client.connect("ESP32Publiser-" + ID, MQTT_USER, MQTT_PASSWORD)) {
				Serial.println("Connected to MQTT server");
			} else {
				Serial.print("MQTT connection failed, rc=");
				Serial.print(client.state());
				Serial.println(" Retrying in 5 seconds");
				delay(5000);
			}
		}
	}

	void publish(const char* topic, const char* payload) { client.publish(topic, payload); }

	bool isConnected() { return client.connected(); }
};

class Application {
private:
	UARTManager uartManager;
	WiFiManager wifiManager;
	MQTTManager mqttManager;

public:
	void setup() {
		Serial.begin(115200);
		Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
		pinMode(LED_PIN, OUTPUT);

		wifiManager.connect();

		mqttManager.connect(ID);
		mqttManager.publish(MQTT_TOPIC, "Initialization complete");
	}

	void loop() {
		if (mqttManager.isConnected()) {
			Serial.println("Waiting for data...");
			String data = uartManager.waitForData();
			if (!data.isEmpty()) {
				digitalWrite(LED_PIN, HIGH);
				Serial.println("Received data: " + data);
				mqttManager.publish("checkIn", data.c_str());
				delay(150);
				digitalWrite(LED_PIN, LOW);
			}
		}
	}
};

Application app;

void setup() { app.setup(); }

void loop() { app.loop(); }
