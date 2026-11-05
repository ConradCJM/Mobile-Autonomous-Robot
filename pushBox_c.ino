//motors
const int A_1B = 5;
const int A_1A = 6;
const int B_1B = 9;
const int B_1A = 10;

//sensors 
const int linePin = 2;

const int irPinR = 3;
const int irPinL = 4;

const int trigPin = 11;
const int echoPin = 12;

//ultrasonic filtering from course tutorial code 
unsigned long previousMillis = 0;
const long interval = 60;

const int window_size = 3;
float window[window_size];
int window_index = 0;

float lastDistance = 999.0;

const unsigned long MS_PER_TURN = 4000;   //time for one full circle - tune this
const int MAX_TURNS = 2;

//states
enum State {
  SEARCH,
  APPROACH,
  CHECKCOLOUR,
  PUSH,
  RECOVER,
  RELOCATE
};


State currentState = SEARCH;
//setup

void setup() {

  Serial.begin(9600);

  pinMode(A_1B, OUTPUT);
  pinMode(A_1A, OUTPUT);
  pinMode(B_1B, OUTPUT);
  pinMode(B_1A, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(irPinR, INPUT);
  pinMode(irPinL, INPUT);

  pinMode(linePin, INPUT);

  for (int i = 0; i < window_size; i++) {
    window[i] = 100;
  }
}


//ultrrasonic from tutorial in ckass 
float readSensorData() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    return lastDistance / 100.0;
  }

  float distance =
      duration * 343.0 * 0.000001 * 0.5;

  return distance;
}
// (slightly modified)
float distance() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    float rawDistance = readSensorData();

    window[window_index] = rawDistance;
    window_index = (window_index + 1) % window_size;

    float sum = 0;

    for (int i = 0; i < window_size; i++) {
      sum += window[i];
    }

    lastDistance = (sum / window_size) * 100.0;
  }

  return lastDistance;
}

//movement
void moveForward() {

  analogWrite(A_1B, 0);
  analogWrite(A_1A, 150); //modded to fix motor inconsistency with quality

  analogWrite(B_1B, 185);
  analogWrite(B_1A, 0);
}

void moveBackward() {

  analogWrite(A_1B, 150);
  analogWrite(A_1A, 0);

  analogWrite(B_1B, 0);
  analogWrite(B_1A, 150);
}

void turnRight() {

  analogWrite(A_1B, 105);
  analogWrite(A_1A, 0);

  analogWrite(B_1B, 150);
  analogWrite(B_1A, 0);
}

void turnLeft() {

  analogWrite(A_1B, 0);
  analogWrite(A_1A, 125);

  analogWrite(B_1B, 0);
  analogWrite(B_1A, 150);
}

void stopMove() {

  analogWrite(A_1B, 0);
  analogWrite(A_1A, 0);

  analogWrite(B_1B, 0);
  analogWrite(B_1A, 0);
}

//loop stuff
void loop() {

  float d = distance();

  switch (currentState) {

    //searchn for da box
    case SEARCH: {

      analogWrite(A_1B, 80);
      analogWrite(A_1A, 0);
      analogWrite(B_1B, 80);
      analogWrite(B_1A, 0);

      unsigned long searchStart = millis();
      bool found = false;

      while (millis() - searchStart < MS_PER_TURN * MAX_TURNS) {

        d = readSensorData() * 100.0;

        if (d < 50.0) {

          stopMove();
          Serial.println("SEARCH -> APPROACH");

          currentState = APPROACH;
          found = true;

          break;
        }
      }

      if (!found) {

        stopMove();
        Serial.println("SEARCH -> RELOCATE");

        currentState = RELOCATE;
      }

      break;
    }

    //go to box state
    case APPROACH:

      // Serial.print("APPROACH Distance=");
      // Serial.println(d);

      moveForward();

      if (digitalRead(linePin)) {

        Serial.println("APPROACH -> RECOVER (BOUNDARY)");

        currentState = RECOVER;
      }
      else if (d < 10.0) { // mod this

        Serial.println("APPROACH -> CHECKCOLOUR");

        currentState = CHECKCOLOUR;
      }
      else if (d > 50.0) { //mod this

        Serial.println("APPROACH -> SEARCH (LOST OBJECT)");

        currentState = SEARCH;
      }

      break;

    //coloour check state
    case CHECKCOLOUR: {
        bool rightWhite;
        bool leftWhite;
        moveBackward();
        delay(300);

        stopMove();

        delay(250);

        for(int i = 0; i < 4; i++){
          turnRight();
          delay(250);
          stopMove();
          delay(300);
          leftWhite = digitalRead(irPinL);

          
          delay(700);
          turnLeft();
          delay(250);
          stopMove();
          delay(300);
          rightWhite = digitalRead(irPinR);
          delay(700);
          Serial.print("IR Right = ");
          Serial.println(rightWhite);
          Serial.print("IR Left = ");
          Serial.println(leftWhite);
          if(!(leftWhite && rightWhite)){
            break;
          }
        }

        Serial.println("CHECKCOLOUR");

        Serial.print("IR Right = ");
        Serial.println(rightWhite);

        Serial.print("IR Left = ");
        Serial.println(leftWhite);

        if (!leftWhite || !rightWhite) { //might need to mod this if we want to prioritize a specific ir sensor

          Serial.println("WHITE BOX");

          currentState = PUSH;
        }
        else {
          Serial.println("BLACK BOX");

          currentState = RECOVER;

        }

        break;
    }
    //push state
    case PUSH:

      Serial.println("PUSH");

      moveForward();

      if (digitalRead(linePin)) {

        Serial.println("PUSH -> RECOVER");

        currentState = RECOVER;
      }

      break;

    case RELOCATE:

      Serial.println("RELOCATE");

      {
        unsigned long start = millis();

        moveForward();

        bool hitBoundary = false;
        while (millis() - start < 1200) {

          if (digitalRead(linePin)) {
            hitBoundary = true;
            break;
          }

          delay(10);
        }

        stopMove();
        delay(200);

        if (hitBoundary) {

          Serial.println("RELOCATE -> RECOVER");
          currentState = RECOVER;
          break;
        }

        turnRight();
        delay(400);

        stopMove();
        delay(200);
      }

      Serial.println("RELOCATE -> SEARCH");
      currentState = SEARCH;

      break;

    // recovere state
    case RECOVER:

      Serial.println("RECOVER");

      moveBackward();
      delay(1100);

      turnRight();
      delay(700);

      stopMove();

      Serial.println("RECOVER -> SEARCH");

      currentState = SEARCH;

      break;
  }
}