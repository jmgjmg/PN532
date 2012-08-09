
// The new PN532 library uses SPI HW implementation from official arduino SPI.h library
// The original PN532 library from seeedstudio uses a dedicated SPI SW implementation in PN532.h library 
//
// The benefit of HW approach is that MISO/MOSI/SCK PINs can be reused between NFC and Ethernet shields. 
// Chip Select (SS) PIN must be different for Ethernet and NFC shields.
// In fact, there are three different SS PINs: 
//      - Ethernet chip hardwired to PIN 10 in Ethernet shield
//      - SD card chip hardwired to PIN 4 in Ethernet shield
//      - NFC chip hardwired to PIN10 in NFC shield
//
// In order to avoid conflict between PIN10 in both shields when NFC shield is stacked on top of 
// Ethernet shield, male PIN 10 of NFC shield must be bent so that it does not
// connect into female PIN 10 of Ethernet shield headers
//
// Connection instructions:
// ========================
// Stack Ethernet shield on top of Arduino Uno (connect all PINs including ICSP)  
// Stack NFC shield on top of Ethernet shield bending male PIN 10 of NFC shield so that it does not connect
// into Ethernet shield
//
// Both official Ethernet and Seeedstudio NFC shields use the ICSP PINs for SPI protocol (instead of the 
// actual digital PINs). Since the NFC shield ICSP PINS are unconnected,
//  they need to be wired to the appropriate digital PINs in the NFC shield.
// This is the required wiring:
//
//   * Wire digital female PINs in NFC shield:
//            NFC_SS  (NFC)  -> 10 (NFC)  SS    
//      Make sure that you choose a free digital PIN for NFC_SS (do not use PINs 0,1,4,10,11,12,13)
//      In this sample code NFC_SS is defined as digital PIN 3
//
//   * Wire female ICSP PINs of NFC shield to the appropriate female PINs of NFC shield:
//            SCK  (ICSP NFC)  -> 13 (NFC)
//            MISO (ICSP NFC)  -> 12 (NFC)
//            MOSI (ICSP NFC)  -> 11 (NFC)
//
//   * Finally wire female ICSP Vcc PIN of NFC shield with the 5V power female PIN of NFC shield
//            Vcc  (ICSP NFC)  -> 5V (NFC) This is the 5 Volts power PIN (WARNING: it is NOT digital PIN 5 )
//
// SPI configuration parameters of PN532 chip (NFC) are different from the SPI parameters of 
// W5100 chip (Ethernet). 
// In particular PN532 uses LSBFIRST bit order while W5100 uses MSBFIRST bit order.
// Since both chips are reusing the same library and HW, we need to backup/restore the SPI configuration 
// everytime that we use NFC functionality from the new PN532 library
//
//
// KNOWN ISSUES
// ============
// The new library does not work when three SPI shields are stacked together (e.g. official Ethernet shield, 
// Seeedstudio NFC shield and Sparkfun MP3 shield). 
// I believe this is due to noise introduced in the system by the wiring. In this case, the official 
// Seeedstudio PN532.h library (SW SPI implementation) can still be used with the appropriate wiring but 
// then different PINs must be selected for 
// NFC_MISO, NFC_MOSI, NFC_SCK (SW library is not compatible with HW library so they must use 
// completely dissociated PIN sets)
//
//
// Example sketch
// ==============
// This sample code initialises NFC and Ethernet shields and then enters an infinite loop waiting for 
// the detection of NFC/RFID tags.
// When a tag is detected, its ID is read and then uploaded to Evrythng server in internet using its 
// http REST API.
// Note that in order to use this service, you must obtain your own credentials from Evrythng 
// (check instructions at evrythng.com). 
// Alternatively, you can use other similar services available in Internet.
//
// You have to retrieve your authentication token from https://evrythng.net/settings/tokens and update 
// its value in your Arduino code: 
//      client.println("X-Evrythng-Token: ...yourAPITokenHere...")
// You must also crate a new thng in evrytng with a property called ReadTag. Once created, you have 
// to update its thngId value in your Arduino code: 
//     client.println("PUT http://evrythng.net/thngs/...yourThingIdHere.../properties/ReadTag HTTP/1.1");
//
//
// License
// =======
// Copyright 2012 Javier Montaner
// This code is licensed under the Apache License, Version 2.0 (the "License"); you may not use this file 
// except in compliance with the License. 
// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software distributed under the License is 
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES 
// OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language 
// governing permissions and limitations under the License.
//
// The new PN532 library is licensed under the MIT license