#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Örnek mesafe değerleri
#define MESAFE 300

// LED ve motor pinleri (bağlantılara göre değiştirin)
#define KIRMIZI_LED_PIN 10
#define SARI_LED_PIN 11
#define YESIL_LED_PIN 12
#define MOTOR_PIN 9

void setup() {
  // Pinleri çıkış olarak ayarlayın
  pinMode(KIRMIZI_LED_PIN, OUTPUT);
  pinMode(SARI_LED_PIN, OUTPUT);
  pinMode(YESIL_LED_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
}

void loop() {
  // Mesafe sensöründen mesafe okuması yapın (örneğin MESAFE değişkeni ile simüle edildi)
  int mesafe = MESAFE;

  if (mesafe >= 250 && mesafe <= 350) {
    // Mesafe 250 - 350 cm aralığında ise sarı LED yanmalı
    digitalWrite(KIRMIZI_LED_PIN, LOW);
    digitalWrite(YESIL_LED_PIN, LOW);
    digitalWrite(SARI_LED_PIN, HIGH);
    digitalWrite(MOTOR_PIN, LOW);  // Motor dönmemeli
  } else if (mesafe > 350) {
    // Mesafe 350 cm'den büyükse yeşil LED yanmalı ve motor saat yönünde dönmeli
    digitalWrite(KIRMIZI_LED_PIN, LOW);
    digitalWrite(SARI_LED_PIN, LOW);
    digitalWrite(YESIL_LED_PIN, HIGH);
    digitalWrite(MOTOR_PIN, HIGH);  // Motor saat yönünde dönmeli
  } else {
    // Mesafe 250 cm'den küçükse kırmızı LED yanmalı ve motor saat yönünün tersine dönmeli
    digitalWrite(YESIL_LED_PIN, LOW);
    digitalWrite(SARI_LED_PIN, LOW);
    digitalWrite(KIRMIZI_LED_PIN, HIGH);
    digitalWrite(MOTOR_PIN, LOW);  // Motor saat yönünün tersine dönmeli
  }
  
  // İşlem yapılması için bir süre bekleyin (örneğin 1 saniye)
  delay(1000);
}