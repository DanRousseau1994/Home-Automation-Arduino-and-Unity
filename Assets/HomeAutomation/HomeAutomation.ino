#include <Arduino.h>
#if !defined(SERIAL_PORT_MONITOR)
#error "Arduino version not supported. Please update your IDE to the latest version."
#endif

#if defined(__SAMD21G18A__)
// Shield Jumper on HW (for Zero, use Programming Port)
#define port SERIAL_PORT_HARDWARE
#define pcSerial SERIAL_PORT_MONITOR
#elif defined(SERIAL_PORT_USBVIRTUAL)
// Shield Jumper on HW (for Leonardo and Due, use Native Port)
#define port SERIAL_PORT_HARDWARE
#define pcSerial SERIAL_PORT_USBVIRTUAL
#else
// Shield Jumper on SW (using pins 12/13 or 8/9 as RX/TX)
#include <SoftwareSerial.h>
SoftwareSerial port(12, 13);
#define pcSerial SERIAL_PORT_MONITOR
#endif

//bool hasTarget = false;

#include <EasyVR.h>
#include <Servo.h>
Servo myServo;

#define sensor A0
bool hasTarget = false;

EasyVR easyvr(port);

enum Groups
{
  GROUP_0  = 0,
  GROUP_1  = 1,
  GROUP_2  = 2,
  GROUP_3  = 3,
  GROUP_4  = 4,
  GROUP_5  = 5,
};

enum Group0
{
  G0_WAKE_UP = 0,
};

enum Group1
{
  G1_CONTROL_DOOR = 0,
  G1_CONTROL_GATE = 1,
  G1_CONTROL_KITCHEN = 2,
  G1_SET_ALARM = 3,
  G1_LIGHTS = 4,
};

enum Group2
{
  G2_LOCK_DOOR = 0,
  G2_UNLOCK_DOOR = 1,
};

enum Group3
{
  G3_OPEN_GATE = 0,
  G3_CLOSE_GATE = 1,
};

enum Group4
{
  G4_KETTLE = 0,
};

enum Group5
{
  G5_TURN_ON_LIGHTS = 0,
  G5_TURN_OFF_LIGHTS = 1,
};

int8_t group, idx;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  SetupServo();

  // setup PC serial port
  pcSerial.begin(9600);
