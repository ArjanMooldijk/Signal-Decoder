// Sketch for Swiss dwarfsignals
// Version 2.1 (01-12-17)  Arjan Mooldijk
// Thanks to Franz-Peter "MicroBahner" (Stummis) for the DCC examples
// Thanks to Alex Leone for the TLC library

#include <NmraDcc.h>
#include <Tlc5940.h>
#include <tlc_fades.h>

#define ZwergAdr1F    1 // Zwerg addr 1,      Fade    3 leds Fade !!STARTPIN!!
#define ZwergAdr2F    2 // Zwerg addr 2,      Fade    3 leds Fade
#define ZwergAdr1nF   3 // Zwerg addr 1,      no Fade 3 leds !!STARTPIN!!
#define ZwergAdr2nF   4 // Zwerg addr 2,      no Fade 3 leds

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AAN TE PASSEN CONSTANTEN ///////////////////////////////////////////////////////////////////////////////////////////
// Decoder nummer. Dit wordt in de decoder zelf opgeslagen en is met de monitor uit te lezen.                       ///
// Dan weet je welke het is.                                                                                        ///
const byte decoderNbr = 1;                                                                                          ///
                                                                                                                    ///
// DCC adressen:                                                                                                    ///
const unsigned int signalAdr[] =   {551, 552, 553, 554, 555, 556, 557, 558, 559, 560};                              ///
                                                                                                                    ///
// Sein types per hierboven opgegeven adres                                                                         ///
const byte signalType[]        =   { ZwergAdr1F,ZwergAdr2F,ZwergAdr1F,ZwergAdr2F,ZwergAdr1F,
                                     ZwergAdr2F,ZwergAdr1F,ZwergAdr2F,ZwergAdr1F,ZwergAdr2F};                       ///
                                                                                                                    ///
// Lichtsterkte van de leds per sein in volgorde van de opgegeven types                                             ///
const int dimConst [16] = {10,10,13,
                           10,10,15,
                           10,10,15,
                           10,10,15,
                           10,10,15,
                           0};             // Brightness per led (max 16) Every row is one signal              ///
                                                                                                                    ///
                                                                                                                    ///
const int anZahl = sizeof(signalType);                                                                              ///
const long fadeConst = 150;                 // Fade in/decrease in mSec.                                            ///
const long darkDelay = 350;                 // Dark phase duration in mSec.                                         ///
const long interval = 800;                  // interval to blink in mSec.                                           ///
///////// HIERONDER HOEFT NIETS MEER AANGEPAST TE WORDEN (TENZIJ JE HET PROGRAMMA WILT VERANDEREN ;-) /////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables:
NmraDcc Dcc;
byte signalLeds[16];                        // # leds per signal
byte signalChannel[16];                     // Eerste pin per het sein
byte pinCounter = 0;                        // Variable to address the right pin on the TLC5940
uint32_t startMillis;                       // millisecond variable for fade
uint32_t endMillis;                         // millisecond variable for fade
unsigned int SeqWait = 3*fadeConst+darkDelay+1; // Total time for one sequence to commplete
uint32_t LastTime[16] = {0};                // will store last time packet received
byte lastBild[12] = {0};                    // last signal bild for Zwerg
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
    Serial.print("Zwerggignal decoder nr ");
    Serial.println(decoderNbr);
    Serial.print("Adressen: ");
    Serial.println(anZahl);
    lastSeq = millis();
    
    for (byte x = 0; x < anZahl; x ++) {   // set all default Halt and register starting pin per signal
      switch (signalType[x]) {
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
      if ( LastTime[index] < (millis()-SeqWait)) {
        switch (signalType[index]) {

//Zwergsignal Addr 1 Fahrt & Halt Fade
          case ZwergAdr1F:
            LastTime[index] = millis();
            LastTime[index+1] = LastTime[index];
            startMillis = millis() + 1;
            dunkelZwergsignal(index);
            if ( OutputAddr & 0x1 ) {
              setZwergsignalF(index, 1);
              lastBild[index] = 1;
              break;
            }
            else {
              setZwergsignalF(index, 0);
              lastBild[index] = 0;
              break;
            }

//Zwergsignal Addr 2 Fahrt mit Vorsicht Fade
          case ZwergAdr2F:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            startMillis = millis() + 1;
            dunkelZwergsignal(index);
            if ( OutputAddr & 0x1 ) {
              setZwergsignalF(index, 2);
              lastBild[index-1] = 2;
              break;
            }
            else {
              if (lastBild[index-1] == 0) {
                setZwergsignalF(index, 3);
                lastBild[index-1] = 1;
                break;
              }
              else {
                if (lastBild[index-1] == 2) {
                  setZwergsignalF(index, 1);
                  lastBild[index-1] = 1;
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
            LastTime[index] = millis();
            LastTime[index+1] = LastTime[index];
            if ( OutputAddr & 0x1 ) {
              setZwergsignalnF(index, 1);              
              lastBild[index] = 1;
              break;
            }
            else {
              setZwergsignalnF(index, 0);              
              lastBild[index] = 1;
              break;
            }

//Zwergsignal Addr 2 Fahrt mit Vorsicht noFade
          case ZwergAdr2nF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            startMillis = millis() + 1;
            if ( OutputAddr & 0x1 ) {
              setZwergsignalnF(index, 2);
              lastBild[index-1] = 2;
              break;
            }
            else {
              if (lastBild[index-1] == 0) {
                setZwergsignalnF(index, 3);
                lastBild[index-1] = 1;
                break;
              }
              else {
                if (lastBild[index-1] == 2) {
                  setZwergsignalnF(index, 1);
                  lastBild[index-1] = 1;
                }
                break;
              }
            }
          default:
            break;
        }
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
    case 3: // van Fb0  via Fb2 naar Fb1
       endMillis = startMillis + fadeConst * 2;      
       tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);      
       tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
       startMillis = endMillis + darkDelay;
       endMillis = startMillis + fadeConst;
       tlc_addFade(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1], 0, startMillis, endMillis);
       tlc_addFade(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2], 0, startMillis, endMillis);
       startMillis = endMillis + darkDelay; 
       endMillis = startMillis + fadeConst * 2;      
       tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]+0], startMillis, endMillis);      
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
    case 3: // van Fb0  via Fb2 naar Fb1
       endMillis = startMillis + 1;      
       tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);      
       tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
       startMillis = endMillis + darkDelay;
       endMillis = startMillis + 1;
       tlc_addFade(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1], 0, startMillis, endMillis);
       tlc_addFade(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2], 0, startMillis, endMillis);
       startMillis = endMillis + darkDelay; 
       endMillis = startMillis + 1;      
       tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]+0], startMillis, endMillis);      
       tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
       break;
    default:
       break;
       }
  Tlc.update();
}
