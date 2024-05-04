#include <Adafruit_NeoPixel.h>
#include "EEPROMAnything.h"
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

#define LED_DIN 6
#define JOYSTICK_PIN 2
#define MAX7219_DIN 3
#define MAX7219_CS 4
#define MAX7219_CLK 5
#define BUTTON_PAUSE 8
#define BUTTON_START 9
#define JOYSTICK_X A0
#define JOYSTICK_Y A1

#define WIDTH 8
#define HEIGHT 16
#define MATRIX_SIZE (WIDTH*HEIGHT)
#define FIGURE_WIDTH 4
#define FIGURE_HEIGHT 4
#define FIGURE_TYPES 7
#define JOYSTICK_CHNG_LVL 30
#define TOP_SPEED 15
#define TOP_ACCELERATION 10

int i = 0;
int top_score = 0;
int score = 0;
int top_score_1 = 0;
int top_score_2 = 0;
int top_score_3 = 0;
int top_score_4 = 0;
int score_1 = 0;
int score_2 = 0;
int score_3 = 0;
int score_4 = 0;
bool pause = false;
bool pause_once = false;
bool pause_pressed = false;
unsigned long prev_millis = 0; 
unsigned long cur_millis = 0;


class Mp3Notify {
public:
  static void OnError(uint16_t errorCode) {
    Serial.println();
    Serial.print("Error ");
    Serial.println(errorCode);
  }
  static void OnPlay(uint16_t globalTrack) {
    Serial.println();
    Serial.print("Start play ");
    Serial.println(globalTrack);   
  }
  static void OnDetect(uint16_t code) {
    Serial.println();
    Serial.print("Card detected ");
    Serial.println(code);     
  }
  static void OnInsert(uint16_t code) {
    Serial.println();
    Serial.print("Card inserted ");
    Serial.println(code); 
  }
  static void OnRemove(uint16_t code) {
    Serial.println();
    Serial.print("Card removed ");
    Serial.println(code);  
  }
};
DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial);

const char figure_I[] = {
  0,0,0,0,
  1,1,1,1,
  0,0,0,0,
  0,0,0,0,

  0,0,1,0,
  0,0,1,0,
  0,0,1,0,
  0,0,1,0,
  
  0,0,0,0,
  0,0,0,0,
  1,1,1,1,
  0,0,0,0,

  0,1,0,0,
  0,1,0,0,
  0,1,0,0,
  0,1,0,0,
};

const char figure_L[] = {
  0,0,1,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,
  
  0,1,0,0,
  0,1,0,0,
  0,1,1,0,
  0,0,0,0,

  0,0,0,0,
  1,1,1,0,
  1,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  0,1,0,0,
  0,1,0,0,
  0,0,0,0,
};

const char figure_J[] = {
  1,0,0,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,
  
  0,1,1,0,
  0,1,0,0,
  0,1,0,0,
  0,0,0,0,

  0,0,0,0,
  1,1,1,0,
  0,0,1,0,
  0,0,0,0,
  
  0,1,0,0,
  0,1,0,0,
  1,1,0,0,
  0,0,0,0,

};

const char figure_T[] = {
  0,1,0,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,

  0,1,0,0,
  0,1,1,0,
  0,1,0,0,
  0,0,0,0,
  
  0,0,0,0,
  1,1,1,0,
  0,1,0,0,
  0,0,0,0,

  0,1,0,0,
  1,1,0,0,
  0,1,0,0,
  0,0,0,0,

};

const char figure_S[] = {
  0,1,1,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,

  0,1,0,0,
  0,1,1,0,
  0,0,1,0,
  0,0,0,0,

  0,0,0,0,
  0,1,1,0,
  1,1,0,0,
  0,0,0,0,

  1,0,0,0,
  1,1,0,0,
  0,1,0,0,
  0,0,0,0,
};

const char figure_Z[] = {
  1,1,0,0,
  0,1,1,0,
  0,0,0,0,
  0,0,0,0,
  
  0,0,1,0,
  0,1,1,0,
  0,1,0,0,
  0,0,0,0,

  0,0,0,0,
  1,1,0,0,
  0,1,1,0,
  0,0,0,0,
  
  0,1,0,0,
  1,1,0,0,
  1,0,0,0,
  0,0,0,0,
};

