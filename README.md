# Signal-Decoder
Arduino Sketches for DCC Signal decoders

Contains two sketches to generate Swiss signals using an Arduino and TLS5940 chip. All controlled with DCC.
Libraries for TLC and DCC are included in the .zip and can also be downloaded on the web for the moste recent version. 
The TLC library is slightly modified to enable fading (parameter setting in the lib).

The third sketch is a test program. With this you can enter address and aspect of the signal you want set using the serial monitor. Address and aspect are to be separated by a comma (eg.: 100,1 to set aspect 1 of address 100)

Added a sketch for German signals. This one also facilitates the use of multiple daisy chained TLCs. In case of use also alter the variable NUM_TLCS in tlc_config.h.
I have not been able to test this sketch as I do not have a daisy chained TLC setup available. Use at your own risk and please let me know if something is wrong.
