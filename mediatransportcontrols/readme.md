# System media transport controls sample

This sample demonstrate how to register and handle media transport events for hardware and software buttons using the [**SystemMediaTransportControls**](http://msdn.microsoft.com/library/windows/apps/dn278677) .

Windows 8.1 introduces [**SystemMediaTransportControls**](http://msdn.microsoft.com/library/windows/apps/dn278677) which replace [**MediaControl**](http://msdn.microsoft.com/library/windows/apps/hh700786) class for interacting with the system media transport controls.

Specifically, this sample shows how to:

-   Register for the play, stop, and pause buttons on the system media transport controls using the [**IsPlayEnabled**](http://msdn.microsoft.com/library/windows/apps/dn278714), [**IsPauseEnabled**](http://msdn.microsoft.com/library/windows/apps/dn278713), and [**IsStopEnabled**](http://msdn.microsoft.com/library/windows/apps/dn278718) properties.
-   Handle button presses on the system media transport controls with the [**ButtonPressed**](http://msdn.microsoft.com/library/windows/apps/dn278706) event. You use this event to update the status of the media playing in the app.
-   Update the status of the system media transport controls using the [**PlaybackStatus**](http://msdn.microsoft.com/library/windows/apps/dn278719) property. You need to update system media transport controls so they are in sync with the local media state in the app.
-   Update the media metadata that the system media transport controls displays using the [**SystemMediaTransportControlsDisplayUpdater**](http://msdn.microsoft.com/library/windows/apps/dn278686). Specifically this sample shows how to use [**CopyFromFileAsync**](http://msdn.microsoft.com/library/windows/apps/dn278694) to automatically extract the metadata from the media file.

The system media transport controls are different from the transport controls on the XAML [**MediaElement**](http://msdn.microsoft.com/library/windows/apps/br242926) object or the HTML [**audio**](http://msdn.microsoft.com/library/windows/apps/hh767373) and [**video**](http://msdn.microsoft.com/library/windows/apps/hh767390) objects.
The system media transport controls are the controls that pop up when hardware or software media keys are pressed, such as the volume control on a pair of headphones or the media buttons on some keyboards.

For more info on the [**SystemMediaTransportControls**](http://msdn.microsoft.com/library/windows/apps/dn278677), see [How to use the system media transport controls](http://msdn.microsoft.com/library/windows/apps/dn263187).

For info on background audio, which requires handling the play and pause buttons of the system media transport controls, see [How to play audio in the background](http://msdn.microsoft.com/library/windows/apps/jj841209).

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[**SystemMediaTransportControls**](http://msdn.microsoft.com/library/windows/apps/dn278677)

[**SystemMediaTransportControlsDisplayUpdater**](http://msdn.microsoft.com/library/windows/apps/dn278686)

[**ButtonPressed**](http://msdn.microsoft.com/library/windows/apps/dn278706)

[**CopyFromFileAsync**](http://msdn.microsoft.com/library/windows/apps/dn278694)

[How to use the system media transport controls](http://msdn.microsoft.com/library/windows/apps/dn263187)

[How to play audio in the background](http://msdn.microsoft.com/library/windows/apps/jj841209)

## System requirements

**Client:** Windows 10 Technical Preview

**Server:** Windows 10 Technical Preview

**Phone:** Windows 10 Technical Preview

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
