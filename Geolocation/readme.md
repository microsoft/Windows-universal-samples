# Geolocation sample
The Geolocation sample shows how to use the Windows.Devices.Geolocation namespace to get the geographic location of the user's device. An app can use the Geolocation namespace to request access to user location, get the location one time, continuously track the location by getting location update events, or get alerts when the device has entered or exited locations of interest.

### This sample also shows how to:

- **Use a [geofence](http://msdn.microsoft.com/library/windows/apps/dn263744):** Receive notifications when the user's device has entered or left an area of interest. 
- **Handle changes in location permissions**:  Register for the Geolocator's StatusChanged event and use the [StatusChangedEventArgs](http://msdn.microsoft.com/library/windows/apps/br225600) to determine the current position status. 
- **Get location from a background task** 
- **Handle geofencing events from a background task** 
- **Obtain satellite data:** When available, use the [GeocoordinateSatelliteData](http://msdn.microsoft.com/library/windows/apps/jj635260) class to obtain additional information on the quality of the satellite based location data. 
- **Display a toast:** Notify the user when a geofencing event has occurred in the background. 
- **Refresh geofence binding:** Refresh the binding on resume and after removal of a geofence. Note that removal requires subscribing to removal events if a geofence is set as single use or a duration is set. 
- **Request access to the user's location:** Request access to the user's location using the RequestAccessAsync method. Important: call the RequestAccessAsync before accessing the user’s location. At that time, your app must be in the foreground and RequestAccessAsync must be called from the UI thread. Until the user grants your app permission to their location, your app can't access location data.  
- **Help the user change location settings:** Link to location privacy settings from your app if the user revokes access to location while your app is in the foreground. Call the LaunchUriAsync method with the URI "ms-settings://privacy/location".

Geofences need to be created in the Foreground Geofencing scenario (Scenario 4) and then you can go to Background Geofencing scenario (Scenario 5) to register for background geofencing events.

The Geolocation sample formats and parses time and dates in the en-US locale using the Gregorian calendar and 24-hour clock. To help other locales in entering data the edit fields have a format example shown below the control. For example, Start Time would be entered mm/dd/yyyy hh:mm:ss format. February 2, 2014 at 10:34 pm would be written 2/2/2014 22:34:00. For the Dwell Time and Duration the format is dd:hh:mm:ss so a time span of 7 days, 23 hours, 45 minutes and 55 seconds would be written as 7:23:45:55. 

### Declaring the Location capability

This sample requires that location capability be set in the *Package.appxmanifest* file to allow the app to access location at runtime. The capability can be set in the app manifest using Microsoft Visual Studio.


## Related topics

[Detect a user's location](https://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh465135.aspx)

[Respond to location updates](https://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh465142.aspx)

[Windows.Devices.Geolocation](http://msdn.microsoft.com/library/windows/apps/br225603)

[Windows.Devices.Geolocation.Geofencing](https://msdn.microsoft.com/library/windows/apps/dn263744)
  
[Guidelines for location-aware apps](https://msdn.microsoft.com/library/windows/apps/hh465148)

## System requirements

**Client:** Windows 10 Technical Preview

**Server:** Windows 10 Technical Preview

**Phone:**  Windows 10 Technical Preview

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
