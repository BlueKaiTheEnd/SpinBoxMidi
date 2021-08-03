#@Author: Demetrius Robinson (BlueKaiTheEnd)

#include <Adafruit_NeoPixel.h>
#include <Metro.h>
#include <Bounce2.h>
#include <Encoder.h>

#define btn0  0
#define btn1  1
#define btn2  2
#define btn3  3
#define btn4  4
#define btn5  5
#define rotA 7
#define rotB 8
#define led0 14
#define led1 15
#define led2 16
#define led3 17
#define ledRingPin 18

const int MIN_BRIGHTNESS = 0;
const int MAX_BRIGHTNESS = 200;
const int BRIGHTNESS_STEPS = 10;
const int MAX_LED_MODES = 3;
const long DEBOUNCE_DELAY = 5;
const int CHANNEL = 1;
const int NUMPIXELS = 60;
const int MIDI_LISTEN_CH = 4;
const int VELOCITY = 127;
const int MIDI_CH = 1;
int note[] = {36,37,38,39,40,41};
int ledBrightness = MAX_BRIGHTNESS;
int blueValue = 50;
int ledMode = 2;
int rotary_value = 0;
long rotary_pevious = -999;

boolean btn4State = false;
boolean btn5State = false;
boolean ledRingPassed = false;
boolean ledRingPulseDirection = 0;

Encoder rotary(rotB, rotA); //Pin B, Pin A
Metro ledRing = Metro(50);
Metro rotaryEncoder = Metro(5);
Metro buttonPress = Metro(3);
Adafruit_NeoPixel pixels(NUMPIXELS, ledRingPin, NEO_GRB + NEO_KHZ800);

Bounce btn_0 = Bounce(btn0, DEBOUNCE_DELAY);
Bounce btn_1 = Bounce(btn1, DEBOUNCE_DELAY);
Bounce btn_2 = Bounce(btn2, DEBOUNCE_DELAY);
Bounce btn_3 = Bounce(btn3, DEBOUNCE_DELAY);
Bounce btn_4 = Bounce(btn4, DEBOUNCE_DELAY);
Bounce btn_5 = Bounce(btn5, DEBOUNCE_DELAY);

void ledOffMode();
void ledStaticMode();
void ledPulseMode();
void rotaryEncoderEvent();
void checkButton45States();

void setup() {
  pinMode(btn0, INPUT_PULLUP);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(btn4, INPUT_PULLUP);
  pinMode(btn5, INPUT_PULLUP);

  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  Serial.begin(115200);
  usbMIDI.begin();
  pixels.begin();
}

