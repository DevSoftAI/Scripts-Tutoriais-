#include <SPI.h>
#include <Ethernet.h>

// ===== CONFIGURAÇÃO DA REDE =====
// MAC address do Ethernet Shield (modifique se precisar)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// IP fixo (ou configure para usar DHCP chamando Ethernet.begin(mac);)
IPAddress ip(192, 168, 0, 177);  

// Porta do servidor HTTP
EthernetServer server(80);

// ===== PINOS DOS RELÉS =====
#define RELE1_PIN 7
#define RELE2_PIN 8

// ===== CONFIGURAÇÃO NÍVEL ATIVO =====
// true = relé ativo em nível LOW (comum em módulos relé)
// false = ativo em nível HIGH
bool releAtivoNivelBaixo = true;

// Estado atual dos relés
bool estadoRele1 = false;
bool estadoRele2 = false;

// ===== Função para definir estado do relé =====
void setRele(uint8_t pin, bool ligar, bool &estado) {
  int nivelDesejado = releAtivoNivelBaixo ? (ligar ? LOW : HIGH) : (ligar ? HIGH : LOW);

  if (digitalRead(pin) != nivelDesejado) {
    digitalWrite(pin, nivelDesejado);
    estado = ligar;
  }
}

// ===== Função para enviar resposta HTML com botões =====
void enviarPaginaPrincipal(EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>Controle de Relés</title></head>");
  client.println("<body>");
  client.println("<h1>Controle de Relés Arduino</h1>");

  client.print("<p>Relé 1: ");
  client.print(estadoRele1 ? "LIGADO" : "DESLIGADO");
  client.println("</p>");
  client.println("<a href=\"/rele1/on\">Ligar Relé 1</a> | <a href=\"/rele1/off\">Desligar Relé 1</a><br><br>");

  client.print("<p>Relé 2: ");
  client.print(estadoRele2 ? "LIGADO" : "DESLIGADO");
  client.println("</p>");
  client.println("<a href=\"/rele2/on\">Ligar Relé 2</a> | <a href=\"/rele2/off\">Desligar Relé 2</a>");

  client.println("</body></html>");
}

void setup() {
  // Inicializa comunicação serial para debug
  Serial.begin(9600);

  // Configura pinos dos relés como saída
  pinMode(RELE1_PIN, OUTPUT);
  pinMode(RELE2_PIN, OUTPUT);

  // Inicializa relés desligados
  if (releAtivoNivelBaixo) {
    digitalWrite(RELE1_PIN, HIGH);
    digitalWrite(RELE2_PIN, HIGH);
  } else {
    digitalWrite(RELE1_PIN, LOW);
    digitalWrite(RELE2_PIN, LOW);
  }

  // Inicializa Ethernet (IP fixo)
  Ethernet.begin(mac, ip);
  delay(1000);
  Serial.print("Servidor iniciado em IP: ");
  Serial.println(Ethernet.localIP());

  server.begin();
}

void loop() {
  EthernetClient client = server.available();

  if (client) {
    Serial.println("Novo cliente conectado");
    bool currentLineIsBlank = true;
    String httpRequest = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        httpRequest += c;

        // Verifica fim do header HTTP (linha em branco)
        if (c == '\n' && currentLineIsBlank) {
          // Processa pedido HTTP

          Serial.print("Requisição: ");
          Serial.println(httpRequest);

          // Verifica qual comando veio na requisição
          if (httpRequest.indexOf("GET /rele1/on") >= 0) {
            setRele(RELE1_PIN, true, estadoRele1);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println("Connection: close");
            client.println();
            client.println("Relé 1 LIGADO");
          }
          else if (httpRequest.indexOf("GET /rele1/off") >= 0) {
            setRele(RELE1_PIN, false, estadoRele1);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println("Connection: close");
            client.println();
            client.println("Relé 1 DESLIGADO");
          }
          else if (httpRequest.indexOf("GET /rele2/on") >= 0) {
            setRele(RELE2_PIN, true, estadoRele2);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println("Connection: close");
            client.println();
            client.println("Relé 2 LIGADO");
          }
          else if (httpRequest.indexOf("GET /rele2/off") >= 0) {
            setRele(RELE2_PIN, false, estadoRele2);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println("Connection: close");
            client.println();
            client.println("Relé 2 DESLIGADO");
          }
          else if (httpRequest.indexOf("GET / ") >= 0 || httpRequest.indexOf("GET /index") >= 0) {
            enviarPaginaPrincipal(client);
          }
          else {
            // Página não encontrada
            client.println("HTTP/1.1 404 Not Found");
            client.println("Content-Type: text/plain");
            client.println("Connection: close");
            client.println();
            client.println("Página não encontrada");
          }

          break; // Sai do while
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }

    delay(1);
    client.stop();
    Serial.println("Cliente desconectado");
  }
}
