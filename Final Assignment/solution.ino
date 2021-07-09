#include "funshield.h"

constexpr byte LETTER_GLYPH[]{
    0b10001000,  // A
    0b10000011,  // b
    0b11000110,  // C
    0b10100001,  // d
    0b10000110,  // E
    0b10001110,  // F
    0b10000010,  // G
    0b10001001,  // H
    0b11111001,  // I
    0b11100001,  // J
    0b10000101,  // K
    0b11000111,  // L
    0b11001000,  // M
    0b10101011,  // n
    0b10100011,  // o
    0b10001100,  // P
    0b10011000,  // q
    0b10101111,  // r
    0b10010010,  // S
    0b10000111,  // t
    0b11000001,  // U
    0b11100011,  // v
    0b10000001,  // W
    0b10110110,  // ksi
    0b10010001,  // Y
    0b10100100,  // Z
};

enum States { RANDOM, GENERATE, CONFIG };
States state = RANDOM;
enum LedState { PRESSED, RELEASED };

char actual_frame[4];
char blank_frame[4] = {'-', '-', '-', '-'};
char number_frame[4] = {'H', 'E', 'L', 'L'};
char config_frame[4] = {'1', 'd', '4', '-'};
long last_change = -200;
int throws = 1;
int dice = 0;

class ButtonPresser {
 public:
  ButtonPresser(int pin) { pin_ = pin; }
  bool pressedOnce(bool button_pressed) {
    if (button_pressed == false) {
      prev_state_ = RELEASED;
      return false;
    }
    if (prev_state_ == PRESSED && button_pressed) return false;
    prev_state_ = PRESSED;
    return true;
  }

  int current_state() { return prev_state_; }

 private:
  LedState prev_state_ = RELEASED;
  int pin_;
};

constexpr int buttons[] = {button1_pin, button2_pin, button3_pin};
ButtonPresser pressers[]{ButtonPresser(button1_pin), ButtonPresser(button2_pin),
                         ButtonPresser(button3_pin)};

class Display {
 public:
  void display_actual_frame() {
    char c = actual_frame[position];
    display_char(c, position);
    position++;
    position %= 4;
  }

  void change_dice_frame() {
    char first_num[7] = {'4', '6', '8', '1', '1', '2', '0'};
    char second_num[7] = {'-', '-', '-', '0', '2', '0', '0'};
    config_frame[2] = first_num[dice];
    config_frame[3] = second_num[dice];
  }

  int from_dice_to_num() {
    int configurations[7] = {4, 6, 8, 10, 12, 20, 100};
    return configurations[dice];
  }

  void change_actual_frame(char frame[]) {
    for (int i = 0; i < 4; ++i) actual_frame[i] = frame[i];
  }
  void change_number_frame(char frame[]) {
    for (int i = 0; i < 4; ++i) number_frame[i] = frame[i];
  }

  void gen_and_display() {
    int num = 0;
    int aux_num = 0;

    //for 2d4 Counter({5: 766, 4: 581, 6: 553, 3: 360, 7: 339, 2: 194, 8: 174}) 

    for (int i = 0; i < throws; ++i) {
      do {
        aux_num =millis() * random(100, 1000) % (from_dice_to_num()+1);
      } while (aux_num < 1 || aux_num > from_dice_to_num());
      num += aux_num;
    }
    
    char str[4] = {'-', '-', '-', '-'};
    change_number_frame(blank_frame);
    sprintf(str, "%d", num);
    int n = log10(num);
    int counter = 0;
    for (int i = 3 - n; i <= 3; ++i) {
      number_frame[i] = str[counter++];
    }
  }

  void display_char(char ch, byte pos) {
    byte glyph;
    if (isAlpha(ch)) {
      glyph = LETTER_GLYPH[ch - (isUpperCase(ch) ? 'A' : 'a')];
    } else if (isDigit(ch))
      glyph = digits[ch - '0'];
    else
      glyph = 0b11111111;
    digitalWrite(latch_pin, LOW);
    shiftOut(data_pin, clock_pin, MSBFIRST, glyph);
    shiftOut(data_pin, clock_pin, MSBFIRST, 1 << pos);
    digitalWrite(latch_pin, HIGH);
  }

  void process_butttons() {
    long time_ = millis();
    if (state == RANDOM) {
      if (pressers[0].current_state() == PRESSED && time_ - last_change > 200) {
        gen_and_display();
      }
      change_actual_frame(number_frame);
    }
    if (state == CONFIG && pressers[0].current_state() != PRESSED) {
      change_actual_frame(config_frame);
    }

    if (pressers[0].pressedOnce(!digitalRead(buttons[0]))) {
      if (state == CONFIG) {
        state = RANDOM;
        last_change = millis();
      }
    }

    if (pressers[1].pressedOnce(!digitalRead(buttons[1])) &&
        pressers[0].current_state() != PRESSED) {
      if (state == CONFIG) {
        throws = throws < 9 ? throws + 1 : 1;
        config_frame[0] = throws + '0';
      }
      if (state == RANDOM) state = CONFIG;
    }

    if (pressers[2].pressedOnce(!digitalRead(buttons[2])) &&
        pressers[0].current_state() != PRESSED) {
      if (state == CONFIG) {
        dice = dice < 6 ? dice + 1 : 0;
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
  randomSeed(random(0, 10));
}

void loop() {
  d.process_butttons();
  d.display_actual_frame();
}
