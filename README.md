<!---
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619979
--->

# Universal Windows Platform (UWP) app samples

This repo contains the samples that demonstrate the API usage patterns for the Universal Windows Platform (UWP) in the Windows Software Development Kit (SDK) for Windows 10. These code samples were created with the Universal Windows Platform templates available in Visual Studio, and are designed to run on desktop, mobile, and future devices that support the Universal Windows Platform.

> **Note:** If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/main.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

## Universal Windows Platform development

These samples require Visual Studio and the Windows Software Development Kit (SDK) for Windows 10.

   [Get a free copy of Visual Studio Community Edition with support for building Universal Windows Platform apps](http://go.microsoft.com/fwlink/p/?LinkID=280676)

Additionally, to stay on top of the latest updates to Windows and the development tools, become a Windows Insider by joining the Windows Insider Program.

   [Become a Windows Insider](https://insider.windows.com/)

## Using the samples

The easiest way to use these samples without using Git is to download the zip file containing the current version (using the following link or by clicking the "Download ZIP" button on the repo page). You can then unzip the entire archive and use the samples in Visual Studio.

   [Download the samples ZIP](../../archive/main.zip)

   **Notes:** 
   * Before you unzip the archive, right-click it, select **Properties**, and then select **Unblock**.
   * Be sure to unzip the entire archive, and not just individual samples. The samples all depend on the SharedContent folder in the archive.   
   * In Visual Studio, the platform target defaults to ARM, so be sure to change that to x64 or x86 if you want to test on a non-ARM device. 
   
The samples use Linked files in Visual Studio to reduce duplication of common files, including sample template files and image assets. These common files are stored in the SharedContent folder at the root of the repository, and are referred to in the project files using links.

**Reminder:** If you unzip individual samples, they will not build due to references to other portions of the ZIP file that were not unzipped. You must unzip the entire archive if you intend to build the samples.

For more info about the programming models, platforms, languages, and APIs demonstrated in these samples, please refer to the guidance, tutorials, and reference topics provided in the Windows 10 documentation available in the [Windows Developer Center](http://go.microsoft.com/fwlink/p/?LinkID=532421). These samples are provided as-is in order to indicate or demonstrate the functionality of the programming models and feature APIs for Windows.

## Contributions

These samples are direct from the feature teams and we welcome your input on issues and suggestions for new samples. At this time we are not accepting new samples from the public, but check back here as we evolve our contribution model.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information, see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/)
or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

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
  <td><a href="Samples/360VideoPlayback">360-degree video playback</a></td>
  <td><a href="Samples/AdaptiveStreaming">Adaptive streaming</a></td>
  <td><a href="Samples/CameraAdvancedCapture">Advanced capture</a></td>
 </tr>
 <tr>
  <td><a href="Samples/AdvancedCasting">Advanced casting</a></td>
  <td><a href="Samples/AudioCategory">Audio categories</a></td>
  <td><a href="Samples/AudioCreation">Audio graphs</a></td>
 </tr>
 <tr>
  <td><a href="Samples/BackgroundMediaPlayback">Background media playback</a></td>
  <td><a href="Samples/CameraStarterKit">Basic camera app</a></td>
  <td><a href="Samples/BasicFaceDetection">Basic face detection</a></td>
 </tr>
 <tr>
  <td><a href="Samples/BasicFaceTracking">Basic face tracking</a></td>
  <td><a href="Samples/BasicMediaCasting">Basic media casting</a></td>
  <td><a href="Samples/CameraFaceDetection">Camera face detection</a></td>
 </tr>
 <tr>
  <td><a href="Samples/CameraFrames">Camera frames</a></td>
  <td><a href="Samples/CameraGetPreviewFrame">Camera preview frame</a></td>
  <td><a href="Samples/CameraProfile">Camera profiles</a></td>
 </tr>
 <tr>
  <td><a href="Samples/CameraResolution">Camera resolution</a></td>
  <td><a href="Samples/CameraStreamCoordinateMapper">Camera stream coordinate mapper</a></td>
  <td><a href="Samples/CameraStreamCorrelation">Camera stream correlation</a></td>
 </tr>
 <tr>
  <td><a href="Samples/LiveDash">DASH streaming</a></td>
  <td><a href="Samples/D2DPhotoAdjustment">Direct2D photo adjustment</a></td>
  <td><a href="Samples/MediaEditing">Media editing</a></td>
 </tr>
 <tr>
  <td><a href="Samples/MediaImport">Media import</a></td>
  <td><a href="Samples/XamlCustomMediaTransportControls">Media transport controls</a></td>
  <td><a href="Samples/MIDI">MIDI</a></td>
 </tr>
 <tr>
  <td><a href="Samples/Playlists">Playlists</a></td>
  <td><a href="Samples/PlayReady">PlayReady</a></td>
  <td><a href="Samples/CameraOpenCV">Processing frames with OpenCV</a></td>
 </tr>
 <tr>
  <td><a href="Samples/SimpleImaging">Simple imaging</a></td>
  <td><a href="Samples/SpatialSound">Spatial audio</a></td>
  <td><a href="Samples/SystemMediaTransportControls">System media transport controls</a></td>
 </tr>
 <tr>
  <td><a href="Samples/MediaTranscoding">Transcoding media</a></td>
  <td><a href="Samples/VideoPlayback">Video playback</a></td>
  <td><a href="Samples/VideoPlaybackSynchronization">Video playback synchronization</a></td>
 </tr>
 <tr>
  <td><a href="Samples/CameraVideoStabilization">Video stabilization</a></td>
  <td><a href="Samples/WindowsAudioSession">Windows audio session (WASAPI)</a></td>
 </tr>
</table>

### Communications

<table>
 <tr>
  <td><a href="Samples/BluetoothRfcommChat">Bluetooth RFCOMM chat</a></td>
  <td><a href="Samples/SimpleCommunication">Real-time communication</a></td>
  <td><a href="Samples/SmsSendAndReceive">SMS send and receive</a></td>
 </tr>
</table>

### Contacts and calendar

<table>
 <tr>
  <td><a href="Samples/Appointments">Appointment calendar</a></td>
  <td><a href="Samples/ContactCards">Contact cards</a></td>
  <td><a href="Samples/ContactPanel">Contact panel</a></td>
 </tr>
 <tr>
  <td><a href="Samples/ContactPicker">Contact picker</a></td>
  <td><a href="Samples/MyPeopleNotifications">My People notifications</a></td>
  <td><a href="Samples/UserDataAccountManager">UserDataAccountManager</a></td>
 </tr>
</table>

### Controls, layout, and text

<table>
 <tr>
  <td><a href="Samples/3DPrinting">3D Printing</a></td>
  <td><a href="Samples/3DPrintingFromUnity">3D Printing from Unity</a></td>
  <td><a href="Samples/XamlBottomUpList">Bottom-up list (XAML)</a></td>
 </tr>
 <tr>
  <td><a href="Samples/Clipboard">Clipboard</a></td>
  <td><a href="Samples/DWriteColorGlyph">Colored glyphs (DirectWrite)</a></td>
  <td><a href="Samples/XamlCommanding">Commanding</a></td>
 </tr>
 <tr>
  <td><a href="Samples/ContextMenu">Context menu</a></td>
  <td><a href="Samples/XamlContextMenu">Context menu (XAML)</a></td>
  <td><a href="Samples/CustomEditControl">Custom text edit control</a></td>
 </tr>
 <tr>
  <td><a href="Samples/XamlDataVirtualization">Data virtualization</a></td>
  <td><a href="Samples/DisplayOrientation">Display orientation</a></td>
  <td><a href="Samples/DWriteTextLayoutCloudFont">Downloadable fonts (DirectWrite)</a></td>
 </tr>
 <tr>
  <td><a href="Samples/XamlCloudFontIntegration">Downloadable fonts (XAML)</a></td>
  <td><a href="Samples/XamlDragAndDrop">Drag and drop</a></td>
  <td><a href="Samples/XamlFocusVisuals">Focus visuals</a></td>
 </tr>
 <tr>
  <td><a href="Samples/FullScreenMode">Full screen mode</a></td>
  <td><a href="Samples/XamlGamepadNavigation">Gamepad-style navigation (XAML)</a></td>
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
  <td><a href="Samples/DpiScaling">Scaling according to DPI</a></td>
  <td><a href="Samples/AppWindow">Secondary windows</a></td>
  <td><a href="Samples/SplashScreen">Splash screen</a></td>
 </tr>
 <tr>
  <td><a href="Samples/XamlStateTriggers">State triggers</a></td>
  <td><a href="Samples/XamlTailoredMultipleViews">Tailored multiple views</a></td>
  <td><a href="Samples/TitleBar">Title bar</a></td>
 </tr>
 <tr>
  <td><a href="https://github.com/Microsoft/Xaml-Controls-Gallery">UI basics (XAML)</a></td>
  <td><a href="Samples/UserInteractionMode">User interaction mode</a></td>
  <td><a href="Samples/XamlWebView">WebView control (XAML)</a></td>
 </tr>
 <tr>
  <td><a href="Samples/ResizeAppView">Window resizing</a></td>
 </tr>
</table>

### Custom user interactions

<table>
 <tr>
  <td><a href="Samples/BasicInput">Basic input</a></td>
  <td><a href="Samples/ComplexInk">Complex inking</a></td>
  <td><a href="Samples/InkAnalysis">Ink analysis</a></td>
 </tr>
 <tr>
  <td><a href="Samples/LowLatencyInput">Low latency input</a></td>
  <td><a href="Samples/PenHaptics">Pen haptics</a></td>
  <td><a href="Samples/SimpleInk">Simple inking</a></td>
 </tr>
 <tr>
  <td><a href="Samples/TouchKeyboard">Touch keyboard</a></td>
  <td><a href="Samples/TouchKeyboardTextInput">Touch keyboard text input</a></td>
 </tr>
</table>

### Data

<table>
 <tr>
  <td><a href="Samples/Compression">Compression</a></td>
  <td><a href="Samples/ContentIndexer">Content indexer</a></td>
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
  <td><a href="Samples/RemoteSystems">Remote systems</a></td>
 </tr>
 <tr>
  <td><a href="Samples/ShareSource">Sharing content source app</a></td>
  <td><a href="Samples/ShareTarget">Sharing content target app</a></td>
  <td><a href="Samples/UserActivity">User activities</a></td>
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
  <td><a href="Samples/BarcodeScannerProvider">Barcode scanner provider</a></td>
  <td><a href="Samples/Barometer">Barometer</a></td>
 </tr>
 <tr>
  <td><a href="Samples/BluetoothAdvertisement">Bluetooth advertisement</a></td>
  <td><a href="Samples/BluetoothLE">Bluetooth Low Energy</a></td>
  <td><a href="Samples/CashDrawer">Cash drawer</a></td>
 </tr>
 <tr>
  <td><a href="Samples/Compass">Compass</a></td>
  <td><a href="Samples/CustomHidDeviceAccess">Custom HID device</a></td>
  <td><a href="Samples/CustomSensors">Custom sensors</a></td>
 </tr>
 <tr>
  <td><a href="Samples/CustomSerialDeviceAccess">Custom serial device</a></td>
  <td><a href="Samples/CustomUsbDeviceAccess">Custom USB device</a></td>
  <td><a href="Samples/DeviceEnumerationAndPairing">Device enumeration and pairing</a></td>
 </tr>
 <tr>
  <td><a href="Samples/IoT-GPIO">General Purpose Input/Output (GPIO)</a></td>
  <td><a href="Samples/Gyrometer">Gyrometer</a></td>
  <td><a href="Samples/Inclinometer">Inclinometer</a></td>
 </tr>
 <tr>
  <td><a href="Samples/IoT-I2C">Inter-Integrated Circuit (I2C)</a></td>
  <td><a href="Samples/LampDevice">Lamp device</a></td>
  <td><a href="Samples/LightSensor">Light sensor</a></td>
 </tr>
 <tr>
  <td><a href="Samples/LineDisplay">Line display</a></td>
  <td><a href="Samples/Magnetometer">Magnetometer</a></td>
  <td><a href="Samples/MagneticStripeReader">Magnetic stripe reader</a></td>
 </tr>
 <tr>
  <td><a href="Samples/Nfc">Near field communication (NFC)</a></td>
  <td><a href="Samples/NfcProvisioner">NFC enterprise provisioner</a></td>
  <td><a href="Samples/OrientationSensor">Orientation sensor</a></td>
 </tr>
 <tr>
  <td><a href="Samples/Pedometer">Pedometer</a></td>
  <td><a href="Samples/PosPrinter">POS printer</a></td>
  <td><a href="Samples/ProximitySensor">Proximity sensor</a></td>
 </tr>
 <tr>
  <td><a href="Samples/RadialController">Radial controller</a></td>
  <td><a href="Samples/RelativeInclinometer">Relative inclinometer</a></td>
  <td><a href="Samples/SerialArduino">Serial Arduino</a></td>
 </tr>
 <tr>
  <td><a href="Samples/IoT-SPI">Serial Peripheral Interface (SPI)</a></td>
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
  <td><a href="Samples/AnimationMetrics">Animation metrics</a></td>
  <td><a href="Samples/CompositionVisual">Composition visual without framework</a></td>
 </tr>
 <tr>
  <td><a href="Samples/D2AdvancedColorImages">Direct2D advanced color image rendering</a></td>
  <td><a href="Samples/D2DCustomEffects">Direct2D custom image effects</a></td>
  <td><a href="Samples/D2DGradientMesh">Direct2D gradient mesh</a></td>
 </tr>
 <tr>
  <td><a href="Samples/D2DSvgImage">Direct2D SVG image rendering</a></td>
  <td><a href="Samples/XamlTransform3DAnimations">Transform3D animations</a></td>
 </tr>
</table>

### Holographic

<table>
 <tr>
  <td><a href="Samples/BasicHologram">Basic hologram</a></td>
  <td><a href="Samples/HolographicFaceTracking">Holographic face tracking</a></td>
  <td><a href="Samples/HolographicMixedRealityCapture">Holographic mixed reality capture</a></td>
 </tr>
 <tr>
  <td><a href="Samples/HolographicSpatialMapping">Holographic spatial mapping</a></td>
  <td><a href="Samples/HolographicSpatialStage">Holographic spatial stage</a></td>
  <td><a href="Samples/HolographicVoiceInput">Holographic voice input</a></td>
 </tr>
 <tr>
  <td><a href="Samples/SpatialInteractionSource">Spatial interaction source</a></td>
  <td><a href="Samples/HolographicTagAlong">Tag-along hologram</a></td>
 </tr>
 <tr>
  <td><a href="Samples/MixedRealityModel">Mixed Reality Model</a></td>
 </tr>
</table>

### Identity, security, and encryption

<table>
 <tr>
  <td><a href="Samples/PasswordVault">Credential locker</a></td>
  <td><a href="Samples/ClientDeviceInformation">Client Device Information</a></td>
  <td><a href="Samples/CredentialPicker">Credential picker</a></td>
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
  <td><a href="Samples/UserCertificateStore">UserCertificateStore</a></td>
  <td><a href="Samples/UserConsentVerifier">UserConsentVerifier</a></td>
 </tr>
 <tr>
  <td><a href="Samples/WebAccountManagement">Web account management</a></td>
  <td><a href="Samples/WebAuthenticationBroker">WebAuthenticationBroker</a></td>
 </tr>
</table>

### Launching and background tasks

<table>
 <tr>
  <td><a href="Samples/AssociationLaunching">Association launching</a></td>
  <td><a href="Samples/BackgroundActivation">Background activation</a></td>
  <td><a href="Samples/BackgroundTask">Background task</a></td>
 </tr>
 <tr>
  <td><a href="Samples/ExtendedExecution">Extended execution</a></td>
  <td><a href="https://aka.ms/Kcrqst">Multi-instance apps</a></td>
  <td><a href="Samples/BasicSuspension">Suspend and resume</a></td>
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
  <td><a href="Samples/XamlPivot">Pivot</a></td>
 </tr>
 <tr>
  <td><a href="Samples/Projection">Projection</a></td>
 </tr>
</table>

### Networking and web services

<table>
 <tr>
  <td><a href="Samples/BackgroundTransfer">Background transfer</a></td>
  <td><a href="Samples/DatagramSocket">DatagramSocket</a></td>
  <td><a href="Samples/HttpClient">HttpClient</a></td>
 </tr>
 <tr>
  <td><a href="Samples/Json">JSON</a></td>
  <td><a href="Samples/MobileBroadband">Mobile broadband</a></td>
  <td><a href="Samples/RadioManager">Radios</a></td>
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
  <td><a href="Samples/HotspotAuthentication">Wi-Fi hotspot authentication</a></td>
  <td><a href="Samples/WiFiScan">Wi-Fi scanning</a></td>
 </tr>
</table>

### Platform architecture

<table>
 <tr>
  <td><a href="Samples/Capabilities">Capabilities</a></td>
  <td><a href="Samples/CustomCapability">Custom capabilities</a></td>
  <td><a href="Samples/ProxyStubsForWinRTComponents">In-process component authoring</a></td>
 </tr>
 <tr>
  <td><a href="Samples/WRLInProcessWinRTComponent">In-process component authoring</a></td>
  <td><a href="Samples/WRLOutOfProcessWinRTComponent">Out-of-process component authoring</a></td>
  <td><a href="Samples/VersionAdaptiveCode">Version adaptive code</a></td>
 </tr>
</table>

### Speech and Cortana

<table>
 <tr>
  <td><a href="Samples/CortanaVoiceCommand">Cortana voice command</a></td>
  <td><a href="Samples/SpeechRecognitionAndSynthesis">Speech recognition and synthesis</a></td>
 </tr>
</table>

### Tiles, toasts, and notifications

<table>
 <tr>
  <td><a href="Samples/Notifications">Notifications</a></td>
  <td><a href="Samples/SecondaryTiles">Secondary tiles</a></td>
 </tr>
</table>
