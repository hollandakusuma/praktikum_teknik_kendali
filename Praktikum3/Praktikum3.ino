// PIN CONFIGURATION

// Ultrasonic Sensor Kiri
#define TRIG_LEFT  5
#define ECHO_LEFT  18

// Ultrasonic Sensor Kanan
#define TRIG_RIGHT 17
#define ECHO_RIGHT 16

// Motor Driver (contoh L298N)
#define ENA 25   // PWM motor kiri
#define IN1 26
#define IN2 27

#define ENB 33   // PWM motor kanan
#define IN3 32
#define IN4 14

// PID PARAMETERS (AWAL)
float Kp = 3.0;
float Ki = 0.05;
float Kd = 1.5;

// VARIABLE PID
float error = 0;
float prev_error = 0;
float integral = 0;
float derivative = 0;
float control = 0;

// SYSTEM PARAMETERS
int base_speed = 130;   // kecepatan dasar
int max_pwm = 255;

unsigned long lastTime = 0;
int Ts = 50; // sampling time (ms)

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);

  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  ledcAttachPin(ENA, 0);
  ledcAttachPin(ENB, 1);

  ledcSetup(0, 5000, 8); // channel 0, 5kHz, 8-bit
  ledcSetup(1, 5000, 8); // channel 1, 5kHz, 8-bit
}

void loop() {
  if (millis() - lastTime >= Ts) {
    lastTime = millis();

    // 1. READ SENSOR
    float d_left  = readUltrasonic(TRIG_LEFT, ECHO_LEFT);
    float d_right = readUltrasonic(TRIG_RIGHT, ECHO_RIGHT);

    // 2. ERROR
    error = d_left - d_right;

    // 3. PID CALCULATION
    integral += error;
    derivative = error - prev_error;

    control = (Kp * error) + (Ki * integral) + (Kd * derivative);

    // 4. MOTOR CONTROL
    int motor_left  = base_speed + control;
    int motor_right = base_speed - control;

    motor_left  = constrain(motor_left, 0, max_pwm);
    motor_right = constrain(motor_right, 0, max_pwm);

    setMotorLeft(motor_left);
    setMotorRight(motor_right);

    prev_error = error;

    // DEBUG SERIAL
    Serial.print("L: "); Serial.print(d_left);
    Serial.print(" | R: "); Serial.print(d_right);
    Serial.print(" | E: "); Serial.print(error);
    Serial.print(" | U: "); Serial.println(control);
  }
}

float readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms

  float distance = duration * 0.034 / 2;

  return distance;
}

void setMotorLeft(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  ledcWrite(0, speed);
}

void setMotorRight(int speed) {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(1, speed);
}