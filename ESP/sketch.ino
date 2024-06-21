#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h> 

const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";

const char* serverName = "http://IP:PORT/temperatures";
const char* timeCheckUrl = "http://IP:PORT/check-time";

#define ONE_WIRE_BUS1 2
#define ONE_WIRE_BUS2 0
#define FAN_PIN 5

OneWire oneWire1(ONE_WIRE_BUS1);
OneWire oneWire2(ONE_WIRE_BUS2);

DallasTemperature sensors1(&oneWire1);
DallasTemperature sensors2(&oneWire2);

WiFiServer server(80);

float temp1 = 0.0;
float temp2 = 0.0;

unsigned long lastTemperatureCheck = 0;
const unsigned long temperatureCheckInterval = 10000;

unsigned long lastAPICheck = 0;
const unsigned long apiCheckInterval = 120000;

bool canRunFans = false;

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  sensors1.begin();
  sensors2.begin();

  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastTemperatureCheck >= temperatureCheckInterval) {
    lastTemperatureCheck = currentMillis;

    sensors1.requestTemperatures();
    sensors2.requestTemperatures();
    temp1 = sensors1.getTempCByIndex(0);
    temp2 = sensors2.getTempCByIndex(0);

    if (canRunFans && temp2 < temp1 && temp2 <= 26.0) {
      digitalWrite(FAN_PIN, HIGH);
      Serial.println("Wentylator włączony");
    } else {
      digitalWrite(FAN_PIN, LOW);
      Serial.println("Wentylator wyłączony");
    }

    Serial.print("Temperatura wewnątrz: ");
    Serial.print(temp1);
    Serial.println(" °C");
    Serial.print("Temperatura na zewnątrz: ");
    Serial.print(temp2);
    Serial.println(" °C");
  }

  if (currentMillis - lastAPICheck >= apiCheckInterval) {
    lastAPICheck = currentMillis;

    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      http.begin(client, timeCheckUrl);
      int httpResponseCode = http.GET();
      if (httpResponseCode > 0) {
        String payload = http.getString();
        Serial.println("Time check response: ");
        Serial.println(payload);

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);
        canRunFans = doc["allowed"];
      } else {
        Serial.print("Error on time check request: ");
        Serial.println(httpResponseCode);
      }
      http.end();

      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/json");

      String jsonData = "{\"inside\":";
      jsonData += temp1;
      jsonData += ",\"outside\":";
      jsonData += temp2;
      jsonData += ",\"username\":\"user_1\",\"password\":\"pass\"}";

      httpResponseCode = http.POST(jsonData);

      if (httpResponseCode > 0) {
        String payload = http.getString();
        Serial.println("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.println("Response payload: ");
        Serial.println(payload);
      } else {
        Serial.print("Error on HTTP request: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("WiFi not connected");
    }
  }

  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.println("<!DOCTYPE html>");
            client.println("<html lang=\"pl\">");
            client.println("<head>");
            client.println("<meta charset=\"UTF-8\">");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
            client.println("<title>Temperatury</title>");
            client.println("<style>");
            client.println("body { display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; font-family: Arial, sans-serif; background-color: #000000; color: #ffffff; }");
            client.println(".temperatures { text-align: center; }");
            client.println(".temperature { margin: 10px 0; font-size: 24px; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<div class=\"temperatures\">");
            client.print("<div class=\"temperature\">Temperatura wewnątrz: ");
            client.print(temp1);
            client.println("°C</div>");
            client.print("<div class=\"temperature\">Temperatura na zewnątrz: ");
            client.print(temp2);
            client.println("°C</div>");
            client.println("</div>");
            client.println("</body>");
            client.println("</html>");

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
