#ifndef HandleAddress_h
#define HandleAddress_h

#include <Arduino.h>
#include <NmraDcc.h>
#include <Tlc5940.h>
#include <tlc_fades.h>
#include <Settings.h>
#include <Vorsignalbilder.h>
#include <Hauptsignalbilder.h>
#include <Zwergsignalbilder.h>
#include <Hilfsignalbilder.h>

void HandleCommand (byte index, uint8_t OutputAddr) {
    if ( LastTime[index] < (millis()-SeqWait)) {
        switch (signalType[index]) {

//Vorsignal Addr 1 Fb1 & Fb0 Fade
        case VorAdr1F:
            LastTime[index] = millis();
            if (signalType[index+1]==VorAdr2F || signalType[index+1]==VorAdr2Dkl) {LastTime[index+1] = LastTime[index];}
            if (signalType[index+2]==VorAdr3F) { LastTime[index+2] = LastTime[index];}
            startMillis = millis() + 1;
            dunkelVorsignal(index);
            if ( OutputAddr & 0x1 ) {
                setFb1VorsignalF(index);
                break; 
            }
            else {
                setFb0VorsignalF(index);
                break;
            }

//Vorsignal 1 Addr Fb1 & Fb0 Fade
        case Vor1AdrF:
            LastTime[index] = millis();
            startMillis = millis() + 1;
            dunkelVorsignal(index);
            if ( OutputAddr & 0x1 ) {
                setFb1VorsignalF(index);
                break;
            }
            else {
                setFb0VorsignalF(index);
                break;
            }

//Vorsignal Addr 2 Fb3 & Fb2 Fade
        case VorAdr2F:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            if (signalType[index+1]==VorAdr3F) {LastTime[index+1] = LastTime[index];}
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

//Vorsignal Addr 2 Dunkel
        case VorAdr2Dkl:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            if (signalType[index+1]==VorAdr3F || signalType[index+1]==VorAdr3nF) {LastTime[index+1] = LastTime[index];}
            startMillis = millis() + 1;
            dunkelVorsignal(index);
            break;

//Vorsignal Addr 3 Dunkel
        case VorAdr3Dkl:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            LastTime[index-2] = LastTime[index];
            startMillis = millis() + 1;
            dunkelVorsignal(index);
            break;

//Vorsignal Addr 3 Fb5 Fade
        case VorAdr3F:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            LastTime[index-2] = LastTime[index];
            startMillis = millis() + 1;
            dunkelVorsignal(index);
            setFb5VorsignalF(index);
            break;

//Vorsignal Addr 1 Fb1 & Fb0 noFade
        case VorAdr1nF :
            LastTime[index] = millis();
            if (signalType[index+1]==VorAdr2nF || signalType[index+1]==VorAdr2Dkl) {LastTime[index+1] = LastTime[index];}
            if (signalType[index+2]==VorAdr3nF) { LastTime[index+2] = LastTime[index];}
            if ( OutputAddr & 0x1 ) {
                setFb1VorsignalnF(index);
                break; 
            }
            else {
                setFb0VorsignalnF(index);
                break;
            }

//Vorsignal 1 Addr Fb1 & Fb0 noFade
        case Vor1AdrnF :
            LastTime[index] = millis();
            if ( OutputAddr & 0x1 ) {
                setFb1VorsignalnF(index);
                break; 
            }
            else {
                setFb0VorsignalnF(index);
                break;
            }

//Vorsignal Addr 2 Fb3 & Fb2 noFade
        case VorAdr2nF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            if (signalType[index+1]==VorAdr3nF) {LastTime[index+1] = LastTime[index];}
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
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            LastTime[index-2] = LastTime[index];
            setFb5VorsignalnF(index);
            break;

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
                        break;
                    }
                }
            }

//SIMhauptsignal An & Aus
        case SIMHaupt:
            LastTime[index] = millis();
            if ( OutputAddr & 0x1 ) {
                 setHilfsignal(index, 2);
                break;
            }
            else {
                setHilfsignal(index, 0);
                break;
            }

