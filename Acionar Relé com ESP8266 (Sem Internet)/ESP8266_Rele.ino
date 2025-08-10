#include <ESP8266WiFi.h>

WiFiServer server(80);

//http://192.168.18.200/?acao=on
//http://192.168.18.200/?acao=off

// ===== CONFIGURAÇÕES DA REDE =====
const char* ssid = "Nome do seu Wi-Fi";
const char* password = "Senha do seu Wifi";

IPAddress ip(192, 168, 18, 200); //IP Fixo do ESP8266
IPAddress gateway(192, 168, 18, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(9, 9, 9, 9); //DNS Seguro Quad9

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH); // inicia desligado

  if (!WiFi.config(ip, gateway, subnet, dns)) {
    Serial.println("⚠ Falha ao configurar IP fixo");
  }

  WiFi.begin(ssid, password);
  Serial.print("Conectando");
  unsigned long inicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < 15000) {
    delay(250);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Conectado ao WiFi");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    server.begin();
  } else {
    Serial.println("\n❌ Falha na conexão WiFi");
  }
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("Novo cliente conectou");
  String req = client.readStringUntil('\r');
  client.flush();
  Serial.println("Requisição: " + req);

  // Só executa se tiver "acao="
  if (req.indexOf("acao=off") != -1) {
    digitalWrite(2, HIGH);
    Serial.println("Relé DESLIGADO");
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("Relé desligado");
  } 
  else if (req.indexOf("acao=on") != -1) {
    digitalWrite(2, LOW);
    Serial.println("Relé LIGADO");
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("Relé ligado");
  } 
  else {
    // Caso só acesse o IP, não faz nada
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("Nenhuma ação executada");
  }

  delay(1);
  client.stop();
  Serial.println("Cliente desconectado");
}
