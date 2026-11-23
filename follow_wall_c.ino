#include <math.h>

// Line sensor
const int LINE_SENSOR = 2;



// motor pins

const int A_1B = 5;
const int A_1A = 6;
const int B_1B = 9;
const int B_1A = 10;

//ultrasonic

// Front sensor

const int FRONT_TRIG = 11;
const int FRONT_ECHO = 12;

// Front-right sensor

const int FR_TRIG = 7;
const int FR_ECHO = 8;

// Back-right sensor

const int BR_TRIG = A0;
const int BR_ECHO = A1;

//state machine

enum State
{
  FOLLOW_WALL,
  TURN_LEFT,
  // TURN_RIGHT,
  STOPPED
};

State currentState = FOLLOW_WALL;


//control params

const float DESIRED_DISTANCE = 15.0;     // cm

const float SENSOR_SPACING = 13.0;       // distance between FR and BR in cm


const float KP = 15.0;
const float KI = 0.05;

float integral = 0.0;

const int WINDOW_SIZE = 3;

float frontWindow[WINDOW_SIZE] = {100,100,100};
float frWindow[WINDOW_SIZE] = {15,15,15};
float brWindow[WINDOW_SIZE] = {15,15,15};

float frontFiltered = 100.0;
float frFiltered = 15.0;
float brFiltered = 15.0;

int windowIndex = 0;

unsigned long previousTime = 0;


//helper
float readUltrasonic(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if(duration == 0)
    return -1.0;

  float distance =
    duration * 0.0343 / 2.0;

  return distance;
}

float movingAverage(
  float raw,
  float window[]
)
{
  if(raw < 0)
  {
    float sum = 0;

    for(int i = 0; i < WINDOW_SIZE; i++)
      sum += window[i];

    return sum / WINDOW_SIZE;
  }

  window[windowIndex] = raw;

  float sum = 0;

  for(int i = 0; i < WINDOW_SIZE; i++)
  {
    sum += window[i];
  }

  return sum / WINDOW_SIZE;
}


//make motors move
//left and right are lowkey backwards since we wired them backwards after switching motors but we already demo'd so im not changing the code and pins
void setMotorSpeed(int leftSpeed, int rightSpeed)
{
  // //offsets for different quality motors


  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  // LEFT MOTOR

  if(leftSpeed >= 0)
  {
    analogWrite(A_1B, 0);
    analogWrite(A_1A, leftSpeed);
  }
  else
  {
    analogWrite(A_1A, 0);
    analogWrite(A_1B, -leftSpeed);
  }

  // RIGHT MOTOR

  if(rightSpeed >= 0)
  {
    analogWrite(B_1B, rightSpeed);
    analogWrite(B_1A, 0);
  }
  else
  {
    analogWrite(B_1B, 0);
    analogWrite(B_1A, -rightSpeed);
  }
}

void stopRobot()
{
  setMotorSpeed(0,0);
}

void turnLeftInPlace()
{
  setMotorSpeed(140,-140);
}

void turnRight()
{
  setMotorSpeed(-125,125);
}

void reverseRobot()
{
  setMotorSpeed(-100,-100);
}


//pi for wall following
void wallFollowControl(
  float frontRight,
  float backRight)
{
  unsigned long now = millis();

  float dt =
    (now - previousTime) / 1000.0;

  previousTime = now;

  if(dt <= 0)
    dt = 0.01;

  //dis est

  float wallDistance =
    (frontRight + backRight) / 2.0;

  
// heading est
  float theta =
    atan(
      (frontRight - backRight)
      / SENSOR_SPACING
    );

  

//errors
  float distanceError =
    DESIRED_DISTANCE - wallDistance;

  float headingError =
    -theta;


// combined error
  float error =
    distanceError +
    (10.0 * headingError);

  
//pi 
  integral += error * dt;

  integral =
    constrain(integral,-50,50);

  float control =
    KP * error +
    KI * integral;

  //motors  

  const int BASE_SPEED = 130;
  const int MIN_SPEED = 97;

  int controlLimit = 80;

  control = constrain(
    control,
    -controlLimit,
    controlLimit
  );

  int leftMotor =
    BASE_SPEED + control;

  int rightMotor =
    BASE_SPEED - control;

  leftMotor =
    constrain(
      leftMotor,
      MIN_SPEED,
      255
    );

  rightMotor =
    constrain(
      rightMotor,
      MIN_SPEED,
      255
    );

  // Serial.print(" Control: ");
  // Serial.print(control);

  // Serial.print(" L: ");
  // Serial.print(leftMotor);

  // Serial.print(" R: ");
  // Serial.println(rightMotor);

  setMotorSpeed(
    leftMotor,
    rightMotor
  );
}

