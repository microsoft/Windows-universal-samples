<!---
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619979
--->

# Universal Windows app samples

This repo contains the samples that demonstrate the API usage patterns for the Universal Windows Platform (UWP) in the Windows Software Development Kit (SDK) for Windows 10. These code samples were created with the Universal Windows templates available in Visual Studio, and are designed to run on desktop, mobile, and future devices that support the Universal Windows Platform.  

## Universal Windows Platform development

These samples require Visual Studio 2015 and the Windows Software Development Kit (SDK) for Windows 10 to build, test, and deploy your Universal Windows apps. 

   [Get a free copy of Visual Studio 2015 Community Edition with support for building Universal Windows apps](http://go.microsoft.com/fwlink/?LinkID=280676)

Additionally, to stay on top of the latest updates to Windows and the development tools, become a Windows Insider by joining the Windows Insider Program.

   [Become a Windows Insider](https://insider.windows.com/)

## Using the samples

The easiest way to use these samples without using Git is to download the zip file containing the current version (using the link below or by clicking the "Download ZIP" button on the repo page). You can then unzip the entire archive and use the samples in Visual Studio 2015.

   [Download the samples ZIP](../../archive/master.zip)

   **Notes:** 
   * Before you unzip the archive, right-click it, select Properties, and then select Unblock.
   * Be sure to unzip the entire archive, and not just individual samples. The samples all depend on the SharedContent folder in the archive.   
   * In Visual Studio 2015, the platform target defaults to ARM, so be sure to change that to x64 or x86 if you want to test on a non-ARM device. 
   
The samples use Linked files in Visual Studio to reduce duplication of common files, including sample template files and image assets.  These common files are stored in the SharedContent folder at the root of the repository and referred to in the project files using links.  

**Reminder:** If you unzip individual samples, they will not build due to references to other portions of the ZIP file that were not unzipped. You must unzip the entire archive if you intend to build the samples.

For more info about the programming models, platforms, languages, and APIs demonstrated in these samples, please refer to the guidance, tutorials, and reference topics provided in the Windows 10 documentation available in the [Windows Developer Center](https://dev.windows.com). These samples are provided as-is in order to indicate or demonstrate the functionality of the programming models and feature APIs for Windows.

## Contributions

These samples are direct from the feature teams and we welcome your input on issues and suggestions for new samples.  At this time we are not accepting new samples from the public, but check back here as we evolve our contribution model.

## See also

For additional Windows samples, see [Windows on GitHub](http://microsoft.github.io/windows/). 

## Samples by category

### App settings

<table>
 <tr>
  <td><a href="Samples/Package">App package information</a></td>
  <td><a href="Samples/ApplicationData">Application data</a></td>
  <td><a href="Samples/Store">Store</a></td>
 </tr>
</table>

### Audio, video, and camera

<table>
 <tr>
  <td><a href="Samples/AdaptiveStreaming">Adaptive streaming</a></td>
  <td><a href="Samples/AdvancedCasting">Advanced casting</a></td>
  <td><a href="Samples/AudioCategory">Audio categories</a></td>
 </tr>
 <tr>
  <td><a href="Samples/AudioCreation">Audio graphs</a></td>
  <td><a href="Samples/BackgroundAudio">Background audio</a></td>
  <td><a href="Samples/CameraStarterKit">Basic camera app</a></td>
 </tr>
 <tr>
  <td><a href="Samples/BasicFaceDetection">Basic face detection</a></td>
  <td><a href="Samples/BasicFaceTracking">Basic face tracking</a></td>
  <td><a href="Samples/BasicMediaCasting">Basic media casting</a></td>
 </tr>
 <tr>
  <td><a href="Samples/CameraFaceDetection">Camera face detection</a></td>
  <td><a href="Samples/CameraGetPreviewFrame">Camera preview frame</a></td>
  <td><a href="Samples/CameraProfile">Camera profiles</a></td>
 </tr>
 <tr>
  <td><a href="Samples/CameraResolution">Camera resolution</a></td>
  <td><a href="Samples/LiveDash">DASH streaming</a></td>
  <td><a href="Samples/D2DPhotoAdjustment">Direct2D photo adjustment</a></td>
 </tr>
 <tr>
  <td><a href="Samples/CameraHdr">High dynamic range</a></td>
  <td><a href="Samples/MediaEditing">Media editing</a></td>
  <td><a href="Samples/XamlCustomMediaTransportControls">Media transport controls</a></td>
 </tr>
 <tr>
  <td><a href="Samples/MIDI">MIDI</a></td>
  <td><a href="Samples/Playlists">Playlists</a></td>
  <td><a href="Samples/SimpleImaging">Simple imaging</a></td>
 </tr>
 <tr>
  <td><a href="Samples/SpatialSound">Spatial audio</a></td>
  <td><a href="Samples/SystemMediaTransportControls">System media transport controls</a></td>
  <td><a href="Samples/MediaTranscoding">Transcoding media</a></td>
 </tr>
 <tr>
  <td><a href="Samples/VideoPlayback">Video playback</a></td>
  <td><a href="Samples/CameraVideoStabilization">Video stabilization</a></td>
  <td><a href="Samples/WindowsAudioSession">Windows audio session (WASAPI)</a></td>
 </tr>
 <tr>
  <td><a href="Samples/MediaImport">Windows media import</a></td>
 </tr>
</table>

### Communications

<table>
 <tr>
  <td><a href="Samples/BluetoothRfcommChat">Bluetooth RFCOMM chat</a></td>
  <td><a href="Samples/CallerID">Caller ID</a></td>
  <td><a href="Samples/CommunicationBlockAndFilter">Communication blocking and filtering</a></td>
 </tr>
 <tr>
  <td><a href="Samples/PhoneCall">Phone call</a></td>
  <td><a href="Samples/SimpleCommunication">Real-time communication</a></td>
  <td><a href="Samples/SmsSendAndReceive">SMS send and receive</a></td>
 </tr>
 <tr>
  <td><a href="Samples/VoIP">Voice over IP (VoIP)</a></td>
 </tr>
</table>

### Contacts and calendar

<table>
 <tr>
  <td><a href="Samples/Appointments">Appointment calendar</a></td>
  <td><a href="Samples/ContactCards">Contact cards</a></td>
  <td><a href="Samples/ContactPicker">Contact picker</a></td>
 </tr>
 <tr>
  <td><a href="Samples/UserDataAccountManager">UserDataAccountManager</a></td>
 </tr>
</table>

### Controls, layout, and text

<table>
 <tr>
  <td><a href="Samples/3DPrintingFromUnity">3D Printing from Unity</a></td>
  <td><a href="Samples/XamlAutoSuggestBox">AutoSuggestBox migration</a></td>
  <td><a href="Samples/Clipboard">Clipboard</a></td>
 </tr>
 <tr>
  <td><a href="Samples/XamlCommanding">Commanding</a></td>
  <td><a href="Samples/ContextMenu">Context menu</a></td>
  <td><a href="Samples/XamlContextMenu">Context menu (XAML)</a></td>
 </tr>
 <tr>
  <td><a href="Samples/XamlDataVirtualization">Data virtualization</a></td>
  <td><a href="Samples/UserSelection">Disabling selection</a></td>
  <td><a href="Samples/DisplayOrientation">Display orientation</a></td>
 </tr>
 <tr>
  <td><a href="Samples/DWriteTextLayoutCloudFont">Downloadable fonts (DirectWrite)</a></td>
  <td><a href="Samples/XamlCloudFontIntegration">Downloadable fonts (XAML)</a></td>
  <td><a href="Samples/XamlDragAndDrop">Drag and drop</a></td>
 </tr>
 <tr>
  <td><a href="Samples/XamlFocusVisuals">Focus visuals</a></td>
  <td><a href="Samples/FullScreenMode">Full screen mode</a></td>
  <td><a href="Samples/DWriteLineSpacingModes">Line spacing (DirectWrite)</a></td>
 </tr>
 <tr>
  <td><a href="Samples/XamlListView">ListView and GridView</a></td>
  <td><a href="Samples/LockScreenApps">Lock screen apps</a></td>
  <td><a href="Samples/MessageDialog">Message dialog</a></td>
 </tr>
 <tr>
  <td><a href="Samples/MultipleViews">Multiple views</a></td>
  <td><a href="Samples/OCR">OCR</a></td>
  <td><a href="Samples/Printing">Printing</a></td>
 </tr>
 <tr>
  <td><a href="Samples/XamlPullToRefresh">Pull-to-refresh</a></td>
  <td><a href="Samples/XamlResponsiveTechniques">Responsiveness techniques</a></td>
  <td><a href="Samples/DpiScaling">Scaling according to DPI</a></td>
 </tr>
 <tr>
  <td><a href="Samples/SplashScreen">Splash screen</a></td>
  <td><a href="Samples/XamlStateTriggers">State triggers</a></td>
  <td><a href="Samples/XamlTailoredMultipleViews">Tailored multiple views</a></td>
 </tr>
 <tr>
  <td><a href="Samples/TitleBar">Title bar</a></td>
  <td><a href="Samples/XamlUIBasics">UI basics (XAML)</a></td>
  <td><a href="Samples/UserInteractionMode">User interaction mode</a></td>
 </tr>
 <tr>
  <td><a href="Samples/WebView">WebView control (HTML)</a></td>
  <td><a href="Samples/XamlWebView">WebView control (XAML)</a></td>
  <td><a href="Samples/ResizeAppView">Window resizing</a></td>
 </tr>
</table>

### Custom user interactions

<table>
 <tr>
  <td><a href="Samples/BasicInput">Basic input</a></td>
  <td><a href="Samples/ComplexInk">Complex inking</a></td>
  <td><a href="Samples/Ink">Inking</a></td>
 </tr>
 <tr>
  <td><a href="Samples/LowLatencyInput">Low latency input</a></td>
  <td><a href="Samples/SimpleInk">Simple inking</a></td>
  <td><a href="Samples/TouchKeyboard">Touch keyboard</a></td>
 </tr>
 <tr>
  <td><a href="Samples/TouchKeyboardTextInput">Touch keyboard text input</a></td>
 </tr>
</table>

### Data

<table>
 <tr>
  <td><a href="Samples/MsBlobBuilder">Blobs</a></td>
  <td><a href="Samples/Compression">Compression</a></td>
  <td><a href="Samples/ContentIndexer">Content indexer</a></td>
 </tr>
 <tr>
  <td><a href="Samples/HtmlFormValidation">Form validation (HTML)</a></td>
  <td><a href="Samples/IndexedDB">IndexedDB</a></td>
  <td><a href="Samples/Logging">Logging</a></td>
 </tr>
 <tr>
  <td><a href="Samples/PdfDocument">PDF document</a></td>
  <td><a href="Samples/DataReaderWriter">Serializing and deserializing data</a></td>
  <td><a href="Samples/XamlBind">x:Bind</a></td>
 </tr>
 <tr>
  <td><a href="Samples/XamlDeferLoadStrategy">x:DeferLoadStrategy</a></td>
  <td><a href="Samples/XmlDocument">XML DOM</a></td>
  <td><a href="Samples/XmlLite">XmlLite</a></td>
 </tr>
</table>

### Deep links and app-to-app communication

<table>
 <tr>
  <td><a href="Samples/AppServices">App services</a></td>
  <td><a href="Samples/JumpList">Jump list customization</a></td>
  <td><a href="Samples/ShareSource">Sharing content source app</a></td>
 </tr>
 <tr>
  <td><a href="Samples/ShareTarget">Sharing content target app</a></td>
 </tr>
</table>

### Devices and sensors

<table>
 <tr>
  <td><a href="Samples/Accelerometer">Accelerometer</a></td>
  <td><a href="Samples/ActivitySensor">Activity detection sensor</a></td>
  <td><a href="Samples/AllJoyn/ConsumerExperiences">AllJoyn consumer experiences</a></td>
 </tr>
 <tr>
  <td><a href="Samples/AllJoyn/ProducerExperiences">AllJoyn producer experiences</a></td>
  <td><a href="Samples/Altimeter">Altimeter</a></td>
  <td><a href="Samples/BackgroundSensors">Background sensors</a></td>
 </tr>
 <tr>
  <td><a href="Samples/BarcodeScanner">Barcode scanner</a></td>
  <td><a href="Samples/Barometer">Barometer</a></td>
  <td><a href="Samples/BluetoothAdvertisement">Bluetooth advertisement</a></td>
 </tr>
 <tr>
  <td><a href="Samples/CashDrawer">Cash drawer</a></td>
  <td><a href="Samples/Compass">Compass</a></td>
  <td><a href="Samples/CustomHidDeviceAccess">Custom HID device</a></td>
 </tr>
 <tr>
  <td><a href="Samples/CustomSensors">Custom sensors</a></td>
  <td><a href="Samples/CustomSerialDeviceAccess">Custom serial device</a></td>
  <td><a href="Samples/CustomUsbDeviceAccess">Custom USB device</a></td>
 </tr>
 <tr>
  <td><a href="Samples/DeviceEnumerationAndPairing">Device enumeration and pairing</a></td>
  <td><a href="Samples/IoT-GPIO">General Purpose Input/Output (GPIO)</a></td>
  <td><a href="Samples/Gyrometer">Gyrometer</a></td>
 </tr>
 <tr>
  <td><a href="Samples/Inclinometer">Inclinometer</a></td>
  <td><a href="Samples/IoT-I2C">Inter-Integrated Circuit (I2C)</a></td>
  <td><a href="Samples/LampDevice">Lamp device</a></td>
 </tr>
 <tr>
  <td><a href="Samples/LightSensor">Light sensor</a></td>
  <td><a href="Samples/MagneticStripeReader">Magnetic stripe reader</a></td>
  <td><a href="Samples/Nfc">Near field communication (NFC)</a></td>
 </tr>
 <tr>
  <td><a href="Samples/NfcProvisioner">NFC enterprise provisioner</a></td>
  <td><a href="Samples/OrientationSensor">Orientation sensor</a></td>
  <td><a href="Samples/Pedometer">Pedometer</a></td>
 </tr>
 <tr>
  <td><a href="Samples/PosPrinter">POS printer</a></td>
  <td><a href="Samples/ProximitySensor">Proximity sensor</a></td>
  <td><a href="Samples/RelativeInclinometer">Relative inclinometer</a></td>
 </tr>
 <tr>
  <td><a href="Samples/RelativeOrientationSensor">Relative orientation sensor</a></td>
  <td><a href="Samples/SerialArduino">Serial Arduino</a></td>
  <td><a href="Samples/IoT-SPI">Serial Peripheral Interface (SPI)</a></td>
 </tr>
 <tr>
  <td><a href="Samples/SimpleOrientationSensor">Simple orientation sensor</a></td>
 </tr>
</table>

### Files, folders, and libraries

<table>
 <tr>
  <td><a href="Samples/FileAccess">File access</a></td>
  <td><a href="Samples/FileThumbnails">File and folder thumbnail</a></td>
  <td><a href="Samples/FilePicker">File picker</a></td>
 </tr>
 <tr>
  <td><a href="Samples/FilePickerContracts">File picker provider</a></td>
  <td><a href="Samples/FileSearch">File search</a></td>
  <td><a href="Samples/FolderEnumeration">Folder enumeration</a></td>
 </tr>
 <tr>
  <td><a href="Samples/HomeGroup">HomeGroup</a></td>
  <td><a href="Samples/LibraryManagement">Library management</a></td>
  <td><a href="Samples/SemanticTextQuery">Semantic text query</a></td>
 </tr>
</table>

### Gaming

<table>
 <tr>
  <td><a href="Samples/Simple3DGameDX">Direct3D game</a></td>
  <td><a href="Samples/Simple3DGameXaml">DirectX and XAML game</a></td>
 </tr>
</table>

### Globalization and localization

<table>
 <tr>
  <td><a href="Samples/ApplicationResources">Application resources and localization</a></td>
  <td><a href="Samples/Calendar">Calendar</a></td>
  <td><a href="Samples/DateTimeFormatting">Date and time formatting</a></td>
 </tr>
 <tr>
  <td><a href="Samples/GlobalizationPreferences">Globalization preferences</a></td>
  <td><a href="Samples/JapanesePhoneticAnalysis">Japanese phonetic analysis</a></td>
  <td><a href="Samples/LanguageFont">Language font mapping</a></td>
 </tr>
 <tr>
  <td><a href="Samples/LinguisticServices">Linguistic services</a></td>
  <td><a href="Samples/NumberFormatting">Number formatting and parsing</a></td>
  <td><a href="Samples/TextSegmentation">Text segmentation</a></td>
 </tr>
 <tr>
  <td><a href="Samples/TextSuggestion">Text suggestions</a></td>
  <td><a href="Samples/Unicode">Unicode string processing</a></td>
 </tr>
</table>

### Graphics and animation

<table>
 <tr>
  <td><a href="Samples/XamlAnimatedGif">Animated GIF playback (XAML)</a></td>
  <td><a href="Samples/AnimationLibrary">Animation library (HTML)</a></td>
  <td><a href="Samples/AnimationMetrics">Animation metrics</a></td>
 </tr>
 <tr>
  <td><a href="Samples/CompositionVisual">Composition visual without framework</a></td>
  <td><a href="Samples/D2DCustomEffects">Direct2D custom image effects</a></td>
  <td><a href="Samples/D2DGradientMesh">Direct2D gradient mesh</a></td>
 </tr>
 <tr>
  <td><a href="Samples/EfficientAnimations">Efficient animations (HTML)</a></td>
  <td><a href="Samples/XamlTransform3DAnimations">Transform3D animations</a></td>
  <td><a href="Samples/XamlTransform3DParallax">Transform3D parallax</a></td>
 </tr>
</table>

### Identity, security, and encryption

<table>
 <tr>
  <td><a href="Samples/PasswordVault">Credential locker</a></td>
  <td><a href="Samples/CredentialPicker">Credential picker</a></td>
  <td><a href="Samples/DeviceLockdownAzureLogin">Device lockdown with Azure login</a></td>
 </tr>
 <tr>
  <td><a href="Samples/DisablingScreenCapture">Disabling screen capture</a></td>
  <td><a href="Samples/EnterpriseDataProtection">Enterprise data protection</a></td>
  <td><a href="Samples/KeyCredentialManager">KeyCredentialManager</a></td>
 </tr>
 <tr>
  <td><a href="Samples/Personalization">Lock screen personalization</a></td>
  <td><a href="Samples/MicrosoftPassport">Microsoft Passport and Windows Hello</a></td>
  <td><a href="Samples/SmartCard">Smart cards</a></td>
 </tr>
 <tr>
  <td><a href="Samples/UserInfo">User information</a></td>
  <td><a href="Samples/UserConsentVerifier">UserConsentVerifier</a></td>
  <td><a href="Samples/WebAccountManagement">Web account management</a></td>
 </tr>
 <tr>
  <td><a href="Samples/WebAuthenticationBroker">WebAuthenticationBroker</a></td>
 </tr>
</table>

### Launching and background tasks

<table>
 <tr>
  <td><a href="Samples/AssociationLaunching">Association launching</a></td>
  <td><a href="Samples/BackgroundTask">Background task</a></td>
  <td><a href="Samples/ExtendedExecution">Extended execution</a></td>
 </tr>
</table>

### Maps and location

<table>
 <tr>
  <td><a href="Samples/Geolocation">Geolocation</a></td>
  <td><a href="Samples/Geotag">Geotag</a></td>
  <td><a href="Samples/MapControl">MapControl</a></td>
 </tr>
</table>

### Navigation

<table>
 <tr>
  <td><a href="Samples/BackButton">Back Button</a></td>
  <td><a href="Samples/XamlMasterDetail">Master/detail</a></td>
  <td><a href="Samples/XamlNavigation">Navigation menu (XAML)</a></td>
 </tr>
 <tr>
  <td><a href="Samples/XamlPivot">Pivot</a></td>
  <td><a href="Samples/Projection">Projection</a></td>
  <td><a href="Samples/PackagedContent">XHR, handling navigation errors, and URL schemes</a></td>
 </tr>
</table>

### Networking and web services

<table>
 <tr>
  <td><a href="Samples/Advertising">Advertising</a></td>
  <td><a href="Samples/BackgroundTransfer">Background transfer</a></td>
  <td><a href="Samples/DatagramSocket">DatagramSocket</a></td>
 </tr>
 <tr>
  <td><a href="Samples/EasProtocol">Exchange Active Sync (EAS)</a></td>
  <td><a href="Samples/HttpClient">HttpClient</a></td>
  <td><a href="Samples/Json">JSON</a></td>
 </tr>
 <tr>
  <td><a href="Samples/MobileBroadband">Mobile broadband</a></td>
  <td><a href="Samples/RadioManager">Radios</a></td>
  <td><a href="Samples/FeedReader">RSS reader end-to-end (JavaScript)</a></td>
 </tr>
 <tr>
  <td><a href="Samples/SocketActivityStreamSocket">Socket activity trigger stream socket</a></td>
  <td><a href="Samples/StreamSocket">StreamSocket</a></td>
  <td><a href="Samples/Syndication">Syndication</a></td>
 </tr>
 <tr>
  <td><a href="Samples/UssdProtcol">USSD protocol</a></td>
  <td><a href="Samples/WebSocket">WebSocket</a></td>
  <td><a href="Samples/WiFiDirect">Wi-Fi Direct</a></td>
 </tr>
 <tr>
  <td><a href="Samples/WiFiDirectServices">Wi-Fi Direct services</a></td>
  <td><a href="Samples/WiFiScan">Wi-Fi scanning</a></td>
 </tr>
</table>

### Platform architecture

<table>
 <tr>
  <td><a href="Samples/ProxyStubsForWinRTComponents">In-process component authoring</a></td>
  <td><a href="Samples/WRLInProcessWinRTComponent">In-process component authoring</a></td>
  <td><a href="Samples/WRLOutOfProcessWinRTComponent">Out-of-process component authoring</a></td>
 </tr>
</table>

### Speech and Cortana

<table>
 <tr>
  <td><a href="Samples/CortanaVoiceCommand">Cortana voice command</a></td>
  <td><a href="Samples/SpeechRecognitionAndSynthesis">Speech recognition and synthesis</a></td>
 </tr>
</table>

### Threading

<table>
 <tr>
  <td><a href="Samples/WebWorkers">JavaScript Web Workers app multithreading</a></td>
 </tr>
</table>

### Tiles, toasts, and notifications

<table>
 <tr>
  <td><a href="Samples/Notifications">Notifications</a></td>
  <td><a href="Samples/SecondaryTiles">Secondary tiles</a></td>
 </tr>
</table>
