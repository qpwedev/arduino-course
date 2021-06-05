#include "funshield.h"

enum States {RANDOM, GENERATE, CONFIG};
States state = RANDOM;
enum LedState {PRESSED, RELEASED};

char actual_frame[4];
char blank_frame[4] = {'-','-','-','-'};
char number_frame[4] = {'H','E','L','L'};
char config_frame[4] = {'1','d','4','-'};
long last_change = -200;
int throws = 1;
int dice = 0;

class ButtonPresser{
  public:
    ButtonPresser(int pin) { pin_ = pin; }
    bool pressedOnce(bool button_pressed)
    {
      if (button_pressed == false) {
        prev_state_ = RELEASED;
        return false;
      }
      if (prev_state_ == PRESSED && button_pressed) return false;
      prev_state_ = PRESSED;
      return true;
    }

    int current_state(){ return prev_state_; }
    
    LedState prev_state_ = RELEASED;
    int pin_;
};

constexpr int buttons[] = { button1_pin, button2_pin, button3_pin };
ButtonPresser pressers[] { ButtonPresser(button1_pin), ButtonPresser(button2_pin), ButtonPresser(button3_pin) };

class Display{
  public:
    void display_actual_frame(){
      char c = actual_frame[position];
      display_char(c, position);
      position++;
      position %= 4;
    }

    void change_dice_frame(){
      switch (dice){
         case 0: config_frame[2] = '4'; config_frame[3] = '-'; break;
         case 1: config_frame[2] = '6'; config_frame[3] = '-'; break;
         case 2: config_frame[2] = '8'; config_frame[3] = '-'; break;
         case 3: config_frame[2] = '1'; config_frame[3] = '0'; break;
         case 4: config_frame[2] = '1'; config_frame[3] = '2'; break;
         case 5: config_frame[2] = '2'; config_frame[3] = '0'; break;
         case 6: config_frame[2] = '0'; config_frame[3] = '0'; break;
      } 
    }

    int from_dice_to_num(){
      switch (dice)
      {
        case 0: return 4;;
        case 1: return 6;;
        case 2: return 8;;
        case 3: return 10;;
        case 4: return 12;;
        case 5: return 20;;
        case 6: return 100;;
      } 
    }
      
    void change_actual_frame(char frame[]){ for (int i = 0; i < 4; ++i) actual_frame[i] = frame[i]; }
    void change_number_frame(char frame[]){ for (int i = 0; i < 4; ++i) number_frame[i] = frame[i]; }

    void gen_and_display(){
      int num;
  
      do{ num = millis()*random(333,3333)  % (throws*(from_dice_to_num())) + throws; }
      while (num < throws || num > throws*from_dice_to_num());
      Serial.print(num);
      Serial.print('\n');
      char str[4] = {'-','-','-','-'};
      change_number_frame(blank_frame);
      sprintf(str, "%d", num);
      int n = log10(num);
      int counter = 0;
      for (int i = 3-n; i <= 3; ++i){
        number_frame[i] = str[counter++];
      }
    }

    void display_char(char ch, byte pos){
      byte glyph;
      if (isAlpha(ch)) {
          switch (ch){
              case 'd': glyph = 0b10100001; break;
              case 'H': glyph = 0b10001001; break;
              case 'E': glyph = 0b10000110; break;
              case 'L': glyph = 0b11000111; break;
          } 
      }
      else if (isDigit(ch)) glyph = digits[ch - '0'];
      else glyph = 0b11111111;
      digitalWrite(latch_pin, LOW);
      shiftOut(data_pin, clock_pin, MSBFIRST, glyph);
      shiftOut(data_pin, clock_pin, MSBFIRST, 1 << pos);
      digitalWrite(latch_pin, HIGH);
      }

    void process_butttons(){
      long time_ = millis();
      if (state == RANDOM){
      if (pressers[0].prev_state_ == PRESSED && time_ - last_change > 200){ gen_and_display(); }
          change_actual_frame(number_frame);
      }
      if (state == CONFIG && pressers[0].prev_state_ != PRESSED ){ change_actual_frame(config_frame); }

      if ( pressers[0].pressedOnce(!digitalRead(buttons[0])) ) {
        if (state == CONFIG) {state = RANDOM; last_change = millis(); }
      }
      
      if ( pressers[1].pressedOnce(!digitalRead(buttons[1])) && pressers[0].prev_state_ != PRESSED ) {
        if (state == CONFIG){
          throws = throws < 9 ? throws+1 : 1;
          config_frame[0] = throws + '0';
        }
        if (state == RANDOM) state = CONFIG;
      }

      if ( pressers[2].pressedOnce(!digitalRead(buttons[2])) && pressers[0].prev_state_ != PRESSED ) {
        if ( state == CONFIG){
          dice = dice < 6 ? dice+1 : 0;
          change_dice_frame();
        }
        if (state == RANDOM) state = CONFIG;
      }
    }

  private:
    int position = 0;
};

Display d;

void setup() {
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
  for (auto button : buttons) pinMode(button, INPUT);
  randomSeed(random(0,10));
}

void loop() {
  d.process_butttons();
  d.display_actual_frame(); 
}
