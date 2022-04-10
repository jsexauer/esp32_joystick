#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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





void setup() {
  Serial.begin(115200);

  pinMode(PIN_JOYSTICK_BTN, INPUT_PULLUP);

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
  if (deltaX < 2  && deltaY < 2){
    score++;
    display.clearDisplay();
    display.setTextSize(2); 
    display.setTextColor(WHITE); 
    display.setCursor(0,0);
    display.printf(" Score: %d", score);
    display.display();
    delay(2000);
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



