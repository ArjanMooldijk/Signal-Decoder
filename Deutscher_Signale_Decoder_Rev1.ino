// Sketch für Deutsche Signale
// Version 1.0 (14-10-18)  Arjan Mooldijk
// Thanks to Franz-Peter "MicroBahner" (Stummis) for the DCC examples
// Thanks to Alex Leone for the TLC library

#include <NmraDcc.h>
#include <Tlc5940.h>
#include <tlc_fades.h>

#define VorAdr1F      1 // Vorsignal addr 1,  Fade    Voorsignaal, eerste adres  beelden Vr0 & Vr1
#define VorAdr2F      2 // Vorsignal addr 2,  Fade    Voorsignaal, tweede adres  beelden Vr2 & donker
#define VorAdr1nF     3 // Vorsignal addr 1,  no Fade Voorsignaal, eerste adres  beelden Vr0 & Vr1
#define VorAdr2nF     4 // Vorsignal addr 2,  no Fade Voorsignaal, tweede adres  beelden Vr2 & donker
//
#define RangAdr1F     5 // Rangier addr 1,    Fade    Rangeersignaal, eerste adres  beelden Sh0 & Sh1
#define RangAdr1nF    6 // Rangier addr 1,    no Fade Rangeersignaal, eerste adres  beelden Sh0 & Sh1
//
#define HauptAdr1F    7 // Haupt addr 1,      Fade    Hoofdsignaal eerste adres beelden Hp0 & Hp1
#define HauptAdr2F    8 // Haupt addr 2,      Fade    Hoofdsignaal tweede adres beeld Hp2
#define AusfAdr1F     9 // Ausfahr addr,      Fade    Ausfahr signaal eerste adres beeld Hp0 & Hp1
#define AusfAdr2F    10 // Ausfahr addr,      Fade    Ausfahr signaal tweede adres beeld Hp2 & Hp0/Sh1
#define HauptAdr1nF  11 // Haupt addr 1,      no Fade    Hoofdsignaal eerste adres beelden Hp0 & Hp1 
#define HauptAdr2nF  12 // Haupt addr 2,      no Fade    Hoofdsignaal tweede adres beeld Hp2
#define AusfAdr1nF   13 // Ausfahr addr,      no Fade    Ausfahr signaal eerste adres beeld Hp0 & Hp1
#define AusfAdr2nF   14 // Ausfahr addr,      no Fade    Ausfahr signaal tweede adres beeld Hp2 & Hp0/Sh1

const byte numTLCpins = NUM_TLCS * 16;        // Num_TLCS = number of daisy chained TLCs. Defined in tlc_config.h

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AAN TE PASSEN CONSTANTEN ///////////////////////////////////////////////////////////////////////////////////////////
// Decoder nummer. Dit wordt in de decoder zelf opgeslagen en is met de monitor uit te lezen.                       ///
// Dan weet je welke het is.                                                                                        ///
const byte decoderNbr = 1;                                                                                          ///
                                                                                                                    ///
// DCC adressen:                                                                                                    ///
const unsigned int signalAdr[] =   { 100, 101, 102, 103, 104};                                                      ///
                                                                                                                    ///
// Sein types per hierboven opgegeven adres                                                                         ///
const byte signalType[]        =   { VorAdr1F, VorAdr2F, HauptAdr1F, HauptAdr2F, RangAdr1F};                        ///
                                                                                                                    ///
// Lichtsterkte van de leds per sein in volgorde van de opgegeven types                                             ///
const int dimConst [numTLCpins] = {40,40,20,20,
                                   20,40,20,40,
                                   10,10,10,
                                    0,0,0,0,0};      // Brightness per led (max 16) Every row is one signal         ///
                                                                                                                    ///
                                                                                                                    ///
