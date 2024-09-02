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

uint8_t id_cnt = 0;
void loop () {
  CANMessage rx_frame;

  if (gBlinkLedDate < millis ()) {
    CANMessage tx_frame;  
    tx_frame.id = id_cnt++;
    tx_frame.len = 8;
    memset(tx_frame.data, tx_frame.id, tx_frame.len);

    gBlinkLedDate += 2000 ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    const bool ok = can.tryToSend (tx_frame) ;
    if (ok) {
      gSentFrameCount += 1 ;
      Serial.printf ("Sent ID:  0x%x \r\n", tx_frame.id) ;
    }else{
      Serial.println ("Send failure") ;
    }
  }

  if (can.available ()) {
    can.receive (rx_frame) ;
    Serial.printf("ID 0x%x, DLC %d, RTR %d, EXT %d, Data ", rx_frame.id, rx_frame.len, rx_frame.rtr, rx_frame.ext);
    for(int i = 0; i < rx_frame.len; i++)
    {
      Serial.printf("0x%x ", rx_frame.data[i]);
    }
    Serial.println();
  }
}
