#include <Arduino.h>

// Sketch voor Zwitserse signalen
// Version 2.2 (30-10-16)  Arjan Mooldijk
// Thanks to Franz-Peter "MicroBahner" (Stummis) for the DCC examples
// Thanks to Alex Leone for the TLC library

#include <NmraDcc.h>
#include <Tlc5940.h>
#include <tlc_fades.h>

#define VorAdr1F      1 // Vorsignal addr 1,  Fade    Voorsignaal, tweede adres  beelden Fb2 & Fb3
#define VorAdr2F      2 // Vorsignal addr 2,  Fade    Voorsignaal, tweede adres  beelden Fb2 & Fb3
#define VorAdr3F      3 // Vorsignal addr 3,  Fade    Voorsignaal, derde adres  beeld Fb5
#define VorAdr1nF     4 // Vorsignal addr 1,  no Fade Voorsignaal, eerste adres  beelden Warnung & Fb1
#define VorAdr2nF     5 // Vorsignal addr 2,  no Fade Voorsignaal, tweede adres  beelden Fb2 & Fb3
#define VorAdr3nF     6 // Vorsignal addr 3,  no Fade Voorsignaal, derde adres  beeld Fb5
//
#define ZwergAdr1F    7 // Zwerg addr 1,      Fade    Dwersignaal, eerste adres  beelden Halt & Fahrt
#define ZwergAdr2F    8 // Zwerg addr 2,      Fade    Dwersignaal, tweede adres  beeld Fahrt mit Vorsicht
#define ZwergAdr1nF   9 // Zwerg addr 1,      no Fade Dwersignaal, eerste adres  beelden Halt & Fahrt
#define ZwergAdr2nF  10 // Zwerg addr 2,      no Fade Dwersignaal, tweede adres  beeld Fahrt mit Vorsicht
//
#define SIMHaupt     11 // SIM Haupt,         SIM hoofdsein
#define SIMVor       12 // SIM Vor,           SIM voorsein
//
#define HauptAdr1F   13 // Haupt addr 1,      Fade    Hoofdsignaal eerste adres beelden Fb0 & Fb1
#define HptAdr2Fb2F  14 // Haupt addr 2,      Fade    Hoofdsignaal tweede adres beeld Fb2 (3-licht sein)
#define HptAdr2Fb3F  15 // Haupt addr 2,      Fade    Hoofdsignaal tweede adres beeld Fb3 (3-licht sein)
#define HauptAdr2F   16 // Haupt addr 2,      Fade    Hoofdsignaal tweede adres beeld Fb2 & Fb3 (4/5/6-licht sein)
#define HptAdr3Fb6F  17 // Haupt addr 3,      Fade    Hoofdsignaal derde adres beeld Fb6 (5-licht sein)
#define HauptAdr3F   18 // Haupt addr 3,      Fade    Hoofdsignaal derde adres beeld Fb5 & Fb6 (6-licht sein)
#define HauptAdr1nF  19 // Haupt addr 1,      no Fade Hoofdsignaal eerste adres beelden Fb0 & Fb1
#define HptAdr2Fb2nF 20 // Haupt addr 2,      no Fade Hoofdsignaal tweede adres beeld Fb2 (3-licht sein)
#define HptAdr2Fb3nF 21 // Haupt addr 2,      no Fade Hoofdsignaal tweede adres beeld Fb3 (3-licht sein)
#define HauptAdr2nF  22 // Haupt addr 2,      no Fade Hoofdsignaal tweede adres beeld Fb2 & Fb3 (4/5/6-licht sein)
#define HptAdr3Fb6nF 23 // Haupt addr 3,      no Fade Hoofdsignaal derde adres beeld Fb6 (5-licht sein)
#define HauptAdr3nF  24 // Haupt addr 3,      no Fade Hoofdsignaal derde adres beeld Fb5 & Fb6 (6-licht sein)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AAN TE PASSEN CONSTANTEN ///////////////////////////////////////////////////////////////////////////////////////////
// Decoder nummer. Dit wordt in de decoder zelf opgeslagen en is met de monitor uit te lezen.                       ///
// Dan weet je welke het is.                                                                                        ///
const byte decoderNbr = 1;                                                                                          ///
                                                                                                                    ///
