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
int bendy = 0;
double noteResists[8] = {0, 148, 293, 440, 587, 730, 880, 1020};
const int bendDefault = 8192;

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
  // Map the 0-1023 value to 58-50
  int softPotPosition = map(softPotADC, 0, 1023, 58, 50);

  bendy = pitchBend(softPotADC);

  currtouched = capa.touched();
  if ((currtouched & _BV(4)) && !(lasttouched & _BV(4)) ){
    Serial.print(softPotPosition);
    Serial.print(" on, bend: ");
    Serial.println(bendy);
    usbMIDI.sendNoteOn(softPotPosition, NOTE_ON_VEL, MIDI_CHANNEL);
    usbMIDI.sendPitchBend(bendy, MIDI_CHANNEL);
  }
  if (!(currtouched & _BV(4)) && (lasttouched & _BV(4)) ){
    usbMIDI.sendNoteOff(lastPosition, NOTE_OFF_VEL, MIDI_CHANNEL);
    Serial.print(lastPosition);
    Serial.println(" off");
  }

  lastPosition = softPotPosition;
  lasttouched = currtouched;
}

int pitchBend(int softPotADC){
  int bend = 8192;
  int mod = 10000;
  if(softPotADC < noteResists[1]){
    bend = (((softPotADC) / noteResists[1]) * mod) + bendDefault;
  }else if(softPotADC < noteResists[2]){
    bend = (((softPotADC) / noteResists[2]) * mod) + bendDefault;
  }else if(softPotADC < noteResists[3]){
    bend = (((softPotADC) / noteResists[3]) * mod) + bendDefault;
  }else if(softPotADC < noteResists[4]){
    bend = (((softPotADC) / noteResists[4]) * mod) + bendDefault;
  }else if(softPotADC < noteResists[5]){
    bend = (((softPotADC) / noteResists[5]) * mod) + bendDefault;
  }else if(softPotADC < noteResists[6]){
    bend = (((softPotADC) / noteResists[6]) * mod) + bendDefault;
  }else{
    bend = (((softPotADC) / noteResists[7]) * mod) + bendDefault;
  }
  return(bend);
}

