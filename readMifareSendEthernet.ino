
#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
#include <Ethernet.h>

#define ETH_SS 10
#define NFC_SS 9

PN532_SPI pn532spi(SPI, NFC_SS);
PN532 nfc(pn532spi);

uint32_t flowState =0;
unsigned long time;
unsigned long responseTime;

#define STATE_IDDLE 0
#define STATE_SENDDATA 15
#define STATE_RESPONSE 20

byte mac[] = {  0x90, 0xA2, 0xDA, 0x00, 0x00, 0x00 };
EthernetClient client;

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");
  
  pinMode (ETH_SS, OUTPUT); 
  pinMode (NFC_SS, OUTPUT);  
  digitalWrite(NFC_SS, LOW);
  digitalWrite(ETH_SS, HIGH);

  initPN532();
  initETH();
}

void initETH() {
  //Initialise Ethernet connection
  Serial.println("Start Ethernet");
  enableETH();
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // stop
    for(;;);
  }
  Serial.println("Start Ethernet done");  
}

void initPN532() {
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); 
  Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); 
  Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); 
  Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  Serial.println("Waiting for an ISO14443A Card ...");
}

void enablePN() {  
  digitalWrite(ETH_SS, HIGH);
  digitalWrite(NFC_SS, LOW);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(LSBFIRST);
  SPI.setClockDivider(SPCR & SPI_CLOCK_MASK);
  delay(10);
}

void enableETH() {
  digitalWrite(ETH_SS, LOW);
  digitalWrite(NFC_SS, HIGH);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4); 
  SPI.setDataMode(SPCR & SPI_MODE_MASK);
  SPCR &= ~(_BV(DORD));
  SPI.setClockDivider( SPCR & SPI_CLOCK_MASK);
  delay(10);
}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
    
  Serial.println("NFC Read");
  enablePN();
  nfc.begin();
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
   
    if (uidLength == 4) {
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
      enableETH();
      if (client.connect("192.168.10.105", 80)) {
        Serial.println("Connected. Making http request");
        client.println("GET http://192.168.10.105 HTTP/1.1");
        client.println(""); 
        client.println("");    
        flowState = STATE_RESPONSE;
      }  else {
        Serial.println("Connection to server failed");       
        flowState = STATE_IDDLE;
      } 
    }
  }

  if (flowState == STATE_RESPONSE) {
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
    if ((millis() - responseTime) > 2000) {
        Serial.println();
        Serial.println("Closing connection to server");
        client.stop();
        flowState = STATE_IDDLE;
    }
  }
}

