#include <Arduino.h>
#include "avr/interrupt.h"
#include <Wire.h>
#include <Encoder.h>

#define I2C_ADDRESS 0x10

volatile long value = 0;
volatile int lastEncoded = 0;

volatile int zeroValue = 0;

Encoder encoder(3, 4);

void writeEncoderData();
void resetEncoder(int bytes);

void setup() {
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
  union longToBytes {
    long value;
    char buffer[4];
  } converter;

  converter.value = encoder.read();

  Wire.write(converter.buffer);
}
