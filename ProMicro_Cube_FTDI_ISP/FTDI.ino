#include <Arduino.h>

#define DTR            7
#define LIGHT          9  
#define LINESTATE_DTR  1

static long baud = 57600;
static long newBaud = baud;


void lineCodingEvent(long baud, byte databits, byte parity, byte charFormat)
{
  newBaud = baud;
}

void lineStateEvent(unsigned char linestate)
{
  if(linestate & LINESTATE_DTR)
    digitalWrite(DTR, HIGH);
  else
    digitalWrite(DTR, LOW);
}


void setupSerial() {
  pinMode(DTR, OUTPUT);
  digitalWrite(DTR, LOW);
  pinMode(LIGHT, OUTPUT);
  analogWrite(LIGHT, 50);
  Serial.begin(baud);
 
}

void loopSerial() {

  // Set the new baud rate
  if(newBaud != baud) {
    baud = newBaud;
    Serial1.end();
    Serial1.begin(baud);
  }

  // copy from virtual serial line to uart and vice versa
  if (Serial.available()) {
    char c = (char)Serial.read();
    Serial1.write(c);
  }
  if (Serial1.available()) {
    char c = (char)Serial1.read();
    Serial.write(c);
  }
}


int mainFTDI(void)
{
  setupSerial();
      
  for (;;)
  {
    loopSerial();
    if (serialEventRun) serialEventRun();
  }
  return 0;
}
