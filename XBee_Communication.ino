#define Rx 17 //DOUT to pin 17
#define Tx 16 //DIN to pin 16
const int button = 10;
const int transmitting = 8; //red
const int receiving = 6; //green

void setup() {
  // put your setup code here, to run once:
  pinMode(button, INPUT);
  pinMode(transmitting, OUTPUT);
  pinMode(receiving, OUTPUT);
  Serial.begin(9600);
  Serial2.begin(9600);
  delay(500);
  Serial.print('q');
}

void loop() {
  // put your main code here, to run repeatedly:
  int buttonState = digitalRead(button);
  if(buttonState == HIGH){
       digitalWrite(transmitting, HIGH);
       Serial2.print('n');
       delay(500);
       digitalWrite(transmitting, LOW);
  }

  if(Serial2.available()>0){
    digitalWrite(receiving,HIGH);
    Serial.print(char(Serial2.read()));
    delay(500);
    digitalWrite(receiving,LOW);
  }
  delay(50);
}
