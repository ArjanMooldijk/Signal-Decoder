#include <Arduino.h>

// Sketch for Swiss signals
// Version 2.1 (19-10-16)  Arjan Mooldijk
// Thanks to Franz-Peter "MicroBahner" (Stummis) for the DCC examples
// Thanks to Alex Leone for the TLC library

#include <NmraDcc.h>
#include <Tlc5940.h>
#include <tlc_fades.h>

#define VorAdr1F      1 // Vorsignal addr 1,  Fade    2-5 leds Warnung & Fb1 !!STARTPIN!!
#define VorAdr2F      2 // Vorsignal addr 2,  Fade    4-5 leds   Fb2 & Fb3
#define VorAdr3F      3 // Vorsignal addr 3,  Fade    5 leds   Fb5
#define VorAdr1nF     4 // Vorsignal addr 1,  no Fade 2-5 leds Warnung & Fb1 !!STARTPIN!!
#define VorAdr2nF     5 // Vorsignal addr 2,  no Fade 4-5 leds   Fb2 & Fb3
#define VorAdr3nF     6 // Vorsignal addr 3,  no Fade 5 leds   Fb5
// 
#define ZwergAdr1F    7 // Zwerg addr 1,      Fade    3 leds Fade !!STARTPIN!!
#define ZwergAdr2F    8 // Zwerg addr 2,      Fade    3 leds Fade
#define ZwergAdr1nF   9 // Zwerg addr 1,      no Fade 3 leds !!STARTPIN!!
#define ZwergAdr2nF  10 // Zwerg addr 2,      no Fade 3 leds
//
#define SIMHaupt     11 // SIM Haupt,         1 led !!STARTPIN!!
#define SIMVor       12 // SIM Vor,           1 led !!STARTPIN!!
//
#define HauptAdr1F   13 // Haupt addr 1,  Fade    2-6 leds Fb0 & Fb1 !!STARTPIN!!
#define HptAdr2Fb2F  14 // Haupt addr 2,      Fade    3 leds Fb2
#define HptAdr2Fb3F  15 // Haupt addr 2,      Fade    3 leds Fb3
#define HauptAdr2F   16 // Haupt addr 2,      Fade    4-6 leds Fb2 & Fb3 
#define HptAdr3Fb6F  17 // Haupt addr 3,      Fade    5-6 leds Fb6 
#define HauptAdr3F   18 // Haupt addr 3,      Fade    6 leds Fb5 & Fb6 
#define HauptAdr1nF  19 // Haupt addr 1,      no Fade 2-6 leds Fb0 & Fb1 !!STARTPIN!!
#define HptAdr2Fb2nF 20 // Haupt addr 2,      no Fade 3 leds Fb2
#define HptAdr2Fb3nF 21 // Haupt addr 2,      no Fade 3 leds Fb3
#define HauptAdr2nF  22 // Haupt addr 2,      no Fade 4-6 leds Fb2 & Fb3 
#define HptAdr3Fb6nF 23 // Haupt addr 3,      no Fade 5-6 leds Fb6 
#define HauptAdr3nF  24 // Haupt addr 3,      no Fade 6 leds Fb5 & Fb6 
// Constants:
const unsigned int signalAdr[] =   { 100, 101, 102, 103, 104, 105};  // DCC-Signal Addresses 
const byte signalType[]        =   { VorAdr1F, VorAdr2F, HauptAdr1F, HauptAdr2F, ZwergAdr1F, ZwergAdr2F};  // Signal typen per adres
const byte signalLeds[]        =   { 4 , 4, 4, 4, 3, 3};  // # leds per signal
const byte signalChannel[]     =   { 0,  0, 4, 4, 8, 8};  // Eerste pin van het sein

const int dimConst [16] = {40,40,20,20,
                           20,40,20,40, 
                           10,10,10,
                           0,0,0,0,0};       // Brightness per led (max 16) Every row is one signal  

