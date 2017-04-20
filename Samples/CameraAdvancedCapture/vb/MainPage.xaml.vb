'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' This code is licensed under the MIT License (MIT).
' THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
' IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
' PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
'
'*********************************************************
Imports System
Imports System.Diagnostics
Imports System.Linq
Imports System.Threading.Tasks
Imports Windows.ApplicationModel
Imports Windows.Devices.Enumeration
Imports Windows.Devices.Sensors
Imports Windows.Foundation
Imports Windows.Foundation.Metadata
Imports Windows.Graphics.Display
Imports Windows.Graphics.Imaging
Imports Windows.Media
Imports Windows.Media.Capture
Imports Windows.Media.Core
Imports Windows.Media.Devices
Imports Windows.Media.MediaProperties
Imports Windows.Phone.UI.Input
Imports Windows.Storage
Imports Windows.Storage.FileProperties
Imports Windows.Storage.Streams
Imports Windows.System.Display
Imports Windows.UI.Core
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Input
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Navigation

Namespace Global.CameraAdvancedCapture

    Public NotInheritable Partial Class MainPage
        Inherits Page

        ' Receive notifications about rotation of the device and UI and apply any necessary rotation to the preview stream and UI controls
        Private ReadOnly _displayInformation As DisplayInformation = DisplayInformation.GetForCurrentView()
        Private ReadOnly _orientationSensor As SimpleOrientationSensor = SimpleOrientationSensor.GetDefault()
        Private _deviceOrientation As SimpleOrientation = SimpleOrientation.NotRotated
        Private _displayOrientation As DisplayOrientations = DisplayOrientations.Portrait

        ' Rotation metadata to apply to the preview stream and recorded videos (MF_MT_VIDEO_ROTATION)
        ' Reference: http://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh868174.aspx
        Private Shared ReadOnly RotationKey As Guid = New Guid("C380465D-2271-428C-9B83-ECEA3B4A85C1")

        ' Folder in which the captures will be stored (initialized in SetupUiAsync)
        Private _captureFolder As StorageFolder = Nothing

        ' Prevent the screen from sleeping while the camera is running
        Private ReadOnly _displayRequest As DisplayRequest = New DisplayRequest()

        ' For listening to media property changes
        Private ReadOnly _systemMediaControls As SystemMediaTransportControls = SystemMediaTransportControls.GetForCurrentView()

        ' MediaCapture and its state variables
        Private _mediaCapture As MediaCapture
        Private _isInitialized As Boolean
        Private _isPreviewing As Boolean

        ' Holds the index of the current Advanced Capture mode (-1 for no Advanced Capture active)
        private _advancedCaptureMode As Integer = -1

        ' Information about the camera device
        Private _mirroringPreview As Boolean
        Private _externalCamera As Boolean

        ' The value at which the certainty from the HDR analyzer maxes out in the graphical representation
        Private Const CERTAINTY_CAP As Double = 0.7

        ' Advanced Capture and Scene Analysis instances
        Private _advancedCapture As AdvancedPhotoCapture
        Private _sceneAnalysisEffect As SceneAnalysisEffect

        ''' <summary>
        ''' Helper class to contain the information that describes an Advanced Capture
        ''' </summary>
        Public Class AdvancedCaptureContext
            Public CaptureFileName As String
            Public CaptureOrientation As PhotoOrientation
        End Class

#Region " Constructor, lifecycle and navigation "
        Public Sub New()
            Me.InitializeComponent()
            NavigationCacheMode = NavigationCacheMode.Disabled
            AddHandler Application.Current.Suspending, AddressOf Application_Suspending
            AddHandler Application.Current.Resuming, AddressOf Application_Resuming
            HdrImpactBar.Maximum = CERTAINTY_CAP
        End Sub

        Private Async Sub Application_Suspending(sender As Object, e As SuspendingEventArgs)
            If Frame.CurrentSourcePageType Is GetType(MainPage) Then
                Dim deferral = e.SuspendingOperation.GetDeferral()
                Await CleanupCameraAsync()
                Await CleanupUiAsync()
                deferral.Complete()
            End If
        End Sub

        Private Async Sub Application_Resuming(sender As Object, o As Object)
            If Frame.CurrentSourcePageType Is GetType(MainPage) Then
                Await SetupUiAsync()
                Await InitializeCameraAsync()
            End If
        End Sub

        Protected Overrides Async Sub OnNavigatedTo(e As NavigationEventArgs)
            Await SetupUiAsync()
            Await InitializeCameraAsync()
        End Sub

        Protected Overrides Async Sub OnNavigatingFrom(e As NavigatingCancelEventArgs)
            Await CleanupCameraAsync()
            Await CleanupUiAsync()
        End Sub

#End Region

#Region " Event handlers "

        ''' <summary>
        ''' In the event of the app being minimized this method handles media property change events. If the app receives a mute
        ''' notification, it is no longer in the foregroud.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="args"></param>
        Private Async Sub SystemMediaControls_PropertyChanged(sender As SystemMediaTransportControls, args As SystemMediaTransportControlsPropertyChangedEventArgs)
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Async Sub()
                If args.Property = SystemMediaTransportControlsProperty.SoundLevel AndAlso Frame.CurrentSourcePageType Is GetType(MainPage) Then
                    If sender.SoundLevel = SoundLevel.Muted Then
                        Await CleanupCameraAsync()
                    ElseIf Not _isInitialized Then
                        Await InitializeCameraAsync()
                    End If
                End If
            End Sub)
        End Sub

        ''' <summary>
        ''' Occurs each time the simple orientation sensor reports a new sensor reading.
        ''' </summary>
        ''' <param name="sender">The event source.</param>
        ''' <param name="args">The event data.</param>
        Private Async Sub OrientationSensor_OrientationChanged(sender As SimpleOrientationSensor, args As SimpleOrientationSensorOrientationChangedEventArgs)
            If args.Orientation <> SimpleOrientation.Faceup AndAlso args.Orientation <> SimpleOrientation.Facedown Then
                _deviceOrientation = args.Orientation
                Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub() UpdateControlOrientation())
            End If
        End Sub

        ''' <summary>
        ''' This event will be raised when the page is rotated, when the DisplayInformation.AutoRotationPreferences value set in the SetupUiAsync() method
        ''' cannot be not honored.
        ''' </summary>
        ''' <param name="sender">The event source.</param>
        ''' <param name="args">The event data.</param>
        Private Async Sub DisplayInformation_OrientationChanged(sender As DisplayInformation, args As Object)
            _displayOrientation = sender.CurrentOrientation
            If _isPreviewing Then
                Await SetPreviewRotationAsync()
            End If

            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub() UpdateControlOrientation())
        End Sub

        ''' <summary>
        ''' This event will be raised every time the effect processes a preview frame, to deliver information about the current scene in the camera preview.
        ''' </summary>
        ''' <param name="sender">The effect raising the event.</param>
        ''' <param name="args">The event data.</param>
        Private Async Sub SceneAnalysisEffect_SceneAnalyzed(sender As SceneAnalysisEffect, args As SceneAnalyzedEventArgs)
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                ' Update the graphical representation of how much detail could be recovered through an HDR capture, ranging
                ' from 0 ("everything is within the dynamic range of the camera") to CERTAINTY_CAP ("at this point the app
                ' strongly recommends an HDR capture"), where CERTAINTY_CAP can be any number between 0 and 1 that the app chooses.
                HdrImpactBar.Value = Math.Min(CERTAINTY_CAP, args.ResultFrame.HighDynamicRange.Certainty)
            End Sub)
        End Sub

        ''' <summary>
        ''' This event will be raised only on devices that support returning a reference photo, which is a normal exposure of the scene
        ''' without processing, also referred to as "EV0".
        ''' </summary>
        ''' <param name="sender">The object raising this event.</param>
        ''' <param name="args">The event data.</param>
        Private Async Sub AdvancedCapture_OptionalReferencePhotoCaptured(sender As AdvancedPhotoCapture, args As OptionalReferencePhotoCapturedEventArgs)
            ' Retrieve the context (i.e. the information about the capture this event belongs to)
            Dim context = TryCast(args.Context, AdvancedCaptureContext)
            
            ' Add "_Reference" at the end of the filename to create when saving the reference photo
            Dim referenceName = context.CaptureFileName.Replace(".jpg", "_Reference.jpg")

            ' Hold a reference to the frame before the async file creation operation so it's not cleaned up before ReencodeAndSavePhotoAsync
            Using frame = args.Frame
                Dim file = Await _captureFolder.CreateFileAsync(referenceName, CreationCollisionOption.GenerateUniqueName)
                Debug.WriteLine("AdvancedCapture_OptionalReferencePhotoCaptured for " & context.CaptureFileName & ". Saving to " & file.Path)

                Await ReencodeAndSavePhotoAsync(frame, file, context.CaptureOrientation)
            End Using
        End Sub

        ''' <summary>
        ''' This event will be raised when all the necessary captures are completed, and at this point the camera is technically ready
        ''' to capture again while the processing takes place.
        ''' </summary>
        ''' <param name="sender">The object raising this event.</param>
        ''' <param name="args">The event data.</param>
        Private Sub AdvancedCapture_AllPhotosCaptured(sender As AdvancedPhotoCapture, args As Object)
            Debug.WriteLine("AdvancedCapture_AllPhotosCaptured")
        End Sub

        Private Async Sub PhotoButton_Click(sender As Object, e As RoutedEventArgs)
            Await TakeAdvancedCapturePhotoAsync()
        End Sub

        Private Async Sub HardwareButtons_CameraPressed(sender As Object, e As CameraEventArgs)
            Await TakeAdvancedCapturePhotoAsync()
        End Sub

        Private Async Sub MediaCapture_Failed(sender As MediaCapture, errorEventArgs As MediaCaptureFailedEventArgs)
            Debug.WriteLine("MediaCapture_Failed: (0x{0:X}) {1}", errorEventArgs.Code, errorEventArgs.Message)
            Await CleanupCameraAsync()
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub() UpdateUi())
        End Sub

