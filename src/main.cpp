#include <Arduino.h>
#include "avr/interrupt.h"
#include <Wire.h>

#define I2C_ADDRESS 0x10

volatile int value = 0;
volatile int lastEncoded = 0;

volatile int zeroValue = 0;

void writeEncoderData();
void resetEncoder(int bytes);

void setup() {
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);

  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(writeEncoderData);
  Wire.onReceive(resetEncoder);

  GIMSK = 0b00100000;       // Enable pin change interrupts
  PCMSK = 0b00011000;       // Enable pin change interrupt for PB3 and PB4
  sei();                    // Turn on interrupts
}

void loop() {
}

void resetEncoder(int bytes) {
  char message[bytes];
  int i = 0;
  while (1 < Wire.available()) {
    message[i] = Wire.read();
  }

  if (message[0] == 'R') {
    zeroValue = value;
  }
}
 
void writeEncoderData() {
  cli();
  char message[4];
  message[3] = (value >> 24) & 0xFF;
  message[2] = (value >> 16) & 0xFF;
  message[1] = (value >> 8) & 0xFF;
  message[0] = value & 0xFF;  

  Wire.write(message);

  sei();
}

ISR(PCINT0_vect) {
  int MSB = digitalRead(3); //MSB = most significant bit
  int LSB = digitalRead(4); //LSB = least significant bit
 
  int encoded = (MSB << 1) |LSB;            //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded;  //adding it to the previous encoded value
 
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    value++;
  }

  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    value--;
  }

  value -= zeroValue;

  lastEncoded = encoded; //store this value for next time
}

