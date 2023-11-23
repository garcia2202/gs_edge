#include <NewPing.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

// Configurações de WiFi
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";

// Configurações do MQTT
const char *BROKER_MQTT = "broker.hivemq.com";
const int BROKER_PORT = 1883;
const char *ID_MQTT = "THIAGO_mqtt";
const char *TOPIC_PUBLISH_ESTOQUE = "thiagovictor/edge/estoque";

#define TRIGGER_PIN 12
#define ECHO_PIN 14
#define LED_PIN_1 0
#define LED_PIN_2 16
#define LED_PIN_3 5

// Defina o intervalo de publicação MQTT desejado
#define PUBLISH_DELAY 5000 

// Variáveis globais
NewPing sonar(TRIGGER_PIN, ECHO_PIN);
WiFiClient espClient;
PubSubClient MQTT(espClient);
unsigned long publishUpdate = 0;

// Protótipos de funções
void initWiFi();
void initMQTT();
void callbackMQTT(char *topic, byte *payload, unsigned int length);
void reconnectMQTT();
void reconnectWiFi();
void checkWiFIAndMQTT();

void initWiFi() {
  Serial.print("Conectando com a rede: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso: ");
  Serial.println(SSID);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(callbackMQTT);
}

void callbackMQTT(char *topic, byte *payload, unsigned int length) {
  String msg = String((char *)payload).substring(0, length);

  Serial.printf("Mensagem recebida via MQTT: %s do tópico: %s\n", msg.c_str(), topic);

  // Adicione aqui qualquer ação que você deseja realizar ao receber uma mensagem MQTT
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Tentando conectar com o Broker MQTT: ");
    Serial.println(BROKER_MQTT);

    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao broker MQTT!");
      // Subscreva a outros tópicos MQTT, se necessário
    } else {
      Serial.println("Falha na conexão com MQTT. Tentando novamente em 2 segundos.");
      delay(2000);
    }
  }
}

void checkWiFIAndMQTT() {
  if (WiFi.status() != WL_CONNECTED)
    reconnectWiFi();
  if (!MQTT.connected())
    reconnectMQTT();
}

void reconnectWiFi(void) {
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Wifi conectado com sucesso");
  Serial.print(SSID);
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  initWiFi();
  initMQTT();
}

