#ifndef Hauptsignalbilder_h
#define Hauptsignalbilder_h

#include <Settings.h>
#include <tlc_fades.h>
/////////////////////////////////////// Hauptsignal Fade ////////////////////////////////////////////////////////////////
  //lamp 1 = grun (hoch)
  //lamp 2 = rot
//////////////
void dunkelHauptsignal(byte pntr) {
  endMillis = startMillis + fadeConst;
  for (byte x=0; x<signalLeds[pntr]; x++){
    if (Tlc.get(signalChannel[pntr]+x) > 0) tlc_addFade(signalChannel[pntr]+x, dimConst[signalChannel[pntr]+x], 0, startMillis, endMillis); //If not off then fade out
  }
  startMillis = endMillis + darkDelay;
}
/////////////////
void setFb0HauptsignalF(byte pntr) {
  // led 2 (x=1) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+1, 0, dimConst[signalChannel[pntr]+1], startMillis, endMillis);
}
/////////////
void setFb1HauptsignalF(byte pntr) {
  // led 1 (x=0) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
}
/////////////
  //lamp 3 = oranje
  //lamp 4 = groen (of mist)
/////////////
void setFb2HauptsignalogF(byte pntr) {
  // led 1&3 (x=0&2) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
}
void setFb3HauptsignalogF(byte pntr) {
  // led 1&4 (x=0&3) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
}
/////////////
//lamp 3 = groen
//lamp 4 = oranje (of mist)
/////////////
void setFb3HauptsignalgoF(byte pntr) {
  // led 1&3 (x=0&2) aan
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
}
void setFb2HauptsignalgoF(byte pntr) {
  // led 1&4 (x=0&3) aan
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
}


/////////////
//lamp 5 = groen
/////////////
void setFb5HauptsignalF5(byte pntr) {
  // led 1&3&5 (x=0&2&4) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+0, 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+4, 0, dimConst[signalChannel[pntr]+4], startMillis, endMillis);
}

/////////////
//lamp 5 = oranje
/////////////
void setFb6HauptsignalF5(byte pntr) {
  // led 3&5 (x=2&4) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+4, 0, dimConst[signalChannel[pntr]+4], startMillis, endMillis);
}
/////////////
//lamp 5 = oranje
//lamp 6 = groen
/////////////
void setFb5HauptsignalF7(byte pntr) {
  // led 1&4&6 (x=0&3&5) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr], 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+3, 0, dimConst[signalChannel[pntr]+3], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+5, 0, dimConst[signalChannel[pntr]+5], startMillis, endMillis);
}
/////////////
void setFb6HauptsignalF7(byte pntr) {
  // led 3&5 (x=2&4) aan, de rest uit
  endMillis = startMillis + fadeConst * 2;
  tlc_addFade(signalChannel[pntr], 0, dimConst[signalChannel[pntr]], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+2, 0, dimConst[signalChannel[pntr]+2], startMillis, endMillis);
  tlc_addFade(signalChannel[pntr]+4, 0, dimConst[signalChannel[pntr]+4], startMillis, endMillis);
}
/////////////////////////////////////// Hauptsignal noFade ////////////////////////////////////////////////////////////////
  //lamp 1 = grun (hoch)
  //lamp 2 = rot
//////////////
void setFb0HauptsignalnF(byte pntr) {
  // led 2 (x=1) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, dimConst[signalChannel[pntr]+1]);
  switch (signalLeds[pntr]){
  case 3:
    Tlc.set(signalChannel[pntr]+2, 0);
    break;
  case 4:
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    break;
  case 5:
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    Tlc.set(signalChannel[pntr]+4, 0);
    break;
  case 6:
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    Tlc.set(signalChannel[pntr]+4, 0);
    Tlc.set(signalChannel[pntr]+5, 0);
    break;
  default:
    break;
  }
  Tlc.update();
}
/////////////
void setFb1HauptsignalnF(byte pntr) {
  // led 1 (x=0) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  switch (signalLeds[pntr]){
  case 3:
    Tlc.set(signalChannel[pntr]+2, 0);
    break;
  case 4:
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    break;
  case 5:
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    Tlc.set(signalChannel[pntr]+4, 0);
    break;
  case 6:
    Tlc.set(signalChannel[pntr]+2, 0);
    Tlc.set(signalChannel[pntr]+3, 0);
    Tlc.set(signalChannel[pntr]+4, 0);
    Tlc.set(signalChannel[pntr]+5, 0);
    break;
  default:
    break;
  }
  Tlc.update();
}

