#include <Arduino.h>

// Sketch for Swiss dwarfsignals
// Version 1.1 (19-10-16)  Arjan Mooldijk
// Thanks to Franz-Peter "MicroBahner" (Stummis) for the DCC examples
// Thanks to Alex Leone for the TLC library

#include <NmraDcc.h>
#include <Tlc5940.h>
#include <tlc_fades.h>

#define ZwergAdr1F    1 // Zwerg addr 1,      Fade    3 leds Fade !!STARTPIN!!
#define ZwergAdr2F    2 // Zwerg addr 2,      Fade    3 leds Fade
#define ZwergAdr1nF   3 // Zwerg addr 1,      no Fade 3 leds !!STARTPIN!!
#define ZwergAdr2nF   4 // Zwerg addr 2,      no Fade 3 leds

// Constants:
const unsigned int signalAdr[] =   {551, 552, 553, 554, 555, 556, 557, 558, 559, 560 };  // DCC-Signal Addresses
const byte signalType[]        =   {ZwergAdr1F,ZwergAdr2F,ZwergAdr1F,ZwergAdr2F,ZwergAdr1F,ZwergAdr2F,ZwergAdr1F,ZwergAdr2F,ZwergAdr1F,ZwergAdr2F};  // Signal typen per adres
const byte signalChannel[]     =   { 0, 0, 3, 3, 6, 6, 9, 9, 12, 12 };  // Eerste pin van het sein

const int dimConst [16] = {10,10,13,
                           10,10,15,
                           10,10,15,
                           10,10,15,
                           10,10,15,
                           0};      // Brightness per led (max 16) Every row is one signal
const int anZahl = sizeof(signalType);
const long fadeConst = 150;                // Fade in/decrease in mSec.
const long darkDelay = 350;                // Dark phase duration in mSec.
const long interval = 800;                 // interval to blink in mSec.


// Variables:
NmraDcc Dcc;
uint32_t startMillis;                       // millisecond variable for fade
uint32_t endMillis;                         // millisecond variable for fade
unsigned int LastAddr = 0;                  // last command address
uint32_t LastTime = 0;                      // will store last time packet received
byte lastBild[12] = {0};
boolean runSequence = true;                 // Switch to run sequence at startup
uint32_t lastSeq;                           // millisecond variable for sequence


///////////////////////////////////////////////////////////////
void setup() {
    Dcc.init( MAN_ID_DIY, 15, FLAGS_OUTPUT_ADDRESS_MODE | FLAGS_DCC_ACCESSORY_DECODER, 0 );
    Dcc.pin(0, 2, 1); // Dcc-Signal an Pin2 ( = Int0 );
    Tlc.init();
    Tlc.clear();                            // Set all leds off
    Tlc.update();                           // And write to Tlc5940
    Serial.begin(9600);
    while (!Serial) {}                      // wait for serial port to connect
    Serial.println("Zwerggignal decoder nr1");
    Serial.print("Adressen: ");
    Serial.println(anZahl);
    lastSeq = millis();

    for (byte x = 0; x < anZahl; x ++) {   // set all default Halt and register starting pin per signal
      switch (signalType[x]) {
        case ZwergAdr1F:
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.println(" Zwergsignal mit Fade");
          setZwergsignalF(signalChannel[x],0);
          break;
        case ZwergAdr1nF:
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.println(" Zwergsignal ohne Fade");
          setZwergsignalnF(signalChannel[x],0);
          break;
        default:
          break;
      }
    }
}
////////////////////////////////////////////////////////////////
void loop() {
  Dcc.process(); // Hier werden die empfangenen Telegramme analysiert

  tlc_updateFades(); //execute fade
  delay(1);

  if (runSequence) {
    setNextZwergBild();
  }
}
//////////////////////////////////////////////////////////////
void notifyDccAccState( uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State ){
  for ( byte index = 0; index < anZahl; index++ ) {
    if (Addr == signalAdr[index]) {                     // ist eigene Adresse
      if ((Addr != LastAddr) || ( LastTime < millis() - 200)) {
        switch (signalType[index]) {
          case ZwergAdr1F:                                       //Zwergsignal Addr 1 Fahrt & Halt Fade
            startMillis = millis() + 1;
            dunkelZwergsignal(signalChannel[index]);
            if ( OutputAddr & 0x1 ) {
              setZwergsignalF(signalChannel[index], 1);
              lastBild[signalChannel[index]] = 1;
              break;
            }
            else {
              setZwergsignalF(signalChannel[index], 0);
              lastBild[signalChannel[index]] = 0;
              break;
            }
          case ZwergAdr2F:                                       //Zwergsignal Addr 2 Fahrt mit Vorsicht Fade
            startMillis = millis() + 1;
            dunkelZwergsignal(signalChannel[index]);
            if ( OutputAddr & 0x1 ) {
              setZwergsignalF(signalChannel[index], 2);
              lastBild[signalChannel[index]] = 2;
              break;
            }
            else {
              if (lastBild[signalChannel[index]] == 0) {
                setZwergsignalF(signalChannel[index], 3);
                lastBild[signalChannel[index]] = 3;
                break;
              }
              else {
                if (lastBild[signalChannel[index]] == 2) {
                  setZwergsignalF(signalChannel[index], 1);
                  lastBild[signalChannel[index]] = 1;
                  break;
                }
                else {
                  setZwergsignalF(signalChannel[index], 1);
                  break;
                }
              }
            }
          case ZwergAdr1nF:                                       //Zwergsignal Addr 1 Fahrt & Halt noFade
            if ( OutputAddr & 0x1 ) {
              setZwergsignalnF(signalChannel[index], 1);
              break;
            }
            else {
              setZwergsignalnF(signalChannel[index], 0);
              break;
            }
          case ZwergAdr2nF:                                       //Zwergsignal Addr 2 Fahrt mit Vorsicht noFade
            setZwergsignalnF(signalChannel[index], 2);
            break;
          default:
            break;
        }
        LastAddr = Addr;
        LastTime = millis();
        runSequence = false;
      }
    }
  }
}
//////////////////////////////// Zwergsignal Sequence ///////////////////////////////////////////////////////////////////////
void setNextZwergBild() {
  if (millis()-lastSeq>3000){
    if (lastBild[0] == 0 ){
      startMillis = millis() + 1;
      dunkelZwergsignal(0);
      setZwergsignalF(0, 2);
      lastBild[0] = 2;
    }
    else {
      if (lastBild[0] == 2 ){
        startMillis = millis() + 1;
        dunkelZwergsignal(0);
        setZwergsignalF(0, 1);
        lastBild[0] = 1;
      }
      else {
        if (lastBild[0] == 1 ){
          startMillis = millis() + 1;
          dunkelZwergsignal(0);
          setZwergsignalF(0, 0);
          lastBild[0] = 0;
        }
      }
    }
    lastSeq = millis();
  }
}


