// =====================================================
// PRAKTIKUM: Filtering Sensor dan Akuisisi Data
// ESP32 + HC-SR04
// Sampling time = 10 ms
// Total data = 1000 sampel
// Data mentah tanpa filtering
// =====================================================

#define PIN_TRIG 5
#define PIN_ECHO 18

const unsigned long waktuSampling = 10;     // ms
const int jumlahSampelTotal = 1000;

String namaMahasiswa = "ISI_NAMA_ANDA";
String nimMahasiswa  = "ISI_NIM_ANDA";

unsigned long waktuSebelumnya = 0;
unsigned long waktuMulai = 0;
int jumlahSampel = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  delay(1000);

  Serial.println(F("===  PRAKTIKUM 1  ==="));
  Serial.println(F("Filtering Sensor dan Akuisisi Data"));
  Serial.print(F("Nama: "));
  Serial.println(namaMahasiswa);
  Serial.print(F("NIM: "));
  Serial.println(nimMahasiswa);
  Serial.println(F("Waktu (ms),jarak (cm)");

  waktuMulai = millis();   // referensi waktu mulai (agar dimulai dari nol)
}

void loop() {
  unsigned long waktuSekarang = millis();

  if ((waktuSekarang - waktuSebelumnya >= waktuSampling) && (jumlahSampel < jumlahSampelTotal)) {    
    waktuSebelumnya = waktuSekarang;
    // Timestamp relatif (dimulai dari 0)
    unsigned long waktuRelatif = waktuSekarang - waktuMulai;
    float jarak = bacaSensorUltrasonik();
    Serial.print(waktuRelatif);  Serial.print(",");
    Serial.println(jarak);
    jumlahSampel++;
  }
}

// -----------------------------------------------------
// Fungsi Pembacaan Sensor Ultrasonik
// -----------------------------------------------------
float bacaSensorUltrasonik() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  long durasi = pulseIn(PIN_ECHO, HIGH);
  float jarakCm = durasi * 0.0343 / 2.0;
  return jarakCm;
}