const int anZahl = sizeof(signalType);                                                                              ///
const long fadeConst = 150;                 // Fade in/decrease in mSec.                                            ///
const long darkDelay = 350;                 // Dark phase duration in mSec.                                         ///
const long interval = 800;                  // interval to blink in mSec.                                           ///
///////// HIERONDER HOEFT NIETS MEER AANGEPAST TE WORDEN (TENZIJ JE HET PROGRAMMA WILT VERANDEREN ;-) /////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NmraDcc Dcc;
byte signalLeds[numTLCpins];                          // # leds per signal
byte signalChannel[numTLCpins];                       // Eerste pin per het sein
byte Blink [16] = {0};                      // Flag to blink pin (for SIM)
byte blinkState [16] = {0};                 // Blink on/off
unsigned long previousMillis[numTLCpins]= {0};      // will store last time LED was updated
byte pinCounter = 0;                            // Variable to address the right pin on the TLC5940
uint32_t startMillis;                       // millisecond variable for fade
uint32_t endMillis;                         // millisecond variable for fade
unsigned int SeqWait = 3*fadeConst+darkDelay+1; // Total time for one sequence to commplete
unsigned long LastTime[numTLCpins] = {0};           // will store last time packet received
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
          signalLeds[x] = 4;
          signalChannel[x] = pinCounter;
          if (signalType[x+1]==VorAdr2F) {
            signalLeds[x+1] = 4;
            signalChannel[x+1] = pinCounter;
          }
          pinCounter = pinCounter + 4;
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Vorsignal Fade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setVr0VorsignalF(x);
          break;

        case VorAdr1nF:
          signalLeds[x] = 4;
          signalChannel[x] = pinCounter;
          if (signalType[x+1]==VorAdr2nF) {
            signalLeds[x+1] = 4;
            signalChannel[x+1] = pinCounter;
          }
          pinCounter = pinCounter + 4;
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Vorsignal noFade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setVr0VorsignalnF(x);
          break;

        case RangAdr1F:
          signalLeds[x] = 2;
          signalLeds[x+1] = 2;
          signalChannel[x] = pinCounter;
          signalChannel[x+1] = pinCounter;
          pinCounter = pinCounter + 2;
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Rangiersignal Fade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setSh0RangiersignalF(x);
          break;

        case RangAdr1nF:
          signalLeds[x] = 2;
          signalLeds[x+1] = 2;
          signalChannel[x] = pinCounter;
          signalChannel[x+1] = pinCounter;
          pinCounter = pinCounter + 2;
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Rangiersignal noFade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setSh0RangiersignalnF(x);
          break;

        case HauptAdr1F:
          if (signalType[x+1]== HauptAdr2F) { // Hauptsignal mit Hp0, Hp1 &Hp2
            signalLeds[x] = 3;
            signalLeds[x+1] = 3;
            signalChannel[x] = pinCounter;
            signalChannel[x+1] = pinCounter;
            pinCounter = pinCounter + 3;
          }
          else {                              //Hauptsignal mit Hp0 & Hp1
            signalLeds[x] = 2;
            signalChannel[x] = pinCounter;
            pinCounter = pinCounter + 2;
          }
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Hauptsignal Fade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setHp0HauptsignalF(x);
          break;

        case AusfAdr1F:                        // Ausfahrsignal mit Hp0, Hp1, Hp2 & Hp0/Sh1
          signalLeds[x] = 5;
          signalLeds[x+1] = 5;
          signalChannel[x] = pinCounter;
          signalChannel[x+1] = pinCounter;
          pinCounter = pinCounter + 5;
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Hauptsignal Fade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setHp0AusfahrsignalF(x);
          break;

        case AusfAdr1nF:                        // Ausfahrsignal mit Hp0, Hp1, Hp2 & Hp0/Sh1
          signalLeds[x] = 5;
          signalLeds[x+1] = 5;
          signalChannel[x] = pinCounter;
          signalChannel[x+1] = pinCounter;
          pinCounter = pinCounter + 5;
          Serial.print("Pin: ");
          Serial.print(signalChannel[x]);
          Serial.print(" Adres: ");
          Serial.print(signalAdr[x]);
          Serial.print(" Hauptsignal noFade ");
          Serial.print(signalLeds[x]);
          Serial.println(" Leds");
          setHp0AusfahrsignalnF(x);
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
}
//////////////////////////////////////////////////////////////
// Unterprogramme, die von der DCC Library aufgerufen werden:
// Die folgende Funktion wird von Dcc.process() aufgerufen, wenn ein Weichentelegramm empfangen wurde
void notifyDccAccState( uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State ){
  for ( byte index = 0; index < anZahl; index++ ) {
    if (Addr == signalAdr[index]) {                     // ist eigene Adresse
      if ( LastTime[index] < (millis()-SeqWait)) {
        switch (signalType[index]) {
          
//Vorsignal Addr 1 Vr1 & Vr0 Fade
          case VorAdr1F:
            LastTime[index] = millis();
            if (signalType[index+1]==VorAdr2F) {LastTime[index+1] = LastTime[index];}
            startMillis = millis() + 1;
            dunkelVorsignal(index);
            if ( OutputAddr & 0x1 ) {
              setVr1VorsignalF(index);
              break;
            }
            else {
              setVr0VorsignalF(index);
              break;
            }

//Vorsignal Addr 2 Dunkel & Vr2 Fade
          case VorAdr2F:
            LastTime[index] = millis();                
            LastTime[index-1] = LastTime[index];
            startMillis = millis() + 1;
            dunkelVorsignal(index);
            if ( OutputAddr & 0x1 ) {
              break;
            }
            else {
              setVr2VorsignalF(index);
              break;
            }

//Vorsignal Addr 1 Vr1 & Vr0 noFade
          case VorAdr1nF :
            LastTime[index] = millis();            
            if (signalType[index+1]==VorAdr2nF) {LastTime[index+1] = LastTime[index];}
            if ( OutputAddr & 0x1 ) {
              setVr1VorsignalnF(index);
              break; 
            }
            else {
              setVr0VorsignalnF(index);
              break;
            }

//Vorsignal Addr 2 Dunkel & Vr2 noFade
          case VorAdr2nF:
            LastTime[index] = millis();                
            LastTime[index-1] = LastTime[index];
            if ( OutputAddr & 0x1 ) {
              dunkelVorsignal(index);
              break;
            }
            else {
              setVr2VorsignalnF(index);
              break;
            }

//Rangiersignal Addr 1 Sh1 & Sh0 Fade
          case RangAdr1F:
            LastTime[index] = millis();
            startMillis = millis() + 1;
            dunkelRangiersignal(index);
            if ( OutputAddr & 0x1 ) {
              setSh1RangiersignalF(index);
              break;
            }
            else {
              setSh0RangiersignalF(index);
              break;
            }

//Rangiersignal Addr 1 Sh1 & Sh0 noFade
          case RangAdr1nF:
            LastTime[index] = millis();
            if ( OutputAddr & 0x1 ) {
              setSh1RangiersignalnF(index);
              break;
            }
            else {
              setSh0RangiersignalnF(index);
              break;
            }

//Hauptsignal Addr 1 Hp1 & Hp0 Fade
          case HauptAdr1F:
            LastTime[index] = millis();
            if (signalType[index+1] == HauptAdr2F) {
              LastTime[index+1] = LastTime[index];
            }  
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            if ( OutputAddr & 0x1 ) {
              setHp1HauptsignalF(index);
              break;
            }
            else {
              setHp0HauptsignalF(index);
              break;
            }

//Hauptsignal Addr 2 Hp2  Fade
          case HauptAdr2F:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index]; 
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            setHp2HauptsignalF(index);
            break;
            
//Ausfahrsignal Addr 1 Hp1 & Hp0 Fade
          case AusfAdr1F:
            LastTime[index] = millis();
            LastTime[index+1] = LastTime[index];
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            if ( OutputAddr & 0x1 ) {
              setHp1AusfahrsignalF(index);
              break;
            }
            else {
              setHp0AusfahrsignalF(index);
              break;
            }

//Ausfahrsignal Addr 2 Hp2 & Hp0/Sh1  Fade
          case AusfAdr2F:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index]; 
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            if ( OutputAddr & 0x1 ) {
              setHp2AusfahrsignalF(index);
              break;
            }
            else {
              setSh1AusfahrsignalF(index);
              break;
            }

//Hauptsignal Addr 1 Hp1 & Hp0 noFade
          case HauptAdr1nF:
            LastTime[index] = millis();
            if (signalType[index+1]== HauptAdr2nF) {
              LastTime[index+1] = LastTime[index];
            }  
            startMillis = millis() + 1;
            if ( OutputAddr & 0x1 ) {
              setHp1HauptsignalnF(index);
              break;
            }
            else {
              setHp0HauptsignalnF(index);
              break;
            }

//Hauptsignal Addr 2 Hp2  noFade
          case HauptAdr2nF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index]; 
            startMillis = millis() + 1;
            setHp2HauptsignalnF(index);
            break;
            
//Ausfahrsignal Addr 1 Hp1 & Hp0 noFade
          case AusfAdr1nF:
            LastTime[index] = millis();
            LastTime[index+1] = LastTime[index];
            startMillis = millis() + 1;
            if ( OutputAddr & 0x1 ) {
              setHp1AusfahrsignalnF(index);
              break;
            }
            else {
              setHp0AusfahrsignalnF(index);
              break;
            }

//Ausfahrsignal Addr 2 Hp2 & Hp0/Sh1  noFade
          case AusfAdr2nF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index]; 
            startMillis = millis() + 1;
            if ( OutputAddr & 0x1 ) {
              setHp2AusfahrsignalnF(index);
              break;
            }
            else {
              setSh1AusfahrsignalnF(index);
              break;
            }
        }
      }
    }
  }
}


