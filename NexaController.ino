//
// NexaController
//
// Controll Nexa lights in combination with LCD screen
// Developed with [embedXcode](http://embedXcode.weebly.com)
//
// Author 		Karl Svensson
// 				Kalle Svensson
//
// Date			2016-08-03 15:17
// Version		<#version#>
//
// Copyright	© Karl Svensson, 2016
// Licence		<#licence#>
//
// See         ReadMe.txt for references
//

//#include "NexaWrapper.h"
#include "RTClib.h"
#include "NexaCtrl.h"

#define TX_PIN 13
#define RX_PIN 8

//NexaWrapper nexa(1, false); // Skapa en NexaWrapper med enhets-ID 1 i off-läge
RTC_DS1307  rtc;            // En RTC-modul för klockan
NexaCtrl nexaCtrl(TX_PIN, RX_PIN);

const static unsigned long controller_id = 20677082;
unsigned int device = 0;
const uint8_t num_of_input_characters = 4;
int incoming_num[num_of_input_characters] = {0};
int incoming_msg = 0;
int typing_delay_ms = 300;


void setup() {
    Serial.begin(9600);
    
    /**
     * Initiera RTC-klockan. Om detta inte funkar skrivs feltexter ut. Kalibrera slutligen klockan.
     */
    
    /**
    if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }
    if (! rtc.isrunning()) {
        Serial.println("RTC is NOT running!");
        while (1);
    }
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // set RTC to date & time this sketch was compiled
    */
    /**
     * Kan också ställas mha:
     * DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
     * t.ex: rtc.adjust(DateTime(2016,8,8,13,43,30)); // verifierad som fungerande exempel
     */
    
    Serial.println("Setup complete!");
}

void loop() {
//    DateTime now = rtc.now();
//    Serial.print(now.hour(), DEC);
//    Serial.print(":");
//    Serial.print(now.minute(), DEC);
//    Serial.print(":");
//    Serial.println(now.second(), DEC);
    
    /*Dim light*/
//    nexaCtrl.DeviceOff(controller_id, device);
//    delay(1000);
//    for (int i = 0; i<=15; i++) {
//        nexaCtrl.DeviceDim(controller_id, device, i);  // 0 <= dimLevel <= 15
//        Serial.print("Dim level:");
//        Serial.println(i);
//        delay(1000);
//    }
    
    if (Serial.available() > 0) {
        for(int i=0; i<num_of_input_characters; i++) {incoming_num[i] = 0;}
        uint8_t i = 0;
        while (Serial.available() > 0) {
            // read incoming byte
            incoming_msg = Serial.read();
            
            incoming_num[i] = incoming_msg-48;
            
            delay(typing_delay_ms);
            i++;
        }
        
        //DEBUG print
//        Serial.print("incoming_num = [");
//        for (int j = 0; j<num_of_input_characters; j++) {
//            Serial.print(incoming_num[j],DEC);
//            j < (num_of_input_characters-1) ? Serial.print(",") : Serial.println("]");
//        }
        
        device = incoming_num[0]-1;
        
        switch (incoming_num[1]) {
            case 0:
                nexaCtrl.DeviceOff(controller_id, device);
                break;
            case 1:
                nexaCtrl.DeviceOn(controller_id, device);
                break;
                
            default:
                break;
        }
    }
//
}
