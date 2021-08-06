#include "funshield.h"

constexpr int leds[] = { led1_pin, led2_pin, led3_pin, led4_pin };
constexpr int ledCount = sizeof(leds) / sizeof(leds[0]);
constexpr int buttons[] = { button1_pin, button2_pin, button3_pin };
constexpr int buttonCount = sizeof(buttons) / sizeof(buttons[0]);

unsigned long prevTime;
int number;

void setup() {
  prevTime = millis();
  number = 0;
  for (auto led : leds) {
    digitalWrite(led, OFF);
    pinMode(led, OUTPUT);
    
  }
  for (auto button : buttons) {
    pinMode(button, INPUT);
  }
}

class Button
{
  public:
    enum InnerStates { NOT_BEING_PRESSED, BEING_PRESSED };
    
    Button(int increment) {
      increment_ = increment;
    }

    void react(bool pressed){
      unsigned long actualTime = millis();
      if (pressed && state_ == BEING_PRESSED) {
        if (actualTime-pressedInTime >= 1000 && actualTime-tik >= 300){
          tik = actualTime;
          increment(actualTime);
        }
        return;
      }
        
      else if (pressed && state_ == NOT_BEING_PRESSED) {
        pressedInTime = actualTime;
        state_ = BEING_PRESSED;
        increment(actualTime);
      }
      else state_ = NOT_BEING_PRESSED; 
    }

    void increment(unsigned long actualTime){
      if (actualTime - prevTime > 0){
          if (increment_){
            if (number < 15) number++;
            else number = 0;
          }
          if (!increment_){
            if (number > 0) number--;
            else number = 15;
          }
          showNumber(number);
          prevTime = actualTime;
        }
    }

    void showNumber(int number){
      int k, c;
      int i = 0;
      char numbers[] { '0','0','0','0'};
  
      for (c = 3; c >= 0; c--)
      {
          k = number >> c;
          if (k & 1) numbers[i]='1';
          i++;
      }
      int pin = 13;
      for (int i = 0; i < 4; i++){
          digitalWrite(pin--, numbers[i] == '1' ? ON : OFF);
      }
    }

  public:
    InnerStates state_ = NOT_BEING_PRESSED;
    unsigned long pressedInTime;
    unsigned long tik = 0;
    int increment_;
};

Button startButtons[] { Button(1), Button(0) };

void loop() {
  for (int i = 0; i < buttonCount; i++) {
    bool pressed = !digitalRead(buttons[i]);
    startButtons[i].react(pressed);
  }
}
