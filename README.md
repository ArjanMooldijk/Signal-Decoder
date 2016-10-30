# Signal-Decoder
Arduino Sketches for CH Signal decoder

Contains two sketches to generate Swiss signals using an Arduino and TLS5940 chip. All controlled with DCC.
Libraries for TLC and DCC are included in the .zip and can also be downloaded on the web for the moste recent version. 
The TLC library is slightly modified to enable fading (parameter setting in the lib).

The third sketch is a test program. With this you can enter address and aspect of the signal you want set using the serial monitor. Address and aspect are to be separated by a comma (eg.: 100,1 to set aspect 1 of address 100)
