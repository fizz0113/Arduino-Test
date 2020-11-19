int motorPin_A = 8;
int motorPin_AP = 9;
int motorPin_B = 10;
int motorPin_BP = 11;
int delayTime = 20;

void setup() {
  pinMode(motorPin_A, OUTPUT);
  pinMode(motorPin_AP, OUTPUT);
  pinMode(motorPin_B, OUTPUT);
  pinMode(motorPin_BP, OUTPUT);
}

void loop() {
  digitalWrite(motorPin_A, LOW);
  digitalWrite(motorPin_AP,HIGH);
  digitalWrite(motorPin_B, HIGH);
  digitalWrite(motorPin_BP,LOW);
  delay(delayTime);
  digitalWrite(motorPin_A, LOW);
  digitalWrite(motorPin_AP,HIGH);
  digitalWrite(motorPin_B, LOW);
  digitalWrite(motorPin_BP,HIGH);
  delay(delayTime);
  digitalWrite(motorPin_A, HIGH);
  digitalWrite(motorPin_AP,LOW);
  digitalWrite(motorPin_B, LOW);
  digitalWrite(motorPin_BP,HIGH);
  delay(delayTime);
  digitalWrite(motorPin_A, HIGH);
  digitalWrite(motorPin_AP,LOW);
  digitalWrite(motorPin_B, HIGH);
  digitalWrite(motorPin_BP,LOW);
  delay(delayTime);
}
