#include <ESP32Servo.h>
#include "DFRobotDFPlayerMini.h"

#define echoPin 4
#define trigPin 16
#define IR_SENSOR_PIN 17
#define SERVO_PIN 2
static const uint8_t PIN_MP3_TX = 26; 
static const uint8_t PIN_MP3_RX = 27; 
Servo myServo;
DFRobotDFPlayerMini player;

int maximumRange = 30;
int minimumRange = 0;
long duration, distance;
int merah = 18;
int kuning = 32;
int hijau = 33;

unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 100; // 100 ms debounce
bool objectDetected = false; // Untuk menyimpan kondisi sensor IR

void setup() {
  pinMode(IR_SENSOR_PIN, INPUT);
  myServo.attach(SERVO_PIN);
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX);

  if (player.begin(Serial1)) {
    Serial.println("DFPlayer Mini terhubung!");
    player.volume(80);
  } else {
    Serial.println("Gagal terhubung dengan DFPlayer Mini!");
  }

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(merah, OUTPUT);
  pinMode(kuning, OUTPUT);
  pinMode(hijau, OUTPUT);
}

void slowCloseServo() {
  int currentAngle = 145; // Posisi saat tutup terbuka
  int targetAngle = 0; // Posisi saat tertutup

  for (int angle = currentAngle; angle >= targetAngle; angle--) {
    myServo.write(angle);
    delay(map(angle, targetAngle, currentAngle, 20, 5)); 
  }
}

void loop() {
  int sensorValue = digitalRead(IR_SENSOR_PIN);
  
  // Logika debounce
  if (sensorValue == LOW && !objectDetected && (millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();
    objectDetected = true;

    Serial.println("Ada benda, servo membuka (90 derajat)");
    myServo.write(145);
    
    // Memutar audio tanpa kondisi
    Serial.println("Memutar audio...");
    player.play(1); 
    delay(5000);         
    Serial1.flush();    

  } else if (sensorValue == HIGH && objectDetected && (millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();
    objectDetected = false;

    Serial.println("Tidak ada benda. Menutup servo perlahan.");
    slowCloseServo();
    delay(3000); 
  }

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;

  if (distance >= maximumRange || distance <= minimumRange) {
    Serial.println("Di Luar jangkauan");
  } else {
    Serial.print("Jarak : ");
    Serial.print(distance);
    Serial.println(" cm");
    delay(100);
  }

  if (distance <= 10) {
    digitalWrite(merah, HIGH);
    digitalWrite(kuning, LOW);
    digitalWrite(hijau, LOW);
  } else if (distance > 10 && distance < 20) {
    digitalWrite(merah, LOW);
    digitalWrite(kuning, HIGH);
    digitalWrite(hijau, LOW);
  } else {
    digitalWrite(merah, LOW);
    digitalWrite(kuning, LOW);
    digitalWrite(hijau, HIGH);
  }

  delay(100);
}