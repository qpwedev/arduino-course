#include "funshield.h"

constexpr int digit_positions = 4;

void writeGlyphBitmask( byte glyph, byte pos_bitmask) {
  digitalWrite( latch_pin, LOW);
  shiftOut( data_pin, clock_pin, MSBFIRST, glyph);
  shiftOut( data_pin, clock_pin, MSBFIRST, pos_bitmask);
  digitalWrite( latch_pin, HIGH);digitalWrite( latch_pin, HIGH);
}

void writeGlyphR(byte glyph, int pos){
  writeGlyphBitmask(glyph, digit_muxpos[digit_positions - pos - 1]);
}

void writeGlyphL(byte glyph, int pos){
  writeGlyphBitmask(glyph, digit_muxpos[pos]);
}

void writeDigit(int n, int pos){
  writeGlyphR(digits[n], pos);
}

enum LedState {PRESSED, RELEASED};

class ButtonPresser{
  public:
    ButtonPresser(int pin) {
      pin_ = pin;
    }
    bool pressedOnce(bool button_pressed)
    {
      if (button_pressed == false) {
        prev_state_ = RELEASED;
        return false;
      }

      if (prev_state_ == PRESSED && button_pressed)
        return false;

      prev_state_ = PRESSED;
      return true;
    }
  private:
    LedState prev_state_ = RELEASED;
    int pin_;
};

int extract_number(int number, int pos){
  int arr[] = {0,0,0,0};
  int i = 0; int r;
  while (number != 0) {
      r = number % 10;
      arr[i] = r;
      i++;
      number = number / 10;
  }
  return arr[pos];
}


constexpr int buttons[] = { button1_pin, button2_pin, button3_pin };
ButtonPresser pressers[] { ButtonPresser(button1_pin), ButtonPresser(button2_pin), ButtonPresser(button3_pin) };
int number = 0;
int position = 0;

void setup() {
  pinMode(latch_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  for (auto button : buttons) {
    pinMode(button, INPUT);
  }
}

void loop() {
  
  if ( pressers[0].pressedOnce(!digitalRead(buttons[0])) ) {
    if (position == 0) number += 1;
    else if (position == 1) number += 10;
    else if (position == 2) number += 100;
    else if (position == 3) number += 1000;
  }

  if ( pressers[1].pressedOnce(!digitalRead(buttons[1]))) {
    if (position == 0) number -= 1;
    else if (position == 1) number -= 10;
    else if (position == 2) number -= 100;
    else if (position == 3) number -= 1000;
  }

  if (pressers[2].pressedOnce(!digitalRead(buttons[2])))
    position +=1;

  if (position < 0) position = 3;
  else if (position > 3) position = 0;
  if (number < 0) number += 10000;
  else if (number > 9999) number -= 10000;


  writeDigit(extract_number(number,position), position);
}