void loop() {
  delay(3000);
  checkWiFIAndMQTT();
  MQTT.loop();

  if ((millis() - publishUpdate) >= PUBLISH_DELAY) {
    publishUpdate = millis();

    // Mede a distância usando o sensor ultrassônico
    int distance = sonar.ping_cm();

    // Define os níveis de estoque com base na distância medida
    if (distance <= 135) {
      // Nível de estoque alto
      digitalWrite(LED_PIN_1, LOW);
      digitalWrite(LED_PIN_2, LOW);
      digitalWrite(LED_PIN_3, HIGH);
      Serial.print("Nível de estoque bom\n");
      // Envia a mensagem MQTT para o tópico correspondente
      MQTT.publish(TOPIC_PUBLISH_ESTOQUE, "Nível de estoque bom");
    } else if (distance > 136 && distance <= 280) {
      // Nível de estoque médio
      digitalWrite(LED_PIN_1, LOW);
      digitalWrite(LED_PIN_2, HIGH);
      digitalWrite(LED_PIN_3, LOW);
      Serial.print("Nível de estoque médio\n");
      // Envia a mensagem MQTT para o tópico correspondente
      MQTT.publish(TOPIC_PUBLISH_ESTOQUE, "Nível de estoque médio");
    } else {
      // Nível de estoque baixo
      digitalWrite(LED_PIN_1, HIGH);
      digitalWrite(LED_PIN_2, LOW);
      digitalWrite(LED_PIN_3, LOW);
      Serial.print("Nível de estoque baixo\n");
      // Envia a mensagem MQTT para o tópico correspondente
      MQTT.publish(TOPIC_PUBLISH_ESTOQUE, "Nível de estoque baixo");
    }

    // Exibe a distância medida
    Serial.print("Distancia: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
}





/*
#include <NewPing.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Configurações de WiFi
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";

//Configurações do MQTT
const char *BROKER_MQTT = "broker.hivemq.com";
const int BROKER_PORT = 1883;
const char *ID_MQTT = "THIAGO_mqtt";
const char *TOPIC_SUBSCRIBE_ESTOQUE = "thiagovictor/edge";

#define TRIGGER_PIN 12
#define ECHO_PIN 14
#define LED_PIN_1 0
#define LED_PIN_2 16
#define LED_PIN_3 5

//Variáveis globais
NewPing sonar(TRIGGER_PIN, ECHO_PIN);
WiFiClient espClient;
PubSubClient MQTT(espClient);
unsigned long publishUpdate = 0;

// Protótipos de funções
void initWiFi();
void initMQTT();
void callbackMQTT(char *topic, byte *payload, unsigned int length);
void reconnectMQTT();
void reconnectWiFi();
void checkWiFIAndMQTT();

void initWiFi() {
  Serial.print("Conectando com a rede: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso: ");
  Serial.println(SSID);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(callbackMQTT);
}

void callbackMQTT(char *topic, byte *payload, unsigned int length) {
  String msg = String((char*)payload).substring(0, length);
  
  Serial.printf("Mensagem recebida via MQTT: %s do tópico: %s\n", msg.c_str(), topic);

  if (strcmp(topic, TOPIC_SUBSCRIBE_LED) == 0) {
    int valor = atoi(msg.c_str());

    if (valor == 1) {
      digitalWrite(PIN_LED, HIGH);
      Serial.println("LED ligado via comando MQTT");
    } else if (valor == 0) {
      digitalWrite(PIN_LED, LOW);
      Serial.println("LED desligado via comando MQTT");
    }
  }
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Tentando conectar com o Broker MQTT: ");
    Serial.println(BROKER_MQTT);

    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao broker MQTT!");
      MQTT.subscribe(TOPIC_SUBSCRIBE_LED);
    } else {
      Serial.println("Falha na conexão com MQTT. Tentando novamente em 2 segundos.");
      delay(2000);
    }
  }
}

void checkWiFIAndMQTT() {
  if (WiFi.status() != WL_CONNECTED) reconnectWiFi();
  if (!MQTT.connected()) reconnectMQTT();
}

void reconnectWiFi(void) {
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Wifi conectado com sucesso");
  Serial.print(SSID);
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  initWiFi();
  initMQTT();
}

void loop() {
  delay(3000);
  checkWiFIAndMQTT();
  MQTT.loop();

  if ((millis() - publishUpdate) >= PUBLISH_DELAY) {
    publishUpdate = millis();
  
  // Mede a distância usando o sensor ultrassônico
  int distance = sonar.ping_cm();
  
  // Define os níveis de estoque com base na distância medida
  if (distance <= 135) {
    // Nível de estoque alto
    digitalWrite(LED_PIN_1, LOW);
    digitalWrite(LED_PIN_2, LOW);
    digitalWrite(LED_PIN_3, HIGH);
    Serial.print("Nível de estoque bom\n");
  } else if (distance > 136 && distance <= 280) {
    // Nível de estoque médio
    digitalWrite(LED_PIN_1, LOW);
    digitalWrite(LED_PIN_2, HIGH);
    digitalWrite(LED_PIN_3, LOW);
    Serial.print("Nível de estoque médio\n");
  } else {
    // Nível de estoque baixo
    digitalWrite(LED_PIN_1, HIGH);
    digitalWrite(LED_PIN_2, LOW);
    digitalWrite(LED_PIN_3, LOW);
    Serial.print("Nível de estoque baixo\n");
  }

  // Exibe a distância medida
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");
}
*/