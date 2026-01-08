#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <ArduinoOTA.h>
#include <ESP32Servo.h> 

const char* ssid = "";
const char* password = "";

const int BLUE_LED = 2;         
const int IR_BEAM_SLOW = 14;     
const int IR_BEAM_FAST = 32;     
const int BUTTON_PIN = 13;       
const int SLOW_BELT_PIN = 26; 
const int FAST_BELT_PIN = 27;
const int SHUTTER_PIN = 33;

#define RXD2 16
#define TXD2 17 

Servo myservo;
const int SERVO_PIN = 15;
const int BTN_BINS[] = {22, 23, 25, 21, 19}; 
const int ANGLES[] = {33, 67, 127, 167, 97}; 
const int NUM_BINS = 5;

bool systemActive = false;
bool waitingForClassification = false;
bool isSlowBeltRunning = false;
bool isFastBeltRunning = false;

AsyncWebServer server(80);

void runStartupSequence() {
    WebSerial.println("Running Startup Test...");
    digitalWrite(SLOW_BELT_PIN, HIGH); delay(250); digitalWrite(SLOW_BELT_PIN, LOW);
    digitalWrite(FAST_BELT_PIN, HIGH); delay(250); digitalWrite(FAST_BELT_PIN, LOW);
    digitalWrite(SLOW_BELT_PIN, HIGH); delay(250); digitalWrite(SLOW_BELT_PIN, LOW);
    digitalWrite(FAST_BELT_PIN, HIGH); delay(250); digitalWrite(FAST_BELT_PIN, LOW);
    
    digitalWrite(SLOW_BELT_PIN, HIGH); 
    digitalWrite(FAST_BELT_PIN, HIGH);
    delay(500);
    digitalWrite(SLOW_BELT_PIN, LOW);
    digitalWrite(FAST_BELT_PIN, LOW);
    WebSerial.println("Startup Test Complete. Button 13: Toggle Start/Stop.");
}

void sortBrick(int binIndex) {
    if(!systemActive) return; 
    
    WebSerial.print("Sorting to Bin: "); WebSerial.println(binIndex);
    int angle = ANGLES[binIndex - 1]; 
    myservo.write(angle);
    delay(500); 

    unsigned long runTime;
    if (binIndex == 2 || binIndex == 3) {
        runTime = 1900; 
    } else {
        runTime = 3300;
    }

    digitalWrite(FAST_BELT_PIN, HIGH);
    delay(runTime); 
    digitalWrite(FAST_BELT_PIN, LOW);

    myservo.write(ANGLES[4]); 
    delay(300);

    waitingForClassification = false;
    if(systemActive) {
        isSlowBeltRunning = true; 
        digitalWrite(SLOW_BELT_PIN, HIGH);
        WebSerial.println("Sorting Complete. Resuming Feed.");
    }
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); 

    pinMode(SLOW_BELT_PIN, OUTPUT);
    pinMode(FAST_BELT_PIN, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(SHUTTER_PIN, OUTPUT); 
    digitalWrite(SHUTTER_PIN, LOW); 
    
    pinMode(IR_BEAM_SLOW, INPUT_PULLUP);
    pinMode(IR_BEAM_FAST, INPUT_PULLUP);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    myservo.attach(SERVO_PIN);
    myservo.write(ANGLES[4]); 

    for (int i = 0; i < NUM_BINS; i++) pinMode(BTN_BINS[i], INPUT_PULLUP);

    digitalWrite(SLOW_BELT_PIN, LOW);
    digitalWrite(FAST_BELT_PIN, LOW);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(500); }

    ArduinoOTA.begin(); 
    WebSerial.begin(&server);
    server.begin();

    runStartupSequence();
}

void loop() {
    ArduinoOTA.handle(); 

    // toggle button on/off
    if (digitalRead(BUTTON_PIN) == LOW) {
        delay(50); 
        if (digitalRead(BUTTON_PIN) == LOW) {
            systemActive = !systemActive; 
            
            if (systemActive) {
                isSlowBeltRunning = true;
                digitalWrite(SLOW_BELT_PIN, HIGH);
            } else {
                
                isSlowBeltRunning = false;
                isFastBeltRunning = false;
                waitingForClassification = false;
                digitalWrite(SLOW_BELT_PIN, LOW);
                digitalWrite(FAST_BELT_PIN, LOW);
                myservo.write(ANGLES[4]); 
                // Return to home
            }
            while(digitalRead(BUTTON_PIN) == LOW);
        }
    }

    if (!systemActive) return; 

    // sensr logic
    bool slowBeamBroken = (digitalRead(IR_BEAM_SLOW) == LOW); 
    bool fastBeamBroken = (digitalRead(IR_BEAM_FAST) == LOW); 

    // slow belt
    if (slowBeamBroken && !isFastBeltRunning && !waitingForClassification) {
        isFastBeltRunning = true;
        digitalWrite(FAST_BELT_PIN, HIGH);
    }

    // fast conveyor
    if (fastBeamBroken && !waitingForClassification) {
        isSlowBeltRunning = false;
        isFastBeltRunning = false;
        digitalWrite(SLOW_BELT_PIN, LOW);
        digitalWrite(FAST_BELT_PIN, LOW);

        digitalWrite(SHUTTER_PIN, HIGH);
        delay(10);
        digitalWrite(SHUTTER_PIN, LOW);
        
        waitingForClassification = true;
        WebSerial.println("Brick at Camera. Waiting for Python...");
    }

    // get bin
    if (waitingForClassification && Serial2.available()) {
        char c = Serial2.read();
        if (c >= '1' && c <= '4') {
            int binID = c - '0'; 
            sortBrick(binID);
        }
    }

    // debugging buttons
    for (int i = 0; i < NUM_BINS; i++) {
        if (digitalRead(BTN_BINS[i]) == LOW) {
            myservo.write(ANGLES[i]);
            delay(250);
        }
    }

}
