// Pin definitions for L9110 motor driver
const int A_1B = 5;   // Right motor input 1
const int A_1A = 6;   // Right motor input 2
const int B_1B = 9;   // Left motor input 1
const int B_1A = 10;  // Left motor input 2
//const int rightOffset = 0.843;

void setup() {
    pinMode(A_1B, OUTPUT);
    pinMode(A_1A, OUTPUT);
    pinMode(B_1B, OUTPUT);
    pinMode(B_1A, OUTPUT);
}

// --- Movement Functions ---
// void moveForward() {

    
//     digitalWrite(A_1B, LOW);
//     digitalWrite(A_1A, HIGH);
//     digitalWrite(B_1B, HIGH);
//     digitalWrite(B_1A, LOW);
// }

void moveForward(){
  analogWrite(A_1B, 0);
  analogWrite(A_1A, 215); // reduced due to different quality of motors //og val for calibration: 215
  analogWrite(B_1B, 255);
  analogWrite(B_1A, 0);
}

void moveBackward() {
//     digitalWrite(A_1B, HIGH);
//     digitalWrite(A_1A, LOW);
//     digitalWrite(B_1B, LOW);
//     digitalWrite(B_1A, HIGH);
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
//     digitalWrite(A_1B, HIGH);
//     digitalWrite(A_1A, LOW);
//     digitalWrite(B_1B, HIGH);
//     digitalWrite(B_1A, LOW);
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
    delay(346);//just modify this when 90 degree angle is offset
    stopMove();
}

// --- Main Loop ---
// bool started = false;
void loop() {


    // if (!started) {
    delay(3000);
        // started = true;
    // }
    moveForward();
    //delay(1000);    
    delay(2250);  //  distance (100 cm)
    stopMove();
    // delay(10000);
    // delay(99999);
    delay(1000);
    turn90Right();
}
