const int TxPin = 6;
#include <SoftwareSerial.h>
SoftwareSerial mySerial = SoftwareSerial(255, TxPin);
void setup() {
  // put your setup code here, to run once:
  pinMode(TxPin, OUTPUT);
  digitalWrite(TxPin, HIGH);
  Serial3.begin(9600);
  delay(100);
  Serial3.write(12);
  Serial3.write(17);
  delay(5);
  Serial3.print("Hello, world...");
  Serial3.write(13);
  Serial3.print("from Parallax!");
  Serial3.write(212);
  Serial3.write(220);
  delay(3000);
  Serial3.write(18);
}

void loop() {
  // put your main code here, to run repeatedly:

}