#End Region

#Region " MediaCapture methods "

        ''' <summary>
        ''' Initializes the MediaCapture, registers events, gets camera device information for mirroring and rotating, starts preview and unlocks the UI
        ''' </summary>
        ''' <returns></returns>
        Private Async Function InitializeCameraAsync() As Task
            Debug.WriteLine("InitializeCameraAsync")
            If _mediaCapture Is Nothing Then

                ' Attempt to get the back camera if one is available, but use any camera device if not
                Dim cameraDevice = Await FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel.Back)
                If cameraDevice Is Nothing Then
                    Debug.WriteLine("No camera device found!")
                    Return
                End If

                _mediaCapture = New MediaCapture()
                AddHandler _mediaCapture.Failed, AddressOf MediaCapture_Failed

                Dim settings = New MediaCaptureInitializationSettings With {.VideoDeviceId = cameraDevice.Id}
                Try
                    Await _mediaCapture.InitializeAsync(settings)
                    _isInitialized = True
                Catch ex As UnauthorizedAccessException
                    Debug.WriteLine("The app was denied access to the camera")
                End Try

                ' If initialization succeeded, start the preview, kick off scene analysis, and enable Advanced Capture
                If _isInitialized Then

                    ' Figure out where the camera is located to account for mirroring and later adjust rotation accordingly
                    If cameraDevice.EnclosureLocation Is Nothing OrElse cameraDevice.EnclosureLocation.Panel = Windows.Devices.Enumeration.Panel.Unknown Then
                        _externalCamera = True
                    Else
                        _externalCamera = False
                        _mirroringPreview =(cameraDevice.EnclosureLocation.Panel = Windows.Devices.Enumeration.Panel.Front)
                    End If

                    Await StartPreviewAsync()

                    ' On successful preview, add a Scene Analysis effect to the pipeline and enter Advanced Capture mode
                    If _isPreviewing Then
                        Await CreateSceneAnalysisEffectAsync()

                        Await EnableAdvancedCaptureAsync()
                    End If

                End If

                ' Update the controls to reflect the current state of the app
                UpdateUi()
            End If
        End Function

        ''' <summary>
        ''' Starts the preview and adjusts it for for rotation and mirroring after making a request to keep the screen on
        ''' </summary>
        ''' <returns></returns>
        Private Async Function StartPreviewAsync() As Task
            _displayRequest.RequestActive()
            PreviewControl.Source = _mediaCapture
            PreviewControl.FlowDirection = If(_mirroringPreview, FlowDirection.RightToLeft, FlowDirection.LeftToRight)
            Await _mediaCapture.StartPreviewAsync()
            _isPreviewing = True
            If _isPreviewing Then
                Await SetPreviewRotationAsync()
            End If
        End Function

        ''' <summary>
        ''' Gets the current orientation of the UI in relation to the device (when AutoRotationPreferences cannot be honored) and applies a corrective rotation to the preview
        ''' </summary>
        Private Async Function SetPreviewRotationAsync() As Task
            If _externalCamera Then
                Return
            End If

            ' Calculate which way and how far to rotate the preview
            Dim rotationDegrees As Integer = ConvertDisplayOrientationToDegrees(_displayOrientation)
            If _mirroringPreview Then
                rotationDegrees =(360 - rotationDegrees) Mod 360
            End If

            ' Add rotation metadata to the preview stream to make sure the aspect ratio / dimensions match when rendering and getting preview frames
            Dim props = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview)
            props.Properties.Add(RotationKey, rotationDegrees)
            Await _mediaCapture.SetEncodingPropertiesAsync(MediaStreamType.VideoPreview, props, Nothing)
        End Function

        ''' <summary>
        ''' Stops the preview and deactivates a display request, to allow the screen to go into power saving modes
        ''' </summary>
        ''' <returns></returns>
        Private Async Function StopPreviewAsync() As Task
            _isPreviewing = False
            Await _mediaCapture.StopPreviewAsync()
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                PreviewControl.Source = Nothing
                _displayRequest.RequestRelease()
            End Sub)
        End Function

        ''' <summary>
        ''' Adds scene analysis to the video preview stream, registers for its event, enables it, and gets the effect instance
        ''' </summary>
        ''' <returns></returns>
        Private Async Function CreateSceneAnalysisEffectAsync() As Task
            ' Create the definition, which will contain some initialization settings
            Dim definition = New SceneAnalysisEffectDefinition()

            ' Add the effect to the video preview stream
            _sceneAnalysisEffect = CType(Await _mediaCapture.AddVideoEffectAsync(definition, MediaStreamType.VideoPreview), SceneAnalysisEffect)
            Debug.WriteLine("SA effect added to pipeline")
            AddHandler _sceneAnalysisEffect.SceneAnalyzed, AddressOf SceneAnalysisEffect_SceneAnalyzed
            _sceneAnalysisEffect.HighDynamicRangeAnalyzer.Enabled = True
        End Function

        ''' <summary>
        ''' Disables and removes the scene analysis effect, and unregisters the event handler for the SceneAnalyzed event of the effect
        ''' </summary>
        ''' <returns></returns>
        Private Async Function CleanSceneAnalysisEffectAsync() As Task
            ' Disable detection and unsubscribe from the events
            _sceneAnalysisEffect.HighDynamicRangeAnalyzer.Enabled = False
            RemoveHandler _sceneAnalysisEffect.SceneAnalyzed, AddressOf SceneAnalysisEffect_SceneAnalyzed

            ' Remove the effect from the pipeline (see ClearEffectsAsync method to remove all effects from a given stream)
            Await _mediaCapture.RemoveEffectAsync(_sceneAnalysisEffect)
            Debug.WriteLine("SA effect removed from pipeline")
            _sceneAnalysisEffect = Nothing
        End Function

        ''' <summary>
        ''' Creates an instance of the AdvancedPhotoCapture and registers for its events
        ''' </summary>
        ''' <returns></returns>
        Private Async Function EnableAdvancedCaptureAsync() As Task
            ' No work to be done if there already is an AdvancedCapture instance
            If _advancedCapture IsNot Nothing Then
                Return
            End If

            ' Configure one of the modes in the control
            CycleAdvancedCaptureMode()

            ' Prepare for an Advanced Capture
            _advancedCapture = Await _mediaCapture.PrepareAdvancedPhotoCaptureAsync(ImageEncodingProperties.CreateJpeg())
            Debug.WriteLine("Enabled dvanced Capture")

            ' Register for events published by the AdvancedCapture
            AddHandler _advancedCapture.AllPhotosCaptured, AddressOf AdvancedCapture_AllPhotosCaptured
            AddHandler _advancedCapture.OptionalReferencePhotoCaptured, AddressOf AdvancedCapture_OptionalReferencePhotoCaptured
        End Function

        '''<summary>
        ''' Configures the AdvancedPhotoControl to the next supported mode
        '''</summary>
        '''<remarks>
        ''' Note that this method can be safely called regardless of whether the AdvancedPhotoCapture is in the "prepared
        ''' state" or not. Internal changes to the mode will be applied  when calling Prepare, or when calling Capture if
        ''' the mode has been changed since the call to Prepare. This allows for fast changing of desired capture modes,
        ''' that can more quickly adapt to rapidly changing shooting conditions.
        '''</remarks>
        Private Sub CycleAdvancedCaptureMode()
            ' Calculate the index for the next supported mode
            _advancedCaptureMode = (_advancedCaptureMode + 1) Mod  _mediaCapture.VideoDeviceController.AdvancedPhotoControl.SupportedModes.Count
            Debug.WriteLine(_advancedCaptureMode)
            ' Configure the settings object to the mode at the calculated index
            Dim settings = New AdvancedPhotoCaptureSettings With
            {
                .Mode = _mediaCapture.VideoDeviceController.AdvancedPhotoControl.SupportedModes(_advancedCaptureMode)
            }

            ' Configure the mode on the control
            _mediaCapture.VideoDeviceController.AdvancedPhotoControl.Configure(settings)

            ' Update the button text to reflect the current mode
            ModeTextBlock.Text = _mediaCapture.VideoDeviceController.AdvancedPhotoControl.Mode.ToString()
        End Sub

        ''' <summary>
        ''' Cleans up the instance of the AdvancedCapture
        ''' </summary>
        ''' <returns></returns>
        Private Async Function DisableAdvancedCaptureAsync() As Task
            ' No work to be done if there is no AdvancedCapture
            If _advancedCapture Is Nothing Then
                Return
            End If

            Await _advancedCapture.FinishAsync()
            _advancedCapture = Nothing

            ' Reset the Advanced Capture Mode index
            _advancedCaptureMode = -1

            Debug.WriteLine("Disabled Advanced Capture")
        End Function

        ''' <summary>
        ''' Takes an Advanced Capture photo to a StorageFile and adds rotation metadata to it
        ''' </summary>
        ''' <returns></returns>
        Private Async Function TakeAdvancedCapturePhotoAsync() As Task
            PhotoButton.IsEnabled = False
            CycleModeButton.IsEnabled = False

            Try
                Debug.WriteLine("Taking Advanced Capture photo...")
                ' Read the current orientation of the camera and the capture time
                Dim photoOrientation = ConvertOrientationToPhotoOrientation(GetCameraOrientation())
                Dim fileName = String.Format("AdvancedCapturePhoto_{0}.jpg", DateTime.Now.ToString("HHmmss"))

                ' Create a context object, to identify the capture later on
                Dim context = New AdvancedCaptureContext With
                {
                    .CaptureFileName = fileName,
                    .CaptureOrientation = photoOrientation
                }

                ' Start capture, and pass the context object to get it back in the OptionalReferencePhotoCaptured event
                Dim capture =  Await _advancedCapture.CaptureAsync(context)

                Using frame = capture.Frame
                    Dim file = Await _captureFolder.CreateFileAsync(fileName, CreationCollisionOption.GenerateUniqueName)
                    Debug.WriteLine("Photo taken! Saving to " & file.Path)

                    Await ReencodeAndSavePhotoAsync(frame, file, photoOrientation)
                End Using

            Catch ex As Exception
                ' File I/O errors are reported as exceptions
                Debug.WriteLine("Exception when taking an Advanced Capture photo: " + ex.ToString())
            Finally
                ' Re-enable the buttons to reflect the current state of the app
                UpdateUi()
            End Try
        End Function

        ''' <summary>
        ''' Cleans up the camera resources (after stopping any video recording and/or preview if necessary) and unregisters from MediaCapture events
        ''' </summary>
        ''' <returns></returns>
        Private Async Function CleanupCameraAsync() As Task
            Debug.WriteLine("CleanupCameraAsync")
            If _isInitialized Then
                If _isPreviewing Then
                    Await StopPreviewAsync()
                End If

                If _advancedCapture IsNot Nothing Then
                    Await DisableAdvancedCaptureAsync()
                End If

                If _sceneAnalysisEffect IsNot Nothing Then
                    Await CleanSceneAnalysisEffectAsync()
                End If

                _isInitialized = False
            End If

            If _mediaCapture IsNot Nothing Then
                RemoveHandler _mediaCapture.Failed, AddressOf MediaCapture_Failed
                _mediaCapture.Dispose()
                _mediaCapture = Nothing
            End If
        End Function

