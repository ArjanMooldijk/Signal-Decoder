#ifndef Vorsignalbilder_h
#define Vorsignalbilder_h

#include <Settings.h>
#include <tlc_fades.h>
////////////////////////////////////// Vorsignal Fade/////////////////////////////////////////////////////////////////
//Signalbilder sind Warnung (Gelb1, Gelb2), Fb1 (Grün1, Grün2), Fb2 (Gelb1, Grün1), Fb3 (Gelb1, Grün1, Grün2), Fb5 (Grün1, Grün2, Gelb3)
//lamp1 = gelb (links)
//lamp2 = gelb (rechts oben)
//lamp3 = grun (rechts)
//lamp4 = grun (links)
//lamp5 = gelb (rechts unter)
//
// oder (Vor1AdrF):
//lamp1 = gelb (2 x oben)
//lamp2 = grun (2 x unter)
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
void setFb0VorsignalF(byte pntr) {
  // led 1&2 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr], 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  if(signalType[pntr]==VorAdr1F) {
    // led 2 ook aan
    tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
  }
}
/////////////
void setFb1VorsignalF(byte pntr) {
  endMillis = startMillis + fadeConst * 2;
  if(signalType[pntr]==VorAdr1F) {
    // led 3&4 aan, de rest uit
    tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
    tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
  }
  else {
    // led 2 aan, de rest uit
    tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
  }
}
/////////////
void setFb2VorsignalF(byte pntr) {
  // led 1&3 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr], 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
}
/////////////
void setFb3VorsignalF(byte pntr) {

  // led 1&3&4 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr], 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
}
/////////////
void setFb5VorsignalF(byte pntr) {
  // led 3&4&5 aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+4, 0, dimConst[signalChannel[pntr]+4], startMillis, endMillis);
}

////////////////////////////////////// Vorsignal noFade/////////////////////////////////////////////////////////////////
//Signalbilder sind Warnung (Gelb1, Gelb2), Fb1 (Grün1, Grün2), Fb2 (Gelb1, Grün1), Fb3 (Gelb1, Grün1, Grün2), Fb5 (Grün1, Grün2, Gelb3)
//lamp1 = gelb (links)
//lamp2 = gelb (rechts oben)
//lamp3 = grun (rechts)
//lamp4 = grun (links)
//lamp5 = gelb (rechts unter)
// oder (Vor1AdrF)
//lamp1 = 2 x gelb (oben)
//lamp2 - 2 x grun (unter)
//////////////
void setFb0VorsignalnF(byte pntr) {
  if(signalType[pntr]==VorAdr1F) {
    // led 1&2 aan, de rest uit
    Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]+0]);
    Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    if (signalLeds[pntr]==5) {Tlc.set(signalChannel[pntr]+4, 0);}
  }
  else {
    // led 1 aan, 2 uit
    Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]+0]);
    Tlc.set(signalChannel[pntr]+1, 0);
  }
  Tlc.update();
}
/////////////
void setFb1VorsignalnF(byte pntr) {
  if(signalType[pntr]==VorAdr1F) {
    // led 3&4 aan, de rest uit
    Tlc.set(signalChannel[pntr]+0, 0);
    Tlc.set(signalChannel[pntr]+1, 0);
    Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
    Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
    if (signalLeds[pntr]==5) {Tlc.set(signalChannel[pntr]+4, 0);}
  }
  else {
    // led 1 uit, 2 aan
    Tlc.set(signalChannel[pntr]+0, 0);
    Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
  }
  Tlc.update();
}
/////////////
void setFb2VorsignalnF(byte pntr) {
  // led 1&3 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]+0]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, 0);
  if (signalLeds[pntr]==5) {Tlc.set(signalChannel[pntr]+4, 0);}
  Tlc.update();
}
/////////////
void setFb3VorsignalnF(byte pntr) {
  // led 1&3&4 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]+0]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
  if (signalLeds[pntr]==5) {Tlc.set(signalChannel[pntr]+4, 0);}
  Tlc.update();
}
/////////////
void setFb5VorsignalnF(byte pntr) {
  // led 3&4&5 aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
  Tlc.set(signalChannel[pntr]+4, dimConst[signalChannel[pntr]+4]);
  Tlc.update();
}



#endif