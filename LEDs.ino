#include "funshield.h"

constexpr int led[] {led1_pin, led2_pin, led3_pin, led4_pin};
constexpr int led_count = sizeof(led)/sizeof(led[0]);
unsigned long last_time = 0;
unsigned int last_pin = led1_pin;
bool up = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(led3_pin, OUTPUT);
  pinMode(led4_pin, OUTPUT);
  
}

void turn_led(int my_led){
  digitalWrite(my_led, LOW);
  for (int i = 0; i < led_count; ++i){
    if (led[i] != my_led){
      digitalWrite(led[i], HIGH);
    }
  }
}

void bounce(int interval){
  auto cur_time = millis();
  if (cur_time >= last_time + 300){
    last_time = cur_time;
    if (!up){
      turn_led(last_pin--);
      if (last_pin < led4_pin){
        up = true;
        ++++last_pin;
      }
    }
    else{
      turn_led(last_pin++);
      if (last_pin > led1_pin){
        up = false;
        ----last_pin;
      }
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  bounce(300);  
  }