////////////////////////////////////// Vorsignal Fade/////////////////////////////////////////////////////////////////
//Signalbilder sind Vr0 (Gelb-o, Gelb-u), Vr1 (Grün-o, Grün-u), Vr2 (Gelb-u, Grün-o)
//led1 = gelb-o (oben)
//led2 = gelb-u (unter)
//led3 = grun-o (oben)
//led4 = grun-u (unter)
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
void setVr0VorsignalF(byte pntr) {
  // led 1&2 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr], 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
}
/////////////
void setVr1VorsignalF(byte pntr) {
  // led 3&4 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
}
/////////////
void setVr2VorsignalF(byte pntr) {
  // led 2&3 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
}

////////////////////////////////////// Vorsignal noFade/////////////////////////////////////////////////////////////////
//Signalbilder sind Vr0 (Gelb-o, Gelb-u), Vr1 (Grün-o, Grün-u), Vr2 (Gelb-u, Grün-o)
//led1 = gelb-o (oben)
//led2 = gelb-u (unter)
//led3 = grun-o (oben)
//led4 = grun-u (unter)
//////////////
void setVr0VorsignalnF(byte pntr) {
  // led 1&2 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]+0]);
  Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
  Tlc.set(signalChannel[pntr]+2, 0);
  Tlc.set(signalChannel[pntr]+3, 0);
  Tlc.update();
}
/////////////
void setVr1VorsignalnF(byte pntr) {
  // led 3&4 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
  Tlc.update();
}
/////////////
void setVr2VorsignalnF(byte pntr) {
  // led 2&3 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, 0);
  Tlc.update();
}