const int anZahl = sizeof(signalType);
const long fadeConst = 150;                 // Fade in/decrease in mSec.
const long darkDelay = 200;                 // Dark phase duration in mSec.
const long interval = 800;                  // interval to blink in mSec.

NmraDcc Dcc;
byte Blink [16] = {0};                      // Flag to blink pin (for SIM)
byte blinkState [16] = {0};                 // Blink on/off
unsigned long previousMillis[16]= {0};      // will store last time LED was updated
byte pinCounter;                            // Variable to address the right pin on the TLC5940
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
    Serial.println("Signal decoder nr1");
    Serial.print("Aantal adressen: ");
    Serial.println(anZahl);
    
    for (byte x = 0; x < anZahl; x ++) {   // set all default Halt and register starting pin per signal
      switch (signalType[x]) {
        case VorAdr1F:
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Vorsignal Fade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setFb0VorsignalF(signalChannel[x]);
          break;
        case VorAdr1nF:
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Vorsignal noFade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setFb0VorsignalnF(signalChannel[x]);
          break;
        case ZwergAdr1F:
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Zwergsignal Fade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setZwergsignalF(signalChannel[x],0);
          break;
        case ZwergAdr1nF:
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Zwergsignal noFade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setZwergsignalnF(signalChannel[x],0);
          break;
        case SIMHaupt:
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.println(" SIM Hauptsignal");
          setSIMsignal(signalChannel[x],0);
          break;
        case SIMVor:
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.println(" SIM Vorsignal");
          setSIMsignal(signalChannel[x],0);
          break;
        case HauptAdr1F:
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Hauptsignal Fade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setFb0HauptsignalF(signalChannel[x]);
          break;
        case HauptAdr1nF:
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Hauptsignal noFade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setFb0HauptsignalnF(signalChannel[x]);
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
//  Serial.print(Addr);
//  Serial.println(" ontvangen adress");
  for ( byte index = 0; index < anZahl; index++ ) {
    if (Addr == signalAdr[index]) {                     // ist eigene Adresse
      if ((Addr != LastAddr) || ( LastTime < millis() - 200)) {
        /*Serial.print ("Adres: ");
        Serial.println (Addr); */
        switch (signalType[index]) {
          case VorAdr1F :                                       //Vorsignal Addr 1 Fb1 & Fb0 Fade
            startMillis = millis() + 1;
            dunkelVorsignal(signalChannel[index]);
            if ( OutputAddr & 0x1 ) {
              setFb1VorsignalF(signalChannel[index]);
              break; }
            else {
              setFb0VorsignalF(signalChannel[index]);
              break;
            }
          case VorAdr2F:                                       //Vorsignal Addr 2 Fb3 & Fb2 Fade
            startMillis = millis() + 1;
            dunkelVorsignal(signalChannel[index]);
            if ( OutputAddr & 0x1 ) {
              setFb3VorsignalF(signalChannel[index]);
              break;
            }
            else {
              setFb2VorsignalF(signalChannel[index]);
              break;
            }
          case VorAdr3F:                                       //Vorsignal Addr 3 Fb5 Fade
            startMillis = millis() + 1;
            dunkelVorsignal(signalChannel[index]);
            setFb5VorsignalF(signalChannel[index]);
            break;
          case VorAdr1nF :                                       //Vorsignal Addr 1 Fb1 & Fb0 noFade
            if ( OutputAddr & 0x1 ) {
              setFb1VorsignalnF(signalChannel[index]);
              break; }
            else {
              setFb0VorsignalnF(signalChannel[index]);
              break;
            }
          case VorAdr2nF:                                       //Vorsignal Addr 2 Fb3 & Fb2 noFade
            if ( OutputAddr & 0x1 ) {
              setFb3VorsignalnF(signalChannel[index]);
              break;
            }
            else {
              setFb2VorsignalnF(signalChannel[index]);
              break;
            }
          case VorAdr3nF:                                       //Vorsignal Addr 3 Fb5 noFade
            setFb5VorsignalnF(signalChannel[index]);
            break;            
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
              setZwergsignalnF(signalChannel[index], 2);
              delay(500);
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
          case SIMHaupt:                                       //SIMhauptsignal An & Aus
            if ( OutputAddr & 0x1 ) {
              setSIMsignal(signalChannel[index], 2);
              break;
            }
            else {
              setSIMsignal(signalChannel[index], 0);
              break;
            }
          case SIMVor:                                       //SIMvorsignal An & Aus
            if ( OutputAddr & 0x1 ) { 
              setSIMsignal(signalChannel[index], 1);
              break;
            }
            else {
            setSIMsignal(signalChannel[index], 0);
            break;
            }            
          case HauptAdr1F:                                    //Hauptsignal Addr Fb1 & Fb0 Fade
            startMillis = millis() + 1;
            dunkelHauptsignal(signalChannel[index], signalLeds[index]);
            if ( OutputAddr & 0x1 ) {
              setFb1HauptsignalF(signalChannel[index]);
              break;
            }
            else {
              setFb0HauptsignalF(signalChannel[index]);
              break;
              }
          case HptAdr2Fb2F:                                    //Hauptsignal Addr 2 Fb2 Fade 
            startMillis = millis() + 1;
            dunkelHauptsignal(signalChannel[index], signalLeds[index]);
            setFb2HauptsignalF3(signalChannel[index]);
            break;
          case HptAdr2Fb3F:                                    //Hauptsignal Addr 2 Fb3 Fade
            startMillis = millis() + 1;
            dunkelHauptsignal(signalChannel[index], signalLeds[index]);
            setFb3HauptsignalF(signalChannel[index]);
            break;
          case HauptAdr2F:                                    //Hauptsignal Addr 2 Fb3 & Fb2 Fade
            startMillis = millis() + 1;
            dunkelHauptsignal(signalChannel[index], signalLeds[index]);
            if ( OutputAddr & 0x1 ) {
              setFb3HauptsignalF(signalChannel[index]);
              break;
            }
            else {
                setFb2HauptsignalF(signalChannel[index]);
              break;
            }
          case HptAdr3Fb6F:                                    //Hauptsignal Addr 3 Fb6 Fade
            startMillis = millis() + 1;
            dunkelHauptsignal(signalChannel[index], signalLeds[index]);
            setFb6HauptsignalF(signalChannel[index]);
            break;
          case HauptAdr3F:                                    //Hauptsignal Addr 3 Fb6 & Fb5 Fade
            startMillis = millis() + 1;
            dunkelHauptsignal(signalChannel[index], signalLeds[index]);
            if ( OutputAddr & 0x1 ) {
              setFb6HauptsignalF(signalChannel[index]);
              break;
            }
            else {
              setFb5HauptsignalF(signalChannel[index]);
              break;
            }
            break;

                      
          case HauptAdr1nF:                                    //Hauptsignal Addr Fb1 & Fb0 noFade
            startMillis = millis() + 1;
            dunkelHauptsignal(signalChannel[index], signalLeds[index]);
            if ( OutputAddr & 0x1 ) {
              setFb1HauptsignalF(signalChannel[index]);
              break;
            }
            else {
              setFb0HauptsignalF(signalChannel[index]);
              break;
              }
          case HptAdr2Fb2nF:                                    //Hauptsignal Addr 2 Fb2 noFade
            startMillis = millis() + 1;
            dunkelHauptsignal(signalChannel[index], signalLeds[index]);
            setFb2HauptsignalF3(signalChannel[index]);
            break;
          case HptAdr2Fb3nF:                                    //Hauptsignal Addr 2 Fb3 noFade
            startMillis = millis() + 1;
            dunkelHauptsignal(signalChannel[index], signalLeds[index]);
            setFb3HauptsignalF(signalChannel[index]);
            break;
          case HauptAdr2nF:                                    //Hauptsignal Addr 2 Fb3 & Fb2 noFade
            startMillis = millis() + 1;
            dunkelHauptsignal(signalChannel[index], signalLeds[index]);
            if ( OutputAddr & 0x1 ) {
              setFb3HauptsignalF(signalChannel[index]);
              break;
            }
            else {
                setFb2HauptsignalF(signalChannel[index]);
              break;
            }
          case HptAdr3Fb6nF:                                    //Hauptsignal Addr 3 Fb6 noFade
            startMillis = millis() + 1;
            dunkelHauptsignal(signalChannel[index], signalLeds[index]);
            setFb6HauptsignalF(signalChannel[index]);
            break;
          case HauptAdr3nF:                                    //Hauptsignal Addr 3 Fb6 & Fb5 noFade
            startMillis = millis() + 1;
            dunkelHauptsignal(signalChannel[index], signalLeds[index]);
            if ( OutputAddr & 0x1 ) {
              setFb6HauptsignalF(signalChannel[index]);
              break;
            }
            else {
              setFb5HauptsignalF(signalChannel[index]);
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
void setSIMsignal (byte startPin, byte Fb){
 switch (Fb){
    case 0: // zet ch1 uit
       Blink[startPin] = 0;
       Tlc.set(startPin, 0);
       break;
    case 1: // zet ch1 blink aan
       Blink[startPin] = 1;
       Tlc.set(startPin, dimConst[startPin]);
       break;
    case 2: // zet ch1 aan
       Tlc.set(startPin, dimConst[startPin]);
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
void dunkelVorsignal(byte startPin) {
// wie viel leds gibt es zum dieses Signal
  byte nbrLeds = 4;
  endMillis = startMillis + fadeConst;
  for (byte x=0; x<nbrLeds; x++){
    Serial.print("Led ");
    Serial.print(startPin+x);
    Serial.print("waarde ");
    Serial.println(Tlc.get(startPin+x));
    if (Tlc.get(startPin+x) > 0) {
      tlc_addFade(startPin+x, dimConst[startPin+x], 0, startMillis, endMillis);                 //If not off then fade out
    }
  }
  startMillis = endMillis + darkDelay;
}
/////////////
void setFb0VorsignalF(byte startPin) {
  // led 1&2 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;      
  tlc_addFade(startPin, 0, dimConst[startPin], startMillis, endMillis);      
  tlc_addFade(startPin+1, 0, dimConst[startPin+1], startMillis, endMillis);
}
/////////////
void setFb1VorsignalF(byte startPin) {
  // led 3&4 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;      
  tlc_addFade(startPin+2, 0, dimConst[startPin+2], startMillis, endMillis);      
  tlc_addFade(startPin+3, 0, dimConst[startPin+3], startMillis, endMillis);
}
/////////////
void setFb2VorsignalF(byte startPin) {
  // led 1&3 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;      
  tlc_addFade(startPin, 0, dimConst[startPin], startMillis, endMillis);      
  tlc_addFade(startPin+2, 0, dimConst[startPin+2], startMillis, endMillis);
}
/////////////
void setFb3VorsignalF(byte startPin) {
  
  // led 1&3&4 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;      
  tlc_addFade(startPin, 0, dimConst[startPin], startMillis, endMillis);      
  tlc_addFade(startPin+2, 0, dimConst[startPin+2], startMillis, endMillis);      
  tlc_addFade(startPin+3, 0, dimConst[startPin+3], startMillis, endMillis);
}
/////////////
void setFb5VorsignalF(byte startPin) {
  // led 3&4&5 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;      
  tlc_addFade(startPin+2, 0, dimConst[startPin+2], startMillis, endMillis);      
  tlc_addFade(startPin+3, 0, dimConst[startPin+3], startMillis, endMillis);      
  tlc_addFade(startPin+4, 0, dimConst[startPin+4], startMillis, endMillis);
}

////////////////////////////////////// Vorsignal noFade/////////////////////////////////////////////////////////////////
//Signalbilder sind Warnung (Gelb1, Gelb2), Fb1 (Grün1, Grün2), Fb2 (Gelb1, Grün1), Fb3 (Gelb1, Grün1, Grün2), Fb5 (Grün1, Grün2, Gelb3)
//ch1 = gelb (links)
//ch2 = gelb (rechts oben)
//ch3 = grun (rechts)
//ch4 = grun (links)
//ch5 = gelb (rechts unter)
//////////////
void setFb0VorsignalnF(byte startPin) {
  // led 1&2 aan, de rest uit
  Tlc.set(startPin+0, dimConst[startPin+0]);
  Tlc.set(startPin+1, dimConst[startPin+1]);
  Tlc.set(startPin+2, 0);
  Tlc.set(startPin+3, 0);
  if (signalLeds[startPin]==5) {Tlc.set(startPin+4, 0);}
  Tlc.update();
}
/////////////
void setFb1VorsignalnF(byte startPin) {
  // led 3&4 aan, de rest uit
  Tlc.set(startPin+0, 0);
  Tlc.set(startPin+1, 0);
  Tlc.set(startPin+2, dimConst[startPin+2]);
  Tlc.set(startPin+3, dimConst[startPin+3]);
  if (signalLeds[startPin]==5) {Tlc.set(startPin+4, 0);}
  Tlc.update();
}
/////////////
void setFb2VorsignalnF(byte startPin) {
  // led 1&3 aan, de rest uit
  Tlc.set(startPin+0, dimConst[startPin+0]);
  Tlc.set(startPin+1, 0);
  Tlc.set(startPin+2, dimConst[startPin+2]);
  Tlc.set(startPin+3, 0);
  if (signalLeds[startPin]==5) {Tlc.set(startPin+4, 0);}
  Tlc.update();
}
/////////////
void setFb3VorsignalnF(byte startPin) {  
  // led 1&3&4 aan, de rest uit
  Tlc.set(startPin+0, dimConst[startPin+0]);
  Tlc.set(startPin+1, 0);
  Tlc.set(startPin+2, dimConst[startPin+2]);
  Tlc.set(startPin+3, dimConst[startPin+3]);
  if (signalLeds[startPin]==5) {Tlc.set(startPin+4, 0);}
  Tlc.update();
}
/////////////
void setFb5VorsignalnF(byte startPin) {
  // led 3&4&5 aan, de rest uit
  Tlc.set(startPin+0, 0);
  Tlc.set(startPin+1, 0);
  Tlc.set(startPin+2, dimConst[startPin+2]);
  Tlc.set(startPin+3, dimConst[startPin+3]);
  Tlc.set(startPin+4, dimConst[startPin+4]);
  Tlc.update();
}

/////////////////////////////////////// Hauptsignal Fade ////////////////////////////////////////////////////////////////
  //ch1 = grun (hoch)  
  //ch2 = rot
  //ch3 = grun (mitten) oder gelb (3-licht)
  //ch4 = gelb (unter)
  //ch5 = grun (nieder)
  //ch6 = gelb (hoch)
//////////////
void dunkelHauptsignal(byte startPin, byte nbrLeds) {
  endMillis = startMillis + fadeConst;
  for (byte x=0; x<nbrLeds; x++){
    if (Tlc.get(startPin+x) > 0) tlc_addFade(startPin+x, dimConst[startPin+x], 0, startMillis, endMillis);                 //If not off then fade out
  }
  startMillis = endMillis + darkDelay;
}
/////////////////
void setFb0HauptsignalF(byte startPin) {
  // led 2 (x=1) aan, de rest uit 
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(startPin+1, 0, dimConst[startPin+1], startMillis, endMillis);
}
/////////////
void setFb1HauptsignalF(byte startPin) {
  // led 1 (x=0) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(startPin+0, 0, dimConst[startPin], startMillis, endMillis);
}
/////////////
void setFb2HauptsignalF(byte startPin) {
  // led 1&5 (x=0&4) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(startPin+0, 0, dimConst[startPin], startMillis, endMillis);
  tlc_addFade(startPin+4, 0, dimConst[startPin+4], startMillis, endMillis);
}
/////////////
void setFb2HauptsignalF3(byte startPin) {
  // led 1&3 (x=0&2) aan
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(startPin+0, 0, dimConst[startPin], startMillis, endMillis);
  tlc_addFade(startPin+2, 0, dimConst[startPin+2], startMillis, endMillis);
}
/////////////
void setFb3HauptsignalF(byte startPin) {
  // led 1&3 (x=0&2) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(startPin+0, 0, dimConst[startPin], startMillis, endMillis);
  tlc_addFade(startPin+2, 0, dimConst[startPin+2], startMillis, endMillis);
}
/////////////
void setFb5HauptsignalF(byte startPin) {
  // led 1&3&5 (x=0&2&4) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(startPin+0, 0, dimConst[startPin], startMillis, endMillis);
  tlc_addFade(startPin+2, 0, dimConst[startPin+2], startMillis, endMillis);
  tlc_addFade(startPin+4, 0, dimConst[startPin+4], startMillis, endMillis);
}
/////////////
void setFb6HauptsignalF(byte startPin) {
  // led 4&6 (x=3&5) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(startPin+3, 0, dimConst[startPin+3], startMillis, endMillis);
  tlc_addFade(startPin+5, 0, dimConst[startPin+5], startMillis, endMillis);
}
/////////////////////////////////////// Hauptsignal noFade ////////////////////////////////////////////////////////////////
  //ch1 = grun (hoch)  
  //ch2 = rot
  //ch3 = grun (mitten)
  //ch4 = gelb (unter)
  //ch5 = grun (nieder)
  //ch6 = gelb (hoch)
//////////////
void setFb0HauptsignalnF(byte startPin) {
  // led 2 (x=1) aan, de rest uit
  Tlc.set(startPin+0, 0);
  Tlc.set(startPin+1, dimConst[startPin+1]);
  switch (signalLeds[startPin]){
  case 3:
    Tlc.set(startPin+2, 0);
    break;
  case 4:
    Tlc.set(startPin+2, 0);
    Tlc.set(startPin+3, 0);
    break;
  case 5:
    Tlc.set(startPin+2, 0);
    Tlc.set(startPin+3, 0);
    Tlc.set(startPin+4, 0);
    break;
  case 6:
    Tlc.set(startPin+2, 0);
    Tlc.set(startPin+3, 0);
    Tlc.set(startPin+4, 0);
    Tlc.set(startPin+5, 0);
    break;
  default:
    break;
  }
  Tlc.update();
}
/////////////
void setFb1HauptsignalnF(byte startPin) {
  // led 1 (x=0) aan, de rest uit
  Tlc.set(startPin+0, dimConst[startPin]);
  Tlc.set(startPin+1, 0);
  switch (signalLeds[startPin]){
  case 3:
    Tlc.set(startPin+2, 0);
    break;
  case 4:
    Tlc.set(startPin+2, 0);
    Tlc.set(startPin+3, 0);
    break;
  case 5:
    Tlc.set(startPin+2, 0);
    Tlc.set(startPin+3, 0);
    Tlc.set(startPin+4, 0);
    break;
  case 6:
    Tlc.set(startPin+2, 0);
    Tlc.set(startPin+3, 0);
    Tlc.set(startPin+4, 0);
    Tlc.set(startPin+5, 0);
    break;
  default:
    break;
  }
  Tlc.update();
}
/////////////
void setFb2HauptsignalnF(byte startPin) {
  // led 1&4 (x=0&3) aan, de rest uit
  Tlc.set(startPin+0, dimConst[startPin]);
  Tlc.set(startPin+1, 0);
  switch (signalLeds[startPin]){
  case 3:
    Tlc.set(startPin+2, 0);
    break;
  case 4:
    Tlc.set(startPin+2, 0);
    Tlc.set(startPin+3, dimConst[startPin+3]);
    break;
  case 5:
    Tlc.set(startPin+2, 0);
    Tlc.set(startPin+3, dimConst[startPin+3]);
    Tlc.set(startPin+4, 0);
    break;
  case 6:
    Tlc.set(startPin+2, 0);
    Tlc.set(startPin+3, dimConst[startPin+3]);
    Tlc.set(startPin+4, 0);
    Tlc.set(startPin+5, 0);
    break;
  default:
    break;
  }
  Tlc.update();
}
/////////////
void setFb2HauptsignalnF3(byte startPin) {
  // led 1&3 (x=0&2) aan, 2 uit
  Tlc.set(startPin+0, dimConst[startPin]);
  Tlc.set(startPin+1, 0);
  Tlc.set(startPin+3, dimConst[startPin+3]);
  Tlc.update();
}
/////////////
void setFb3HauptsignalnF(byte startPin) {
  // led 1&3 (x=0&2) aan, de rest uit
  Tlc.set(startPin+0, dimConst[startPin]);
  Tlc.set(startPin+1, 0);
  switch (signalLeds[startPin]){
  case 3:
    Tlc.set(startPin+2, dimConst[startPin+2]);
    break;
  case 4:
    Tlc.set(startPin+2, dimConst[startPin+2]);
    Tlc.set(startPin+3, 0);
    break;
  case 5:
    Tlc.set(startPin+2, dimConst[startPin+2]);
    Tlc.set(startPin+3, 0);
    Tlc.set(startPin+4, 0);
    break;
  case 6:
    Tlc.set(startPin+2, dimConst[startPin+2]);
    Tlc.set(startPin+3, 0);
    Tlc.set(startPin+4, 0);
    Tlc.set(startPin+5, 0);
    break;
  default:
    break;
  }
  Tlc.update();
}
/////////////
void setFb5HauptsignalnF(byte startPin) {
  // led 1&3&5 (x=0&2&4) aan, de rest uit
  Tlc.set(startPin+0, dimConst[startPin]);
  Tlc.set(startPin+1, 0);
  switch (signalLeds[startPin]){
  case 5:
    Tlc.set(startPin+2, dimConst[startPin+2]);
    Tlc.set(startPin+3, 0);
    Tlc.set(startPin+4, dimConst[startPin+4]);
    break;
  case 6:
    Tlc.set(startPin+2, dimConst[startPin+2]);
    Tlc.set(startPin+3, 0);
    Tlc.set(startPin+4, dimConst[startPin+4]);
    Tlc.set(startPin+5, 0);
    break;
  default:
    break;
  }
  Tlc.update();
}
/////////////
void setFb6HauptsignalnF(byte startPin) {
  // led 4&6 (x=3&5) aan, de rest uit
  Tlc.set(startPin+0, 0);
  Tlc.set(startPin+1, 0);
  Tlc.set(startPin+2, 0);
  Tlc.set(startPin+3, dimConst[startPin+3]);
  Tlc.set(startPin+4, 0);
  Tlc.set(startPin+5, dimConst[startPin+5]);
  Tlc.update();
}
//////////////////////////////// Zwergsignal Fade ///////////////////////////////////////////////////////////////////////
void dunkelZwergsignal(byte startPin) {
  endMillis = startMillis + fadeConst;
  for (byte x=0; x<3; x++){
    if (Tlc.get(startPin+x) > 0) tlc_addFade(startPin+x, dimConst[startPin+x], 0, startMillis, endMillis);                 //If not off then fade out
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
    default:
       break;
       }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
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