#End Region

#Region " Helper methods "

        ''' <summary>
        ''' Attempts to lock the page orientation, hide the StatusBar (on Phone) and registers event handlers for hardware buttons and orientation sensors
        ''' </summary>
        ''' <returns></returns>
        Private Async Function SetupUiAsync() As Task
            ' Attempt to lock page to landscape orientation to prevent the CaptureElement from rotating, as this gives a better experience
            DisplayInformation.AutoRotationPreferences = DisplayOrientations.Landscape
            
            ' Hide the status bar
            If ApiInformation.IsTypePresent("Windows.UI.ViewManagement.StatusBar") Then
                Await Windows.UI.ViewManagement.StatusBar.GetForCurrentView().HideAsync()
            End If

            ' Populate orientation variables with the current state
            _displayOrientation = _displayInformation.CurrentOrientation
            If _orientationSensor IsNot Nothing Then
                _deviceOrientation = _orientationSensor.GetCurrentOrientation()
            End If

            RegisterEventHandlers()

            Dim picturesLibrary = Await StorageLibrary.GetLibraryAsync(KnownLibraryId.Pictures)
            ' Fall back to the local app storage if the Pictures Library is not available
            _captureFolder = If(picturesLibrary.SaveFolder, ApplicationData.Current.LocalFolder)
        End Function

        ''' <summary>
        ''' Unregisters event handlers for hardware buttons and orientation sensors, allows the StatusBar (on Phone) to show, and removes the page orientation lock
        ''' </summary>
        ''' <returns></returns>
        Private Async Function CleanupUiAsync() As Task
            UnregisterEventHandlers()

            If ApiInformation.IsTypePresent("Windows.UI.ViewManagement.StatusBar") Then
                Await Windows.UI.ViewManagement.StatusBar.GetForCurrentView().ShowAsync()
            End If

            DisplayInformation.AutoRotationPreferences = DisplayOrientations.None
        End Function

        ''' <summary>
        ''' This method will enable/disable the buttons depending on the current state of the app
        ''' </summary>
        Private Sub UpdateUi()
            PhotoButton.IsEnabled = _isPreviewing
            CycleModeButton.IsEnabled = _isPreviewing
        End Sub

        ''' <summary>
        ''' Registers event handlers for hardware buttons and orientation sensors, and performs an initial update of the UI rotation
        ''' </summary>
        Private Sub RegisterEventHandlers()
            If ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons") Then
                AddHandler HardwareButtons.CameraPressed, AddressOf HardwareButtons_CameraPressed
            End If

            If _orientationSensor IsNot Nothing Then
                AddHandler _orientationSensor.OrientationChanged, AddressOf OrientationSensor_OrientationChanged
                UpdateControlOrientation()
            End If

            ' Have the Advanced Capture button call the method that cycles through the modes
            AddHandler CycleModeButton.Click, sub(sender, args) CycleAdvancedCaptureMode()

            AddHandler _displayInformation.OrientationChanged, AddressOf DisplayInformation_OrientationChanged
            AddHandler _systemMediaControls.PropertyChanged, AddressOf SystemMediaControls_PropertyChanged
        End Sub

        ''' <summary>
        ''' Unregisters event handlers for hardware buttons and orientation sensors
        ''' </summary>
        Private Sub UnregisterEventHandlers()
            If ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons") Then
                RemoveHandler HardwareButtons.CameraPressed, AddressOf HardwareButtons_CameraPressed
            End If

            If _orientationSensor IsNot Nothing Then
                RemoveHandler _orientationSensor.OrientationChanged, AddressOf OrientationSensor_OrientationChanged
            End If

            RemoveHandler _displayInformation.OrientationChanged, AddressOf DisplayInformation_OrientationChanged
            RemoveHandler _systemMediaControls.PropertyChanged, AddressOf SystemMediaControls_PropertyChanged
        End Sub

        ''' <summary>
        ''' Attempts to find and return a device mounted on the panel specified, and on failure to find one it will return the first device listed
        ''' </summary>
        ''' <param name="desiredPanel">The desired panel on which the returned device should be mounted, if available</param>
        ''' <returns></returns>
        Private Shared Async Function FindCameraDeviceByPanelAsync(desiredPanel As Windows.Devices.Enumeration.Panel) As Task(Of DeviceInformation)
            ' Get available devices for capturing pictures
            Dim allVideoDevices = Await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture)
            ' Get the desired camera by panel
            Dim desiredDevice As DeviceInformation = allVideoDevices.FirstOrDefault(Function(x) x.EnclosureLocation IsNot Nothing AndAlso x.EnclosureLocation.Panel = desiredPanel)
            Return If(desiredDevice, allVideoDevices.FirstOrDefault())
        End Function

        ''' <summary>
        ''' Applies the given orientation to a photo stream and saves it as a StorageFile
        ''' </summary>
        ''' <param name="stream">The photo stream</param>
        ''' <param name="file">The StorageFile in which the photo stream will be saved</param>
        ''' <param name="photoOrientation">The orientation metadata to apply to the photo</param>
        ''' <returns></returns>
        Private Shared Async Function ReencodeAndSavePhotoAsync(stream As IRandomAccessStream, file As StorageFile, photoOrientation As PhotoOrientation) As Task
            Using inputStream = stream
                Dim decoder = Await BitmapDecoder.CreateAsync(inputStream)
                Using outputStream = Await file.OpenAsync(FileAccessMode.ReadWrite)
                    Dim encoder = Await BitmapEncoder.CreateForTranscodingAsync(outputStream, decoder)
                    Dim properties = New BitmapPropertySet From {{"System.Photo.Orientation", New BitmapTypedValue(photoOrientation, PropertyType.UInt16)}}
                    Await encoder.BitmapProperties.SetPropertiesAsync(properties)
                    Await encoder.FlushAsync()
                End Using
            End Using
        End Function

#End Region

#Region " Rotation helpers "

        ''' <summary>
        ''' Calculates the current camera orientation from the device orientation by taking into account whether the camera is external or facing the user
        ''' </summary>
        ''' <returns>The camera orientation in space, with an inverted rotation in the case the camera is mounted on the device and is facing the user</returns>
        Private Function GetCameraOrientation() As SimpleOrientation
            If _externalCamera Then
                Return SimpleOrientation.NotRotated
            End If

            Dim result = _deviceOrientation
            If _displayInformation.NativeOrientation = DisplayOrientations.Portrait Then
                Select result
                    Case SimpleOrientation.Rotated90DegreesCounterclockwise
                        result = SimpleOrientation.NotRotated
                         Case SimpleOrientation.Rotated180DegreesCounterclockwise
                        result = SimpleOrientation.Rotated90DegreesCounterclockwise
                         Case SimpleOrientation.Rotated270DegreesCounterclockwise
                        result = SimpleOrientation.Rotated180DegreesCounterclockwise
                         Case SimpleOrientation.NotRotated
                        result = SimpleOrientation.Rotated270DegreesCounterclockwise
                         End Select
            End If

            If _mirroringPreview Then
                Select result
                    Case SimpleOrientation.Rotated90DegreesCounterclockwise
                        Return SimpleOrientation.Rotated270DegreesCounterclockwise
                    Case SimpleOrientation.Rotated270DegreesCounterclockwise
                        Return SimpleOrientation.Rotated90DegreesCounterclockwise
                End Select
            End If

            Return result
        End Function

        ''' <summary>
        ''' Converts the given orientation of the device in space to the corresponding rotation in degrees
        ''' </summary>
        ''' <param name="orientation">The orientation of the device in space</param>
        ''' <returns>An orientation in degrees</returns>
        Private Shared Function ConvertDeviceOrientationToDegrees(orientation As SimpleOrientation) As Integer
            Select orientation
                Case SimpleOrientation.Rotated90DegreesCounterclockwise
                    Return 90
                Case SimpleOrientation.Rotated180DegreesCounterclockwise
                    Return 180
                Case SimpleOrientation.Rotated270DegreesCounterclockwise
                    Return 270
                Case Else
                    Return 0
            End Select
        End Function

        ''' <summary>
        ''' Converts the given orientation of the app on the screen to the corresponding rotation in degrees
        ''' </summary>
        ''' <param name="orientation">The orientation of the app on the screen</param>
        ''' <returns>An orientation in degrees</returns>
        Private Shared Function ConvertDisplayOrientationToDegrees(orientation As DisplayOrientations) As Integer
            Select orientation
                Case DisplayOrientations.Portrait
                    Return 90
                Case DisplayOrientations.LandscapeFlipped
                    Return 180
                Case DisplayOrientations.PortraitFlipped
                    Return 270
                Case Else
                    Return 0
            End Select
        End Function

        ''' <summary>
        ''' Converts the given orientation of the device in space to the metadata that can be added to captured photos
        ''' </summary>
        ''' <param name="orientation">The orientation of the device in space</param>
        ''' <returns></returns>
        Private Shared Function ConvertOrientationToPhotoOrientation(orientation As SimpleOrientation) As PhotoOrientation
            Select orientation
                Case SimpleOrientation.Rotated90DegreesCounterclockwise
                    Return PhotoOrientation.Rotate90
                Case SimpleOrientation.Rotated180DegreesCounterclockwise
                    Return PhotoOrientation.Rotate180
                Case SimpleOrientation.Rotated270DegreesCounterclockwise
                    Return PhotoOrientation.Rotate270
                Case Else
                    Return PhotoOrientation.Normal
            End Select
        End Function

        ''' <summary>
        ''' Uses the current device orientation in space and page orientation on the screen to calculate the rotation
        ''' transformation to apply to the controls
        ''' </summary>
        Private Sub UpdateControlOrientation()
            Dim device As Integer = ConvertDeviceOrientationToDegrees(_deviceOrientation)
            Dim display As Integer = ConvertDisplayOrientationToDegrees(_displayOrientation)
            If _displayInformation.NativeOrientation = DisplayOrientations.Portrait Then
                device -= 90
            End If

            ' Combine both rotations and make sure that 0 <= result < 360
            Dim angle = (360 + display + device) Mod 360
            ' Rotate the buttons in the UI to match the rotation of the device
            Dim transform = New RotateTransform With {.Angle = angle}
            PhotoButton.RenderTransform = transform
            CycleModeButton.RenderTransform = transform
            HdrImpactBar.FlowDirection = If((angle = 180 OrElse angle = 270), FlowDirection.RightToLeft, FlowDirection.LeftToRight)
        End Sub
#End Region
    End Class
End Namespace
