#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// Pins to connect the buttons
const int BTN_1 = 4;
const int BTN_2 = 5;
const int BTN_3 = 6;
const int BTN_4 = 7;

// Pins to connect the charges(We are using LEDs)
const int LED_1 = 8;
const int LED_2 = 9;
const int LED_3 = 10;
const int LED_4 = 11;

// Enumeration for LED status
enum LedStatus {
  LED_OFF = 0,
  LED_ON = 1
};

// Objects instances
StaticJsonDocument<200> doc;
SoftwareSerial lora(2, 3);  //Rx e TX


// Constants
const unsigned long delayTime = 2000;  // delay in milliseconds
unsigned long previousMillis = 0;      // Variable to store the initial moment of delay

// LED status variables
LedStatus led1Status = LED_OFF;
LedStatus led2Status = LED_OFF;
LedStatus led3Status = LED_OFF;
LedStatus led4Status = LED_OFF;

void setup() {
  Serial.begin(9600);
  lora.begin(9600);
  
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_4, INPUT_PULLUP);

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
}

void loop() {
  checkButtons();
  updateData();
  runLoRa();
}

void checkButtons() {
  checkButtonState(BTN_1, led1Status, LED_1);
  checkButtonState(BTN_2, led2Status, LED_2);
  checkButtonState(BTN_3, led3Status, LED_3);
  checkButtonState(BTN_4, led4Status, LED_4);
}

void runLoRa() {
  if (Serial.available() > 0) {
    String data = Serial.readString();
    data.trim();  // remove blank spaces

    if (data) {
      handleLedCommands(data);
    }
  }
}

void handleLedCommands(String command) {
  if (command.equals("led1on")) {
    led1Status = LED_ON;
  } else if (command.equals("led1off")) {
    led1Status = LED_OFF;
  } else if (command.equals("led2on")) {
    led2Status = LED_ON;
  } else if (command.equals("led2off")) {
    led2Status = LED_OFF;
  } else if (command.equals("led3on")) {
    led3Status = LED_ON;
  } else if (command.equals("led3off")) {
    led3Status = LED_OFF;
  } else if (command.equals("led4on")) {
    led4Status = LED_ON;
  } else if (command.equals("led4off")) {
    led4Status = LED_OFF;
  } else if (command.equals("ledAllon")) {
    led1Status = led2Status = led3Status = led4Status = LED_ON;
  } else if (command.equals("ledAlloff")) {
    led1Status = led2Status = led3Status = led4Status = LED_OFF;
  }

  updateLeds();
}

void checkButtonState(int buttonPin, LedStatus& ledStatus, int ledPin) {
  static bool btnFlag = LOW;
  bool currentButtonState = digitalRead(buttonPin);

  if (!currentButtonState && btnFlag) {
    ledStatus = static_cast<LedStatus>(!ledStatus);
    controlLed(ledStatus, ledPin);
  }

  btnFlag = currentButtonState;
}

void controlLed(LedStatus status, int pin) {
  digitalWrite(pin, status);
}

void updateData() {
  unsigned long currentMillis = millis();

  // Updating LEDs into App
  if (currentMillis - previousMillis >= delayTime) {
    updateLeds();

    doc["led1"] = (led1Status == LED_ON ? "on" : "off");
    doc["led2"] = (led2Status == LED_ON ? "on" : "off");
    doc["led3"] = (led3Status == LED_ON ? "on" : "off");
    doc["led4"] = (led4Status == LED_ON ? "on" : "off");

    String jsonString;
    serializeJson(doc, jsonString);

    // Send the JSON string via LoRa
    Serial.println(jsonString);

    previousMillis = currentMillis;
  }
}

void updateLeds() {
  controlLed(led1Status, LED_1);
  controlLed(led2Status, LED_2);
  controlLed(led3Status, LED_3);
  controlLed(led4Status, LED_4);
}