const char figure_O[] = {
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
};


const char *figures[FIGURE_TYPES] = {
  figure_S,
  figure_Z,
  figure_L,
  figure_J,
  figure_O,
  figure_T,
  figure_I,
};


const long figure_colors[FIGURE_TYPES] = {
  0x009900, // green S
  0xFF0000, // red Z
  0xFF8000, // orange L
  0x000044, // blue J
  0xFFFF00, // yellow O
  0xFF00FF, // purple T
  0x00FFFF,  // cyan I
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(MATRIX_SIZE, LED_DIN, NEO_RGB + NEO_KHZ800);

int old_button = 0;
int old_px = 0;
int old_turn = 0;
int figure_id;
int figure_rotation;
int figure_x;
int figure_y;
char figure_sequence[FIGURE_TYPES];
char sequence_i = FIGURE_TYPES;
long last_move;
long move_delay = 100;
long last_drop;
long drop_delay = 500;
long last_draw;
long draw_delay = 30;
long grid[WIDTH*HEIGHT];

void num_module(byte send_addr, byte send_data){
  digitalWrite(MAX7219_LOAD_PIN, LOW);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, send_addr);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, send_data);
  digitalWrite(MAX7219_LOAD_PIN, HIGH);
}

void pos(int x, int y, long color) {
  int a = (HEIGHT - 1 - y) * WIDTH;
  a %= MATRIX_SIZE;
  strip.setPixelColor(a, color);
}

void draw_grid() {
  int x, y;
  for(y=0; y<HEIGHT; ++y) {
    for(x=0; x<WIDTH; ++x) {
      if( grid[y*WIDTH+x] != 0 ) {
        pos(x,y,grid[y*WIDTH+x]);
      } else {
        pos(x, y, 0);
      }
    }
  }
  strip.show();
}

void rand_figure() {
  if(sequence_i >= FIGURE_TYPES) {
    int i, j, k;
    for(i = 0; i < FIGURE_TYPES; ++i) {
      do {
        j = rand() % FIGURE_TYPES;
        for(k = 0; k < i; ++k) {
          if(figure_sequence[k]==j) break;
        }
      } while(k < i);
      figure_sequence[i] = j;
    }
    sequence_i = 0;
  }
  figure_id = figure_sequence[sequence_i++];
  figure_y = -4;
  figure_x = 3;
  figure_rotation=0;
}

void remove_figure() {
  int x, y;
  const char *figure = figures[figure_id] + (figure_rotation * FIGURE_HEIGHT * FIGURE_WIDTH);
  for(y=0; y<FIGURE_HEIGHT; ++y) {
    for(x=0; x<FIGURE_WIDTH; ++x) {
      int nx = figure_x + x;
      int ny = figure_y + y;
      if(ny<0 || ny>HEIGHT) continue;
      if(nx<0 || nx>WIDTH) continue;
      if(figure[y*FIGURE_WIDTH+x]==1) {
        grid[ny*WIDTH+nx]=0;
      }
    }
  }
}

void add_figure() {
  int x, y;
  const char *figure = figures[figure_id] + (figure_rotation * FIGURE_HEIGHT * FIGURE_WIDTH);
  for(y=0; y<FIGURE_HEIGHT; ++y) {
    for(x=0; x<FIGURE_WIDTH; ++x) {
      int nx = figure_x + x;
      int ny = figure_y + y;
      if(ny<0 || ny>HEIGHT) continue;
      if(nx<0 || nx>WIDTH) continue;
      if(figure[y*FIGURE_WIDTH+x]==1) {
        grid[ny*WIDTH+nx]=figure_colors[figure_id];
      }
    }
  }
}