/////////////
  //lamp 3 = oranje
  //lamp 4 = groen (of mist)
/////////////
void setFb2HauptsignalognF(byte pntr) {
  // led 1&3 (x=0&2) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  switch (signalLeds[pntr]){
    case 4:
      Tlc.set(signalChannel[pntr]+3, 0);
    case 5:
      Tlc.set(signalChannel[pntr]+4, 0);
      break;
    case 6:
      Tlc.set(signalChannel[pntr]+4, 0);
      Tlc.set(signalChannel[pntr]+5, 0);
      break;
    default:
      break;
  }
  Tlc.update();
}
/////////////
void setFb3HauptsignalognF(byte pntr) {
  // led 1&4 (x=0&3) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, 0);
  Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
  switch (signalLeds[pntr]){
    case 4:
      Tlc.set(signalChannel[pntr]+3, 0);
    case 5:
      Tlc.set(signalChannel[pntr]+4, 0);
      break;
    case 6:
      Tlc.set(signalChannel[pntr]+4, 0);
      Tlc.set(signalChannel[pntr]+5, 0);
      break;
    default:
      break;
  }
  Tlc.update();
}/////////////
// lamp 3 = groen
// lamp 4 = oranje (of mist)
void setFb3HauptsignalgonF(byte pntr) {
  // led 1&4 (x=0&3) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  switch (signalLeds[pntr]){
    case 4:
      Tlc.set(signalChannel[pntr]+3, 0);
    case 5:
      Tlc.set(signalChannel[pntr]+4, 0);
      break;
    case 6:
      Tlc.set(signalChannel[pntr]+4, 0);
      Tlc.set(signalChannel[pntr]+5, 0);
      break;
    default:
      break;
  }
  Tlc.update();
}
/////////////
 void setFb2HauptsignalgonF(byte pntr) {
  // led 1&4 (x=0&3) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, 0);
  Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
  switch (signalLeds[pntr]){
  case 5:
    Tlc.set(signalChannel[pntr]+3, 0);
    Tlc.set(signalChannel[pntr]+4, 0);
    break;
  case 6:
    Tlc.set(signalChannel[pntr]+3, 0);
    Tlc.set(signalChannel[pntr]+4, 0);
    Tlc.set(signalChannel[pntr]+5, 0);
    break;
  default:
    break;
  }
  Tlc.update();
}
/////////////
void setFb5HauptsignalnF5(byte pntr) {
  // led 1&3&5 (x=0&2&4) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, 0);
  Tlc.set(signalChannel[pntr]+4, dimConst[signalChannel[pntr]+4]);
  Tlc.update();
}
/////////////
void setFb5HauptsignalnF7(byte pntr) {
  // led 1&4&6 (x=0&3&5) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, dimConst[signalChannel[pntr]]);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, 0);
  Tlc.set(signalChannel[pntr]+3, dimConst[signalChannel[pntr]+3]);
  Tlc.set(signalChannel[pntr]+4, 0);
  Tlc.set(signalChannel[pntr]+5, dimConst[signalChannel[pntr]+5]);
  Tlc.update();
}
/////////////
void setFb6HauptsignalnF5(byte pntr) {
  // led 3&5 (x=2&4) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, 0);
  Tlc.set(signalChannel[pntr]+4, dimConst[signalChannel[pntr]+4]);
  Tlc.update();
}
/////////////
void setFb6HauptsignalnF7(byte pntr) {
  // led 3&5 (x=2&4) aan, de rest uit
  Tlc.set(signalChannel[pntr]+0, 0);
  Tlc.set(signalChannel[pntr]+1, 0);
  Tlc.set(signalChannel[pntr]+2, dimConst[signalChannel[pntr]+2]);
  Tlc.set(signalChannel[pntr]+3, 0);
  Tlc.set(signalChannel[pntr]+4, dimConst[signalChannel[pntr]+4]);
  Tlc.set(signalChannel[pntr]+5, 0);
  Tlc.update();
}
#endif