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
//#include "RTClib.h"
#include "NexaCtrl.h"

#define TX_PIN 13
#define RX_PIN 8

//NexaWrapper nexa(1, false); // Skapa en NexaWrapper med enhets-ID 1 i off-läge
//RTC_DS1307  rtc;            // En RTC-modul för klockan
NexaCtrl nexaCtrl(TX_PIN, RX_PIN);

const static unsigned long controller_id = 20677082;
const uint8_t num_of_input_characters = 3;
unsigned int device = 0;
unsigned int dim_level = 0;
bool sequence_read = false;

String delimiter = "/";
String inData;
//int incoming_num[num_of_input_characters] {99};
int c_incoming_num[num_of_input_characters] {0};
uint8_t index = 0;

//ASCII-codes
const uint8_t ascii_eol = 0x25; // '%'
const char ascii_delimiter = 0x2F;
const char ascii_zero = 0x30;
const char ascii_nine = 0x39;

void setup() {
    Serial.begin(9600);
    Serial.println("Setup complete!");
    for(int i=0; i<num_of_input_characters; i++) {c_incoming_num[i] = 99;}
}

void loop() {
    if (Serial.available() > 0) {
//         Serial.println("DEBUG");
        // Set all elements to 99 (= default value)
        //for(int i=0; i<num_of_input_characters; i++) {incoming_num[i] = 99;}
        
        while (Serial.available() > 0) {
            char received = Serial.read();
            Serial.println(received,HEX);
            if ( received != ascii_eol ) {
                
                if ( received == ascii_delimiter ) {
                    c_incoming_num[index] = inData.toInt();
                    inData = "";
                    index++;
                } else {
                    if ( received >= ascii_zero && received <= ascii_nine ) {    // Accept only integers
                        inData += received;
                    }
                }
                
            } else {
                Serial.println("DEBUG");
                if ( inData != "" ) {
                    c_incoming_num[index] = inData.toInt();
                    inData = "";
                }
                index = 0;
                sequence_read = true;
                break;
            }
        }
        
        // DEBUG print
        if (sequence_read) {
            Serial.print(c_incoming_num[0]);
            Serial.print(delimiter);
            Serial.print(c_incoming_num[1]);
            Serial.print(delimiter);
            Serial.print(c_incoming_num[2]);
            Serial.print("\n");
        }
        
        
//        uint8_t i = 0;
//        while (Serial.available() > 0) {
//            // read incoming string, terminated by timeout or '\n'
//            String incoming_str = Serial.readStringUntil('\n');
//            
//            // parse string to control data
//            // incoming_str: "device,mode,dim_level" <-- ints separated by comma
//            String token;
//            int pos = incoming_str.indexOf(delimiter);
//            int j = 0;
//            while ( pos != -1 ) {
//                token = incoming_str.substring(0, pos);
//                Serial.println(incoming_str);
//                incoming_num[j] = token.toInt();
//                incoming_str.remove( 0, pos+delimiter.length() );
//                pos = incoming_str.indexOf(delimiter);
//                j++;
//            }
//            incoming_num[j] = incoming_str.toInt();
//            i++;
//        }
        
        //DEBUG print
//        Serial.print("incoming_num = [");
//        for (int j = 0; j<num_of_input_characters; j++) {
//            Serial.print(incoming_num[j],DEC);
//            j < (num_of_input_characters-1) ? Serial.print(",") : Serial.println("]");
//        }
        if (sequence_read) {
            device = c_incoming_num[0]-1;
            dim_level = c_incoming_num[2];
            
            if (dim_level <= 15) { // A dim level was specified. Dim level in [0..15]
                nexaCtrl.DeviceDim(controller_id, device, dim_level);
            } else {
                switch (c_incoming_num[1]) {  // Check specified mode for device
                    case 0:
                        Serial.println("Sending device off");
                        nexaCtrl.DeviceOff(controller_id, device);
                        break;
                    case 1:
                        Serial.println("Sending device on");
                        nexaCtrl.DeviceOn(controller_id, device);
                        break;
                    default:
                        break;
                }
            }
            
            // Reset and get ready for next transfer
            for(int i=0; i<num_of_input_characters; i++) {c_incoming_num[i] = 99;}
            sequence_read = false;
        }
    }
//
}