void add_score(int y) {
  score = score + 10;
  if(score > top_score)
  {
    EEPROM_writeAnything(1, score);
  }
  EEPROM_readAnything(1, top_score);
  top_score_1 = top_score - ((top_score/10) * 10);
  top_score_2 = ((top_score - (( top_score/100) * 100)) - top_score_1) / 10;
  top_score_3 = ((top_score - ((top_score/1000) * 1000)) - top_score_1 - top_score_2) / 100;
  top_score_4 = (top_score - top_score_1 - top_score_2 - top_score_3) / 1000;
  score_1 = score - (( score/10) * 10);
  score_2 = ((score - ((score/100 ) * 100)) - score_1) / 10;
  score_3 = ((score - ((score/1000) * 1000)) - score_1 - score_2) / 100;
  score_4 = (score - score_1 - score_2 - score_3) / 1000;
  num_module(0x08, score_4);
  num_module(0x07, score_3);
  num_module(0x06, score_2);
  num_module(0x05, score_1);
  num_module(0x04, top_score_4);
  num_module(0x03, top_score_3);
  num_module(0x02, top_score_2);
  num_module(0x01, top_score_1);
  int x;
  for(y>0; --y) {
    for(x=0; x<WIDTH; ++x) {
      grid[y*WIDTH+x] = grid[(y-1)*WIDTH+x];
    }
  }
  for(x=0;x<WIDTH;++x) {
    grid[x]=0;
  }
}

void add_acceleration() { 
  if(drop_delay > TOP_SPEED) drop_delay -= TOP_ACCELERATION;
}

void remove_row() {
  int x, y, c;
  char row_removed = 0;
  for(y=0; y<HEIGHT; ++y) {
    c = 0;
    for(x=0; x<WIDTH; ++x) {
      if(grid[y*WIDTH+x] > 0) c++;
    }
    if(c==WIDTH) {
      add_score(y);
      add_acceleration();
    }
  }  
}

void pos_side() {
  int dx = map(analogRead(JOYSTICK_X), 0, 1023, 512, -512);
  int new_px = 0;
  if(dx>JOYSTICK_CHNG_LVL) {
    new_px=-1;
  }
  if(dx<-JOYSTICK_CHNG_LVL) {
    new_px=1;
  }
  if(new_px!=old_px && pos_fit(figure_x+new_px, figure_y, figure_rotation)==1) {
    figure_x += new_px;
  }
  old_px = new_px;
}

void pos_rotate() {
  int turning = 0;
  int dy = map(analogRead(JOYSTICK_Y), 0, 1023, 512, -512);
  if(dy>JOYSTICK_CHNG_LVL) turning=1;
  if(turning==1 && turning != old_turn) {
    int new_pr = (figure_rotation + 1) % 4;
    if(pos_fit(figure_x, figure_y, new_pr)) {
      figure_rotation = new_pr;
    } else {
      if(pos_fit(figure_x-1, figure_y, new_pr)) {
        figure_x = figure_x - 1;
        figure_rotation = new_pr;
      } else if(pos_fit(figure_x+1,figure_y,new_pr)) {
        figure_x = figure_x+1;
        figure_rotation = new_pr;
      } 
    }
  }
  old_turn = turning;
}

int pos_fit(int px, int py, int pr) {
  if(pos_hit_edg(px, py, pr)) return 0;
  if(pos_hit_fgr(px, py, pr)) return 0;
  return 1;
}

int pos_hit_edg(int px, int py, int pr) {
  int x,y;
  const char *figure = figures[figure_id] + (pr * FIGURE_HEIGHT * FIGURE_WIDTH);
  for(y=0; y<FIGURE_HEIGHT; ++y) {
    for(x=0; x<FIGURE_WIDTH; ++x) {
      int nx = px + x;
      int ny = py + y;
      if(ny < 0) continue;
      if(figure[y*FIGURE_WIDTH+x]>0) {
        if(nx<0) return 1;
        if(nx>=WIDTH ) return 1;
      }
    }
  }
  return 0;
}

