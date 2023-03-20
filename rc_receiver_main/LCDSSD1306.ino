#include "pin.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C


#define refresh_Display 100
long lastDisplay = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET,1000000,1000000);


uint16_t allError = 0;
uint16_t allSend = 0;

void lcd_start() {
  #if is_test_mode == true
    serial_FTDI.println("Start LCDSS1306");
  #endif
  while(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    digitalWrite(pin_BUZZER, HIGH);
    delay(25);
    digitalWrite(pin_BUZZER, LOW);
    delay(25);
    digitalWrite(pin_BUZZER, HIGH);
    delay(25);
    digitalWrite(pin_BUZZER, LOW);
    delay(25);
    digitalWrite(pin_BUZZER, HIGH);
    delay(25);
    digitalWrite(pin_BUZZER, LOW);
    delay(300);
  }
  display.clearDisplay();
}

void lcd_update() {

  if(lastDisplay + refresh_Display < millis()) {   
    display.clearDisplay();

    //display.fillRect(0, display.height() -16 , 16, display.height(), 1);
    lastDisplay = millis();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(5,0);
    display.println(allError);

    display.setCursor(5,15);
    display.println(allSend); 

    display.setCursor(70,0);
    display.println(radio.testCarrier()); 

    display.setCursor(70,15);
    display.println(radio.testRPD()); 
    allError = 0;
    allSend = 0;

    display.display();
  }
}


