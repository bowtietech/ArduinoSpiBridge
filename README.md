# ArduinoSpiBridge
## USB to SPI bridge (Transmit Only)
Copyright (C) Bowtie Technology, Inc.
Released under the terms of the MIT License. 

A simple transmit-only USB -> SPI bridge, designed for the Arduino Nano:
https://store.arduino.cc/usa/arduino-nano

They're cheaper on Amazon, but official seemed right to put in here. 

Timing:

UART (USB Serial) - 115200 baud
SPI               - 125000 baud

Characters are transfered by sending a command, followed by options 
(if available), then data. 

Spitting data mercilessly at this may break it; make sure your command
characters are appropriate:

```
   '4'              - Will send the next 4 bytes of data received. Be 
                      careful to send only 4 bytes after the command or 
                      you may trigger another write on accident
   'w', <length>    - Will write <length> bits to the SPI. Be careful to 
                      check the length carefully for fear of triggering another
                      write, effectively locking the system and providing 
                      junk results.
```


EOF
