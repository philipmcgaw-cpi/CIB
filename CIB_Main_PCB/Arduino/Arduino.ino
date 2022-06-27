/*
Vrekrer_scpi_parser library
.
SCPI Dimmer example.

Demonstrates the control of the brightness of a LED using SCPI commands.

Hardware required:
A LED attached to digital pin 9
or a resistor from pin 9 to pin 13 to use the built-in LED

Commands:
  *IDN?
    Gets the instrument's identification string

  SYSTem:LED:BRIGhtness <value>
    Sets the LED's brightness to <value>
    Valid values : 0 (OFF) to 10 (Full brightness)

  SYSTem:LED:BRIGhtness?
    Queries the current LED's brightness value

  SYSTem:LED:BRIGhtness:INCrease
    Increases the LED's brightness value by one

  SYSTem:LED:BRIGhtness:DECrease
    Decreases the LED's brightness value by one
*/


#include "Arduino.h"
#include <ArduinoUniqueID.h>
#include "Vrekrer_scpi_parser.h"



#include <Wire.h>
#include <MCP23018.h>
#define RESET_PIN 5 
#define MCP_ADDRESS 0x20 

SCPI_Parser my_instrument;
int brightness = 0;
const int ledPin = 9;
const int intensity[11] = {0, 3, 5, 9, 15, 24, 38, 62, 99, 159, 255};

void setup()
{
  my_instrument.RegisterCommand(F("*IDN?"), &Identify);
  my_instrument.SetCommandTreeBase(F("CIB"));
    my_instrument.RegisterCommand(F(":RESEt"), &CIBReset);
    my_instrument.RegisterCommand(F(":RELAy"), &CIBRelay);
  my_instrument.SetCommandTreeBase(F("SYSTem:LED"));
    my_instrument.RegisterCommand(F(":BRIGhtness"), &SetBrightness);
    my_instrument.RegisterCommand(F(":BRIGhtness?"), &GetBrightness);
    my_instrument.RegisterCommand(F(":BRIGhtness:INCrease"), &IncDecBrightness);
    my_instrument.RegisterCommand(F(":BRIGhtness:DECrease"), &IncDecBrightness);

  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(LED_BUILTIN, INPUT);
  analogWrite(ledPin, 0);
}

void loop()
{
  my_instrument.ProcessInput(Serial, "\n");
}

void Identify(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  // Uses the ArduinoUniqueID Library to get the Unique ID / Manufacture Serial Number from the Atmel AVR.
  interface.print(F("ChargePoint,CIB,v0.1,SN"));
  for(size_t i = 0; i < UniqueIDsize; i++)
    interface.print(UniqueID[i], HEX);
  interface.println(F(""));
}

void CIBReset(SCPI_C commands, SCPI_P parameters, Stream& interface) { 
  interface.println(F("Reset CIB to initial state"));
}

void CIBRelay(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  interface.println('commands');
  interface.println('parameters');
}




void SetBrightness(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  // For simplicity no bad parameter check is done.
  if (parameters.Size() > 0) {
    brightness = constrain(String(parameters[0]).toInt(), 0, 10);
    analogWrite(ledPin, intensity[brightness]);
  }
}

void GetBrightness(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  interface.println(String(brightness, DEC));
}

void IncDecBrightness(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  String last_header = String(commands.Last());
  last_header.toUpperCase();
  if (last_header.startsWith("INC")) {
    brightness = constrain(brightness + 1, 0, 10);
  } else { // "DEC"
    brightness = constrain(brightness - 1, 0, 10);
  }
  analogWrite(ledPin, intensity[brightness]);
}