int pos_hit_fgr(int px, int py, int pr) {
  int x, y;
  const char *figure = figures[figure_id] + (pr * FIGURE_HEIGHT * FIGURE_WIDTH);
  for(y=0; y<FIGURE_HEIGHT; ++y) {    
    int ny = py + y;
    if(ny < 0) continue;
    for(x=0; x<FIGURE_WIDTH; ++x) {
      int nx = px + x;
      if(figure[y*FIGURE_WIDTH+x]>0) {
        if(ny>=HEIGHT ) return 1;
        if(grid[ny*WIDTH+nx]!=0 ) return 1;
      }
    }
  }
  return 0;
}

void draw_restart() {
  for(int i=0; i<MATRIX_SIZE; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); 
   }
   strip.setPixelColor(55, strip.Color(150, 150, 150)); 
   strip.setPixelColor(74, strip.Color(150, 150, 150)); 
   strip.setPixelColor(77, strip.Color(150, 150, 150));
   strip.setPixelColor(83, strip.Color(150, 150, 150));  
   strip.setPixelColor(85, strip.Color(150, 150, 150)); 
   strip.setPixelColor(90, strip.Color(150, 150, 150)); 
   strip.setPixelColor(91, strip.Color(150, 150, 150)); 
   strip.setPixelColor(92, strip.Color(150, 150, 150)); 
   strip.setPixelColor(93, strip.Color(150, 150, 150)); 
   strip.setPixelColor(98, strip.Color(150, 150, 150)); 
   strip.setPixelColor(101, strip.Color(150, 150, 150)); 
   strip.setPixelColor(106, strip.Color(150, 150, 150)); 
   strip.setPixelColor(107, strip.Color(150, 150, 150)); 
   strip.setPixelColor(108, strip.Color(150, 150, 150)); 
   strip.setPixelColor(109, strip.Color(150, 150, 150)); 
   strip.show();
}

void draw_white() {
  for(int i=0;i<MATRIX_SIZE;i++) {
    strip.setPixelColor(i, strip.Color(120, 120, 120));
    strip.show();
    delay(3);
  }  
}

void game_end() {
  score = 0;
  mp3.playMp3FolderTrack(4);
  delay(1000);
  int x, y;
  long over_time = millis();
  draw_restart();
  cur_millis = millis();
  prev_millis = cur_millis;
  int led_number = 1;
  while(millis() - over_time < 8000) {
    cur_millis = millis();
    if(cur_millis - prev_millis >= 1000){
      prev_millis += 1000;  
      strip.setPixelColor(55-led_number, strip.Color(150, 150, 150));  
      strip.show();
      led_number += 1; 
    }    
    if(!digitalRead(BUTTON_START)) {
      mp3.playMp3FolderTrack(3);
      draw_white();
      delay(400);
      break;
    }
  }
  mp3.playMp3FolderTrack(3);
  draw_white();
  setup();
  return;
}

void pos_drop() {
  remove_figure();  
  if(pos_fit(figure_x, figure_y+1, figure_rotation)) {
    figure_y++;
    add_figure();
  } else {
    add_figure();
    remove_row();
    if(check_end()==1) {
      game_end();
    }
    choose_new_figure();
  }
}

void drop_fast() {
  int y = map(analogRead(JOYSTICK_Y), 0, 1023, 512, -512);
  if(y<-JOYSTICK_CHNG_LVL) {
    pos_drop();
  }
}

void game_start() {
  remove_figure();
  pos_side();
  pos_rotate();
  add_figure();
  drop_fast();
}

int check_end() {
  int x, y;
  const char *figure = figures[figure_id] + (figure_rotation * FIGURE_HEIGHT * FIGURE_WIDTH);
  for(y=0; y<FIGURE_HEIGHT; ++y) {
    for(x=0; x<FIGURE_WIDTH; ++x) {      
      int ny = figure_y + y;
      int nx = figure_x + x;
      if(figure[y*FIGURE_WIDTH+x]>0) {
        if(ny<0) return 1;
      }
    }
  }
  return 0;
}

