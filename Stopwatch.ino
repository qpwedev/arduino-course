#include "funshield.h"

constexpr int digit_positions = sizeof(digit_muxpos) / sizeof(digit_muxpos[0]);

void writeGlyphBitmask( byte glyph, byte pos_bitmask) {
  digitalWrite( latch_pin, LOW);
  shiftOut( data_pin, clock_pin, MSBFIRST, glyph);
  shiftOut( data_pin, clock_pin, MSBFIRST, pos_bitmask);
  digitalWrite( latch_pin, HIGH);
}

void writeGlyphR(byte glyph, int pos) { writeGlyphBitmask(glyph, digit_muxpos[digit_positions - pos - 1]); }
void writeGlyphL(byte glyph, int pos) { writeGlyphBitmask(glyph, digit_muxpos[pos]); }
void writeDigit(int n, int pos, bool dot, bool nulll) { 
  if (nulll) { writeGlyphR(0b11111111, pos); } 
  else if (dot){ writeGlyphR(digits[n] & 0b01111111, pos); }
  else { writeGlyphR(digits[n], pos);}
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

int mocniny[] { 1, 10, 100, 1000 };

class Display{
  public:
    void set(int cislo){ cislo_ = cislo; }
    void reset(){ cislo_ = 0; pozicia_ = 0; active_num_ = 2; }
    void set_active_numbers(int active_num) { active_num_ = (active_num + 5) % 5; }
    
    void loop()
    {
      int cislica = (cislo_ / mocniny[pozicia_]) % 10;
      if (pozicia_ >= active_num_) { writeDigit(cislica, pozicia_, true, true); }
      else if (pozicia_ == 1) { writeDigit(cislica, pozicia_, true, false); }
      else { writeDigit(cislica, pozicia_, false, false); }
      pozicia_++;
      pozicia_ %= 4;
    }


    int count_digits(long number){
      int count = 0;
      
      while (number != 0) {
          number /= 10;
          ++count;
      }
      return count;
    }
    
  private:
    int cislo_;
    int pozicia_ = 0;
    int active_num_ = 2;
};


constexpr int buttons[] = { button1_pin, button2_pin, button3_pin};
ButtonPresser pressers[] { ButtonPresser(button1_pin), ButtonPresser(button2_pin), ButtonPresser(button3_pin)};

Display d;

void setup() {
  pinMode(latch_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  for (auto button : buttons) {
    pinMode(button, INPUT);
  }
}


int citac = 0;
unsigned long start_time = 0;
unsigned long stop_time = 0;
unsigned long difference_time = 0;
unsigned long actual_time = 0;
bool start_ = false;
bool loop_ = false;

void loop() {
  
  if (start_){ actual_time = (millis()-difference_time)/100; }

  if ( pressers[0].pressedOnce(!digitalRead(buttons[0]))){
    if (!start_){
      start_time = millis();
      difference_time += (start_time - stop_time);
      start_ = true;
    }
    else{
      stop_time = millis();
      start_ = false;
    }
  }

  if (pressers[1].pressedOnce(!digitalRead(buttons[1]))){
    if (start_){
      if (!loop_){ loop_ = true; }
      else { loop_ = false; }
    }
  }
  
  if (pressers[2].pressedOnce(!digitalRead(buttons[2]))){
    if (!start_){ 
      difference_time = 0; 
      start_time = 0;
      actual_time = 0;
      stop_time = 0;
      d.reset();
    }
  }

  if (d.count_digits(actual_time) > 2 && !loop_){ d.set_active_numbers(d.count_digits(actual_time)); }
  if (!loop_) { d.set(actual_time); }
  d.loop();
}