//////////////////////////////// Zwergsignal Fade ///////////////////////////////////////////////////////////////////////
void dunkelZwergsignal(byte startPin) {
  endMillis = startMillis + fadeConst;
  for (byte x=0; x<3; x++){
    if (Tlc.get(startPin+x) > 0) {
      tlc_addFade(startPin+x, dimConst[startPin+x], 0, startMillis, endMillis);                 //If not off then fade out
    }
  }
  startMillis = endMillis + darkDelay;
}
//////////////
void setZwergsignalF(byte startPin, byte Fb){
//ch1 links unter
//ch2 rechts unter
//ch3 oben
 switch (Fb){
    case 0: // zet ch1 en ch2 aan en ch3 uit
       endMillis = startMillis + fadeConst * 2;
       tlc_addFade(startPin+0, 0, dimConst[startPin+0], startMillis, endMillis);
       tlc_addFade(startPin+1, 0, dimConst[startPin+1], startMillis, endMillis);
       break;
    case 1: // zet ch1 en ch3 aan, ch2 uit
       endMillis = startMillis + fadeConst * 2;
       tlc_addFade(startPin+0, 0, dimConst[startPin+0], startMillis, endMillis);
       tlc_addFade(startPin+2, 0, dimConst[startPin+2], startMillis, endMillis);
       break;
    case 2: // zet ch1 uit en ch2, ch3 aan
       endMillis = startMillis + fadeConst * 2;
       tlc_addFade(startPin+1, 0, dimConst[startPin+1], startMillis, endMillis);
       tlc_addFade(startPin+2, 0, dimConst[startPin+2], startMillis, endMillis);
       break;
    case 3: // van Fb0  via Fb2 naar Fb1
       endMillis = startMillis + fadeConst * 2;
       tlc_addFade(startPin+1, 0, dimConst[startPin+1], startMillis, endMillis);
       tlc_addFade(startPin+2, 0, dimConst[startPin+2], startMillis, endMillis);
       startMillis = endMillis + darkDelay;
       endMillis = startMillis + fadeConst;
       tlc_addFade(startPin+1, dimConst[startPin+1], 0, startMillis, endMillis);
       tlc_addFade(startPin+2, dimConst[startPin+2], 0, startMillis, endMillis);
       startMillis = endMillis + darkDelay;
       endMillis = startMillis + fadeConst * 2;
       tlc_addFade(startPin+0, 0, dimConst[startPin+0], startMillis, endMillis);
       tlc_addFade(startPin+2, 0, dimConst[startPin+2], startMillis, endMillis);
       break;
    default:
       break;
       }
}
//////////////////////////////////Zwergsignal no Fade //////////////////////////////////////////////////////////
void setZwergsignalnF(byte startPin, byte Fb){
//ch1 links unter
//ch2 rechts unter
//ch3 oben
 switch (Fb){
    case 0: // zet ch1 en ch2 aan en ch3 uit
//       Serial.println(" Halt");
       Tlc.set(startPin+0, dimConst[startPin]);
       Tlc.set(startPin+1, dimConst[startPin+1]);
       Tlc.set(startPin+2, 0);
       break;
    case 1: // zet ch1 en ch3 aan, ch2 uit
//       Serial.println(" Fahrt");
       Tlc.set(startPin+0, dimConst[startPin]);
       Tlc.set(startPin+1, 0);
       Tlc.set(startPin+2, dimConst[startPin+2]);
       break;
    case 2: // zet ch1 uit en ch2, ch3 aan
//       Serial.println(" Fahrt mit Vorsicht");
       Tlc.set(startPin+0, 0);
       Tlc.set(startPin+1, dimConst[startPin+1]);
       Tlc.set(startPin+2, dimConst[startPin+2]);
       break;
    default:
       break;
       }
  Tlc.update();
}
//////////////////////
