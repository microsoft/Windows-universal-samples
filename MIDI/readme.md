MIDI Windows Runtime sample
===========================

This sample demonstrates how to use the Windows.Devices.Midi API in a Windows Runtime app.

This sample demonstrates the following features:
-   Enumerate MIDI In and MIDI Out ports.
-   Attach a device watcher to monitor port arrival and removal.
-   Query the properties of a MIDI port.
-   Open MIDI In ports.
-   Open MIDI Out ports.
-   Create MIDI messages and send them to a MIDI Out ports.

Related topics
--------------
AudioGraph, Video editing

Operating system requirements
-----------------------------
Client: Windows 10
Server: Windows Server 2012 R2

Build the sample
----------------
1.  Start Visual Studio for Windows --\> and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory in which you unzipped the sample. Go to the directory named for the sample, and double-click the Visual Studio for Windows Solution (.sln) file.
3.  Press F6 or use **Build** \> **Build Solution** to build the sample.

Run the sample
--------------
To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

Known Issues
--------------
There is currently a problem with referencing the Microsoft.Midi.GmDls framework package. Because of this, the GS Synth is not currently usable.
The code required for the dependency is in package.appxmanifest, but has been commented out until the issue with referencing the package is resolved.