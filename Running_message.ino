#include "funshield.h"
#include "input.h"

constexpr byte LETTER_GLYPH[] {
0b10001000, 0b10000011,0b11000110,0b10100001,0b10000110,
0b10001110,0b10000010,0b10001001,0b11111001,0b11100001,
0b10000101,0b11000111,0b11001000,0b10101011,0b10100011,
0b10001100,0b10011000,0b10101111,0b10010010,0b10000111,
0b11000001,0b11100011,0b10000001,0b10110110,0b10010001,
0b10100100,};
constexpr byte EMPTY_GLYPH = 0xFF;   
constexpr int pos_count = 4;
constexpr long scrollingInterval = 300000;
SerialInputHandler input;

class Display
{
  public:
    void modify_cur_message_pointer(){
        message_pointer++;
        if (*message_pointer == '\0') last_index--;
    }

    byte char_to_glyph(char ch){
        byte glyph = EMPTY_GLYPH;
        if (isAlpha(ch))
            glyph = LETTER_GLYPH[ ch - (isUpperCase(ch) ? 'A' : 'a') ];
        return glyph;
    }

    void dispay_now(){
        for (int i = 1; i < pos_count; i++)
            display_info[i-1] = display_info[i];
        
        display_info[pos_count - 1] = EMPTY_GLYPH;

        if (last_index == pos_count) {
            display_info[pos_count - 1] = char_to_glyph(*message_pointer);
            message_pointer++;
            if (*message_pointer == '\0') last_index--;
        } 
        else {
            last_index--;
            if (last_index < 0) {
                last_index = pos_count;
                message_pointer = (char*)input.getMessage();
            }
        }
    }

    void displayChar(byte glyph, byte pos){
        digitalWrite(latch_pin, LOW);
        shiftOut(data_pin, clock_pin, MSBFIRST, glyph);
        shiftOut(data_pin, clock_pin, MSBFIRST, 1 << pos);
        digitalWrite(latch_pin, HIGH);
    }

    void display_frame(){
        displayChar(display_info[cur_index], byte(cur_index));
        cur_index = (cur_index + 1) % pos_count;
    }
    public:
        long time;
        char* message_pointer; 
        byte display_info[pos_count]; 
    private:
        int last_index = pos_count;
        int cur_index = 0; 
};

Display d;

void setup(){
    pinMode(latch_pin, OUTPUT);
    pinMode(clock_pin, OUTPUT);
    pinMode(data_pin, OUTPUT);
    for (int i = 0; i < pos_count; i++) d.display_info[i] = EMPTY_GLYPH;
    input.initialize();
    d.message_pointer = (char*)input.getMessage();
    d.time = millis();
}

void loop(){
    long actual_time = millis();
    input.updateInLoop();
    if (millis() - d.time >= 300) {
        d.dispay_now();
        d.time = actual_time;
    }
    d.display_frame();
}
