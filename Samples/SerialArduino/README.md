<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620595
--->

# Serial Arduino sample

Shows how to use the [Windows.Devices.SerialCommunication](https://msdn.microsoft.com/library/windows/apps/windows.devices.serialcommunication.aspx) 
APIs to communicate with an Arduino device.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample allows the user to configure and communicate with an Arduino board that has simple wired circuitry consisting of 4 LEDs and a DHT temperature & humidity sensor. 
This sample demonstrates the following

-   Connect/Disconnect using Device Selection list
-   Communicating with the Arduino board and toggle the LEDs and get readings from the DHT temperature & humidity sensor.

**Connect/Disconnect using Device Selection list**

When the application starts, a list of available matching Arduino devices is displayed along with options to Connect/Disconnect to/from a selected device.

**LED/Temperature**

This scenario demonstrates the use of Input and Output streams on the SerialDevice object in order to communicate with the Serial device specifically to 
toggle the four LEDs, get temperature and humidity readings from the temperature sensor.

## Hardware requirements

Any Arduino board with a header that can be used to wire up the simple circuit of LEDs and the DHT temperature & humidity sensor. The Arduino uses a custom sketch that 
goes along with the sample and is included in the *SerialCommand* folder as SerialCommand.ino.

The wiring diagram is also provided as SerialCommand.fzz can be opened with Fritzing application. (Download at http://Fritzing.org)

## Operating system requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Compile and Upload the sketch 

1. Install Arduino IDE based on https://www.arduino.cc/en/Guide/Windows 
2. Install Steven Cogswell's ArduinoSerialCommand library from https://github.com/scogswell/ArduinoSerialCommand
3. Install the Adafruit DHT library based on https://learn.adafruit.com/dht/overview
4. Open the SerialCommand.ino sketch
5. Compile the sketch
6. Connect the Arduino board with the Windows 10 device, which runs Arduino IDE 
6. Upload the sketch to the connected Arduino board

## Test the uploaded sketch

1. Open *Serial Monitor* from the Tools menu of Arduino IDE
2. Make sure that the selected baud rate is *9600*
3. Make sure that *Carriage return* is selected as line ending
4. Type some commands like *temp*, *hum*, *ledon 5*, *ledoff 5* into the text box and click Send

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To run this sample after building it, press F5 (run with debugging enabled) or Ctrl+F5 (run without debugging enabled) from Visual Studio. (Or select the corresponding options from the Debug menu.)
