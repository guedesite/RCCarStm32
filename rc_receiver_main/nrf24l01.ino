#include "pin.h"
#include "SPI.h"
#include "RF24.h"


uint8_t nrf_address[][6] = { "1Node", "2Node" };
bool nrf_radioNumber = 0;
uint16_t nrf_payload = 0;


void nrf24l01_start() {
  #if is_test_mode == true
    //serial_FTDI.println("Start NRF24L01");
  #endif
  while (!radio.begin()) {
    buzzer(200);
  }

  radio.setDataRate(RF24_2MBPS);
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_MAX);
  radio.setPayloadSize(sizeof(nrf_payload));
  radio.openWritingPipe(nrf_address[nrf_radioNumber]);
  radio.openReadingPipe(1, nrf_address[!nrf_radioNumber]);
}

void nrf24l01_send(uint8_t ESC, uint8_t servo) {
  radio.stopListening();
  nrf_payload = (ESC << 8) | servo;
  flag = radio.write(&nrf_payload, sizeof(nrf_payload));
  if(!flag) {
    allError++;
  } else {
    allSend++;
  }
  //serial_FTDI.println("send "+String(ESC)+" - "+String(servo)) ;
  radio.startListening();
}

void nrf24l01_update() {
  uint8_t pipe;
  bool flag = false;
  while(radio.available(&pipe)) {              // is there a payload? get the pipe number that recieved it
      radio.read(&nrf_payload, sizeof(nrf_payload));             // fetch payload from FIFO

      tension = nrf_payload >> 8;
      speed = nrf_payload;
      //serial_FTDI.println("NRF recieve: "+String(speed)+ " "+String(tension));
      flag = true;
    }

    if(flag) {
      lcd_update();
    }
}