//SIMvorsignal An & Aus
        case SIMVor:
            LastTime[index] = millis();
            if ( OutputAddr & 0x1 ) {
                setHilfsignal(index, 1);
                break;
            }
            else {
                setHilfsignal(index, 0);
                break;
            } 

//HbBsignal An & Aus
        case HbB:
            LastTime[index] = millis();
            if ( OutputAddr & 0x1 ) {
                setHilfsignal(index, 1);                         //Hetzelfde als SIM Vorsignal; gebruik dezelfde routines voor aansturing
                break;
            }
            else {
                setHilfsignal(index, 0);                         //Hetzelfde als SIM Vorsignal; gebruik dezelfde routines voor aansturing
                break;
            } 

//Fahrtstellungsmelder An & Aus
        case FSM:
            LastTime[index] = millis();
            if ( OutputAddr & 0x1 ) {
                setHilfsignal(index, 1);                         //Hetzelfde als SIM Vorsignal; gebruik dezelfde routines voor aansturing
                break;
            }
            else {
                setHilfsignal(index, 0);                         //Hetzelfde als SIM Vorsignal; gebruik dezelfde routines voor aansturing
                break;
            }

//Hauptsignal Addr 1 Fb1 & Fb0 Fade
        case HptAdr1F:
            LastTime[index] = millis();
            if (signalType[index+1]==HptAdr2OF || signalType[index+1]== HptAdr2GF || signalType[index+1]== HptAdr2OGF || signalType[index+1]== HptAdr2GOF) {
                LastTime[index+1] = LastTime[index];
            }
            if (signalType[index+2]== HptAdr3OF || signalType[index+2]== HptAdr3GF || signalType[index+2]== HptAdr3OGF) {
                LastTime[index+2] = LastTime[index];
            }
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
        case HptAdr2OF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            if (signalType[index+1]== HptAdr3OF || signalType[index+1]== HptAdr3GF || signalType[index+1]== HptAdr3OGF) {
                LastTime[index+1] = LastTime[index];
            }
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            setFb2HauptsignalogF(index);
            break;

//Hauptsignal Addr 2 Fb3 Fade
        case HptAdr2GF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            if (signalType[index+1]== HptAdr3OF || signalType[index+1]== HptAdr3GF || signalType[index+1]== HptAdr3OGF) {
                LastTime[index+1] = LastTime[index];
            }
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            setFb3HauptsignalgoF(index);
            break;

//Hauptsignal Addr 2 Fb3 & Fb2 Fade Oranje, Groen
        case HptAdr2OGF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            if (signalType[index+1]== HptAdr3OF || signalType[index+1]== HptAdr3GF || signalType[index+1]== HptAdr3OGF) {
                LastTime[index+1] = LastTime[index];
            }
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            if ( OutputAddr & 0x1 ) {
                setFb3HauptsignalogF(index);
                break;
            }
            else {
                setFb2HauptsignalogF(index);
                break;
            }

//Hauptsignal Addr 2 Fb3 & Fb2 Fade Groen, Oranje
        case HptAdr2GOF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            if (signalType[index+1]== HptAdr3OF || signalType[index+1]== HptAdr3GF || signalType[index+1]== HptAdr3OGF) {
                LastTime[index+1] = LastTime[index];
            }
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            if ( OutputAddr & 0x1 ) {
                setFb3HauptsignalgoF(index);
                break;
            }
            else {
                setFb2HauptsignalgoF(index);
                break;
            }

//Hauptsignal 5L Addr 3 Fb5 Fade
        case HptAdr3GF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            LastTime[index-2] = LastTime[index];
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            setFb5HauptsignalF5(index);
            break;

//Hauptsignal 5L Addr 3 Fb6 Fade
        case HptAdr3OF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            LastTime[index-2] = LastTime[index];
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            setFb6HauptsignalF5(index);
            break;
            
//Hauptsignal Addr 3 Fb6 & Fb5 Fade
        case HptAdr3OGF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            LastTime[index-2] = LastTime[index];
            startMillis = millis() + 1;
            dunkelHauptsignal(index);
            if ( OutputAddr & 0x1 ) {
                setFb6HauptsignalF7(index);
                break;
            }
            else {
                setFb5HauptsignalF7(index);
                break;
            }
             

