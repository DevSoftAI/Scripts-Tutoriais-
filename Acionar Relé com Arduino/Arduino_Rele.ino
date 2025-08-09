#define RELE1_PIN 7
#define RELE2_PIN 8

bool releAtivoNivelBaixo = true;

void setup() {
  Serial.begin(9600);
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

  Serial.println("Digite comandos: '1on', '1off', '2on', '2off'");
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "1on") {
      digitalWrite(RELE1_PIN, releAtivoNivelBaixo ? LOW : HIGH);
      Serial.println("Relé 1 LIGADO");
    } 
    else if (cmd == "1off") {
      digitalWrite(RELE1_PIN, releAtivoNivelBaixo ? HIGH : LOW);
      Serial.println("Relé 1 DESLIGADO");
    } 
    else if (cmd == "2on") {
      digitalWrite(RELE2_PIN, releAtivoNivelBaixo ? LOW : HIGH);
      Serial.println("Relé 2 LIGADO");
    } 
    else if (cmd == "2off") {
      digitalWrite(RELE2_PIN, releAtivoNivelBaixo ? HIGH : LOW);
      Serial.println("Relé 2 DESLIGADO");
    }
    else {
      Serial.println("Comando inválido!");
      Serial.println("Use: '1on', '1off', '2on', '2off'");
    }
  }
}
