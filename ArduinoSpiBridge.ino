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
#define ECHO_ON     1   // Echo characters over serial
//#define TEST_MODE_COUNTING  // Uncomment to make it count up each byte
// ===========================================================================
// Hardware Configuration
#define CS_PIN      16   // Chip Select pin number (Active Low)
#define LED_PIN     13  // LED transfer indicator pin number


// ===========================================================================
// Hardware Initialization
void setup()
{
  // Serial Configuration
  Serial.begin(115200);       // Kick off the serial at 115,200 baud
  Serial.println("");
  Serial.println("SPI Bridge Active");

  // Pin Configuration
  digitalWrite(CS_PIN, 1);    // Set the CS pin to output HIGH (Disabled)
  pinMode(CS_PIN, OUTPUT);    // Set the CS pin to an output
  digitalWrite(LED_PIN, 0);    // Set the LED pin to output LOW (Disabled)
  pinMode(LED_PIN, OUTPUT);    // Set the LED pin to an output

  // SPI Configuration
  SPI.setBitOrder(MSBFIRST);              // Set the bit order 
  SPI.setClockDivider(SPI_CLOCK_DIV128);  // Set the Clock Divider (125 kHz)
//  SPI.setFrequency(400000);             // Set Clock Frequency
  SPI.setDataMode(SPI_MODE0);             // Set the SPI Mode
  SPI.begin();                            // Kick off the SPI
    SPI.setClockDivider(SPI_CLOCK_DIV8);  // Set the Clock Divider (125 kHz)
  SPI.setFrequency(1000000);             // Set Clock Frequency
  
}

#ifdef TEST_MODE_COUNTING
  unsigned char testChar = 0x1; // Test character
#endif 

// ===========================================================================
// Main Loop
void loop()
{
  unsigned int  dataLength  = 0;    // Data Length setting from client
  unsigned int  charCount   = 0;    // Current character count during transfer
  unsigned char inputChar   = '\0'; // Input character container
  unsigned char commandChar = '\0'; // Command character container

#ifdef TEST_MODE_COUNTING

  commandChar = '4';

#else

  // Wait for serial data in
  while (!Serial.available());

  // Get the command byte
  commandChar = Serial.read();

#endif
  
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

  if (0 == dataLength) {
    return;
  }
  
  digitalWrite(LED_PIN, 1); // Turn on the LED (Before CS Enable)
  digitalWrite(CS_PIN, 0);  // Enable Chip Select (Active Low)

  // Write the appropriate amount of data
  while (charCount < dataLength) 
  {
    while (!Serial.available());  // Wait for the next byte to be available
#ifdef TEST_MODE_COUNTING
    inputChar = testChar;
#else
    inputChar = Serial.read();    // Grab it by the bits
#endif
    SPI.transfer(inputChar);      // Transfer it via SPI
    charCount++;                  // Tally it up
#if ECHO_ON
    Serial.write(inputChar);      // Local Echo ON
#endif
  }

  // Disable Chip Select
  digitalWrite(CS_PIN, 1);  // Disable Chip Select (Active Low)
  digitalWrite(LED_PIN, 0); // Turn off the LED (After CS Disabled)

#ifdef TEST_MODE_COUNTING
  testChar++;
   delay(2000);
#else
  delayMicroseconds(10);    // Wait a bit, let any ISRs process
#endif
}

// EOF
