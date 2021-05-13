#ifndef Zwergsignalbilder_h
#define Zwergsignalbilder_h

#include <Settings.h>
#include <tlc_fades.h>
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




#endif