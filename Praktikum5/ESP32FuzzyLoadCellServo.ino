/*
  =========================================================
  FUZZY MAMDANI + CENTER OF GRAVITY (CoG)
  IMPLEMENTASI PADA ESP32
  =========================================================

  Input  :
  - Load Cell + HX711

  Output :
  - Servo SG90

  Metode :
  - Mamdani
  - Min-Max Inference
  - Centroid / Center of Gravity

  =========================================================
*/

#include "HX711.h"
#include <ESP32Servo.h>

#define DT_PIN     4
#define SCK_PIN    5
#define SERVO_PIN  25

HX711 scale;
Servo myservo;

float berat;
float calibration_factor = 420.0;

void setup() {
  Serial.begin(9600);

  // HX711
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();

  // SERVO
  myservo.attach(SERVO_PIN);
  myservo.write(90);
  delay(1000);

  Serial.println("\r\n=== ROBOT SORTIR BARANG BERBASIS FUZZY ESP32 ===");
  Serial.println("Berat (gram), μ Ringan , μ Sedang  , μ Berat , Servo Output (°)");
}

void loop() {
  // BACA SENSOR
  berat = scale.get_units();

  if (berat < 0)  berat = 0;

  // FUZZIFIKASI INPUT
  float mu_ringan = segitiga(berat, 0, 0, 100);
  float mu_sedang = segitiga(berat, 50, 125, 200);
  float mu_berat = segitiga(berat, 150, 250, 250);

  // RULE BASE
  // R1: IF berat ringan THEN servo kiri
  float rule_kiri = mu_ringan;

  // R2: IF berat sedang THEN servo tengah
  float rule_tengah = mu_sedang;

  // R3: IF berat berat THEN servo kanan
  float rule_kanan = mu_berat;

  // AGREGASI + CENTROID
  float numerator = 0;
  float denominator = 0;

  // sampling output servo
  for (int z = 0; z <= 180; z++) {
    // clipping rule (MIN)
    float kiri = min(rule_kiri, mf_kiri(z));
    float tengah = min(rule_tengah, mf_tengah(z));
    float kanan = min(rule_kanan, mf_kanan(z));

    // agregasi (MAX)
    float mu_agregasi = max(kiri,max(tengah, kanan));
    // centroid
    numerator += z * mu_agregasi;
    denominator += mu_agregasi;
  }

  // hasil CoG
  float output_servo = 90;
  if (bawah != 0) output_servo = nmerator / dnominator;

  // GERAKKAN SERVO
  myservo.write(outputservo);

  // SERIAL MONITOR
  Serial.print(berat);
  Serial.print(",");
  Serial.print(mu_ringan, 3);
  Serial.print(",");
  Serial.print(mu_sedang, 3);
  Serial.print(",");
  Serial.print(mu_berat, 3);
  Serial.print(",");
  Serial.println(output_servo);
  delay(5000);
}
