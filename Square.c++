// Pin definitions for L9110 motor driver
const int A_1B = 5;   // Right motor input 1
const int A_1A = 6;   // Right motor input 2
const int B_1B = 9;   // Left motor input 1
const int B_1A = 10;  // Left motor input 2
const int rightOffset = 0.9;

void setup() {
    pinMode(A_1B, OUTPUT);
    pinMode(A_1A, OUTPUT);
    pinMode(B_1B, OUTPUT);
    pinMode(B_1A, OUTPUT);
}

// --- Movement Functions ---
void moveForward(){
  analogWrite(A_1B, 0);
  analogWrite(A_1A, 215); // reduced due to different quality of motors
  analogWrite(B_1B, 255);
  analogWrite(B_1A, 0);
}

void moveBackward() {
  analogWrite(A_1B, 255);
  analogWrite(A_1A, 0);
  analogWrite(B_1B, 0);
  analogWrite(B_1A, 255);
}

// void turnLeft() {
//     digitalWrite(A_1B, LOW);
//     digitalWrite(A_1A, HIGH);
//     digitalWrite(B_1B, LOW);
//     digitalWrite(B_1A, HIGH);
// }

void turnRight() {
    analogWrite(A_1B, 255);
    analogWrite(A_1A, LOW);
    analogWrite(B_1B, 255);
    analogWrite(B_1A, LOW);
}

void stopMove() {
    analogWrite(A_1B, LOW);
    analogWrite(A_1A, LOW);
    analogWrite(B_1B, LOW);
    analogWrite(B_1A, LOW);
}

void turn90Right(){
    turnRight();
    delay(235);
    stopMove();
}

// --- Main Loop ---
void loop() {
    delay(2500);
    moveForward();
    delay(3000); // modify this for distance (100 cm)
    stopMove();
    delay(500);
    turn90Right();
}
