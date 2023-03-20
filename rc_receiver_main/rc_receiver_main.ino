#include "pin.h"
#include "SPI.h"
#include "RF24.h"
RF24 radio(pin_NRF24_CE, pin_NRF24_CSN);

uint8_t temp8;
int16_t ntemp16;
uint16_t temp16;
bool flag;
uint8_t potDirSpe = 0;

float opts[6] = {1,0.5,0.5,0.5,0.5,1};

void setup() {
  pinMode(pin_BUZZER, OUTPUT);
  pinMode(PC13, OUTPUT);

  buzzer(100);

  #if is_test_mode == true
    digitalWrite(PC13, HIGH);
    serial_FTDI.begin(115200);
    delay(500);
    serial_FTDI.println("start debug");
    digitalWrite(PC13, LOW);
  #endif

  IO_start();
  STP16_start();
  nrf24l01_start();
  lcd_start();
}

void loop() {
  STP16_update();
  lcd_update();
  IO_update();
}

long bipTime = 0;

void buzzer(uint16_t time) {
  bipTime = millis();
  while(bipTime + time > millis()) {
    digitalWrite(pin_BUZZER, HIGH);
    delayMicroseconds(500);
    digitalWrite(pin_BUZZER, LOW);
    delayMicroseconds(500);
  }
}


