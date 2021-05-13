#ifndef Settings_h
#define Settings_h

#include <Arduino.h>

enum SignalTypes {
     VorAdr1F       , // Vorsignal addr 1,  Fade    Voorsignaal, eerste adres  beelden Warnung & Fb1
     Vor1AdrF       , // Vorsignal 1 addr,  Fade    Voorsignaal, één adres (2 pins)  beelden Warnung & Fb1
     VorAdr2F       , // Vorsignal addr 2,  Fade    Voorsignaal, tweede adres  beelden Fb2 & Fb3
     VorAdr3F       , // Vorsignal addr 3,  Fade    Voorsignaal, derde adres  beeld Fb5
     VorAdr2Dkl     , // Vorsignal addr 2,  Fade    Voorsignaal, tweede adres donker schakelen
     VorAdr3Dkl     , // Vorsignal addr 3,  Fade    Voorsignaal, derde adres donker schakelen
     VorAdr1nF      , // Vorsignal addr 1,  no Fade Voorsignaal, eerste adres  beelden Warnung & Fb1
     Vor1AdrnF      , // Vorsignal 1 addr,  no Fade Voorsignaal, één adres (2 pins)  beelden Warnung & Fb1
     VorAdr2nF      , // Vorsignal addr 2,  no Fade Voorsignaal, tweede adres  beelden Fb2 & Fb3
     VorAdr3nF      , // Vorsignal addr 3,  no Fade Voorsignaal, derde adres  beeld Fb5
//
     HptAdr1F       , // Haupt addr 1,     Fade    Hoofdsignaal eerste adres beelden Fb0 & Fb1                         G,R
     HptAdr2OF      , // Haupt addr 2,     Fade    Hoofdsignaal tweede adres beeld Fb2 (3e licht oranje)               g,r,O
     HptAdr2GF      , // Haupt addr 2,     Fade    Hoofdsignaal tweede adres beeld Fb3 (3e licht groen)                g,r,G
     HptAdr2OGF     , // Haupt addr 2,     Fade    Hoofdsignaal tweede adres beeld Fb2 & Fb3 (3e oranje, 4e groen)     g,r,O,G,..
     HptAdr2GOF     , // Haupt addr 2,     Fade    Hoofdsignaal tweede adres beeld Fb2 & Fb3 (3e groen, 4e oranje)     g,r,G,O,..
     HptAdr3OF      , // Haupt addr 3,     Fade    Hoofdsignaal derde adres beeld Fb6 (5e licht oranje)                g,r,o,g,O
     HptAdr3GF      , // Haupt addr 3,     Fade    Hoofdsignaal derde adres beeld Fb5 (5e licht groen)                 g,r,g,o,G
     HptAdr3OGF     , // Haupt addr 3,     Fade    Hoofdsignaal derde adres beeld Fb5 & Fb6 (5e oranje, 6e groen)      g,r,o,g,O,G,r (niet aangesloten)

     HptAdr1nF      , // Haupt addr 1,   noFade    Hoofdsignaal eerste adres beelden Fb0 & Fb1                         G,R
     HptAdr2OnF     , // Haupt addr 2,   noFade    Hoofdsignaal tweede adres beeld Fb2 (3e licht oranje)               g,r,O
     HptAdr2GnF     , // Haupt addr 2,   noFade    Hoofdsignaal tweede adres beeld Fb3 (3e licht groen)                g,r,G
     HptAdr2OGnF    , // Haupt addr 2,   noFade    Hoofdsignaal tweede adres beeld Fb2 & Fb3 (3e oranje, 4e groen)     g,r,O,G,..
     HptAdr2GOnF    , // Haupt addr 2,   noFade    Hoofdsignaal tweede adres beeld Fb2 & Fb3 (3e groen, 4e oranje)     g,r,G,O,..
     HptAdr3OnF     , // Haupt addr 3,   noFade    Hoofdsignaal derde adres beeld Fb6 (5e licht oranje)                g,r,o,g,O
     HptAdr3GnF     , // Haupt addr 3,   noFade    Hoofdsignaal derde adres beeld Fb5 (5e licht groen)                 g,r,g,o,G
     HptAdr3OGnF    , // Haupt addr 3,   noFade    Hoofdsignaal derde adres beeld Fb5 & Fb6 (5e oranje, 6e groen)      g,r,o,g,O,G,r (niet aangesloten)
//
     ZwergAdr1F     , // Zwerg addr 1,      Fade    Dwersignaal, eerste adres  beelden Halt & Fahrt
     ZwergAdr2F     , // Zwerg addr 2,      Fade    Dwersignaal, tweede adres  beeld Fahrt mit Vorsicht
     ZwergAdr1nF    , // Zwerg addr 1,      no Fade Dwersignaal, eerste adres  beelden Halt & Fahrt
     ZwergAdr2nF    , // Zwerg addr 2,      no Fade Dwersignaal, tweede adres  beeld Fahrt mit Vorsicht
//
     SIMHaupt       , // SIM Haupt,         SIM hoofdsein
     SIMVor         , // SIM Vor,           SIM voorsein
     HbB            , // HbB addr 1,        Halt bei Bedarf sein
     FSM            , // FSM addr 1,        Fahrtstellungsmelder 
};


byte signalLeds[16];                          // # leds per signal
byte signalChannel[16];                       // Eerste pin per het sein
byte Blink [16] = {0};                      // Flag to blink pin (for SIM)
byte blinkState [16] = {0};                 // Blink on/off
unsigned long previousMillis[16]= {0};      // will store last time LED was updated
byte pinCounter = 0;                            // Variable to address the right pin on the TLC5940
uint32_t startMillis;                       // millisecond variable for fade
uint32_t endMillis;                         // millisecond variable for fade
unsigned long LastTime[16] = {0};           // will store last time packet received
byte lastBild[12] = {0};                    // last signal bild for Zwergconst int anZahl = sizeof(signalType);

#endif