// DCC adressen:                                                                                                    ///
const unsigned int signalAdr[] =   { 100, 101, 102, 103, 104, 105};                                                 ///
                                                                                                                    ///
// Sein types per hierboven opgegeven adres                                                                         ///
const byte signalType[]        =   { VorAdr1F, VorAdr2F, HauptAdr1F, HauptAdr2F, ZwergAdr1F, ZwergAdr2F};           ///
                                                                                                                    ///
// Lichtsterkte van de leds per sein in volgorde van de opgegeven types                                             ///
const int dimConst [16] = {40,40,20,20,
                           20,40,20,40,
                           10,10,10,
                           0,0,0,0,0};       // Brightness per led (max 16) Every row is one signal                 ///
                                                                                                                    ///
                                                                                                                    ///
const int anZahl = sizeof(signalType);                                                                              ///
const long fadeConst = 150;                 // Fade in/decrease in mSec.                                            ///
const long darkDelay = 350;                 // Dark phase duration in mSec.                                         ///
const long interval = 800;                  // interval to blink in mSec.                                           ///
///////// HIERONDER HOEFT NIETS MEER AANGEPAST TE WORDEN (TENZIJ JE HET PROGRAMMA WILT VERANDEREN ;-) /////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NmraDcc Dcc;
byte signalLeds[16];                          // # leds per signal
byte signalChannel[16];                       // Eerste pin per het sein
byte Blink [16] = {0};                      // Flag to blink pin (for SIM)
byte blinkState [16] = {0};                 // Blink on/off
unsigned long previousMillis[16]= {0};      // will store last time LED was updated
byte pinCounter = 0;                            // Variable to address the right pin on the TLC5940
uint32_t startMillis;                       // millisecond variable for fade
uint32_t endMillis;                         // millisecond variable for fade
unsigned int LastAddr = 0;                  // last command address
unsigned long LastTime = 0;                 // will store last time packet received
byte lastBild[12] = {0};                    // last signal bild for Zwerg