bridge:
  // bridge mode?
  int mode = easyvr.bridgeRequested(pcSerial);
  switch (mode)
  {
    case EasyVR::BRIDGE_NONE:
      // setup EasyVR serial port
      port.begin(9600);
      // run normally
      pcSerial.println(F("Bridge not requested, run normally"));
      pcSerial.println(F("---"));
      break;

    case EasyVR::BRIDGE_NORMAL:
      // setup EasyVR serial port (low speed)
      port.begin(9600);
      // soft-connect the two serial ports (PC and EasyVR)
      easyvr.bridgeLoop(pcSerial);
      // resume normally if aborted
      pcSerial.println(F("Bridge connection aborted"));
      pcSerial.println(F("---"));
      break;

    case EasyVR::BRIDGE_BOOT:
      // setup EasyVR serial port (high speed)
      port.begin(115200);
      pcSerial.end();
      pcSerial.begin(115200);
      // soft-connect the two serial ports (PC and EasyVR)
      easyvr.bridgeLoop(pcSerial);
      // resume normally if aborted
      pcSerial.println(F("Bridge connection aborted"));
      pcSerial.println(F("---"));
      break;
  }

  // initialize EasyVR
  while (!easyvr.detect())
  {
    pcSerial.println(F("EasyVR not detected!"));
    for (int i = 0; i < 10; ++i)
    {
      if (pcSerial.read() == '?')
        goto bridge;
      delay(100);
    }
  }

  pcSerial.print(F("EasyVR detected, version "));
  pcSerial.print(easyvr.getID());

  if (easyvr.getID() < EasyVR::EASYVR3)
    easyvr.setPinOutput(EasyVR::IO1, LOW);

  if (easyvr.getID() < EasyVR::EASYVR)
    pcSerial.print(F(" = VRbot module"));
  else if (easyvr.getID() < EasyVR::EASYVR2)
    pcSerial.print(F(" = EasyVR module"));
  else if (easyvr.getID() < EasyVR::EASYVR3)
    pcSerial.print(F(" = EasyVR 2 module"));
  else
    pcSerial.print(F(" = EasyVR 3 module"));
  pcSerial.print(F(", FW Rev."));
  pcSerial.println(easyvr.getID() & 7);

  easyvr.setDelay(0); // speed-up replies

  easyvr.setTimeout(5);
  easyvr.setLanguage(0);

  group = EasyVR::TRIGGER;
   digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  DistanceSensor();

  if (easyvr.getID() < EasyVR::EASYVR3)
    easyvr.setPinOutput(EasyVR::IO1, HIGH);

  if (group < 0) // SI wordset/grammar
  {
    pcSerial.print("Say a word in Wordset ");
    pcSerial.println(-group);
    easyvr.recognizeWord(-group);
  }
  else // SD group
  {
    pcSerial.print("Say a command in Group ");
    pcSerial.println(group);
    easyvr.recognizeCommand(group);
  }

  do
  {
    if (pcSerial.read() == '?')
    {
      setup();
      return;
    }
  }
  while (!easyvr.hasFinished());

  if (easyvr.getID() < EasyVR::EASYVR3)
    easyvr.setPinOutput(EasyVR::IO1, LOW); // LED off

  idx = easyvr.getWord();
  if (idx == 0 && group == EasyVR::TRIGGER)
  {
    // beep
    easyvr.playSound(0, EasyVR::VOL_FULL);
    // print debug message
    pcSerial.println("Word: ROBOT");
    group = GROUP_1;
    return;
  }
  else if (idx >= 0)
  {
    // beep
    easyvr.playSound(0, EasyVR::VOL_FULL);
    // print debug message
    uint8_t flags = 0, num = 0;
    char name[32];
    pcSerial.print("Word: ");
    pcSerial.print(idx);
    if (easyvr.dumpGrammar(-group, flags, num))
    {
      for (uint8_t pos = 0; pos < num; ++pos)
      {
        if (!easyvr.getNextWordLabel(name))
          break;
        if (pos != idx)
          continue;
        pcSerial.print(F(" = "));
        pcSerial.println(name);
        break;
      }
    }
    // perform some action
    action();
    return;
  }
  idx = easyvr.getCommand();
  if (idx >= 0)
  {
    // beep
    easyvr.playSound(0, EasyVR::VOL_FULL);
    // print debug message
    uint8_t train = 0;
    char name[32];
    pcSerial.print("Command: ");
    pcSerial.print(idx);
    if (easyvr.dumpCommand(group, idx, name, train))
    {
      pcSerial.print(" = ");
      pcSerial.println(name);
    }
    else
      pcSerial.println();
    // perform some action
    action();
  }
  else // errors or timeout
  {
    if (easyvr.isTimeout())
      pcSerial.println("Timed out, try again...");
    int16_t err = easyvr.getError();
    if (err >= 0)
    {
      pcSerial.print("Error ");
      pcSerial.println(err, HEX);
    }
  }
}

void action()
{
  switch (group)
  {
    case GROUP_0:
      switch (idx)
      {
        case G0_WAKE_UP:
          group = GROUP_1;
          break;
      }
      break;
    case GROUP_1:
      switch (idx)
      {
        case G1_CONTROL_DOOR:
          group = GROUP_2;
          break;
        case G1_CONTROL_GATE:
          group = GROUP_3;
          break;
        case G1_CONTROL_KITCHEN:
          group = GROUP_4;
          break;
        case G1_LIGHTS:
          group = GROUP_5;
          break;
      }
      break;
    case GROUP_2:
      switch (idx)
      {
        case G2_LOCK_DOOR:
          ServoClose("CloseDoor");
          group = GROUP_0;
          break;
        case G2_UNLOCK_DOOR:
          ServoOpen("UnlockDoor");
          break;
      }
      break;
    case GROUP_3:
      switch (idx)
      {
        case G3_OPEN_GATE:
          ServoOpen("OpenGate");
          break;
        case G3_CLOSE_GATE:
          ServoClose("CloseGate");
          group = GROUP_0;
          break;
      }
      break;
    case GROUP_4:
      switch (idx)
      {
        case G4_KETTLE:
          //Wait for Seconds then go back to group 0
          //Callums Code would be cool
          break;
      }
      break;

    case GROUP_5:
      switch (idx)
      {
        case G5_TURN_ON_LIGHTS:
          LightsOn();
          delay(1000);
          group = GROUP_0;
          break;
        case G5_TURN_OFF_LIGHTS:
          LightsOff();
          delay(1000);
          group = GROUP_0;
          break;
      }
      break;
  }

}
