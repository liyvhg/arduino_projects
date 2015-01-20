#include <Bounce2.h>
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

#define CLIP_COUNT 12
#define UNCONNECTED_PIN 20
#define HOOK_PIN 0
#define RING_PIN 1
#define DTMF_VOLUME 0.5
#define OUTPUT_VOLUME 0.7
#define RING_VOLUME 1.0

//In "Homer the Smithers", Mr. Burns attempting to reach Smithers, accidentally dials the telephone number for Moe's Tavern. The number is revealed as 7648-4377 which spells out (S-M-I-T-H-E-R-S).
#define SMITHERS "76484377"

//http://simpsons.wikia.com/wiki/Telephone_numbers_in_The_Simpsons
#define MOES "5551239"

// change this to match your SD shield or module;
// Teensy 3.0: pin 10
// Audio Shield for Teensy 3.0: pin 10
const int chipSelect = 10;

Bounce hook = Bounce();
Bounce ringer = Bounce();

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
AudioPlaySdWav     wav;
AudioOutputI2S     audioOut;

//DTMF support
AudioSynthWaveform       sine1;  // 2 sine wave
AudioSynthWaveform       sine2;  // to create DTMF

AudioMixer4              mixer; //Reminder: mixer has only output

// Create Audio connections between the components
AudioConnection c10(sine1, 0, mixer, 0);
AudioConnection c11(sine2, 0, mixer, 1);

AudioConnection c1(wav, 0, mixer, 2);
AudioConnection c2(wav, 1, mixer, 3);

AudioConnection c12(mixer, 0, audioOut, 0);
AudioConnection c13(mixer, 0, audioOut, 1);

// Create an object to control the audio shield.
AudioControlSGTL5000 audioShield;

unsigned int i = 0;

String s(int state) {
  return state == HIGH ? "HIGH" : "LOW";
}

String t(boolean state) {
  return state ? "True" : "False";
}

void setup(){
  Serial.begin(9600);

  pinMode(HOOK_PIN, INPUT_PULLUP);
  hook.attach(HOOK_PIN);
  hook.interval(5);
  randomSeed(analogRead(UNCONNECTED_PIN));
  pinMode(RING_PIN, INPUT); //Held low by Simple RF M4 receiver
  ringer.attach(RING_PIN);
  ringer.interval(5);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  audioShield.enable();
  audioShield.volume(OUTPUT_VOLUME);
  mixer.gain(2, DTMF_VOLUME);//Quieter DTMF
  mixer.gain(3, DTMF_VOLUME);//Quieter DTMF

  SPI.setMOSI(7);
  SPI.setSCK(14);
  if (SD.begin(chipSelect)) {
    wav.play("ring.wav");//1 min clip of phone ringing
    delay(1000);//play through one ring
    wav.stop();
  } else {
    Serial.println("SD.begin() failure");
  }
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();
}

void loop(){
  boolean hookChanged = hook.update();
  int hookState = hook.read();
  boolean ringerChanged = ringer.update();
  int ringState = ringer.read();
  boolean isPlaying = wav.isPlaying();

  if (hookChanged) {
    Serial.println("New state: " + s(hookState));
  }

  if (ringerChanged) {
    Serial.println("New ringer state: " + s(ringState));
    if (ringState == HIGH && hookState == HIGH && !isPlaying) { //ringer keyfob clicked
      audioShield.volume(RING_VOLUME);
      wav.play("ring.wav");//1 min clip of phone ringing
      delay(3000);//play through one ring
      wav.stop();
      audioShield.volume(OUTPUT_VOLUME);
    }
  }

  if ( hookChanged && hookState == LOW && !isPlaying) { //Picking up handset
    dialtone(1000);
    dial(MOES);

    char filename[7];
    unsigned int randNumber = random(1, CLIP_COUNT);
    sprintf(filename, "%02i.wav", randNumber);

    Serial.println("wav.play(" + String(filename) + ")");
    wav.play(filename);
    print_stats();
  } else if (hookChanged && hookState == HIGH) {
    Serial.println("wav.stop()");
    wav.stop();
  }

  //Serial.println("End of loop: " + String(i++));
}


void print_progress() {
  if (wav.isPlaying()) {
    Serial.print("Playing wav: ");
    Serial.print(wav.positionMillis());
    Serial.print("ms of ");
    Serial.print(wav.lengthMillis());
    Serial.print("ms");
    Serial.println();
  }
}

void print_stats() {
    Serial.print("Processor: ");
    Serial.print(AudioProcessorUsage());
    Serial.print(" Processor(max): ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" Memory: ");
    Serial.print(AudioMemoryUsage());
    Serial.print(" Memory(max): ");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
}

void dialtone(int milliseconds) {
    // Dial tone: 350 and 440
    AudioNoInterrupts();  // disable audio library momentarily
    sine1.begin(0.4, 440, TONE_TYPE_SINE);
    sine2.begin(0.45, 330, TONE_TYPE_SINE);
    AudioInterrupts();    // enable, both tones will start together
    delay(milliseconds);           // let the sound play for 0.1 second
    AudioNoInterrupts();
    sine1.amplitude(0);
    sine2.amplitude(0);
    AudioInterrupts();
}

int low_key_freq(int key) {
  int low;
  switch(key) {
    case '1':
    case '2':
    case '3':
      low = 697;
      break;
    case '4':
    case '5':
    case '6':
      low = 770;
      break;
    case '7':
    case '8':
    case '9':
      low = 852;
      break;
    case '*':
    case '0':
    case '#':
      low = 941;
      break;
  }
  return low;
}

int high_key_freq(int key) {
  int high;
  switch(key) {
    case '1':
    case '4':
    case '7':
    case '*':
      high = 1209;
      break;
    case '2':
    case '5':
    case '8':
    case '0':
      high = 1336;
      break;
    case '3':
    case '6':
    case '9':
    case '#':
      high = 1477;
      break;
  }
  return high;
}

void play_key(int key) {
  int low = low_key_freq(key);
  int high = high_key_freq(key);
  if (low > 0 && high > 0) {
    Serial.print("Output sound for key ");
    Serial.print(key);
    Serial.print(", low freq=");
    Serial.print(low);
    Serial.print(", high freq=");
    Serial.print(high);
    Serial.println();

    AudioNoInterrupts();  // disable audio library momentarily
    sine1.begin(0.4, low, TONE_TYPE_SINE);
    sine2.begin(0.45, high, TONE_TYPE_SINE);
    AudioInterrupts();    // enable, both tones will start together
    delay(100);           // let the sound play for 0.1 second
    AudioNoInterrupts();
    sine1.amplitude(0);
    sine2.amplitude(0);
    AudioInterrupts();
    delay(50);            // make sure we have 0.05 second silence after
  }
}

void dial(String phonenumber) {
  int length = phonenumber.length();
  for (int i = 0; i < length; i++) {
    play_key(phonenumber.charAt(i));
  }
}

