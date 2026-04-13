// =======================
// PIN CONFIGURATION
// =======================

// Ultrasonic Sensor Kiri
#define TRIG_LEFT  5
#define ECHO_LEFT  18

// Ultrasonic Sensor Kanan
#define TRIG_RIGHT 17
#define ECHO_RIGHT 16

// Motor Driver
#define ENA 25
#define IN1 26
#define IN2 27

#define ENB 33
#define IN3 32
#define IN4 14

// =======================
// PID PARAMETERS
// =======================
float Kp = 3.0;
float Ki = 0.05;
float Kd = 1.5;

// =======================
// PID VARIABLES
// =======================
float error = 0;
float prev_error = 0;
float integral = 0;
float derivative = 0;
float control = 0;

// =======================
// SYSTEM PARAMETERS
// =======================
int base_speed = 130;
int max_pwm = 255;

unsigned long lastTime = 0;
unsigned long startTime = 0;
int Ts = 50; // sampling time (ms)

// =======================
// SETUP
// =======================
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

  ledcSetup(0, 5000, 8);
  ledcSetup(1, 5000, 8);

  Serial.println("SYSTEM INIT... Robot diam 5 detik");

  // 🚨 DIAM 5 DETIK SEBELUM MULAI
  delay(5000);

  Serial.println("time_ms,d_left,d_right,error,control");

  startTime = millis(); // start logging time
}

// =======================
// LOOP
// =======================
void loop() {
  if (millis() - lastTime >= Ts) {
    lastTime = millis();

    // TIME STAMP
    unsigned long currentTime = millis() - startTime;

    // =======================
    // 1. READ SENSOR
    // =======================
    float d_left  = readUltrasonic(TRIG_LEFT, ECHO_LEFT);
    float d_right = readUltrasonic(TRIG_RIGHT, ECHO_RIGHT);

    // =======================
    // 2. ERROR
    // =======================
    error = d_left - d_right;

    // =======================
    // 3. PID CALCULATION (DISKRIT)
    // =======================
    float dt = Ts / 1000.0; // konversi ke detik

    integral += error * dt;
    derivative = (error - prev_error) / dt;

    control = (Kp * error) + (Ki * integral) + (Kd * derivative);

    // =======================
    // 4. MOTOR CONTROL
    // =======================
    int motor_left  = base_speed + control;
    int motor_right = base_speed - control;

    motor_left  = constrain(motor_left, 0, max_pwm);
    motor_right = constrain(motor_right, 0, max_pwm);

    setMotorLeft(motor_left);
    setMotorRight(motor_right);

    prev_error = error;

    // =======================
    // 5. SERIAL LOGGING (CSV READY)
    // =======================
    Serial.print(currentTime);
    Serial.print(",");
    Serial.print(d_left);
    Serial.print(",");
    Serial.print(d_right);
    Serial.print(",");
    Serial.print(error);
    Serial.print(",");
    Serial.println(control);
  }
}

// =======================
// ULTRASONIC FUNCTION
// =======================
float readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  float distance = duration * 0.034 / 2;

  return distance;
}

// =======================
// MOTOR CONTROL
// =======================
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