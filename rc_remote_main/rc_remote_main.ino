
#include "pin.h"

#include "RF24.h"

#include <Servo.h>



RF24 radio(PA11, PA12); 

bool radioNumber = 1;
uint16_t payload = 0;
uint8_t address[][6] = { "1Node", "2Node" };
long lastSentMsgDelay = 0;
long lastRPM = 0;


Servo ESC,SERVO;

volatile uint8_t speed = 0;
volatile long l = 0;
void timerInterrupt() {
  if (digitalRead(PA2) == HIGH) {
    speed = ((((((float)0.3487) / ((float)((millis() - l)) / (float)1000))) * ((float)3.6)) / ((float)3));
    l = millis();
  }
}

void setup() {
  pinMode(PC13, OUTPUT);
  pinMode(PA2, INPUT_PULLUP);
  pinMode(sensor_TENSION, INPUT);
  attachInterrupt(digitalPinToInterrupt(PA2), timerInterrupt, CHANGE);
  //serial_FTDI.begin(115200);
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

radio.setDataRate(RF24_2MBPS);
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_MAX);
  radio.setPayloadSize(sizeof(payload));
  radio.openWritingPipe(address[radioNumber]);
  radio.openReadingPipe(1, address[!radioNumber]);
  radio.startListening();
  l = millis();
}


void loop() {
    uint8_t pipe;
    while(radio.available(&pipe)) {              // is there a payload? get the pipe number that recieved it
      radio.read(&payload, sizeof(payload));             // fetch payload from FIFO

      uint8_t a = payload >> 8;
      uint8_t b = payload;
      //serial_FTDI.println("NRF recieve: type:"+String(a)+", "+String(b));

    

      ESC.write(a);
      b = 180 - b;
      SERVO.write(b);
      lastSentMsgDelay = millis();
    }
    if(millis() - 100 > lastRPM) {
      long start = millis();
      radio.stopListening();

      uint8_t toSendTension = map(((analogRead(sensor_TENSION)*330)/1023), 232, 297,100,128) ;

      

      //speed; 0.35m/rpm
      payload = (toSendTension << 8) | speed;
      //serial_FTDI.println(String(speed)+"Send:"+String(toSendRpm)+" t"+String(toSendTension));
      radio.write(&payload, sizeof(payload));
      lastRPM = millis();
      speed = 0;
      radio.startListening();
      //serial_FTDI.println(String((millis() - start)));
    }
    if(millis() - lastSentMsgDelay > 250) {
      ESC.write(90);
      SERVO.write(90);
    } 

}

