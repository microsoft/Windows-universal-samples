// The MIT License (MIT)
//
// Copyright (c) 2015 Microsoft
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 

#include <SoftwareSerial.h>
#include <SerialCommand.h>      // Steven Cogswell ArduinoSerialCommand library from http://GitHub.com
#include <DallasTemperature.h>  // Miles Burton Arduino-Temperature-Contol library from http://GitHub.com
#include <OneWire.h>            // Paul Stoffregen OneWire library from http://www.pjrc.com

#define ONE_WIRE_BUS 2
#define LED_LOW      3
#define LED_HIGH     6
#define TEMP_MAX     99.99
#define TEMP_MIN     00.00
#define DBGMSG(A)    if (dbg){ Serial.print("DBG: "); Serial.println(A);}

//
// Globals
//
SerialCommand     serialCommand;
OneWire           oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress     thermometerAddress = {-1};
boolean           dbg = false;

//
// Initialize the serial command table, I/O pins, and the temperature sensor
//
void setup() {
  Serial.begin(9600);
  serialCommand.addCommand("ledon", SetLedOn );
  serialCommand.addCommand("ledoff", SetLedOff );
  serialCommand.addCommand("temp", GetTemp );
  serialCommand.addCommand("debug", SetDebug ); 
  serialCommand.addDefaultHandler(UnrecognizedCommand );
  
  for ( byte i = LED_LOW; i <= LED_HIGH; i++ ) {
    pinMode( i, OUTPUT );
  }

  sensors.begin();
}

//
// Read and respond to commands recieved over the serial port
//
void loop() {
  serialCommand.readSerial();
}

//
// Turn on the LED connected to the specified port
//
void SetLedOn() {
  char *arg = serialCommand.next();
  
  if (arg != NULL ){
      if ( atoi(arg) >= LED_LOW && atoi(arg) <= LED_HIGH){
        DBGMSG( F("SetLedOn") );
        digitalWrite(atoi(arg), HIGH);
      } else {
        DBGMSG( F("out of range.  3 <= led <= 6") );
      } 
  } else {
    DBGMSG( F("led not specified") );
  }
}

//
// Turn off the led connected to the specified port
//
void SetLedOff() {
  char *arg = serialCommand.next();
  
  if (arg != NULL){
    if (atoi(arg) >= LED_LOW && atoi(arg) <= LED_HIGH){
      DBGMSG( F("SetLedOff") );
      digitalWrite(atoi(arg), LOW);
    } else {
        DBGMSG( F("out of range.  3 <= led <= 6") );
    }      
  } else {
    DBGMSG( F("led not specified") );
  }
}

//
// Locate the temperature sensor, read the temperature in Celcius,
// and return the temperature via the serial interface as a 5 character
// string.  Valid temperatures are from 0.00 to 99.99 degrees
// Celcius.  -9.99 is returned on an error condition.
//
void GetTemp() {
  float temperature=-9.99;
  
  if (!sensors.getAddress(thermometerAddress, 0)){
    DBGMSG( F("Unable to locate sensor") );
  } else {
    sensors.setWaitForConversion(true);
    sensors.requestTemperaturesByAddress(thermometerAddress);  
    temperature = sensors.getTempC(thermometerAddress);
    sensors.requestTemperaturesByAddress(thermometerAddress);  
    temperature = sensors.getTempC(thermometerAddress);   
  }
  
  if ( temperature < 9.995 && temperature > TEMP_MIN )
    Serial.print("0");
  if (temperature > TEMP_MAX || temperature < TEMP_MIN ){
    DBGMSG( F("Temperature out of bounds") );
    temperature = -9.99;
  }
  
  Serial.println(temperature, 2);
}

//
// Enable or disable debug messages from being printed
// on the serial terminal
//
void SetDebug() {
  char *arg = serialCommand.next();
  
  if (arg != NULL){
    if ( strcmp(arg, "on" ) == 0){
      dbg=true;
      DBGMSG(F("Turn on debug"));
    }
    if ( strcmp(arg, "off" ) == 0){
      DBGMSG(F("Turn off debug"));
      dbg=false;
    }
  }
}

//
// An unrecognized command was recieved
//
void UnrecognizedCommand(){  
  DBGMSG(F("Unrecognized command"));
  DBGMSG(F(" ledon 3  - turn on led connected to digital I/O 3"));
  DBGMSG(F(" ledon 4  - turn on led connected to digital I/O 4"));
  DBGMSG(F(" ledon 5  - turn on led connected to digital I/O 5"));
  DBGMSG(F(" ledon 6  - turn on led connected to digital I/O 6"));
  DBGMSG(F(" ledoff 3 - turn off led connected to digital I/O 3"));
  DBGMSG(F(" ledoff 4 - turn off led connected to digital I/O 4"));
  DBGMSG(F(" ledoff 5 - turn off led connected to digital I/O 5"));
  DBGMSG(F(" ledoff 6 - turn off led connected to digital I/O 6"));
  DBGMSG(F(" temp     - read temperature" ));
  DBGMSG(F(" debug on - turn on debug messages" ));
  DBGMSG(F(" debug off- turn off debug messages" ));
}
