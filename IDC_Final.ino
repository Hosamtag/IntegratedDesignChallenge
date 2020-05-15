#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>

//ALWAYS SET THE COLOR DISKS AT THE BACK END OF HASHES

// FOR COLORSENSE & Comm
#include "Adafruit_TCS34725.h"
const int transmitting = 8; //Used for transmitting LED now for green
const int sigtrans = 9;     // now for transmitting
const int receiving = 6; //Used for Receiving LED now for Red
#define Rx 17 //DOUT to pin 17
#define Tx 16 //DIN to pin 16


// COLORTEMP            G Val     R Val   w/Hood G    w/ Hood R
// NORMAL: 5850          294       394      2209         1640
// GREEN ~5950           309       388      2196         1563
// RED ~5690             285       425      2132         1630

// see where differences are whether in RG or Colortemp
/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);


// FOR LINEFOLLOW
Servo servoLeft;                             // Declare left and right servos
Servo servoRight;

int count = 0;        //Used in checking number of hashes (Black T’s) crossed
int thrsh = 250;      //threshold value, anything above is considered black
int stdDelay = 10;      //Standard delay value
char Colors[] = {'b', 'b', 'b', 'b', 'b'};  // colors to start, later b is green, B red
char Rec[] = {'b', 'b', 'b', 'b', 'b'};  // colors received
int k = 0;                               // First counter for comm
int Score = 0;                         // Intermediate Score
int q = 0;                             // Second counter
char Rscore = 'u';                     // Rachel Score
boolean Done = false;                  // to finish code in the end

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
  digitalWrite(2, HIGH);     //Turns on swivel LED, indicating robot is looking for line.
  for (int i = 0; i < 2; i++) {   //Causes bot to swivel left
    turnLeft(100);     //Checks for black line each 100 milliseconds.
    if (rcTime(49) > thrsh || rcTime(51) > thrsh || rcTime(53) > thrsh) {
      return true;      //Loop can be adjusted to better look for line.
    }
  }
  for (int i = 0; i < 4; i++) {  //Causes bot to swivel right
    turnRight(100);      //Checks for black line each 100 milliseconds.
    if (rcTime(49) > thrsh || rcTime(51) > thrsh || rcTime(53) > thrsh) {
      return true;
    }
  }
  turnLeft(200);      //Straightens out bot.
  return false;
}


int ColorRG() {          //ColorSense Function
  int  var = 0;
  char Col = 'b';
  while (var < 2) {
    uint16_t r, g, b, c, colorTemp, lux;

    tcs.getRawData(&r, &g, &b, &c);
    // colorTemp = tcs.calculateColorTemperature(r, g, b);
    colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
    lux = tcs.calculateLux(r, g, b);

    Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
    Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
    Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
    Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
    Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
    Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
    Serial.println(" ");

    if (colorTemp > 3500) {
      digitalWrite(transmitting, HIGH); // FOR GREEN
      Col = 'B';
      delay(100);
      digitalWrite(transmitting, LOW);
    }

    else if (colorTemp < 3500) {
      digitalWrite(receiving, HIGH); // FOR RED
      Col = 'b';
      delay(100);
      digitalWrite(receiving, LOW);
    }
    var++;
  }
  return Col;
}

