#include "pin.h"
uint16_t pot[3] = {0,0,0};

void IO_start() {
   #if is_test_mode == true
    serial_FTDI.println("Start IO");
  #endif
  pinMode(pin_BTN_0, INPUT);
  pinMode(pin_BTN_1, INPUT);
  pinMode(pin_BTN_2, INPUT);
  pinMode(pin_BTN_3, INPUT);

  pinMode(pin_POT_0, INPUT);
  pinMode(pin_POT_1, INPUT);
  pinMode(pin_POT_2, INPUT);
}

uint8_t getPot(uint8_t t) {
  return pot[t];
}

long lastIO = 0;

void IO_update() {
    if(lastIO + 5 < millis()) {
      lastIO = millis();
        pot[2] = analogRead(pin_POT_2);
        pot[1] = analogRead(pin_POT_1);
        nrf24l01_send(convertPot(1, pot[1]), convertPot(2, pot[2]));

        if(pot[0] != analogRead(pin_POT_0) / 4) {
          ntemp16 = analogRead(pin_POT_0) / 4;

          if(pot[0] - 5 > ntemp16 || pot[0] + 5 < ntemp16) {
            #if is_test_mode == true
              serial_FTDI.println("option update "+((String)ntemp16));
            #endif
            pot[0] = ntemp16;
            updateIntPot(255-ntemp16);
          }
        }
        if(digitalRead(pin_BTN_3) == LOW) {
           if(actualmenu == 0 || actualmenu == 5) {
            opts[actualmenu] = 1;
            buzzer(25);
          } else  {
            if(opts[actualmenu] < 1) {
              opts[actualmenu] += 0.01;
              buzzer(25);
            }
          }
          updateIntPot(255-pot[0]);
        }
        else if(digitalRead(pin_BTN_2) == LOW) {
          if(actualmenu == 0 || actualmenu == 5) {
            opts[actualmenu] = 0;
            buzzer(25);
          } else  {
            if(opts[actualmenu] > 0.02) {
              opts[actualmenu] -= 0.01;
              buzzer(25);
            }
          }
          updateIntPot(255-pot[0]);
        }

    }
}

uint8_t convertPot(uint8_t t, uint16_t v) {
  if(t == 1) {
    int8_t trim = (opts[3] * 20) - 10;// option 3 trottle trim
    //serial_FTDI.println(String(v)+ " | TRIM: "+String(trim));
    uint16_t midl = 433 +trim;
    //serial_FTDI.println("MIDDLE: "+String(midl));
    //serial_FTDI.println("DRIVE: "+String(uint8_t((1.0-opts[4])*100* 5)));
    if(v < midl - 6 ) {
      if(v <330) {
        v = 330;
      }
      return map(v, 330, midl - 6, 0+ uint8_t((1.0-opts[4])*10* 8), 89); // option 4 trottle drive
    } else if( v > midl + 6 ) {
      if(v >683) {
        v = 683;
      }
      return map(v, midl + 6, 683, 91, 180-uint8_t((1.0-opts[4])*10* 8)); // option 4 trottle drive
    } else {
        return 90;
    }
  } else if(t == 2) {
    int8_t trim = (opts[2] * 20) - 10;// option 2 steering trim
    uint16_t midl = 431 + (trim * 4);
    if(v < midl - 6 ) {
      if(v <70) {
        v = 70;
      }
      return map(v, 70, midl - 6, 0+ uint8_t((1.0-opts[1])*10* 8), 89); // option 1 steering drive
    } else if( v > midl + 6 ) {
      if(v >855) {
        v = 855;
      }
      return map(v, midl + 6, 855, 91, 180-uint8_t((1.0-opts[1])*10* 8)); // option 1 steering drive
    } else {
        return 90;
    }
  }
  return 0;
}