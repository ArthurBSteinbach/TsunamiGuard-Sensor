#include <Wire.h> // Biblioteca para comunicação I2C
#include <LiquidCrystal_I2C.h> // Biblioteca para controle de LCD via I2C

// Inicializa o objeto LCD com o endereço 0x27 e tamanho 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Definição de pinos
#define alarmLed 2 // Pino para o LED de alarme
#define buzzer 3 // Pino para o buzzer
#define outsideLed 4 // Pino para o LED externo
#define waterLevel A3 // Pino analógico para medir o nível de água
int ECHO_PIN = 10; // Pino para o sinal de eco do sensor ultrassônico
int TRIG_PIN = 11; // Pino para o sinal de disparo do sensor ultrassônico
int valueWater = 0; // Variável para armazenar o valor do nível de água
int loopKey = 0; // Variável para controlar o loop de alarme

// Função para declarar os pinos como entrada ou saída
void declaration() {
  pinMode(TRIG_PIN, OUTPUT); // Define o pino TRIG como saída
  pinMode(ECHO_PIN, INPUT); // Define o pino ECHO como entrada
  pinMode(alarmLed, OUTPUT); // Define o pino do LED de alarme como saída
  pinMode(buzzer, OUTPUT); // Define o pino do buzzer como saída
  pinMode(outsideLed, OUTPUT); // Define o pino do LED externo como saída
}

// Função de configuração inicial
void setup() {
  Serial.begin(9600); // Inicia a comunicação serial a 9600 bps
  declaration(); // Chama a função de declaração de pinos
  lcd.begin(16, 2); // Inicializa o LCD
  lcd.backlight(); // Liga a luz de fundo do LCD
  
  // Exibe uma mensagem inicial no LCD
  lcd.setCursor(0, 0);
  lcd.print("Projeto:");
  lcd.setCursor(0, 1);
  lcd.print("TsunamiGuard");
  delay(2000); // Aguarda 2 segundos
  lcd.clear(); // Limpa o LCD
}

// Loop principal
void loop() {                  
  valueWater = analogRead(waterLevel); // Lê o nível de água do sensor

  long duration, distance; // Variáveis para armazenar a duração e a distância
  
  // Medindo a distância usando o sensor ultrassônico
  digitalWrite(TRIG_PIN, LOW); // Garante que o TRIG esteja em LOW
  delayMicroseconds(2); // Aguarda 2 microssegundos
  digitalWrite(TRIG_PIN, HIGH); // Envia um pulso HIGH
  delayMicroseconds(10); // Aguarda 10 microssegundos
  digitalWrite(TRIG_PIN, LOW); // Retorna o TRIG para LOW
  
  duration = pulseIn(ECHO_PIN, HIGH); // Lê a duração do pulso de eco
  distance = (duration * 0.0343) / 2; // Calcula a distância em cm

  // Verificando condições de alarme
  bool dropDistance = distance < 7; // Verifica se a distância é menor que 7 cm
  bool dropWaterLevel = valueWater < 570; // Verifica se o nível de água é menor que 570

  // Se houver um alerta de tsunami
  if ((dropDistance || loopKey == 1) && (dropWaterLevel || loopKey == 1)) {
    loopKey = 1; // Ativa o estado de alarme
    
    lcd.clear(); // Limpa o LCD
    lcd.print("  AVISO TSUNAMI  "); // Exibe mensagem de alerta
    lcd.setCursor(0, 1);
    lcd.print("PERIGO IMINENTE");
    
    // Ativa o buzzer com diferentes tons
    tone(buzzer, 3000);
    delay(500);
    tone(buzzer, 2000);
    delay(500);
    analogWrite(alarmLed, 255); // Liga o LED de alarme
    digitalWrite(outsideLed, 0); // Desliga o LED externo
  } else {
    noTone(buzzer); // Desliga o buzzer
    analogWrite(outsideLed, 255); // Liga o LED externo
    digitalWrite(alarmLed, 0); // Desliga o LED de alarme
    
    // Exibindo dados no LCD
    lcd.setCursor(0, 0);
    lcd.print(distance); //