#include "config.h"
#include <LowPower.h>
#if USE_RFM12B
#include <RFM12B.h>
#elif USE_RFM69
#include <RFM69.h>
#include <SPI.h>
#endif
#include <Node.h>

NodeClass Node;

static void changeSwitch(byte id, bool onOff);


static void command(byte cmd, long arg, byte len, byte* raw)
{
  static byte minor = 0;
  switch (cmd)
  {
    case KEY_MINOR:
      minor = (byte)arg;
      break;
    case KEY_POWER_ONOFF:
      changeSwitch(minor, arg ? true : false);
      break;
  }
}

void setup()
{
#if defined(SERIAL_BAUD)
  Serial.begin(SERIAL_BAUD);
  Serial.println("Starting...");
#endif

  Node.initialize(NODEID, MASTERID, NETWORKID, command);
  Node.setPowerManagement(false);
  Node.setReceiver(true);
}

void loop()
{
  Node.tick(NULL);
}

static void changeSwitch(byte id, bool onOff)
{
  static const byte switches[2][5][25] =
  {
    {
      // On
      { 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0 },
      { 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0 },
      { 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0 },
      { 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0 },
      { 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0 }
    },
    {
      // Off
      { 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0 }
    }
  };
  
  if (id < 1 || id > 5)
  {
    return;
  }
 
  const byte* sequence = switches[onOff ? 0 : 1][id - 1];
  for (byte r = 0; r < SWITCH_RADIO_REPEAT; r++)
  {
    for (byte i = 0; i < 25; i++)
    {
      if (sequence[i])
      {
        // 1 symbol = 0.66ms high, 0.22ms low
        digitalWrite(SWITCH_RADIO, HIGH);
        delayMicroseconds(660);
        digitalWrite(SWITCH_RADIO, LOW);
        delayMicroseconds(220);
      }
      else
      {
        // 0 symbol = 0.22ms high, 0.66ms low
        digitalWrite(SWITCH_RADIO, HIGH);
        delayMicroseconds(220);
        digitalWrite(SWITCH_RADIO, LOW);
        delayMicroseconds(660);
      }
    }
    // 6.15ms gap between words
    delayMicroseconds(6150);
  }
}