void setup() {
  Serial.begin(115200);  
  mp3.begin();
  uint16_t volume = mp3.getVolume();
  mp3.setVolume(30);  
  uint16_t count = mp3.getTotalTrackCount();
  delay(1000);
  mp3.playMp3FolderTrack(1);
  pinMode(MAX7219_DIN, OUTPUT);
  pinMode(MAX7219_LOAD_PIN, OUTPUT);
  pinMode(MAX7219_CLK, OUTPUT);
  digitalWrite(MAX7219_CLK, HIGH);
  pinMode(BUTTON_PAUSE, INPUT_PULLUP);
  pinMode(BUTTON_START, INPUT_PULLUP);
  delay(1);

  num_module(0x0f, 0x00);
  num_module(0x0c, 0x01);
  num_module(0x0b, 0x07);
  num_module(0x0a, 0x0f);
  num_module(0x09, 0xff);
  EEPROM_readAnything(1,top_score);
  Serial.println(top_score);
  top_score_1 = top_score - ((top_score/10 ) * 10);
  top_score_2 = ((top_score - ((top_score/100 ) * 100)) - top_score_1) / 10;
  top_score_3 = ((top_score - ((top_score/1000 ) * 1000)) - top_score_1 - top_score_2) / 100;
  top_score_4 = (top_score - top_score_1 - top_score_2 - top_score_3) / 1000;
  num_module(0x08, 0x0f);
  num_module(0x07, 0x0f);
  num_module(0x06, 0x0f);
  num_module(0x05, 0x0f);
  num_module(0x04, top_score_4);
  num_module(0x03, top_score_3);
  num_module(0x02, top_score_2);
  num_module(0x01, top_score_1);

  int i;
  strip.begin();
  strip.show();
  pinMode(JOYSTICK_PIN, INPUT);
  digitalWrite(JOYSTICK_PIN, HIGH);
  for(i=0; i<WIDTH*HEIGHT; ++i) {
    grid[i]=0;
  }
  randomSeed(analogRead(JOYSTICK_Y)+analogRead(2)+analogRead(3));
  choose_new_figure();
  move_delay = 100;
  drop_delay = 500;
  draw_delay = 30;
  last_draw = last_drop = last_move = millis();
}

void draw_pause() {  
   for(int i=0; i<STRAND_LENGTH; i++) {
    strip.setPixelColor(i, strip.Color(0,0,0)); 
   }
   strip.setPixelColor(53, strip.Color(150,150,150)); 
   strip.setPixelColor(58, strip.Color(150,150,150)); 
   strip.setPixelColor(66, strip.Color(150,150,150)); 
   strip.setPixelColor(67, strip.Color(150,150,150)); 
   strip.setPixelColor(68, strip.Color(150,150,150)); 
   strip.setPixelColor(69, strip.Color(150,150,150)); 
   strip.setPixelColor(74, strip.Color(150,150,150)); 
   strip.setPixelColor(77, strip.Color(150,150,150)); 
   strip.setPixelColor(82, strip.Color(150,150,150)); 
   strip.setPixelColor(83, strip.Color(150,150,150)); 
   strip.setPixelColor(84, strip.Color(150,150,150)); 
   strip.setPixelColor(85, strip.Color(150,150,150)); 
   strip.show();
   if(!pause_once) {
    mp3.playMp3FolderTrack(2);
    pause_onece = true;
    delay(1000);
   }
}

void loop() {
  long t = millis();
  if(!pause) {
    if(!digitalRead(BUTTON_PAUSE) && !pause_pressed) {
      pause = !pause;
      pause_pressed = true;
      pause_once = false;
    }
    if(digitalRead(BUTTON_PAUSE) && pause_pressed) {      
      pause_pressed = false;
    }
    if(t - last_move > move_delay ) {
      last_move = t;
      game_start();
    }
    if(t - last_drop > drop_delay ) {
      last_drop = t;
      pos_drop();
    }
    if(t - last_draw > draw_delay ) {
      last_draw = t;
      draw_grid();
    }
  }
  else {
    if(!digitalRead(BUTTON_PAUSE) && !pause_pressed) {
      pause = !pause;
      pause_pressed = true;
      mp3.playMp3FolderTrack(1);
    }
    if(digitalRead(BUTTON_PAUSE) && pause_pressed) {      
      pause_pressed = false;
    }
    draw_pause();
    delay(1);
  }
}