#ifndef Initialisatie_h
#define Initialisatie_h

#include <Settings.h>
#include <Vorsignalbilder.h>
#include <Hauptsignalbilder.h>
#include <Hilfsignalbilder.h>
#include <Zwergsignalbilder.h>

//////////////////////////
void Initialiseer_decoder (){
  for (byte x = 0; x < anZahl; x ++) {   // set all default Halt and register starting pin per signal
    switch (signalType[x]) {
      case VorAdr1F:
        if (signalType[x+1]==VorAdr2F || signalType[x+1]==VorAdr2Dkl) {
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
             if (signalType[x+2]==VorAdr3Dkl) {
               signalLeds[x+2] = 4;
               signalChannel[x+2] = pinCounter;
             }
             pinCounter = pinCounter + 4;
          }
        }
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

      case Vor1AdrF:
        signalLeds[x] = 2;
        signalChannel[x] = pinCounter;
        if (signalType[x+1]==VorAdr2Dkl) { signalChannel[x+1] = pinCounter;}
        pinCounter = pinCounter + 2;
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
        if (signalType[x+1]==VorAdr2nF || signalType[x+1]==VorAdr2Dkl) {
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
          }
        }
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

      case Vor1AdrnF:
        signalLeds[x] = 2;
        signalChannel[x] = pinCounter;
        if (signalType[x+1]==VorAdr2Dkl) { signalChannel[x+1] = pinCounter;}
        pinCounter = pinCounter + 2;
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
        setHilfsignal(x,0);
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
        setHilfsignal(x,0);
        break;

      case HbB:
        signalLeds[x] = 1;
        signalChannel[x] = pinCounter;
        pinCounter = pinCounter + 1;
        Serial.print("Pin: ");
        Serial.print(signalChannel[x]);
        Serial.print(" Adres: ");
        Serial.print(signalAdr[x]);
        Serial.println(" Halt bei Bedarf Signal");
        setHilfsignal(x,0);                         //Hetzelfde als SIM Vorsignal; gebruik dezelfde routines voor aansturing
        break;

      case FSM:
        signalLeds[x] = 1;
        signalChannel[x] = pinCounter;
        pinCounter = pinCounter + 1;
        Serial.print("Pin: ");
        Serial.print(signalChannel[x]);
        Serial.print(" Adres: ");
        Serial.print(signalAdr[x]);
        Serial.println(" Halt bei Bedarf Signal");
        setHilfsignal(x,0);                         //Hetzelfde als SIM Vorsignal; gebruik dezelfde routines voor aansturing
        break;

      case HptAdr1F:
        if (signalType[x+1]== HptAdr2OF || signalType[x+1]== HptAdr2GF) {
          signalLeds[x] = 3;
          signalLeds[x+1] = 3;
          signalChannel[x] = pinCounter;
          signalChannel[x+1] = pinCounter;
          pinCounter = pinCounter + 3;
        }
        else {
          if (signalType[x+1]== HptAdr2OGF || signalType[x+1]== HptAdr2GOF) {
            if (signalType[x+2]== HptAdr3OF || signalType[x+1]== HptAdr3GF) {
              signalLeds[x] = 5;
              signalLeds[x+1] = 5;
              signalLeds[x+2] = 5;
              signalChannel[x] = pinCounter;
              signalChannel[x+1] = pinCounter;
              signalChannel[x+2] = pinCounter;
              pinCounter = pinCounter + 5;
            }
            else {
              if (signalType[x+2]== HptAdr3OGF) {
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

      case HptAdr1nF:        
          if (signalType[x+1]== HptAdr2OGnF || signalType[x+1]== HptAdr2GOnF) {
            if (signalType[x+2]== HptAdr3OnF || signalType[x+1]== HptAdr3GnF) {
              signalLeds[x] = 5;
              signalLeds[x+1] = 5;
              signalLeds[x+2] = 5;
              signalChannel[x] = pinCounter;
              signalChannel[x+1] = pinCounter;
              signalChannel[x+2] = pinCounter;
              pinCounter = pinCounter + 5;
            }
            else {
              if (signalType[x+2]== HptAdr3OGnF) {
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
            }
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

#endif