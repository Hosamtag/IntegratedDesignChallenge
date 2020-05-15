/*
 Robotics with the BOE Shield - ForwardLeftRightBackward
 Move forward, left, right, then backward for testing and tuning.
 Make sure the servo pins in the code match your own.

 WIDTH BETWEEN SENSORS : 3/4th of an inch.

 Notation of sensors -
  B = Black       W = White
  Letter position corresponds to position of sensor (left, middle, right)
All 3 detecting white =     W     W      W       
                          Left  Middle  Right

*/

#include <Servo.h>                           // Include servo library

Servo servoLeft;                             // Declare left and right servos
Servo servoRight;

int count = 0;        //Used in checking number of hashes (Black T’s) crossed
int thrsh = 200;      //threshold value, anything above is considered black
int stdDelay = 10;      //Standard delay value

void setup()                                 // Built-in initialization block
{ 
  servoLeft.attach(11);                      // Attach left signal to P11 
  servoRight.attach(12);                     // Attach right signal to P12
  int QTIPinL {49};     // Used for changing QTI pins
  int QTIPinM {51};
  int QTIPinR {53};

  pinMode(2, OUTPUT);   //Sets up LEDs
  pinMode(3, OUTPUT);   //Used for testing, you don’t really need it.
  
  Serial.begin(9600);     //Used for monitoring QTI sensor values
}  

//FUNCTIONS

long rcTime(int pin) {      //Takes value of QTI sensor
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delayMicroseconds(230);
  pinMode(pin, INPUT);
  digitalWrite(pin, LOW);
  long time  = micros();
  while (digitalRead(pin));
  time = micros() - time;
  return time;
}

void turnLeft(int t) {        //Turns left for given t of milliseconds
  servoLeft.writeMicroseconds(1300);
  servoRight.writeMicroseconds(1300);
  delay(t);
}

void turnRight(int t) {         //Turns right for given t of milliseconds
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1700);
  delay(t);
}

void moveForward(int t) {     //Moves Forward for given t of milliseconds
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1300);
  delay(t);
}

void reverse(int t) {       //Reverses if checkForBlack returns false
  if (checkForBlack() == false) {
    digitalWrite(2, LOW);   //Turns off swivel LED, indicating bot is no longer swiveling
    servoLeft.writeMicroseconds(1300);
    servoRight.writeMicroseconds(1700);
    delay(t);
  }
}

  //Causes robot to do a little swivel, checking for black line
  //Used in case robot loses track of line (QTI sensors only detect white)
boolean checkForBlack() {   
  digitalWrite(2,HIGH);      //Turns on swivel LED, indicating robot is looking for line.
  for (int i = 0; i < 2; i++) {   //Causes bot to swivel left 
     turnLeft(100);     //Checks for black line each 100 milliseconds. 
     if (rcTime(49) > thrsh || rcTime(51) > thrsh || rcTime(53) > thrsh) {
      return true;      //Loop can be adjusted to better look for line.
     }
  }
   for (int i = 0; i <4; i++) {   //Causes bot to swivel right
     turnRight(100);      //Checks for black line each 100 milliseconds. 
     if (rcTime(49) > thrsh || rcTime(51) > thrsh || rcTime(53) > thrsh) {
      return true;
     }
  }
  turnLeft(200);      //Straightens out bot.
  return false;
}

void hashPoint() {      //Stops bot, checks to see position
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
  digitalWrite(3, HIGH);    //Indicates a hash has been accounted for
  delay(1000);
  digitalWrite(3, LOW);
  count++;        //counts how many times a hash has passed
  if (count == 5) {     //Stops bot if 5 hashes has been passed
    servoLeft.detach();     //Useful for IDC to make sure 
    servoRight.detach();    //seeker team doesn’t collide into other bots
  }
  else {          //if less than 5 hashes has been passed, bot keeps going.
    servoLeft.writeMicroseconds(1700);
    servoRight.writeMicroseconds(1300);
    delay(200);
  }
}



void loop()                                    // Main loop auto-repeats
{ 
  int l = rcTime(49);     //Takes QTI values and assigns to corresponding letter 
  int m = rcTime(51);     //for every loop
  int r = rcTime(53);
  Serial.print(l);                  //prints QTI sensor values in neat format
  Serial.print(", ");
  Serial.print(m);
  Serial.print(", ");
  Serial.println(r);
  delay(stdDelay);    //standard delay used to read what’s being printed to monitor
  if(l < thrsh && m < thrsh && r < thrsh) { //checks to see if WWW
    reverse(100);     
  }
  if(l < thrsh && m < thrsh && r > thrsh){  //checks to see if WWB
      turnRight(stdDelay);
  }
  if (l < thrsh && m > thrsh && r < thrsh){ //Checks to see if WBW
    moveForward(stdDelay);
  }
  if (l < thrsh && m > thrsh && r > thrsh){ //checks to see if WBB
    turnRight(stdDelay);
  }
  if (l > thrsh && m < thrsh && r < thrsh){ //checks to see if BWW
      turnLeft(stdDelay);
  }
  if (l > thrsh && m > thrsh && r < thrsh) {  //checks to see if BBW
    turnLeft(stdDelay);
  }
  if (l > thrsh && m > thrsh && r > thrsh) {  //Checks to see if BBB (Hash detected)
    hashPoint();
  }
} 
