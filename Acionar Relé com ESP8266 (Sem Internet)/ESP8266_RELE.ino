#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "NOME_DA_REDE";      // Seu Wi-Fi
const char* password = "SENHA_DO_WIFI"; // Sua senha

#define RELE1_PIN D1
#define RELE2_PIN D2

bool releAtivoNivelBaixo = true;
bool estadoRele1 = false; //relé 1
bool estadoRele2 = false; // relé 2

ESP8266WebServer server(80);

//verifica se o relé está em nível baixo ou alto
void setRele(uint8_t pin, bool ligar, bool &estado) {
  int nivelDesejado = releAtivoNivelBaixo ? (ligar ? LOW : HIGH) : (ligar ? HIGH : LOW);
  if (digitalRead(pin) != nivelDesejado) {
    digitalWrite(pin, nivelDesejado);
    estado = ligar;
  }
}

//interface html
void handleRele1On()  { setRele(RELE1_PIN, true, estadoRele1);  server.send(200, "text/plain", "Relé 1 LIGADO"); }
void handleRele1Off() { setRele(RELE1_PIN, false, estadoRele1); server.send(200, "text/plain", "Relé 1 DESLIGADO"); }
void handleRele2On()  { setRele(RELE2_PIN, true, estadoRele2);  server.send(200, "text/plain", "Relé 2 LIGADO"); }
void handleRele2Off() { setRele(RELE2_PIN, false, estadoRele2); server.send(200, "text/plain", "Relé 2 DESLIGADO"); }

void handleRoot() {
  String html = "<html><body><h1>Controle de Relés</h1>";
  html += "<p>Relé 1: " + String(estadoRele1 ? "LIGADO" : "DESLIGADO") + "</p>";
  html += "<a href='/rele1/on'>Ligar Relé 1</a> | <a href='/rele1/off'>Desligar Relé 1</a><br><br>";
  html += "<p>Relé 2: " + String(estadoRele2 ? "LIGADO" : "DESLIGADO") + "</p>";
  html += "<a href='/rele2/on'>Ligar Relé 2</a> | <a href='/rele2/off'>Desligar Relé 2</a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200); //pmelhor velocidade de comunição entre o PC e o ESP8266

  pinMode(RELE1_PIN, OUTPUT);
  pinMode(RELE2_PIN, OUTPUT);

  if (releAtivoNivelBaixo) {
    digitalWrite(RELE1_PIN, HIGH);
    digitalWrite(RELE2_PIN, HIGH);
  } else {
    digitalWrite(RELE1_PIN, LOW);
    digitalWrite(RELE2_PIN, LOW);
  }

  Serial.println();
  Serial.print("Conectando na rede WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado! IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/rele1/on", handleRele1On);
  server.on("/rele1/off", handleRele1Off);
  server.on("/rele2/on", handleRele2On);
  server.on("/rele2/off", handleRele2Off);

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}
