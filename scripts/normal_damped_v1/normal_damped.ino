#define IN1 5
#define IN2 6
#define ENA 9

#define ENCODER_A 2
#define ENCODER_B 3

volatile long encoderCount = 0;
int direction = 1;
int maxTicks = 255;
int baseSpeed = 90;
int currentSpeed = baseSpeed;
int damping = 1;

void setup() {
  // put your setup code here, to run once:
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_A), readEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), readEncoder, CHANGE);

  Serial.begin(9600);

  encoderCount = 0;
  swingMotor(direction, currentSpeed);

}

// motor swing, changes the motor driver H-bridge connections around
void swingMotor(int dir, int speed) {
  if (dir > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }

  analogWrite(ENA, speed);
}

// encoder reading function, increases by one when signals match
void readEncoder() {
  bool A = digitalRead(ENCODER_A);
  bool B = digitalRead(ENCODER_B);

  if (A == B) {
    encoderCount++;
  }
}
void loop() {

  // print encoder count in the serial
  static long lastPrinted = 0;
  if (millis() - lastPrinted > 100) { // every 100ms
    Serial.println(encoderCount);
    lastPrinted = millis();
  }
  
  int distance = abs(encoderCount);
  int effectiveSpeed = currentSpeed;

  // slowing the swing down near the max amplitude to improve realism
  if (distance > maxTicks*0.7) {
    float scale = pow(1.0 - ((float)(distance - maxTicks * 0.7) / (maxTicks * 0.2)), 2);
    effectiveSpeed = max(currentSpeed * scale, 30);
  }

  swingMotor(direction, effectiveSpeed);

  //Change direction at the end of swing
  if (distance >= maxTicks) {
    direction *= -1;
    encoderCount = 0;
    
    // decreases amplitude (creating a dampening effect)
    if (maxTicks > 60) {
      maxTicks -= 5;
    }
    else {
      // resets the amplitude when the movement gets too small
      maxTicks = 255;
    }
    
    delay(200);
  }
}