//Hauptsignal Addr 1 Fb1 & Fb0 noFade
        case HptAdr1nF:
            LastTime[index] = millis();
            if (signalType[index+1]==HptAdr2OnF || signalType[index+1]== HptAdr2GnF || signalType[index+1]== HptAdr2OGnF || signalType[index+1]== HptAdr2GOnF) {
                LastTime[index+1] = LastTime[index];
            }
            if (signalType[index+2]== HptAdr3OnF || signalType[index+2]== HptAdr3GnF || signalType[index+2]== HptAdr3OGnF) {
                LastTime[index+2] = LastTime[index];
            }
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
        case HptAdr2OnF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            if (signalType[index+1]== HptAdr3OnF || signalType[index+2]== HptAdr3GnF || signalType[index+2]== HptAdr3OGnF) {
                LastTime[index+1] = LastTime[index];
            }
            startMillis = millis() + 1;
            setFb2HauptsignalognF(index);
            break;

//Hauptsignal Addr 2 Fb3 noFade
        case HptAdr2GnF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            if (signalType[index+1]==  HptAdr3OnF || signalType[index+2]== HptAdr3GnF || signalType[index+2]== HptAdr3OGnF) {
                LastTime[index+1] = LastTime[index];
            }
            startMillis = millis() + 1;
            setFb3HauptsignalgonF(index);
            break;

//Hauptsignal Addr 2 Fb3 & Fb2 noFade
        case HptAdr2OGnF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            if (signalType[index+1]== HptAdr3OnF || signalType[index+2]== HptAdr3GnF || signalType[index+2]== HptAdr3OGnF) {
                LastTime[index+1] = LastTime[index];
            }
            startMillis = millis() + 1;
            if ( OutputAddr & 0x1 ) {
                setFb3HauptsignalognF(index);
                break;
            }
            else {
                setFb2HauptsignalognF(index);
                break;
            }

//Hauptsignal Addr 2 Fb3 & Fb2 noFade
        case HptAdr2GOnF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            if (signalType[index+1]== HptAdr3OnF || signalType[index+2]== HptAdr3GnF || signalType[index+2]== HptAdr3OGnF) {
                LastTime[index+1] = LastTime[index];
            }
            startMillis = millis() + 1;
            if ( OutputAddr & 0x1 ) {
                setFb3HauptsignalgonF(index);
                break;
            }
            else {
                setFb2HauptsignalgonF(index);
                break;
            }

//Hauptsignal 5L Addr 3 Fb65 noFade
        case HptAdr3GnF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            LastTime[index-2] = LastTime[index];
            startMillis = millis() + 1;
            setFb5HauptsignalnF5(index);
            break;

//Hauptsignal 5l Addr 3 Fb6 noFade
        case HptAdr3OnF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            LastTime[index-2] = LastTime[index];
            startMillis = millis() + 1;
            setFb6HauptsignalnF5(index);
            break;

//Hauptsignal 7LAddr 3 Fb6 & Fb5 noFade
        case HptAdr3OGnF:
            LastTime[index] = millis();
            LastTime[index-1] = LastTime[index];
            LastTime[index-2] = LastTime[index];
            startMillis = millis() + 1;
            if ( OutputAddr & 0x1 ) {
                setFb6HauptsignalnF7(index);
                break;
            }
            else {
                setFb5HauptsignalnF7(index);
                break;
            }

        default:
          break;
        }
      }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void handle_blink() {
  unsigned long currentMillis = millis();
  for ( byte blindex = 0; blindex < 16; blindex++ ) {
    if (Blink[blindex]==1){                                           //Blinkt deze pin/led?
      if (currentMillis - previousMillis[blindex] >= interval) {      //Is de wachtperiode al voorbij?
        previousMillis[blindex] = currentMillis;
        if (blinkState[blindex]==1) {                                 //Led is aan, zet uit
          Tlc.set(blindex,0);
          blinkState[blindex] = 0;
        }
        else {                                                        //Led is uit, zet aan
          Tlc.set(blindex,dimConst[blindex]);
          blinkState[blindex] = 1;
        }
        Tlc.update();
      }
    }
  }
}
#endif