void hashPoint() {           //Stops bot, checks to see position
  int Col = 0;
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
  delay(100);               //Delay to allow for function of colorsense
  //COLORSENSE FUNCTION
  Colors[count] = ColorRG();
  // Col = ColorRG();
  /*if (Col == 1){
    digitalWrite(sigtrans, HIGH);
    //Serial2.print('G');
    digitalWrite(sigtrans, LOW);
    }
    else if (Col == 0){
    digitalWrite(sigtrans, HIGH);
    //Serial2.print('R');
    digitalWrite(sigtrans, LOW);
    }
  */
  digitalWrite(3, HIGH);     //Indicates a hash has been accounted for
  delay(1000);
  digitalWrite(3, LOW);
  count++;                   //counts how many times a hash has passed
  if (count == 5) {          //Stops bot if 5 hashes has been passed
    servoLeft.detach();      //Useful for IDC to make sure
    servoRight.detach();     //seeker team doesn’t collide into other bots
    //Serial2.print("test");
    delay(1000);
    Serial2.print('Z');      // stop and send to Rachel so we can begin receiving
    Serial.print(Colors);
    while (k < 5) {
      if (Serial2.available() > 0) {
        digitalWrite(receiving, HIGH);
        char Let = char(Serial2.read());
        if ((Let == 'b') || (Let == 'B')) {
          Rec[k] = Let;
          Serial.print(Let);
          k += 1;
        }
      }
    }
    if (k = 5) {
      while (q < 5) {
        if ((Rec[q] == Colors[q]) && Rec[q] == 'B') {
          Score = Score + 1;
          Serial.println(Score);
        }
        q += 1;
      }
      if (Score == 0) {
        Rscore = 'u';
      }
      else if (Score == 1) {
        Rscore = 'v';
      }
      else if (Score == 2) {
        Rscore = 'w';
      }
      else if (Score == 3) {
        Rscore = 'x';
      }
      else if (Score == 4) {
        Rscore = 'y';
      }
      else if (Score == 5) {
        Rscore = 'z';
      }
      Serial2.print(Rscore);
      Serial.print(Rscore);
      Score = Score * 10;
    }
    Serial3.write(12);
    Serial3.write(17);
    delay(5);
    Serial3.print(Score);
    delay(500);
bool Rachel = true;
while (Rachel){
    char IsSeekerReady = Serial2.read();
    if  (IsSeekerReady == 'p'){
        Serial2.print(Rscore);
        Rachel = false;      
}
    }
   bool GoodGod= true;
    while (GoodGod){
    char Incoming = Serial2.read();
  int TwoDigitReceived = 0;
  if (isdigit(Incoming)) {
     TwoDigitReceived = int(Incoming);
  }
int FinalScore = ((TwoDigitReceived - 10)* 10);
Serial3.print("Final:");
Serial3.print(FinalScore);
GoodGod = false;
}


Done = true;
  }
  else {          //if less than 5 hashes has been passed, bot keeps going.
    servoLeft.writeMicroseconds(1700);
    servoRight.writeMicroseconds(1300);
    delay(200);
  }
}



void setup(void) {
  Serial.begin(9600);
  //For RGB
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }
  pinMode(transmitting, OUTPUT); //Sets pin transmitting to output
  pinMode(receiving, OUTPUT); //Sets pin receiving to output
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  delay(500);
  Serial.print('q');

  //For Line
  servoLeft.attach(11);                      // Attach left signal to P11
  servoRight.attach(12);                     // Attach right signal to P12
  int QTIPinL {49};     // Used for changing QTI pins
  int QTIPinM {51};
  int QTIPinR {53};

  pinMode(2, OUTPUT);   //Sets up LEDs
  pinMode(3, OUTPUT);   //Used for testing, you don’t really need it.

  Serial.begin(9600);     //Used for monitoring QTI sensor values


  // Now we're ready to get readings!
}

void loop(void) {
  if(Done) return;
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
    if (l < thrsh && m < thrsh && r > thrsh) { //checks to see if WWB
      turnRight(stdDelay);
    }
    if (l < thrsh && m > thrsh && r < thrsh) { //Checks to see if WBW
      moveForward(stdDelay);
    }
    if (l < thrsh && m > thrsh && r > thrsh) { //checks to see if WBB
      turnRight(stdDelay);
    }
    if (l > thrsh && m < thrsh && r < thrsh) { //checks to see if BWW
      turnLeft(stdDelay);
    }
    if (l > thrsh && m > thrsh && r < thrsh) {  //checks to see if BBW
      turnLeft(stdDelay);
    }
    if (l > thrsh && m > thrsh && r > thrsh) {  //Checks to see if BBB (Hash detected)
      hashPoint();
    }
  }
}
