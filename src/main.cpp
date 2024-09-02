#include <ACAN2515.h>

#define MCP2515_MISO  23
#define MCP2515_MOSI  19
#define MCP2515_SCK   18

#define MCP2515_CS    5
#define MCP2515_INT   16
//——————————————————————————————————————————————————————————————————————————————
//  MCP2515 Driver object
//——————————————————————————————————————————————————————————————————————————————
SPIClass vspi(VSPI);

ACAN2515 can (MCP2515_CS, vspi, MCP2515_INT) ;

//——————————————————————————————————————————————————————————————————————————————
//  MCP2515 Quartz: adapt to your design
//——————————————————————————————————————————————————————————————————————————————

static const uint32_t QUARTZ_FREQUENCY = 8UL * 1000UL * 1000UL ; // 8 MHz

// //——————————————————————————————————————————————————————————————————————————————
// //   SETUP
// //——————————————————————————————————————————————————————————————————————————————

void setup () {

//--- Switch on builtin led
  pinMode (LED_BUILTIN, OUTPUT) ;
  digitalWrite (LED_BUILTIN, HIGH) ;
//--- Start serial
  Serial.begin (115200) ;
//--- Wait for serial (blink led at 10 Hz during waiting)
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_BUILTIN, !digitalRead(LED_BUILTIN)) ;
  }
//--- Begin SPI
  vspi.begin(MCP2515_SCK, MCP2515_MISO, MCP2515_MOSI, MCP2515_CS); // SCLK, MISO, MOSI, SS
  // SPI.begin (VSPI) ;

//--- Configure ACAN2515
  Serial.println ("Configure ACAN2515") ;
  ACAN2515Settings settings (QUARTZ_FREQUENCY, 500UL * 1000UL) ; // CAN bit rate 500 kb/s
  settings.mRequestedMode = ACAN2515Settings::LoopBackMode ; // Select loopback mode
  const uint16_t errorCode = can.begin (settings, [] { can.isr () ; }) ;
  if (errorCode == 0) {
    Serial.print ("Bit Rate prescaler: ") ;
    Serial.println (settings.mBitRatePrescaler) ;
    Serial.print ("Propagation Segment: ") ;
    Serial.println (settings.mPropagationSegment) ;
    Serial.print ("Phase segment 1: ") ;
    Serial.println (settings.mPhaseSegment1) ;
    Serial.print ("Phase segment 2: ") ;
    Serial.println (settings.mPhaseSegment2) ;
    Serial.print ("SJW: ") ;
    Serial.println (settings.mSJW) ;
    Serial.print ("Triple Sampling: ") ;
    Serial.println (settings.mTripleSampling ? "yes" : "no") ;
    Serial.print ("Actual bit rate: ") ;
    Serial.print (settings.actualBitRate ()) ;
    Serial.println (" bit/s") ;
    Serial.print ("Exact bit rate ? ") ;
    Serial.println (settings.exactBitRate () ? "yes" : "no") ;
    Serial.print ("Sample point: ") ;
    Serial.print (settings.samplePointFromBitStart ()) ;
    Serial.println ("%") ;
  }else{
    Serial.print ("Configuration error 0x") ;
    Serial.println (errorCode, HEX) ;
  }
}

// //----------------------------------------------------------------------------------------------------------------------

static uint32_t gBlinkLedDate = 0 ;
static uint32_t gReceivedFrameCount = 0 ;
static uint32_t gSentFrameCount = 0 ;

// //——————————————————————————————————————————————————————————————————————————————

void loop () {
  CANMessage frame ;
  if (gBlinkLedDate < millis ()) {
    gBlinkLedDate += 2000 ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    const bool ok = can.tryToSend (frame) ;
    if (ok) {
      gSentFrameCount += 1 ;
      Serial.print ("Sent: ") ;
      Serial.println (gSentFrameCount) ;
    }else{
      Serial.println ("Send failure") ;
    }
  }
  if (can.available ()) {
    can.receive (frame) ;
    gReceivedFrameCount ++ ;
    Serial.print ("Received: ") ;
    Serial.println (gReceivedFrameCount) ;
  }
}

/* The ESP32 has four SPi buses, however as of right now only two of
 * them are available to use, HSPI and VSPI. Simply using the SPI API
 * as illustrated in Arduino examples will use VSPI, leaving HSPI unused.
 *
 * However if we simply intialise two instance of the SPI class for both
 * of these buses both can be used. However when just using these the Arduino
 * way only will actually be outputting at a time.
 *
 * Logic analyser capture is in the same folder as this example as
 * "multiple_bus_output.png"
 *
 * created 30/04/2018 by Alistair Symonds
 */
#include <SPI.h>
#include "Arduino.h"

static const int spiClk = 1000000; // 1 MHz


void spiCommand(SPIClass *spi, byte data);
uint8_t spiReadReg(SPIClass *spi, uint8_t addr);


// the loop function runs over and over again until power down or reset
// void loop()
// {
//   // use the SPI buses
//   uint8_t read_val = spiReadReg(&vspi, 0x1F); // junk data to illustrate usage
//   Serial.printf("Read Val 0x%.2x \r\n", read_val);
//   delay(100);
// }

// void spiCommand(SPIClass *spi, uint8_t addr)
// {
//   // use it as you would the regular arduino SPI API
//   spi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
//   digitalWrite(spi->pinSS(), LOW); // pull SS slow to prep other end for transfer
//   spi->transfer(0x03);
//   spi->transfer(addr);

//   digitalWrite(spi->pinSS(), HIGH); // pull ss high to signify end of data transfer
//   spi->endTransaction();
// }

// uint8_t spiReadReg(SPIClass *spi, uint8_t addr)
// {
//   uint8_t val;
//   spi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
//   digitalWrite(spi->pinSS(), LOW); // pull SS slow to prep other end for transfer

//   spi->transfer(addr);
//   val =  spi->transfer(0);

//   digitalWrite(spi->pinSS(), HIGH); // pull ss high to signify end of data transfer
//   spi->endTransaction();

//   return val;
// }