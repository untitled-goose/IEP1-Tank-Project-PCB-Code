//for ESP32 RC remote PCB for controlls for RC tank. uploaded on arduino coding app/download
#include <esp_now.h>
#include <WiFi.h>

#define LJ_X 33 //Left joystick X connected to pin 33
#define LJ_Y 32 //Left joystick Y connected to pin 32
#define jLButton 25  // Left Joystick button is connected to pin 25

#define RJ_X 34 //Right joystick X connected to pin 34
#define RJ_Y 35 //Right joystick Y connected to pin 35
#define jRButton 26  // Right Joystick button is connected to pin 26

#define Pot1 39 //Left Potentiometer for speed
#define Pot2 36 //Right Potentiometer for servo

#define togSwitch 18 // Toggle switch for DC motors
#define b1 27        // Push button for servo

uint8_t tankAddress[] = {0x58,0x2A,0xBD,0x60,0xFF,0x88}; // Receiver's MAC address 58:2A:BD:60:FF:88

//Turret input 
typedef struct __attribute__((packed)) struct_message {// Packing a dataToSend structure to send to reciever
    byte jLeftX;
    byte jLeftY;
    byte jLeftButton;
    byte jRightX;
    byte jRightY;
    byte jRightButton;
    byte pot1;
    byte pot2;
    byte togSwitchL;
    byte togSwitchR;
    byte button1;
    byte button2;
    byte button3;
    byte button4;

} struct_message;

struct_message dataToSend;
esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pinMode(togSwitch, INPUT_PULLUP);
  pinMode(b1, INPUT_PULLUP);       

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  memcpy(peerInfo.peer_addr, tankAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  dataToSend.jLeftX = 127;  // The Joysticks resting position is 127.
  dataToSend.jLeftY = 127;  // Values from range from 0 to 255. 
  dataToSend.jRightX = 127;
  dataToSend.jRightY = 127;
  dataToSend.pot1 = 1;
  dataToSend.pot2 = 1;
  dataToSend.jLeftButton = 1;  // All buttons and switches defaut to 1, because we are using pull up resistors.
  dataToSend.jRightButton = 1;  // Value equals 1 when pressed/switched.
  dataToSend.togSwitchL = 1;
  dataToSend.togSwitchR = 1;
  dataToSend.button1 = 1;
  dataToSend.button2 = 1;
  dataToSend.button3 = 1;
  dataToSend.button4 = 1;
}

void loop() {

  dataToSend.jLeftX = map(analogRead(LJ_X), 0, 4095, 0, 255);   // Read all analog inputs and mapping them to one byte values.
  dataToSend.jLeftY = map(analogRead(LJ_Y), 0, 4095, 0, 255);
  dataToSend.jRightX = map(analogRead(RJ_X), 0, 4095, 0, 255);
  dataToSend.jRightY = map(analogRead(RJ_Y), 0, 4095, 0, 255);
  dataToSend.pot1 = map(analogRead(Pot1), 0, 4095, 0, 255);
  dataToSend.pot2 = map(analogRead(Pot2), 0, 4095, 0, 255);
  dataToSend.togSwitchL = digitalRead(togSwitch); // 0 = switch ON, 1 = OFF
  dataToSend.button1 = digitalRead(b1);           // 0 = pressed, 1 = released
    

 esp_now_send(tankAddress, (uint8_t *) &dataToSend, sizeof(dataToSend));
 delay(20); 

}

