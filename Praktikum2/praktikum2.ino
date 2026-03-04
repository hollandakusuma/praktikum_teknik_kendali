#define PIN_TRIGGER 5
#define PIN_ECHO    18

#define PIN_ENA     25
#define PIN_ENB     26
#define PIN_IN1     27
#define PIN_IN2     14
#define PIN_IN3     12
#define PIN_IN4     13

// IDENTITAS PRAKTIKAN
String nama = "ISI_NAMA";
String nim  = "ISI_NIM";

// PARAMETER SISTEM
float setpoint = 40.0;     // cm
float Kp = 1.5;            // ubah sesuai eksperimen
int mode = 0;              // 0 = RAW, 1 = MOVING AVERAGE FILTER

unsigned long waktuSebelumnya = 0;
unsigned long intervalSampling = 10;   // 10 ms
unsigned long waktuMulai;

// MOVING AVERAGE
const int windowSize = 10;
float bufferMA[windowSize];
int indexBuffer = 0;
float totalMA = 0;
bool bufferPenuh = false;

// PWM SETUP ESP32
const int channelA = 0;
const int channelB = 1;
const int freq = 5000;
const int resolution = 8;   // 0-255

void setup() {

  Serial.begin(115200);
  Serial.println("PRAKTIKUM 2 - ANALISIS RESPON SISTEM DAN KONTROL PROPORSIONAL");
  Serial.println("waktu_ms,jarak_cm");

  pinMode(PIN_TRIGGER, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);

  // arah maju
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);

  ledcSetup(channelA, freq, resolution);
  ledcSetup(channelB, freq, resolution);

  ledcAttachPin(PIN_ENA, channelA);
  ledcAttachPin(PIN_ENB, channelB);

  waktuMulai = millis();
}

void loop() {
  unsigned long waktuSekarang = millis();
  if (waktuSekarang - waktuSebelumnya >= intervalSampling) {
    waktuSebelumnya = waktuSekarang;
    unsigned long waktuRelatif = waktuSekarang - waktuMulai;
	
    // 1. Baca sensor
    float jarakRaw = bacaJarak();
    float jarakDipakai;

    // 2. Mode RAW / FILTER
    if (mode == 0) {
      jarakDipakai = jarakRaw;
    } else {
      jarakDipakai = movingAverage(jarakRaw);
    }

    // 3. Hitung error
    float error = setpoint - jarakDipakai;

    // 4. Kontrol P
    float kontrol = Kp * error;

    // 5. Batasi PWM
    int pwm = abs(kontrol);

    if (pwm > 255) pwm = 255;
    if (pwm < 0) pwm = 0;

    // 6. Kirim PWM ke kedua motor
    ledcWrite(channelA, pwm);
    ledcWrite(channelB, pwm);

    // 7. Logging CSV
    Serial.print(waktuRelatif);
    Serial.print(",");
    Serial.println(jarakDipakai);
  }
}

// FUNGSI BACA ULTRASONIK
float bacaJarak() {
  digitalWrite(PIN_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIGGER, LOW);

  long durasi = pulseIn(PIN_ECHO, HIGH, 30000);
  float jarak = durasi * 0.034 / 2.0;
  return jarak;
}

// FUNGSI MOVING AVERAGE
float movingAverage(float inputBaru) {

  totalMA -= bufferMA[indexBuffer];
  bufferMA[indexBuffer] = inputBaru;
  totalMA += inputBaru;

  indexBuffer++;
  if (indexBuffer >= windowSize) {
    indexBuffer = 0;
    bufferPenuh = true;
  }

  if (bufferPenuh)
    return totalMA / windowSize;
  else
    return totalMA / (indexBuffer == 0 ? 1 : indexBuffer);
}