/////////////////////////////////////// Hauptsignal Fade ////////////////////////////////////////////////////////////////
//Signalbilder sind Hp0 (Rot), Hp1 (Grün), Hp2 (Gelb, Grün)
//
//Led 1 = rot
//led 2 = grun
//led 3 = gelb
//////////////
void dunkelHauptsignal(byte pntr) {
  endMillis = startMillis + fadeConst;
  for (byte x=0; x<signalLeds[pntr]; x++){
    if (Tlc.get(signalChannel[pntr]+x) > 0) tlc_addFade(signalChannel[pntr]+x, dimConst[signalChannel[pntr]+x], 0, startMillis, endMillis); //If not off then fade out
  }
  startMillis = endMillis + darkDelay;
}
/////////////////
void setHp0HauptsignalF(byte pntr) {
  // led 1 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
}
/////////////
void setHp1HauptsignalF(byte pntr) {
  // led 2 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
}
/////////////
void setHp2HauptsignalF(byte pntr) {
  // led 2&3 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
}
/////////////////////////////////////// Hauptsignal noFade ////////////////////////////////////////////////////////////////
//Signalbilder sind Hp0 (Rot), Hp1 (Grün), Hp2 (Gelb, Grün)
//
//Led 1 = rot
//led 2 = grun
//led 3 = gelb
//////////////
void setHp0HauptsignalnF(byte pntr) {
  // led 1 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, 0);
  Tlc.update();
}
/////////////
void setHp1HauptsignalnF(byte pntr) {
  // led 1 aan, de rest uit  
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
  Tlc.set(signalChannel[pntr]+2, 0);
  Tlc.update();
}
/////////////
void setHp2HauptsignalnF(byte pntr) {
  // led 2&3 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.update();
}

