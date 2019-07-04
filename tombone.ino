#include <Wire.h>
#include <Adafruit_MPR121.h>
#include <MIDI.h>
#include <Bounce.h>
#include <Encoder.h>

#define MIDI_CHANNEL (0x01)
#define NOTE_ON (0x90+MIDI_CHANNEL)
#define NOTE_OFF (0x80+MIDI_CHANNEL)
#define PITCH_BEND (0xE0+MIDI_CHANNEL)
#define NOTE_ON_VEL (127)  //0x7F
#define NOTE_OFF_VEL (0)  //0x7F

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

const int SOFT_POT_PIN = A14; // Pin connected to softpot wiper
uint16_t lasttouched = 0;
uint16_t currtouched = 0;
int lastPosition = 0;

Adafruit_MPR121 capa = Adafruit_MPR121();

void setup() 
{
  Serial.begin(9600);
  pinMode(SOFT_POT_PIN, INPUT);
  capa.begin(0x5A);
}

void loop() 
{
  // Read in the soft pot's ADC value
  int softPotADC = analogRead(SOFT_POT_PIN);
  // Map the 0-1023 value to 54-64
  int softPotPosition = map(softPotADC, 0, 1023, 54, 64);
  currtouched = capa.touched();

  Serial.println(softPotPosition);
  if ((currtouched & _BV(4)) && !(lasttouched & _BV(4)) ){
    Serial.println(softPotPosition + " on");
    usbMIDI.sendNoteOn(softPotPosition, NOTE_ON_VEL, MIDI_CHANNEL);
  }
  if (!(currtouched & _BV(4)) && (lasttouched & _BV(4)) ){
    usbMIDI.sendNoteOff(lastPosition, NOTE_OFF_VEL, MIDI_CHANNEL);
    Serial.println(lastPosition + " off");
  }

  lastPosition = softPotPosition;
  lasttouched = currtouched;
}
