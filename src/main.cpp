#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Tone32.h>

///////////////////////////////////////////////////////////////////////////////////////////////
// Initilaize screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// SCL is GPIO22 (aka D22 on PCB)
// SDA is GPIO21 (aka D21 on PCB)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


///////////////////////////////////////////////////////////////////////////////////////////////
// Initialize joystick
#define PIN_JOYSTICK_X 2 // D2 on PCB
#define PIN_JOYSTICK_Y 4 // D4 on PCB
#define PIN_JOYSTICK_BTN 15 // D15 on PCB
#define PIN_BUZZER 23 // D23 on PCB
#define melodyPin 23
#define BUZZER_CHANNEL 0



struct screenPosition {
  int x;
  int y;
};

screenPosition targetPosition;
int score;

void randomizeTargetPosition(){
  targetPosition.x = random(0, SCREEN_WIDTH-1);
  targetPosition.y = random(0, SCREEN_HEIGHT-1);
}


void debugJoystick(){
  int n = 0;
  while(1){

    n++;

    display.clearDisplay();
    display.setTextSize(1); 
    display.setTextColor(WHITE); 
    display.setCursor(0,0); 
    display.println(n);
    display.printf("X =  %d\n", analogRead(PIN_JOYSTICK_X));
    display.printf("Y = %d\n", analogRead(PIN_JOYSTICK_Y));
    display.printf("Button = %d\n", digitalRead(PIN_JOYSTICK_BTN));
    display.display();

    delay(500);

  }
}



//Mario main theme melody
// adapted from: https://www.hackster.io/jrance/super-mario-theme-song-w-piezo-buzzer-and-arduino-1cc2e4
int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};
//Mario main them tempo
int tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};
//Underworld melody
int underworld_melody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0
};
//Underwolrd tempo
int underworld_tempo[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};

void buzz(int targetPin, long freq, long length){
  tone(PIN_BUZZER, freq, length, BUZZER_CHANNEL);
}


void sing(int song) {
  // iterate over the notes of the melody:
  int quarterNoteDuration = 650;
  if (song ==1 ){
    Serial.println(" 'Mario Theme'");
    int size = sizeof(melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {

      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = quarterNoteDuration / tempo[thisNote];

      buzz(melodyPin, melody[thisNote], noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 0.75;
      delay(pauseBetweenNotes);

      // stop the tone playing:
      buzz(melodyPin, 0, noteDuration);

    }
  }else{
    Serial.println(" 'Underworld Theme'");
    int size = sizeof(underworld_melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {

      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = quarterNoteDuration / underworld_tempo[thisNote];

      buzz(melodyPin, underworld_melody[thisNote], noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 0.75;
      delay(pauseBetweenNotes);

      // stop the tone playing:
      buzz(melodyPin, 0, noteDuration);

    }
  }
  
}




void playMarioTheme()
{
  //sing the tunes
  sing(1);
  sing(1);
  sing(2);
}




void setup() {
  Serial.begin(115200);

  pinMode(PIN_JOYSTICK_BTN, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);

  //playMarioTheme();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(500);

  // Clear the buffer
  display.clearDisplay();

  randomizeTargetPosition();
  score = 0;

  tone(PIN_BUZZER, NOTE_C4, 500, BUZZER_CHANNEL);
  tone(PIN_BUZZER, NOTE_D4, 500, BUZZER_CHANNEL);
  tone(PIN_BUZZER, NOTE_E4, 600, BUZZER_CHANNEL);
  // Debug the joystick
  //debugJoystick();


}



screenPosition getJoystickPosition(){ 
  // Calculate the X and Y position the user wants based on the joystick position
  screenPosition result;
  result.x = (analogRead(PIN_JOYSTICK_X)* (SCREEN_WIDTH-1)/4095);
  result.y = SCREEN_HEIGHT - 1 - (analogRead(PIN_JOYSTICK_Y)* (SCREEN_HEIGHT-1)/4095);
  Serial.printf("X=%d Y=%d\n", result.x, result.y);

  return result;
}




void loop() { 

  if (digitalRead(PIN_JOYSTICK_BTN) == 0){
    randomizeTargetPosition();
  }
  
  screenPosition joystickPosition = getJoystickPosition();

  // See if we've scored
  int deltaX = abs(joystickPosition.x - targetPosition.x);
  int deltaY = abs(joystickPosition.y - targetPosition.y);
  if (deltaX < 3  && deltaY < 3){
    score++;
    display.clearDisplay();
    display.setTextSize(2); 
    display.setTextColor(WHITE); 
    display.setCursor(0,0);
    display.printf(" Score: %d", score);
    display.display();

    if (score == 3){
      sing(1);
    }
    else if (score == 10){
      playMarioTheme();
    }else{
      tone(PIN_BUZZER, NOTE_E4, 300, BUZZER_CHANNEL);
      delay(200);
      tone(PIN_BUZZER, NOTE_E4, 300, BUZZER_CHANNEL);
      delay(1000);
    }
    
    randomizeTargetPosition();
  }


  // Render the scene
  display.clearDisplay();

  display.drawCircle(targetPosition.x, targetPosition.y, 2, WHITE);
  display.drawCircle(targetPosition.x, targetPosition.y, 4, WHITE);
  display.drawCircle(targetPosition.x, targetPosition.y, 6, WHITE);

  display.drawPixel(joystickPosition.x, joystickPosition.y, WHITE);
  display.drawCircle(joystickPosition.x, joystickPosition.y, 2, WHITE);
  
  display.display();

  delay(250);
}



