#include "pin.h"

uint8_t leds[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};

bool stp_change = true;
uint8_t actualmenu = 0;
long ledLastEdit = 0; 

void STP16_start() {
  #if is_test_mode == true
    serial_FTDI.println("Start STP16");
  #endif
  pinMode(pin_SDI_LED, OUTPUT);
  pinMode(pin_LE_LED, OUTPUT);
  pinMode(pin_CLK_LED, OUTPUT);
  digitalWrite(pin_LE_LED, HIGH);

}

void setLed(uint8_t id, uint8_t value) {
  if(leds[id] != value) {
    stp_change = true;
    leds[id] = value;
  }
}
void STP16_clear() {
  for (uint8_t n=0; n<16; n++)
  {
    leds[n] = 0;
  }
  stp_change = true;
}


void STP16_update() {
  if(stp_change) {
    uint8_t n;
    digitalWrite(pin_CLK_LED, LOW);
    digitalWrite(pin_LE_LED, HIGH);

    int v = 5;
    for (n=0; n<16; n++)
    {
        if (leds[n] == 1)  // most sig bit first
        {
            digitalWrite(pin_SDI_LED, HIGH);
        }
        else
        {
            digitalWrite(pin_SDI_LED, LOW);
        }

        digitalWrite(pin_CLK_LED, HIGH);
        digitalWrite(pin_CLK_LED, LOW); 
    } 
    digitalWrite(pin_LE_LED, LOW);
    digitalWrite(pin_LE_LED, HIGH);
    stp_change = false;
  }
  if(ledLastEdit != 0 && ledLastEdit + 5000 < millis()) {
    STP16_clear();
    ledLastEdit = 0;
  }
}


void updateIntPot(uint8_t newv) {
      ledLastEdit = millis();
      if(actualmenu != (newv / 44)) {
        buzzer(100);
        actualmenu = (newv / 44);
      }
      STP16_clear();
      setLed(actualmenu,1);

      temp8 = opts[actualmenu] * 10;
      for(uint8_t i = 0; i <= temp8; i++) {
        setLed(6+i,1);
      }

      serial_FTDI.println(actualmenu);

}