void loop() {
//=============================================================================
//=============================== BUTTON EVENT ================================
//=============================================================================
  while (usbMIDI.read()) {}
  
  if(buttonPress.check()){
    btn_0.update();
    btn_1.update();
    btn_2.update();
    btn_3.update();
    btn_4.update();
    btn_5.update();

    if(btn_0.fallingEdge()) {
//      Serial.println("Button 0 Pressed");
      digitalWrite(led0, HIGH);
      usbMIDI.sendNoteOn(note[0], VELOCITY, MIDI_CH);
    }
    if(btn_0.risingEdge()) {
//      Serial.println("Button 0 Released");
      digitalWrite(led0, LOW);
      usbMIDI.sendNoteOff(note[0], VELOCITY, MIDI_CH);
    }
    if(btn_1.fallingEdge()) {
//       Serial.println("Button 1 Pressed");
      digitalWrite(led1, HIGH);
      usbMIDI.sendNoteOn(note[1], VELOCITY, MIDI_CH);
    }
    if(btn_1.risingEdge()) {
//       Serial.println("Button 1 Released");
      digitalWrite(led1, LOW);
      usbMIDI.sendNoteOff(note[1], VELOCITY, MIDI_CH);
    }
    if(btn_2.fallingEdge()) {
      // Serial.println("Button 2 Pressed");
      digitalWrite(led2, HIGH);
      usbMIDI.sendNoteOn(note[2], VELOCITY, MIDI_CH);
    }
    if(btn_2.risingEdge()) {
      // Serial.println("Button 2 Released");
      digitalWrite(led2, LOW);
      usbMIDI.sendNoteOff(note[2], VELOCITY, MIDI_CH);
    }
    if(btn_3.fallingEdge()) {
      // Serial.println("Button 3 Pressed");
      digitalWrite(led3, HIGH);
      usbMIDI.sendNoteOn(note[3], VELOCITY, MIDI_CH);
    }
    if(btn_3.risingEdge()) {
      // Serial.println("Button 3 Released");
      digitalWrite(led3, LOW);
      usbMIDI.sendNoteOff(note[3], VELOCITY, MIDI_CH);
    }

    
    if(btn_4.fallingEdge()) {
      btn4State = true;
      checkButton45States();
      // Serial.println("Button 4 Pressed");
      usbMIDI.sendNoteOn(note[4], VELOCITY, MIDI_CH);
    }
    if(btn_4.risingEdge()) {
      btn4State = false;
      // Serial.println("Button 4 Released");
      usbMIDI.sendNoteOff(note[4], VELOCITY, MIDI_CH);
    }
    if(btn_5.fallingEdge()) {
      btn5State = true;
      // Serial.println("Button 5 Pressed");
      checkButton45States();
      usbMIDI.sendNoteOn(note[5], VELOCITY, MIDI_CH);
    }
    if(btn_5.risingEdge()) {
      btn5State = false;
      // Serial.println("Button 5 Released");
      usbMIDI.sendNoteOff(note[5], VELOCITY, MIDI_CH);
    }
  }

//=============================================================================
//============================== ROTARTY EVENT ================================
//=============================================================================
 if(rotaryEncoder.check()){
  rotaryEncoderEvent();
 }

  
//=============================================================================
//=============================== LED RING EVENT ==============================
//=============================================================================
  if(ledRing.check()){
    switch(ledMode) {
      case 0:
        ledOffMode();
        break;
      case 1:
        ledStaticMode();
        break;
      case 2:
        ledPulseMode();
        break;
      default:
        ledStaticMode();
        break;
    }
    
    
  }
}

//=============================================================================
//=============================== LED RING Modes ==============================
//=============================================================================
void ledOffMode() {
    pixels.fill(pixels.Color(0,0,blueValue), 0, 60);
    pixels.setBrightness(0);
    pixels.show();
}

void ledStaticMode() {
    pixels.fill(pixels.Color(0,0,blueValue), 0, 60);
    pixels.setBrightness(ledBrightness);
    pixels.show();
}

void ledPulseMode() {
    if (ledRingPulseDirection == 0) {
      pixels.fill(pixels.Color(0,0,blueValue), 0, 60);
      pixels.setBrightness(ledBrightness);
      pixels.show();
      ledBrightness -= BRIGHTNESS_STEPS;
      if (ledBrightness <= MIN_BRIGHTNESS) {ledRingPulseDirection = 1; ledBrightness = MIN_BRIGHTNESS;}
    }
    if (ledRingPulseDirection == 1) {
      pixels.fill(pixels.Color(0,0,blueValue), 0, 60);
      pixels.setBrightness(ledBrightness);
      pixels.show();
      ledBrightness += BRIGHTNESS_STEPS;
      if (ledBrightness >= MAX_BRIGHTNESS) {ledRingPulseDirection = 0; ledBrightness = MAX_BRIGHTNESS;}
    }
}

//=============================================================================
//================================ Buttons ====================================
//=============================================================================
void checkButton45States() {
  if (btn4State & btn5State) {
    ledMode++;
    if (ledMode > MAX_LED_MODES - 1) {ledMode=0;}
  }
}

//=============================================================================
//=============================== Rotary Encoder ==============================
//=============================================================================
void rotaryEncoderEvent() {
  rotary_value = rotary.read();
  if (rotary_value > 1) {
    rotary.write(1);
    usbMIDI.sendControlChange(MIDI_LISTEN_CH, rotary_value, MIDI_CH);
    Serial.print("CW: ");
    Serial.println(rotary_value);
  }
  else if (rotary_value < 0) {
    rotary.write(0);
    usbMIDI.sendControlChange(MIDI_LISTEN_CH, rotary_value, MIDI_CH);
    Serial.print("CCW: ");
    Serial.println(rotary_value);
  }

  rotary_value = constrain(rotary_value, 0, 1);
}
