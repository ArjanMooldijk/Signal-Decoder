# Signal-Decoder
Arduino Sketches for DCC Signal decoders

Contains sketches to generate Swiss signals using an Arduino and TLS5940 chip. All controlled with DCC.
------- OLD -------
    Libraries for TLC and DCC are included in the .zip and can also be downloaded on the web for the moste recent version. 
    The TLC library is slightly modified to enable fading (parameter setting in the lib).
    
    The third sketch is a test program. With this you can enter address and aspect of the signal you want set using the serial monitor. Address and aspect are to be separated by a comma (eg.: 100,1 to                 set aspect 1 of address 100)
    
    Added a sketch for German signals. This one also facilitates the use of multiple daisy chained TLCs. In case of use also alter the variable NUM_TLCS in tlc_config.h.
    I have not been able to test this sketch as I do not have a daisy chained TLC setup available. Use at your own risk and please let me know if something is wrong.

    May 2021: these sketches can be found in /older versions
--------------------
November 2019: 
-
Added new constants:
Vor1AdrF & Vor1AdrnF for Vorsignale with orange and green leds in serie. These can only display Warnung (orange, orange) and Fahrt (green, green). The orange and green lights have one wire/port each.

HbB = Halt bei Bedarf Signal. This is a special signal with two flashing white lights when a train is to stop at a station. It has the same behavior as a SIM Vorsignal.

In the code I created a separate routine for the blinking.

May 2020: 
-
Did some maintenance. Replaced the #define by enum's

May 2021:
-
Rewrote everything to use the PlatformIO platfrom in VS Code.
Split up the source in multiple header files.

All that is needed now is 
1. fill out the addresses of the decoder (line 18 in /src/signal_decoder.cpp),
2. fill out the type of connected signal ( line 21 in /src/signal_decoder.cpp). Values for the types can be found in /include/Settings.h,
3. adjust the brightnes of the individual leds on line 24 (array dim_cnst)

