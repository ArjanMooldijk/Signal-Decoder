// Sketch for Swiss Signals
// Version 3 (01-05-21)  Arjan Mooldijk
// Thanks to Franz-Peter "MicroBahner" (Stummis) for the DCC examples
// Thanks to Alex Leone for the TLC library

#include <Arduino.h>
#include <NmraDcc.h>
#include <Tlc5940.h>
#include <tlc_fades.h>
#include <Settings.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTANTS TO BE ADAPTED  ///////////////////////////////////////////////////////////////////////////////////////////
// Decoder number. This value is stored in the decoder and displayed on the serial monitor.                         ///
// Only used for your own administration. Not functional use                                                        ///
const byte decoderNbr = 1;                                                                                          ///
                                                                                                                    ///
// DCC adresses:                                                                                                    ///
const unsigned int signalAdr[] =   {400, 401, 402, 403, 404, 405};                           /// Values just an Example
                                                                                                                    ///
// Signal types per in line 18 listed address  (types can be found in /include/Settings.h)                          ///
const byte signalType[]        =   { HptAdr1F, HptAdr2OGF, HptAdr3OF, VorAdr1F, VorAdr2F, VorAdr3Dkl}; /// Example
                                                                                                                    ///
// Brightness per ledper signal as listed above                                                                     ///
const int dimConst [16] = {20,20,20,20,20,
                           20,20,20,20,
                           0,0,0,0,0,0,0};       // Brightness per led (max 16) Every row a signal (just a tip)     ///
                                                                                                                    ///
                                                                                                                    /// 
const long fadeConst = 150;                 // Fade in/decrease in mSec.                                            ///
const long darkDelay = 350;                 // Dark phase duration in mSec.                                         ///
const long interval = 800;                  // interval to blink in mSec.                                           ///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--------    DO NOT MAKE ANY CHANGES BELOW, UNLESS YOU WANT TO ALTER THE PROGRAM ;-)    ---------------------------///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int anZahl = sizeof(signalType);
unsigned int SeqWait = 3*fadeConst+darkDelay+1; // Total time for one sequence to commplete
#include <Vorsignalbilder.h>
#include <Hauptsignalbilder.h>
#include <Zwergsignalbilder.h>
#include <Hilfsignalbilder.h>
#include <Initialisatie.h>
#include <HandleAddress.h>
NmraDcc Dcc;
//////////////////////////////////////////////////////////////
// Unterprogramme, die von der DCC Library aufgerufen werden:
// Die folgende Funktion wird von Dcc.process() aufgerufen, wenn ein Weichentelegramm empfangen wurde
void notifyDccAccState( uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State ){
  for ( byte index = 0; index < anZahl; index++ ) {
    if (Addr == signalAdr[index]) {                     // ist eigene Adresse
        HandleCommand(index,OutputAddr);
    }
  }
}

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
    Serial.println("Software version 3 210501"); 
    Serial.print("Nbr of adresses: ");
    Serial.println(anZahl);
    Initialiseer_decoder();  
}
////////////////////////////////////////////////////////////////
void loop() {
  Dcc.process(); // Hier werden die empfangenen Telegramme analysiert

  //execute fade
  delay(1);
  tlc_updateFades();

  handle_blink ();
}