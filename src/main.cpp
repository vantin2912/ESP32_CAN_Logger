// //——————————————————————————————————————————————————————————————————————————————
// //  ACAN2515 Demo in loopback mode, for ESP32
// //——————————————————————————————————————————————————————————————————————————————

// #ifndef ARDUINO_ARCH_ESP32
//   #error "Select an ESP32 board"
// #endif

// //——————————————————————————————————————————————————————————————————————————————

// #include <ACAN2515.h>

// //——————————————————————————————————————————————————————————————————————————————
// //  For using SPI on ESP32, see demo sketch SPI_Multiple_Buses
// //  Two SPI busses are available in Arduino, HSPI and VSPI.
// //  By default, Arduino SPI uses VSPI, leaving HSPI unused.
// //  Default VSPI pins are: SCK=18, MISO=19, MOSI=23.
// //  You can change the default pin with additional begin arguments
// //    SPI.begin (MCP2515_SCK, MCP2515_MISO, MCP2515_MOSI)
// //  CS input of MCP2515 should be connected to a digital output port
// //  INT output of MCP2515 should be connected to a digital input port, with interrupt capability
// //  Notes:
// //    - GPIOs 34 to 39 are GPIs – input only pins. These pins don’t have internal pull-ups or
// //      pull-down resistors. They can’t be used as outputs.
// //    - some pins do not support INPUT_PULLUP (see https://www.esp32.com/viewtopic.php?t=439)
// //    - All GPIOs can be configured as interrupts
// // See https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
// //——————————————————————————————————————————————————————————————————————————————

// static const byte MCP2515_SCK  = 18 ; // SCK input of MCP2515
// static const byte MCP2515_MOSI = 18 ; // SDI input of MCP2515
// static const byte MCP2515_MISO = 23 ; // SDO output of MCP2515

#define MCP2515_MISO 23
#define MCP2515_MOSI 19
#define MCP2515_SCK 18
#define MCP2515_CS 26

// static const byte MCP2515_CS  = 26 ; // CS input of MCP2515 (adapt to your design)
// static const byte MCP2515_INT = 25 ; // INT output of MCP2515 (adapt to your design)
// static const byte MCP2515_RESET = 27 ; // RESET input of MCP2515 (adapt to your design)

// //——————————————————————————————————————————————————————————————————————————————
// //  MCP2515 Driver object
// //——————————————————————————————————————————————————————————————————————————————

// ACAN2515 can (MCP2515_CS, SPI, MCP2515_INT) ;

// //——————————————————————————————————————————————————————————————————————————————
// //  MCP2515 Quartz: adapt to your design
// //——————————————————————————————————————————————————————————————————————————————

// static const uint32_t QUARTZ_FREQUENCY = 8UL * 1000UL * 1000UL ; // 8 MHz

// //——————————————————————————————————————————————————————————————————————————————
// //   SETUP
// //——————————————————————————————————————————————————————————————————————————————

// void setup () {
// //--- RESET MCP2515
//   pinMode (MCP2515_RESET, OUTPUT) ;
//   digitalWrite (MCP2515_RESET, LOW) ;
//   delay (10) ;
//   digitalWrite (MCP2515_RESET, HIGH) ;
// //--- Switch on builtin led
//   pinMode (LED_BUILTIN, OUTPUT) ;
//   digitalWrite (LED_BUILTIN, HIGH) ;
// //--- Start serial
//   Serial.begin (115200) ;
// //--- Wait for serial (blink led at 10 Hz during waiting)
//   while (!Serial) {
//     delay (50) ;
//     digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
//   }
// //--- Begin SPI
//   SPI.begin (MCP2515_SCK, MCP2515_MISO, MCP2515_MOSI) ;
//   // SPI.begin (VSPI) ;

// //--- Configure ACAN2515
//   Serial.println ("Configure ACAN2515") ;
//   ACAN2515Settings settings (QUARTZ_FREQUENCY, 500UL * 1000UL) ; // CAN bit rate 500 kb/s
//   settings.mRequestedMode = ACAN2515Settings::LoopBackMode ; // Select loopback mode
//   const uint16_t errorCode = can.begin (settings, [] { can.isr () ; }) ;
//   if (errorCode == 0) {
//     Serial.print ("Bit Rate prescaler: ") ;
//     Serial.println (settings.mBitRatePrescaler) ;
//     Serial.print ("Propagation Segment: ") ;
//     Serial.println (settings.mPropagationSegment) ;
//     Serial.print ("Phase segment 1: ") ;
//     Serial.println (settings.mPhaseSegment1) ;
//     Serial.print ("Phase segment 2: ") ;
//     Serial.println (settings.mPhaseSegment2) ;
//     Serial.print ("SJW: ") ;
//     Serial.println (settings.mSJW) ;
//     Serial.print ("Triple Sampling: ") ;
//     Serial.println (settings.mTripleSampling ? "yes" : "no") ;
//     Serial.print ("Actual bit rate: ") ;
//     Serial.print (settings.actualBitRate ()) ;
//     Serial.println (" bit/s") ;
//     Serial.print ("Exact bit rate ? ") ;
//     Serial.println (settings.exactBitRate () ? "yes" : "no") ;
//     Serial.print ("Sample point: ") ;
//     Serial.print (settings.samplePointFromBitStart ()) ;
//     Serial.println ("%") ;
//   }else{
//     Serial.print ("Configuration error 0x") ;
//     Serial.println (errorCode, HEX) ;
//   }
// }

// //----------------------------------------------------------------------------------------------------------------------

// static uint32_t gBlinkLedDate = 0 ;
// static uint32_t gReceivedFrameCount = 0 ;
// static uint32_t gSentFrameCount = 0 ;

// //——————————————————————————————————————————————————————————————————————————————

// void loop () {
//   CANMessage frame ;
//   if (gBlinkLedDate < millis ()) {
//     gBlinkLedDate += 2000 ;
//     digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
//     const bool ok = can.tryToSend (frame) ;
//     if (ok) {
//       gSentFrameCount += 1 ;
//       Serial.print ("Sent: ") ;
//       Serial.println (gSentFrameCount) ;
//     }else{
//       Serial.println ("Send failure") ;
//     }
//   }
//   if (can.available ()) {
//     can.receive (frame) ;
//     gReceivedFrameCount ++ ;
//     Serial.print ("Received: ") ;
//     Serial.println (gReceivedFrameCount) ;
//   }
// }

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

#define MCP2515_MISO 23
#define MCP2515_MOSI 19
#define MCP2515_SCK 18
#define VSPI_SS 26

static const int spiClk = 1000000; // 1 MHz

// uninitalised pointers to SPI objects
SPIClass *vspi = NULL;

void spiCommand(SPIClass *spi, byte data);

void setup()
{
  // initialise two instances of the SPIClass attached to VSPI and HSPI respectively
  vspi = new SPIClass(VSPI);

  vspi->begin(MCP2515_SCK, MCP2515_MISO, MCP2515_MOSI, VSPI_SS); // SCLK, MISO, MOSI, SS

  pinMode(vspi->pinSS(), OUTPUT); // VSPI SS
}

// the loop function runs over and over again until power down or reset
void loop()
{
  // use the SPI buses
  spiCommand(vspi, 0b01010101); // junk data to illustrate usage
  delay(100);
}

void spiCommand(SPIClass *spi, byte data)
{
  // use it as you would the regular arduino SPI API
  spi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(spi->pinSS(), LOW); // pull SS slow to prep other end for transfer
  spi->transfer(data);
  digitalWrite(spi->pinSS(), HIGH); // pull ss high to signify end of data transfer
  spi->endTransaction();
}
