#ifndef Hilfsignalbilder_h
#define Hilfsignalbilder_h

#include <Settings.h>
#include <tlc_fades.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Hilfsignale:
//    SIM : 1 pin
//    Haltbeibedarf : 1 pin
//    Fahrtstellungsmelder : 1 pin
///////////////////////////////////////
void setHilfsignal (byte pntr, byte Fb){
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


#endif