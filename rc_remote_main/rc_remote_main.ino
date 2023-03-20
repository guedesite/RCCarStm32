#include "pin.h"

#include "RF24.h"

#include <Servo.h>

RF24 radio(PA11, PA12); 

bool radioNumber = 1;
uint16_t payload = 0;
uint8_t address[][6] = { "1Node", "2Node" };
long lastSentMsgDelay = 0;


Servo ESC,SERVO;


void setup() {
  pinMode(PC13, OUTPUT);
  serial_FTDI.begin(115200);
  digitalWrite(PC13, HIGH);
  delay(100);
  digitalWrite(PC13, LOW);

  //init_sensor();

  ESC.attach(pin_SERVO_ESC );
  SERVO.attach(pin_SERVO_SERVO);
  ESC.write(90);
  SERVO.write(90);
 

  while (!radio.begin()) {
    digitalWrite(PC13, HIGH);
    delay(30);
    digitalWrite(PC13, LOW);
    delay(30);
  }

  radio.setDataRate(RF24_1MBPS);
  //radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_MAX);
  radio.setPayloadSize(sizeof(payload));
  radio.openWritingPipe(address[radioNumber]);
  radio.openReadingPipe(1, address[!radioNumber]);

  radio.startListening();

}


void loop() {
    uint8_t pipe;
    while(radio.available(&pipe)) {              // is there a payload? get the pipe number that recieved it
      radio.read(&payload, sizeof(payload));             // fetch payload from FIFO

      uint8_t a = payload >> 8;
      uint8_t b = payload;
      serial_FTDI.println("NRF recieve: type:"+String(a)+", "+String(b));

      ESC.write(a);
      b = 180 - b;
      SERVO.write(b);
      lastSentMsgDelay = millis();

    }
    if(millis() - lastSentMsgDelay > 150) {
      ESC.write(90);
      SERVO.write(90);
    } 

}