///////////////////////////////////////////////////////////////
void setup() {
    Dcc.init( MAN_ID_DIY, 15, FLAGS_OUTPUT_ADDRESS_MODE | FLAGS_DCC_ACCESSORY_DECODER, 0 );
    Dcc.pin(0, 2, 1); // Dcc-Signal an Pin2 ( = Int0 );
    Tlc.init();
    Tlc.clear();                            // Set all leds off
    Tlc.update();                           // And write to Tlc5940
    Serial.begin(9600);
    while (!Serial) {}                      // wait for serial port to connect
    Serial.print("Signal decoder nr ");
    Serial.println(decoderNbr);
    Serial.print("Aantal adressen: ");
    Serial.println(anZahl);

    for (byte x = 0; x < anZahl; x ++) {   // set all default Halt and register starting pin per signal
      switch (signalType[x]) {
        case VorAdr1F:
          if (signalType[x+1]==VorAdr2F) {
            if (signalType[x+2]==VorAdr3F) {
              signalLeds[x] = 5;
              signalLeds[x+1] = 5;
              signalLeds[x+2] = 5;
              signalChannel[x] = pinCounter;
              signalChannel[x+1] = pinCounter;
              signalChannel[x+2] = pinCounter;
              pinCounter = pinCounter + 5;
            }
            else {
              signalLeds[x] = 4;
              signalLeds[x+1] = 4;
              signalChannel[x] = pinCounter;
              signalChannel[x+1] = pinCounter;
              pinCounter = pinCounter + 4;
            }}
          else {
            signalLeds[x] = 4;
            signalChannel[x] = pinCounter;
            pinCounter = pinCounter + 4;
          }
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Vorsignal Fade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setFb0VorsignalF(x);
          break;

        case VorAdr1nF:
          if (signalType[x+1]==VorAdr2nF) {
            if (signalType[x+2]==VorAdr3nF) {
              signalLeds[x] = 5;
              signalLeds[x+1] = 5;
              signalLeds[x+2] = 5;
              signalChannel[x] = pinCounter;
              signalChannel[x+1] = pinCounter;
              signalChannel[x+2] = pinCounter;
              pinCounter = pinCounter + 5;
            }
            else {
              signalLeds[x] = 4;
              signalLeds[x+1] = 4;
              signalChannel[x] = pinCounter;
              signalChannel[x+1] = pinCounter;
              pinCounter = pinCounter + 4;
            }}
          else {
            signalLeds[x] = 4;
            signalChannel[x] = pinCounter;
            pinCounter = pinCounter + 4;
          }
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Vorsignal noFade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setFb0VorsignalnF(x);
          break;

        case ZwergAdr1F:
          signalLeds[x] = 3;
          signalLeds[x+1] = 3;
          signalChannel[x] = pinCounter;
          signalChannel[x+1] = pinCounter;
          pinCounter = pinCounter + 3;
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Zwergsignal Fade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setZwergsignalF(x,0);
          break;

        case ZwergAdr1nF:
          signalLeds[x] = 3;
          signalLeds[x+1] = 3;
          signalChannel[x] = pinCounter;
          signalChannel[x+1] = pinCounter;
          pinCounter = pinCounter + 3;Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Zwergsignal noFade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setZwergsignalnF(x,0);
          break;

        case SIMHaupt:
          signalLeds[x] = 1;
          signalChannel[x] = pinCounter;
          pinCounter = pinCounter + 1;
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.println(" SIM Hauptsignal");
          setSIMsignal(x,0);
          break;

        case SIMVor:
          signalLeds[x] = 1;
          signalChannel[x] = pinCounter;
          pinCounter = pinCounter + 1;
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.println(" SIM Vorsignal");
          setSIMsignal(x,0);
          break;

        case HauptAdr1F:
          if (signalType[x+1]== HptAdr2Fb2F || signalType[x+1]== HptAdr2Fb3F ) {
            signalLeds[x] = 3;
            signalLeds[x+1] = 3;
            signalChannel[x] = pinCounter;
            signalChannel[x+1] = pinCounter;
            pinCounter = pinCounter + 3;
          }
          else {
            if (signalType[x+1]== HauptAdr2F) {
              if (signalType[x+2]== HptAdr3Fb6F) {
                signalLeds[x] = 5;
                signalLeds[x+1] = 5;
                signalLeds[x+2] = 5;
                signalChannel[x] = pinCounter;
                signalChannel[x+1] = pinCounter;
                signalChannel[x+2] = pinCounter;
                pinCounter = pinCounter + 5;
              }
              else {
                if (signalType[x+2]== HauptAdr3F) {
                  signalLeds[x] = 6;
                  signalLeds[x+1] = 6;
                  signalLeds[x+2] = 6;
                  signalChannel[x] = pinCounter;
                  signalChannel[x+1] = pinCounter;
                  signalChannel[x+2] = pinCounter;
                  pinCounter = pinCounter + 6;
                }
                else {
                  signalLeds[x] = 4;
                  signalLeds[x+1] = 4;
                  signalChannel[x] = pinCounter;
                  signalChannel[x+1] = pinCounter;
                  pinCounter = pinCounter + 4;
                }
              }}
              else {
                signalLeds[x] = 2;
                signalChannel[x] = pinCounter;
                pinCounter = pinCounter + 2;
              }}
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Hauptsignal Fade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setFb0HauptsignalF(x);
          break;

        case HauptAdr1nF:
          if (signalType[x+1]== HptAdr2Fb2nF || signalType[x+1]== HptAdr2Fb3nF ) {
            signalLeds[x] = 3;
            signalLeds[x+1] = 3;
            signalChannel[x] = pinCounter;
            signalChannel[x+1] = pinCounter;
            pinCounter = pinCounter + 3;
          }
          else {
            if (signalType[x+1]== HauptAdr2nF) {
              if (signalType[x+2]== HptAdr3Fb6nF) {
                signalLeds[x] = 5;
                signalLeds[x+1] = 5;
                signalLeds[x+2] = 5;
                signalChannel[x] = pinCounter;
                signalChannel[x+1] = pinCounter;
                signalChannel[x+2] = pinCounter;
                pinCounter = pinCounter + 5;
              }
              else {
                if (signalType[x+2]== HauptAdr3nF) {
                  signalLeds[x] = 6;
                  signalLeds[x+1] = 6;
                  signalLeds[x+2] = 6;
                  signalChannel[x] = pinCounter;
                  signalChannel[x+1] = pinCounter;
                  signalChannel[x+2] = pinCounter;
                  pinCounter = pinCounter + 6;
                }
                else {
                  signalLeds[x] = 4;
                  signalLeds[x+1] = 4;
                  signalChannel[x] = pinCounter;
                  signalChannel[x+1] = pinCounter;
                  pinCounter = pinCounter + 4;
                }
              }}
              else {
                signalLeds[x] = 2;
                signalChannel[x] = pinCounter;
                pinCounter = pinCounter + 2;
              }}
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Hauptsignal noFade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setFb0HauptsignalnF(x);
          break;
        default:
          break;
        }
    }
}
////////////////////////////////////////////////////////////////
void loop() {
  Dcc.process(); // Hier werden die empfangenen Telegramme analysiert

  //execute fade
  delay(1);
  tlc_updateFades();

  //blink for SIM
  unsigned long currentMillis = millis();
  for ( byte blindex = 0; blindex < 16; blindex++ ) {
    if (Blink[blindex]==1){
      if (currentMillis - previousMillis[blindex] >= interval) {
        previousMillis[blindex] = currentMillis;
        if (blinkState[blindex]==1) {
          Tlc.set(blindex,0);
          blinkState[blindex] = 0;
        }
        else {
          Tlc.set(blindex,dimConst[blindex]);
          blinkState[blindex] = 1;
        }
        Tlc.update();
      }
    }
  }
}
//////////////////////////////////////////////////////////////
// Unterprogramme, die von der DCC Library aufgerufen werden:
// Die folgende Funktion wird von Dcc.process() aufgerufen, wenn ein Weichentelegramm empfangen wurde
void notifyDccAccState( uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State ){
  for ( byte index = 0; index < anZahl; index++ ) {
    if (Addr == signalAdr[index]) {                     // ist eigene Adresse
      if ((Addr != LastAddr) || ( LastTime < millis() - 200)) {
        switch (signalType[index]) {
          
//Vorsignal Addr 1 Fb1 & Fb0 Fade
          case VorAdr1F :
            startMillis = millis() + 1;
            dunkelVorsignal(index);
            if ( OutputAddr & 0x1 ) {
              setFb1VorsignalF(index);
              break; }
            else {
              setFb0VorsignalF(index);
              break;
            }

//Vorsignal Addr 2 Fb3 & Fb2 Fade
          case VorAdr2F:
            startMillis = millis() + 1;
            dunkelVorsignal(index);
            if ( OutputAddr & 0x1 ) {
              setFb3VorsignalF(index);
              break;
            }
            else {
              setFb2VorsignalF(index);
              break;
            }

//Vorsignal Addr 3 Fb5 Fade
          case VorAdr3F:
            startMillis = millis() + 1;
            dunkelVorsignal(index);
            setFb5VorsignalF(index);
            break;

//Vorsignal Addr 1 Fb1 & Fb0 noFade
          case VorAdr1nF :
            if ( OutputAddr & 0x1 ) {
              setFb1VorsignalnF(index);
              break; }
            else {
              setFb0VorsignalnF(index);
              break;
            }

//Vorsignal Addr 2 Fb3 & Fb2 noFade
          case VorAdr2nF:
            if ( OutputAddr & 0x1 ) {
              setFb3VorsignalnF(index);
              break;
            }
            else {
              setFb2VorsignalnF(index);
              break;
            }

//Vorsignal Addr 3 Fb5 noFade
          case VorAdr3nF:
            setFb5VorsignalnF(index);
            break;

//Zwergsignal Addr 1 Fahrt & Halt Fade
          case ZwergAdr1F:
            startMillis = millis() + 1;
            dunkelZwergsignal(index);
            if ( OutputAddr & 0x1 ) {
              setZwergsignalF(index, 1);
              lastBild[signalChannel[index]] = 1;
              break;
            }
            else {
              setZwergsignalF(index, 0);
              lastBild[signalChannel[index]] = 0;
              break;
            }

//Zwergsignal Addr 2 Fahrt mit Vorsicht Fade
          case ZwergAdr2F:
            startMillis = millis() + 1;
            dunkelZwergsignal(index);
            if ( OutputAddr & 0x1 ) {
              setZwergsignalF(index, 2);
              lastBild[signalChannel[index]] = 2;
              break;
            }
            else {
              if (lastBild[signalChannel[index]] == 0) {
                setZwergsignalF(index, 3);
                lastBild[signalChannel[index]] = 3;
                break;
              }
              else {
                if (lastBild[signalChannel[index]] == 2) {
                  setZwergsignalF(index, 1);
                  lastBild[signalChannel[index]] = 1;
                  break;
                }
                else {
                  setZwergsignalF(index, 1);
                  break;
                }
              }
            }

//Zwergsignal Addr 1 Fahrt & Halt noFade
          case ZwergAdr1nF:
            if ( OutputAddr & 0x1 ) {
              setZwergsignalnF(index, 2);
              delay(500);
              setZwergsignalnF(index, 1);
              break;
            }
            else {
              setZwergsignalnF(index, 0);
              break;
            }

//Zwergsignal Addr 2 Fahrt mit Vorsicht noFade
          case ZwergAdr2nF:
            setZwergsignalnF(index, 2);
            break;

//SIMhauptsignal An & Aus
          case SIMHaupt:
            if ( OutputAddr & 0x1 ) {
              setSIMsignal(index, 2);
              break;
            }
            else {
              setSIMsignal(index, 0);
              break;
            }

//SIMvorsignal An & Aus
          case SIMVor:
            if ( OutputAddr & 0x1 ) {
              setSIMsignal(index, 1);
              break;
            }
            else {
            setSIMsignal(index, 0);
            break;
            }

//Hauptsignal Addr Fb1 & Fb0 Fade
          case HauptAdr1F:
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            if ( OutputAddr & 0x1 ) {
              setFb1HauptsignalF(index);
              break;
            }
            else {
              setFb0HauptsignalF(index);
              break;
              }

//Hauptsignal Addr 2 Fb2 Fade
          case HptAdr2Fb2F:
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            setFb2HauptsignalF3(index);
            break;

//Hauptsignal Addr 2 Fb3 Fade
          case HptAdr2Fb3F:
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            setFb3HauptsignalF3(index);
            break;

//Hauptsignal Addr 2 Fb3 & Fb2 Fade
          case HauptAdr2F:
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            if ( OutputAddr & 0x1 ) {
              setFb3HauptsignalF(index);
              break;
            }
            else {
                setFb2HauptsignalF(index);
              break;
            }

//Hauptsignal Addr 3 Fb6 Fade
          case HptAdr3Fb6F:
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            setFb6HauptsignalF(index);
            break;

//Hauptsignal Addr 3 Fb6 & Fb5 Fade
          case HauptAdr3F:
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            if ( OutputAddr & 0x1 ) {
              setFb6HauptsignalF(index);
              break;
            }
            else {
              setFb5HauptsignalF(index);
              break;
            }
            break;

//Hauptsignal Addr Fb1 & Fb0 noFade
          case HauptAdr1nF:
            startMillis = millis() + 1;
            if ( OutputAddr & 0x1 ) {
              setFb1HauptsignalnF(index);
              break;
            }
            else {
              setFb0HauptsignalnF(index);
              break;
              }

//Hauptsignal Addr 2 Fb2 noFade
          case HptAdr2Fb2nF:
            startMillis = millis() + 1;
            setFb2HauptsignalnF3(index);
            break;

//Hauptsignal Addr 2 Fb3 noFade
          case HptAdr2Fb3nF:
            startMillis = millis() + 1;
            setFb3HauptsignalnF3(index);
            break;

//Hauptsignal Addr 2 Fb3 & Fb2 noFade
          case HauptAdr2nF:
            startMillis = millis() + 1;
            if ( OutputAddr & 0x1 ) {
              setFb3HauptsignalnF(index);
              break;
            }
            else {
                setFb2HauptsignalnF(index);
              break;
            }

//Hauptsignal Addr 3 Fb6 noFade
          case HptAdr3Fb6nF:
            startMillis = millis() + 1;
            setFb6HauptsignalnF(index);
            break;

//Hauptsignal Addr 3 Fb6 & Fb5 noFade
          case HauptAdr3nF:
            startMillis = millis() + 1;
            if ( OutputAddr & 0x1 ) {
              setFb6HauptsignalnF(index);
              break;
            }
            else {
              setFb5HauptsignalnF(index);
              break;
            }
            break;
          default:
            break;
        }
        LastAddr = Addr;
        LastTime = millis();
      }
    }
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void setSIMsignal (byte pntr, byte Fb){
 switch (Fb){
    case 0: // zet ch1 uit
       Blink[signalChannel[pntr]] = 0;
       Tlc.set(signalChannel[pntr], 0);
       break;
    case 1: // zet ch1 blink aan
       Blink[signalChannel[pntr]] = 1;
       Tlc.set(signalChannel[pntr], dimConst[signalChannel[pntr]]);
       break;
    case 2: // zet ch1 aan
       Tlc.set(signalChannel[pntr], dimConst[signalChannel[pntr]]);
       break;
    default:
       break;
       }
  Tlc.update();
}
////////////////////////////////////// Vorsignal Fade/////////////////////////////////////////////////////////////////
//Signalbilder sind Warnung (Gelb1, Gelb2), Fb1 (Grün1, Grün2), Fb2 (Gelb1, Grün1), Fb3 (Gelb1, Grün1, Grün2), Fb5 (Grün1, Grün2, Gelb3)
//ch1 = gelb (links)
//ch2 = gelb (rechts oben)
//ch3 = grun (rechts)
//ch4 = grun (links)
//ch5 = gelb (rechts unter)
//////////////
void dunkelVorsignal(byte pntr) {
  endMillis = startMillis + fadeConst;
  for (byte x=0; x<signalLeds[pntr]; x++){
    if (Tlc.get(signalChannel[pntr]+x) > 0) {
      tlc_addFade(signalChannel[pntr]+x, dimConst[signalChannel[pntr]+x], 0, startMillis, endMillis);                 //If not off then fade out
    }
  }
  startMillis = endMillis + darkDelay;
}
/////////////
void setFb0VorsignalF(byte pntr) {
  // led 1&2 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr], 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
}
/////////////
void setFb1VorsignalF(byte pntr) {
  // led 3&4 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
}
/////////////
void setFb2VorsignalF(byte pntr) {
  // led 1&3 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr], 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
}
/////////////
void setFb3VorsignalF(byte pntr) {

  // led 1&3&4 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr], 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
}
/////////////
void setFb5VorsignalF(byte pntr) {
  // led 3&4&5 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+4, 0, dimConst[signalChannel[pntr]+4], startMillis, endMillis);
}

////////////////////////////////////// Vorsignal noFade/////////////////////////////////////////////////////////////////
//Signalbilder sind Warnung (Gelb1, Gelb2), Fb1 (Grün1, Grün2), Fb2 (Gelb1, Grün1), Fb3 (Gelb1, Grün1, Grün2), Fb5 (Grün1, Grün2, Gelb3)
//ch1 = gelb (links)
//ch2 = gelb (rechts oben)
//ch3 = grun (rechts)
//ch4 = grun (links)
//ch5 = gelb (rechts unter)
//////////////
void setFb0VorsignalnF(byte pntr) {
  // led 1&2 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]+0]);
  Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
  Tlc.set(signalChannel[pntr]+2, 0);
  Tlc.set(signalChannel[pntr]+3, 0);
  if (signalLeds[pntr]==5) {Tlc.set(signalChannel[pntr]+4, 0);}
  Tlc.update();
}
/////////////
void setFb1VorsignalnF(byte pntr) {
  // led 3&4 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
  if (signalLeds[pntr]==5) {Tlc.set(signalChannel[pntr]+4, 0);}
  Tlc.update();
}
/////////////
void setFb2VorsignalnF(byte pntr) {
  // led 1&3 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]+0]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, 0);
  if (signalLeds[pntr]==5) {Tlc.set(signalChannel[pntr]+4, 0);}
  Tlc.update();
}
/////////////
void setFb3VorsignalnF(byte pntr) {
  // led 1&3&4 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]+0]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
  if (signalLeds[pntr]==5) {Tlc.set(signalChannel[pntr]+4, 0);}
  Tlc.update();
}
/////////////
void setFb5VorsignalnF(byte pntr) {
  // led 3&4&5 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
  Tlc.set(signalChannel[pntr]+4, dimConst[signalChannel[pntr]+4]);
  Tlc.update();
}

/////////////////////////////////////// Hauptsignal Fade ////////////////////////////////////////////////////////////////
  //lamp 1 = grun (hoch)
  //lamp 2 = rot
  //lamp 3 = grun of gelb (3/4-licht)
  //lamp 4 = groen 
  //lamp 5 = geel 
  //lamp 6 = groen 
//////////////
void dunkelHauptsignal(byte pntr) {
  endMillis = startMillis + fadeConst;
  for (byte x=0; x<signalLeds[pntr]; x++){
    if (Tlc.get(signalChannel[pntr]+x) > 0) tlc_addFade(signalChannel[pntr]+x, dimConst[signalChannel[pntr]+x], 0, startMillis, endMillis); //If not off then fade out
  }
  startMillis = endMillis + darkDelay;
}
/////////////////
void setFb0HauptsignalF(byte pntr) {
  // led 2 (x=1) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
}
/////////////
void setFb1HauptsignalF(byte pntr) {
  // led 1 (x=0) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
}
/////////////
void setFb2HauptsignalF(byte pntr) {
  // led 1&3 (x=0&2) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
}
/////////////
void setFb2HauptsignalF3(byte pntr) {
  // led 1&3 (x=0&2) aan
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
}/////////////
void setFb3HauptsignalF3(byte pntr) {
  // led 1&3 (x=0&2) aan
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
}
/////////////
void setFb3HauptsignalF(byte pntr) {
  // led 1&4 (x=0&3) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
}
/////////////
void setFb5HauptsignalF(byte pntr) {
  // led 1&3&5 (x=0&2&4) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+4, 0, dimConst[signalChannel[pntr]+4], startMillis, endMillis);
}
/////////////
void setFb6HauptsignalF(byte pntr) {
  // led 4&6 (x=3&5) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+5, 0, dimConst[signalChannel[pntr]+5], startMillis, endMillis);
}
/////////////////////////////////////// Hauptsignal noFade ////////////////////////////////////////////////////////////////
  //lamp 1 = grun (hoch)
  //lamp 2 = rot
  //lamp 3 = grun of gelb (3/4-licht)
  //lamp 4 = groen 
  //lamp 5 = geel 
  //lamp 6 = groen 
//////////////
void setFb0HauptsignalnF(byte pntr) {
  // led 2 (x=1) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
  switch (signalLeds[pntr]){
  case 3:
    Tlc.set(signalChannel[pntr]+2, 0);
    break;
  case 4:
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    break;
  case 5:
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    Tlc.set(signalChannel[pntr]+4, 0);
    break;
  case 6:
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    Tlc.set(signalChannel[pntr]+4, 0);
    Tlc.set(signalChannel[pntr]+5, 0);
    break;
  default:
    break;
  }
  Tlc.update();
}
/////////////
void setFb1HauptsignalnF(byte pntr) {
  // led 1 (x=0) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  switch (signalLeds[pntr]){
  case 3:
    Tlc.set(signalChannel[pntr]+2, 0);
    break;
  case 4:
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    break;
  case 5:
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    Tlc.set(signalChannel[pntr]+4, 0);
    break;
  case 6:
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    Tlc.set(signalChannel[pntr]+4, 0);
    Tlc.set(signalChannel[pntr]+5, 0);
    break;
  default:
    break;
  }
  Tlc.update();
}
/////////////
void setFb2HauptsignalnF(byte pntr) {
  // led 1&3 (x=0&2) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, 0);
  switch (signalLeds[pntr]){
  case 5:
    Tlc.set(signalChannel[pntr]+4, 0);
    break;
  case 6:
    Tlc.set(signalChannel[pntr]+4, 0);
    Tlc.set(signalChannel[pntr]+5, 0);
    break;
  default:
    break;
  }
  Tlc.update();
}
/////////////
void setFb2HauptsignalnF3(byte pntr) {
  // led 1&3 (x=0&2) aan, 2 uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.update();
}/////////////
void setFb3HauptsignalnF3(byte pntr) {
  // led 1&3 (x=0&2) aan, 2 uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.update();
}
/////////////
void setFb3HauptsignalnF(byte pntr) {
  // led 1&4 (x=0&3) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);  
  Tlc.set(signalChannel[pntr]+2, 0);
  Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
  switch (signalLeds[pntr]){
  case 5:
    Tlc.set(signalChannel[pntr]+3, 0);
    Tlc.set(signalChannel[pntr]+4, 0);
    break;
  case 6:
    Tlc.set(signalChannel[pntr]+3, 0);
    Tlc.set(signalChannel[pntr]+4, 0);
    Tlc.set(signalChannel[pntr]+5, 0);
    break;
  default:
    break;
  }
  Tlc.update();
}
/////////////
void setFb5HauptsignalnF(byte pntr) {
  // led 1&3&5 (x=0&2&4) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, 0);
  Tlc.set(signalChannel[pntr]+4, dimConst[signalChannel[pntr]+4]);
  if (signalLeds[pntr]== 6){
      Tlc.set(signalChannel[pntr]+5, 0);
  }
  Tlc.update();
}
/////////////
void setFb6HauptsignalnF(byte pntr) {
  // led 4&6 (x=3&5) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, 0);
  Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
  Tlc.set(signalChannel[pntr]+4, 0);
  Tlc.set(signalChannel[pntr]+5, dimConst[signalChannel[pntr]+5]);
  Tlc.update();
}
//////////////////////////////// Zwergsignal Fade ///////////////////////////////////////////////////////////////////////
void dunkelZwergsignal(byte pntr) {
  endMillis = startMillis + fadeConst;
  for (byte x=0; x<3; x++){
    if (Tlc.get(signalChannel[pntr]+x) > 0) tlc_addFade(signalChannel[pntr]+x, dimConst[signalChannel[pntr]+x], 0, startMillis, endMillis);                 //If not off then fade out
  }
  startMillis = endMillis + darkDelay;
}
//////////////
void setZwergsignalF(byte pntr, byte Fb){
//ch1 links unter
//ch2 rechts unter
//ch3 oben
 switch (Fb){
    case 0: // zet ch1 en ch2 aan en ch3 uit
       endMillis = startMillis + fadeConst * 2;
       tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]+0], startMillis, endMillis);
       tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
       break;
    case 1: // zet ch1 en ch3 aan, ch2 uit
       endMillis = startMillis + fadeConst * 2;
       tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]+0], startMillis, endMillis);
       tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
       break;
    case 2: // zet ch1 uit en ch2, ch3 aan
       endMillis = startMillis + fadeConst * 2;
       tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
       tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
       break;
    default:
       break;
       }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void setZwergsignalnF(byte pntr, byte Fb){
//ch1 links unter
//ch2 rechts unter
//ch3 oben
 switch (Fb){
    case 0: // zet ch1 en ch2 aan en ch3 uit
//       Serial.println(" Halt");
       Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
       Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
       Tlc.set(signalChannel[pntr]+2, 0);
       break;
    case 1: // zet ch1 en ch3 aan, ch2 uit
//       Serial.println(" Fahrt");
       Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
       Tlc.set(signalChannel[pntr]+1, 0);
       Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
       break;
    case 2: // zet ch1 uit en ch2, ch3 aan
//       Serial.println(" Fahrt mit Vorsicht");
       Tlc.set(signalChannel[pntr]+0, 0);
       Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
       Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
       break;
    default:
       break;
       }
  Tlc.update();
}
