// Sketch voor Zwitserse signalen
// Version 2.8 (01-05-20)  Arjan Mooldijk
// Thanks to Franz-Peter "MicroBahner" (Stummis) for the DCC examples
// Thanks to Alex Leone for the TLC library
//
//Toepassing voor Stadler portaal linker signalen (één 6-licht Hoofdsignaal en een 4-licht Voorsignaal)

#include <Arduino.h>
#include <NmraDcc.h>
#include <Tlc5940.h>
#include <tlc_fades.h>
#include <Settings.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AAN TE PASSEN CONSTANTEN ///////////////////////////////////////////////////////////////////////////////////////////
// Decoder nummer. Dit wordt in de decoder zelf opgeslagen en is met de monitor uit te lezen.                       ///
// Dan weet je welke het is.                                                                                        ///
const byte decoderNbr = 1;                                                                                          ///
                                                                                                                    ///
// DCC adressen:                                                                                                    ///
const unsigned int signalAdr[] =   {400, 401, 402, 403, 404, 405};                                                  ///
                                                                                                                    ///
// Sein types per hierboven opgegeven adres                                                                         ///
const byte signalType[]        =   { HptAdr1F, HptAdr2OGF, HptAdr3OF, VorAdr1F, VorAdr2F, VorAdr3Dkl};          ///
                                                                                                                    ///
// Lichtsterkte van de leds per sein in volgorde van de opgegeven types                                             ///
const int dimConst [16] = {20,20,20,20,20,
                           20,20,20,20,
                           0,0,0,0,0,0,0};       // Lichtsterkte per led (max 16) Iedere rij is één signaal   ///
                                                                                                                    ///
                                                                                                                    /// 
const long fadeConst = 150;                 // Fade in/decrease in mSec.                                            ///
const long darkDelay = 350;                 // Dark phase duration in mSec.                                         ///
const long interval = 800;                  // interval to blink in mSec.                                           ///
///////// HIERONDER HOEFT NIETS MEER AANGEPAST TE WORDEN (TENZIJ JE HET PROGRAMMA WILT VERANDEREN ;-) /////////////////
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
    Serial.println("Software versie 2.9 210424"); 
    Serial.print("Aantal adressen: ");
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