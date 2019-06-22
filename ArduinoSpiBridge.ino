/*
   USB to SPI bridge (Transmit Only)
   Copyright (C) Bowtie Technology, Inc.
   Released under the terms of the MIT License. Google it. It's rad.

   Spitting data mercilessly at this may break it; make sure your command
   characters are appropriate:

   '4'              - Will send the next 4 bytes of data received. Be 
                      careful to send only 4 bytes after the command or 
                      you may trigger another write on accident
   'w', <length>    - Will write <length> bits to the SPI. Be careful to 
                      check the length carefully for fear of triggering another
                      write, effectively locking the system and providing 
                      junk results.
 */
#include <SPI.h>

// ===========================================================================
// Software Configuration
#define ECHO_ON     0   // Echo characters over serial

// ===========================================================================
// Hardware Configuration
#define CS_PIN      9   // Chip Select pin number (Active Low)
#define LED_PIN     13  // LED transfer indicator pin number


// ===========================================================================
// Hardware Initialization
void setup()
{
  Serial.begin(115200);
  digitalWrite(CS_PIN, 1);
  pinMode(CS_PIN, OUTPUT);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV64);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();
}


// ===========================================================================
// Main Loop
void loop()
{
  unsigned int  dataLength  = 0;
  unsigned int  charCount   = 0;
  unsigned char inputChar   = '\0';
  unsigned char commandChar = '\0';

  // Wait for serial data in
  while (!Serial.available());

  // Get the command byte
  commandChar = Serial.read();
  
  switch (commandChar)
  {
      // Send 4 bytes of data (one word) 
      // --
      // May look random, but we need it for a special purpose...
      // Testing in the terminal with the 'w' is pretty tough, given
      // how much data it will take to get to most characters. We
      // can't terminate a raw write with a character or sequence,
      // because it may happen in the data by chance... ruining that 
      // transfer.
      case '4': 
        dataLength = 4;
        break;
        
      // Raw Write
      // --
      // Do a raw write, send 'w' followed by a length (int, not ASCII), 
      // then the data packet
      case 'w':
        while (!Serial.available());
        dataLength = Serial.read();
        break;
        
      // Default - Do, uhh... nada
      // --
      // Don't bother reading or sending errors, reset the
      // data length so that we catch the next character and test it
      default:
        dataLength = 0; 
        break;
  }

  charCount = 0;  // Reset Character Count
  
  digitalWrite(LED_PIN, 1); // Turn on the LED (Before CS Enable)
  digitalWrite(CS_PIN, 0);  // Enable Chip Select (Active Low)

  // Write the appropriate amount of data
  while (charCount < dataLength) 
  {
    while (!Serial.available());  // Wait for the next byte to be available
    inputChar = Serial.read();    // Grab it by the bits
    SPI.transfer(inputChar);      // Transfer it via SPI
    charCount++;                  // Tally it up
#if ECHO_ON
    Serial.write(inputChar);      // Local Echo ON
#endif
  }

  // Disable Chip Select
  digitalWrite(CS_PIN, 1);  // Disable Chip Select (Active Low)
  digitalWrite(LED_PIN, 0); // Turn off the LED (After CS Disabled)

  delayMicroseconds(10);    // Wait a bit, let any ISRs process
}

// EOF