// setup

void setup()
{
  Serial.begin(9600);
  pinMode(LINE_SENSOR, INPUT);

  pinMode(A_1B, OUTPUT);
  pinMode(A_1A, OUTPUT);
  pinMode(B_1B, OUTPUT);
  pinMode(B_1A, OUTPUT);

  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(FR_TRIG, OUTPUT);
  pinMode(FR_ECHO, INPUT);

  pinMode(BR_TRIG, OUTPUT);
  pinMode(BR_ECHO, INPUT);

  previousTime = millis();
}


//loop
void loop()
{
  // Read sensors one at a time to reduce interference

  float frontDist =
    readUltrasonic(FRONT_TRIG, FRONT_ECHO);

  

  float frontRight =
    readUltrasonic(FR_TRIG, FR_ECHO);

  

  float backRight =
    readUltrasonic(BR_TRIG, BR_ECHO);

  frontFiltered =
  movingAverage(
    frontDist,
    frontWindow
  );

frFiltered =
  movingAverage(
    frontRight,
    frWindow
  );

brFiltered =
  movingAverage(
    backRight,
    brWindow
  );

windowIndex =
  (windowIndex + 1)
  % WINDOW_SIZE;

  //------------------------------------------------

//   Serial.print("Front: ");
//   Serial.print(frontDist);

//   Serial.print(" FR: ");
//   Serial.print(frontRight);

//   Serial.print(" BR: ");
//   Serial.println(backRight);


//   Serial.print("Front: ");
// Serial.print(frontFiltered);

// Serial.print(" FR: ");
// Serial.print(frFiltered);

// Serial.print(" BR: ");
// Serial.println(brFiltered);

  if(digitalRead(LINE_SENSOR) == HIGH)
  {
    currentState = STOPPED;
  }

  //------------------------------------------------

  switch(currentState)
  {
     
    // follow wall
     

    case FOLLOW_WALL:
    {
      wallFollowControl(
        frFiltered,
        brFiltered
      );

      //left turn

      if(frontFiltered < 22)
      {
        currentState = TURN_LEFT;

        Serial.println(
          "FOLLOW -> TURN_LEFT"
        );
      }

      // right turn

      // else if(
      //   frontRight > 40 &&
      //   backRight > 40 &&
      //   frontDist > 40
      // )
      // {
      //   currentState = TURN_RIGHT;

      //   Serial.println(
      //     "FOLLOW -> TURN_RIGHT"
      //   );
      // }

      break;
    }

     
    // left corner     

    case TURN_LEFT:
    {
      turnLeftInPlace();

      if(
        frFiltered < 30 &&
        brFiltered < 30 &&
        frontFiltered > 40
      )
      {
        stopRobot();

        currentState = FOLLOW_WALL;

        Serial.println(
          "TURN_LEFT -> FOLLOW"
        );
      }

      break;
    }

    case STOPPED:
{
    stopRobot();

    Serial.println("BLACK LINE DETECTED");

    break;
}

     
    // rigth corner
     
    // case TURN_RIGHT:
    // {
    //   turnRight();

    //   if(
    //     frontRight < 30 ||
    //     backRight < 30
    //   )
    //   {
    //     stopRobot();

    //     currentState = FOLLOW_WALL;

    //     Serial.println(
    //       "TURN_RIGHT -> FOLLOW"
    //     );
    //   }

    //   break;
    // }
  }
}