/////////////////////////////////////// Ausfahrsignal Fade ////////////////////////////////////////////////////////////////
//Signalbilder sind Hp0 (Rot, Rot), Hp1 (Grün), Hp2 (Gelb, Grün), Hp0/Sh1 (Rot, Weiss, Weiss)
//
//Led 1 = rot (links)
//led 2 = rot (rechts)
//led 3 = grun
//led 4 = gelb
//led 5 = beide weisse leds 
//////////////
void setHp0AusfahrsignalF(byte pntr) {
  // led 1 & 2  aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]+0], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
}
/////////////
void setHp1AusfahrsignalF(byte pntr) {
  // led 3 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
}
/////////////
void setHp2AusfahrsignalF(byte pntr) {
  // led 3&4 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
}
/////////////
void setSh1AusfahrsignalF(byte pntr) {
  // led 1&5 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+4, 0, dimConst[signalChannel[pntr]+4], startMillis, endMillis);
}

/////////////////////////////////////// Ausfahrsignal noFade ////////////////////////////////////////////////////////////////
//Signalbilder sind Hp0 (Rot, Rot), Hp1 (Grün), Hp2 (Gelb, Grün), Hp0/Sh1 (Rot, Weiss, Weiss)
//
//Led 1 = rot (links)
//led 2 = rot (rechts)
//led 3 = grun
//led 4 = gelb
//led 5 = beide weisse leds 
//////////////
void setHp0AusfahrsignalnF(byte pntr) {
  // led 1 & 2  aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
  Tlc.set(signalChannel[pntr]+2, 0);
  Tlc.set(signalChannel[pntr]+3, 0);
  Tlc.set(signalChannel[pntr]+4, 0);
  Tlc.update();
}
/////////////
void setHp1AusfahrsignalnF(byte pntr) {
  // led 3 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, 0);
  Tlc.set(signalChannel[pntr]+4, 0);
  Tlc.update();
}
/////////////
void setHp2AusfahrsignalnF(byte pntr) {
  // led 3&4 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
  Tlc.set(signalChannel[pntr]+4, 0);
  Tlc.update();
}
/////////////
void setSh1AusfahrsignalnF(byte pntr) {
  // led 1&5 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, 0);
  Tlc.set(signalChannel[pntr]+3, 0);
  Tlc.set(signalChannel[pntr]+4, dimConst[signalChannel[pntr]+4]);
  Tlc.update();
}

//////////////////////////////// Rangiersignal Fade ///////////////////////////////////////////////////////////////////////
//Signalbilder sind Sh0 (Rot, Rot), Sh1 (Weiss, Weiss)
//
//Led 1 = rot rot (beide leds)
//led 2 = weiss weiss (beide leds) 
//////////////
void dunkelRangiersignal(byte pntr) {
  endMillis = startMillis + fadeConst;
  for (byte x=0; x<2; x++){
    if (Tlc.get(signalChannel[pntr]+x) > 0) {tlc_addFade(signalChannel[pntr]+x, dimConst[signalChannel[pntr]+x], 0, startMillis, endMillis); }               //If not off then fade out
  }
  startMillis = endMillis + darkDelay;
}
//////////////
void setSh0RangiersignalF(byte pntr){
  // led 1  aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]+0], startMillis, endMillis);
}
/////////////
void setSh1RangiersignalF(byte pntr){
  // led 2  aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
}

//////////////////////////////// Rangiersignal no Fade ///////////////////////////////////////////////////////////////////////
//Signalbilder sind Sh0 (Rot, Rot), Sh1 (Weiss, Weiss)
//
//Led 1 = rot rot (beide leds)
//led 2 = weiss weiss (beide leds) 
//////////////
void setSh0RangiersignalnF(byte pntr){
  // led 1  aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.update();
}
/////////////
void setSh1RangiersignalnF(byte pntr){
  // led 2  aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
  Tlc.update();
}
  

