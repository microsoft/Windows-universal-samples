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
Imports Windows.Media.Core
Imports Windows.Media.Capture
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
Imports Windows.UI.Xaml.Shapes
Imports Windows.Media.FaceAnalysis
Imports Windows.UI
Imports System.Collections.Generic

Namespace Global.FaceDetection

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

        Private _previewProperties As IMediaEncodingProperties

        Private _isInitialized As Boolean

        Private _isRecording As Boolean

        ' Information about the camera device
        Private _mirroringPreview As Boolean

        Private _externalCamera As Boolean

        Private _faceDetectionEffect As FaceDetectionEffect

#Region " Constructor, lifecycle and navigation "
        Public Sub New()
            Me.InitializeComponent()
            NavigationCacheMode = NavigationCacheMode.Disabled
            AddHandler Application.Current.Suspending, AddressOf Application_Suspending
            AddHandler Application.Current.Resuming, AddressOf Application_Resuming
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
                Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub() UpdateButtonOrientation())
            End If
        End Sub

        ''' <summary>
        ''' This event will fire when the page is rotated, when the DisplayInformation.AutoRotationPreferences value set in the SetupUiAsync() method cannot be not honored.
        ''' </summary>
        ''' <param name="sender">The event source.</param>
        ''' <param name="args">The event data.</param>
        Private Async Sub DisplayInformation_OrientationChanged(sender As DisplayInformation, args As Object)
            _displayOrientation = sender.CurrentOrientation
            If _previewProperties IsNot Nothing Then
                Await SetPreviewRotationAsync()
            End If

            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub() UpdateButtonOrientation())
        End Sub

        Private Async Sub PhotoButton_Click(sender As Object, e As RoutedEventArgs)
            Await TakePhotoAsync()
        End Sub

        Private Async Sub VideoButton_Click(sender As Object, e As RoutedEventArgs)
            If Not _isRecording Then
                Await StartRecordingAsync()
            Else
                Await StopRecordingAsync()
            End If

            UpdateCaptureControls()
        End Sub

        Private Async Sub FaceDetectionButton_Click(sender As Object, e As RoutedEventArgs)
            If _faceDetectionEffect Is Nothing OrElse Not _faceDetectionEffect.Enabled Then
                FacesCanvas.Children.Clear()
                Await CreateFaceDetectionEffectAsync()
            Else
                Await CleanUpFaceDetectionEffectAsync()
            End If

            UpdateCaptureControls()
        End Sub

        Private Async Sub HardwareButtons_CameraPressed(sender As Object, e As CameraEventArgs)
            Await TakePhotoAsync()
        End Sub

        Private Async Sub MediaCapture_RecordLimitationExceeded(sender As MediaCapture)
            Await StopRecordingAsync()
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub() UpdateCaptureControls())
        End Sub

        Private Async Sub MediaCapture_Failed(sender As MediaCapture, errorEventArgs As MediaCaptureFailedEventArgs)
            Debug.WriteLine("MediaCapture_Failed: (0x{0:X}) {1}", errorEventArgs.Code, errorEventArgs.Message)
            Await CleanupCameraAsync()
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub() UpdateCaptureControls())
        End Sub

        Private Async Sub FaceDetectionEffect_FaceDetected(sender As FaceDetectionEffect, args As FaceDetectedEventArgs)
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub() HighlightDetectedFaces(args.ResultFrame.DetectedFaces))
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
                ' Attempt to get the front camera if one is available, but use any camera device if not
                Dim cameraDevice = Await FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel.Front)
                If cameraDevice Is Nothing Then
                    Debug.WriteLine("No camera device found!")
                    Return
                End If

                _mediaCapture = New MediaCapture()
                AddHandler _mediaCapture.RecordLimitationExceeded, AddressOf MediaCapture_RecordLimitationExceeded
                AddHandler _mediaCapture.Failed, AddressOf MediaCapture_Failed
                Dim settings = New MediaCaptureInitializationSettings With {.VideoDeviceId = cameraDevice.Id}
                Try
                    Await _mediaCapture.InitializeAsync(settings)
                    _isInitialized = True
                Catch ex As UnauthorizedAccessException
                    Debug.WriteLine("The app was denied access to the camera")
                End Try

                If _isInitialized Then
                    If cameraDevice.EnclosureLocation Is Nothing OrElse cameraDevice.EnclosureLocation.Panel = Windows.Devices.Enumeration.Panel.Unknown Then
                        _externalCamera = True
                    Else
                        _externalCamera = False
                        _mirroringPreview =(cameraDevice.EnclosureLocation.Panel = Windows.Devices.Enumeration.Panel.Front)
                    End If

                    Await StartPreviewAsync()
                    UpdateCaptureControls()
                End If
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
            _previewProperties = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview)
            If _previewProperties IsNot Nothing Then
                _displayOrientation = _displayInformation.CurrentOrientation
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
            _previewProperties = Nothing
            Await _mediaCapture.StopPreviewAsync()
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                PreviewControl.Source = Nothing
                _displayRequest.RequestRelease()
            End Sub)
        End Function

        ''' <summary>
        ''' Adds face detection to the preview stream, registers for its events, enables it, and gets the FaceDetectionEffect instance
        ''' </summary>
        ''' <returns></returns>
        Private Async Function CreateFaceDetectionEffectAsync() As Task
            ' Create the definition, which will contain some initialization settings
            Dim definition = New FaceDetectionEffectDefinition()
            definition.SynchronousDetectionEnabled = False
            definition.DetectionMode = FaceDetectionMode.HighPerformance
            _faceDetectionEffect = CType(Await _mediaCapture.AddVideoEffectAsync(definition, MediaStreamType.VideoPreview), FaceDetectionEffect)
            AddHandler _faceDetectionEffect.FaceDetected, AddressOf FaceDetectionEffect_FaceDetected
            _faceDetectionEffect.DesiredDetectionInterval = TimeSpan.FromMilliseconds(33)
            _faceDetectionEffect.Enabled = True
        End Function

        ''' <summary>
        '''  Disables and removes the face detection effect, and unregisters the event handler for face detection
        ''' </summary>
        ''' <returns></returns>
        Private Async Function CleanUpFaceDetectionEffectAsync() As Task
            _faceDetectionEffect.Enabled = False
            RemoveHandler _faceDetectionEffect.FaceDetected, AddressOf FaceDetectionEffect_FaceDetected

            ' Remove the effect (see ClearEffectsAsync method to remove all effects from a stream)
            Await _mediaCapture.RemoveEffectAsync(_faceDetectionEffect)
            _faceDetectionEffect = Nothing
        End Function

        ''' <summary>
        ''' Takes a photo to a StorageFile and adds rotation metadata to it
        ''' </summary>
        ''' <returns></returns>
        Private Async Function TakePhotoAsync() As Task
            VideoButton.IsEnabled = _mediaCapture.MediaCaptureSettings.ConcurrentRecordAndPhotoSupported
            VideoButton.Opacity = If(VideoButton.IsEnabled, 1, 0)
            Dim stream = New InMemoryRandomAccessStream()
            Debug.WriteLine("Taking photo...")
            Await _mediaCapture.CapturePhotoToStreamAsync(ImageEncodingProperties.CreateJpeg(), stream)
            Try
                Dim file = Await _captureFolder.CreateFileAsync("SimplePhoto.jpg", CreationCollisionOption.GenerateUniqueName)
                Debug.WriteLine("Photo taken! Saving to " & file.Path)
                Dim photoOrientation = ConvertOrientationToPhotoOrientation(GetCameraOrientation())
                Await ReencodeAndSavePhotoAsync(stream, file, photoOrientation)
            Catch ex As Exception
                Debug.WriteLine("Exception when taking a photo: " & ex.ToString())
            End Try

            VideoButton.IsEnabled = True
            VideoButton.Opacity = 1
        End Function

        ''' <summary>
        ''' Records an MP4 video to a StorageFile and adds rotation metadata to it
        ''' </summary>
        ''' <returns></returns>
        Private Async Function StartRecordingAsync() As Task
            Try
                ' Create storage file for the capture
                Dim videoFile = Await _captureFolder.CreateFileAsync("SimpleVideo.mp4", CreationCollisionOption.GenerateUniqueName)
                Dim encodingProfile = MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Auto)
                ' Calculate rotation angle, taking mirroring into account if necessary
                Dim rotationAngle = 360 - ConvertDeviceOrientationToDegrees(GetCameraOrientation())
                encodingProfile.Video.Properties.Add(RotationKey, PropertyValue.CreateInt32(rotationAngle))
                Debug.WriteLine("Starting recording to " & videoFile.Path)
                Await _mediaCapture.StartRecordToStorageFileAsync(encodingProfile, videoFile)
                _isRecording = True
                Debug.WriteLine("Started recording!")
            Catch ex As Exception
                Debug.WriteLine("Exception when starting video recording: " & ex.ToString())
            End Try
        End Function

        ''' <summary>
        ''' Stops recording a video
        ''' </summary>
        ''' <returns></returns>
        Private Async Function StopRecordingAsync() As Task
            Debug.WriteLine("Stopping recording...")
            _isRecording = False
            Await _mediaCapture.StopRecordAsync()
            Debug.WriteLine("Stopped recording!")
        End Function

        ''' <summary>
        ''' Cleans up the camera resources (after stopping any video recording and/or preview if necessary) and unregisters from MediaCapture events
        ''' </summary>
        ''' <returns></returns>
        Private Async Function CleanupCameraAsync() As Task
            Debug.WriteLine("CleanupCameraAsync")
            If _isInitialized Then
                If _isRecording Then
                    Await StopRecordingAsync()
                End If

                If _faceDetectionEffect IsNot Nothing Then
                    Await CleanUpFaceDetectionEffectAsync()
                End If

                If _previewProperties IsNot Nothing Then
                    Await StopPreviewAsync()
                End If

                _isInitialized = False
            End If

            If _mediaCapture IsNot Nothing Then
                RemoveHandler _mediaCapture.RecordLimitationExceeded, AddressOf MediaCapture_RecordLimitationExceeded
                RemoveHandler _mediaCapture.Failed, AddressOf MediaCapture_Failed
                _mediaCapture.Dispose()
                _mediaCapture = Nothing
            End If
        End Function

#End Region

#Region " Helper functions "

        ''' <summary>
        ''' Attempts to lock the page orientation, hide the StatusBar (on Phone) and registers event handlers for hardware buttons and orientation sensors
        ''' </summary>
        ''' <returns></returns>
        Private Async Function SetupUiAsync() As Task
            DisplayInformation.AutoRotationPreferences = DisplayOrientations.Landscape
            If ApiInformation.IsTypePresent("Windows.UI.ViewManagement.StatusBar") Then
                Await Windows.UI.ViewManagement.StatusBar.GetForCurrentView().HideAsync()
            End If

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
        ''' This method will update the icons, enable/disable and show/hide the photo/video buttons depending on the current state of the app and the capabilities of the device
        ''' </summary>
        Private Sub UpdateCaptureControls()
            PhotoButton.IsEnabled = _previewProperties IsNot Nothing
            VideoButton.IsEnabled = _previewProperties IsNot Nothing
            FaceDetectionButton.IsEnabled = _previewProperties IsNot Nothing
            FaceDetectionDisabledIcon.Visibility = If((_faceDetectionEffect Is Nothing OrElse Not _faceDetectionEffect.Enabled), Visibility.Visible, Visibility.Collapsed)
            FaceDetectionEnabledIcon.Visibility = If((_faceDetectionEffect IsNot Nothing AndAlso _faceDetectionEffect.Enabled), Visibility.Visible, Visibility.Collapsed)
            FacesCanvas.Visibility = If((_faceDetectionEffect IsNot Nothing AndAlso _faceDetectionEffect.Enabled), Visibility.Visible, Visibility.Collapsed)
            StartRecordingIcon.Visibility = If(_isRecording, Visibility.Collapsed, Visibility.Visible)
            StopRecordingIcon.Visibility = If(_isRecording, Visibility.Visible, Visibility.Collapsed)
            If _isInitialized AndAlso Not _mediaCapture.MediaCaptureSettings.ConcurrentRecordAndPhotoSupported Then
                PhotoButton.IsEnabled = Not _isRecording
                PhotoButton.Opacity = If(PhotoButton.IsEnabled, 1, 0)
            End If
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
                UpdateButtonOrientation()
            End If

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
        ''' <returns>An angle in degrees to rotate the controls so they remain upright to the user regardless of device and page
        ''' orientation</returns>
        Private Sub UpdateButtonOrientation()
            Dim device As Integer = ConvertDeviceOrientationToDegrees(_deviceOrientation)
            Dim display As Integer = ConvertDisplayOrientationToDegrees(_displayOrientation)
            If _displayInformation.NativeOrientation = DisplayOrientations.Portrait Then
                device -= 90
            End If

            ' Combine both rotations and make sure that 0 <= result < 360
            Dim angle =(360 + display + device) Mod 360
            ' Rotate the buttons in the UI to match the rotation of the device
            Dim transform = New RotateTransform With {.Angle = angle}
            PhotoButton.RenderTransform = transform
            VideoButton.RenderTransform = transform
            FaceDetectionButton.RenderTransform = transform
        End Sub

        ''' <summary>
        ''' Uses the current display orientation to calculate the rotation transformation to apply to the face detection bounding box canvas
        ''' and mirrors it if the preview is being mirrored
        ''' </summary>
        Private Sub SetFacesCanvasRotation()
            ' Calculate how much to rotate the canvas
            Dim rotationDegrees As Integer = ConvertDisplayOrientationToDegrees(_displayOrientation)
            If _mirroringPreview Then
                rotationDegrees =(360 - rotationDegrees) Mod 360
            End If

            ' Apply the rotation
            Dim transform = New RotateTransform With {.Angle = rotationDegrees}
            FacesCanvas.RenderTransform = transform
            Dim previewArea = GetPreviewStreamRectInControl(TryCast(_previewProperties, VideoEncodingProperties), PreviewControl)
            If _displayOrientation = DisplayOrientations.Portrait OrElse _displayOrientation = DisplayOrientations.PortraitFlipped Then
                FacesCanvas.Width = previewArea.Height
                FacesCanvas.Height = previewArea.Width
                Canvas.SetLeft(FacesCanvas, previewArea.X - (previewArea.Height - previewArea.Width) / 2)
                Canvas.SetTop(FacesCanvas, previewArea.Y - (previewArea.Width - previewArea.Height) / 2)
            Else
                FacesCanvas.Width = previewArea.Width
                FacesCanvas.Height = previewArea.Height
                Canvas.SetLeft(FacesCanvas, previewArea.X)
                Canvas.SetTop(FacesCanvas, previewArea.Y)
            End If

            FacesCanvas.FlowDirection = If(_mirroringPreview, FlowDirection.RightToLeft, FlowDirection.LeftToRight)
        End Sub

#End Region

#Region " Face detection helpers "

        ''' <summary>
        ''' Iterates over all detected faces, creating and adding Rectangles to the FacesCanvas as face bounding boxes
        ''' </summary>
        ''' <param name="faces">The list of detected faces from the FaceDetected event of the effect</param>
        Private Sub HighlightDetectedFaces(faces As IReadOnlyList(Of DetectedFace))
            FacesCanvas.Children.Clear()
            For i = 0 To faces.Count - 1
                ' Face coordinate units are preview resolution pixels, which can be a different scale from our display resolution, so a conversion may be necessary
                Dim faceBoundingBox As Rectangle = ConvertPreviewToUiRectangle(faces(i).FaceBox)
                faceBoundingBox.StrokeThickness = 2
                faceBoundingBox.Stroke =(If(i = 0, New SolidColorBrush(Colors.Blue), New SolidColorBrush(Colors.DeepSkyBlue)))
                FacesCanvas.Children.Add(faceBoundingBox)
            Next

            SetFacesCanvasRotation()
        End Sub

        ''' <summary>
        ''' Takes face information defined in preview coordinates and returns one in UI coordinates, taking
        ''' into account the position and size of the preview control.
        ''' </summary>
        ''' <param name="faceBoxInPreviewCoordinates">Face coordinates as retried from the FaceBox property of a DetectedFace, in preview coordinates.</param>
        ''' <returns>Rectangle in UI (CaptureElement) coordinates, to be used in a Canvas control.</returns>
        Private Function ConvertPreviewToUiRectangle(faceBoxInPreviewCoordinates As BitmapBounds) As Rectangle
            Dim result = New Rectangle()
            Dim previewStream = TryCast(_previewProperties, VideoEncodingProperties)
            If previewStream Is Nothing Then
                Return result
            End If

            If previewStream.Width = 0 OrElse previewStream.Height = 0 Then
                Return result
            End If

            Dim streamWidth As Double = previewStream.Width
            Dim streamHeight As Double = previewStream.Height
            If _displayOrientation = DisplayOrientations.Portrait OrElse _displayOrientation = DisplayOrientations.PortraitFlipped Then
                streamHeight = previewStream.Width
                streamWidth = previewStream.Height
            End If

            ' Get the rectangle that is occupied by the actual video feed
            Dim previewInUI = GetPreviewStreamRectInControl(previewStream, PreviewControl)
            result.Width =(faceBoxInPreviewCoordinates.Width / streamWidth) * previewInUI.Width
            result.Height =(faceBoxInPreviewCoordinates.Height / streamHeight) * previewInUI.Height
            ' Scale the X and Y coordinates from preview stream coordinates to window coordinates
            Dim x =(faceBoxInPreviewCoordinates.X / streamWidth) * previewInUI.Width
            Dim y =(faceBoxInPreviewCoordinates.Y / streamHeight) * previewInUI.Height
            Canvas.SetLeft(result, x)
            Canvas.SetTop(result, y)
            Return result
        End Function

        ''' <summary>
        ''' Calculates the size and location of the rectangle that contains the preview stream within the preview control, when the scaling mode is Uniform
        ''' </summary>
        ''' <param name="previewResolution">The resolution at which the preview is running</param>
        ''' <param name="previewControl">The control that is displaying the preview using Uniform as the scaling mode</param>
        ''' <returns></returns>
        Public Function GetPreviewStreamRectInControl(previewResolution As VideoEncodingProperties, previewControl As CaptureElement) As Rect
            Dim result = New Rect()
            If previewControl Is Nothing OrElse previewControl.ActualHeight < 1 OrElse previewControl.ActualWidth < 1 OrElse previewResolution Is Nothing OrElse previewResolution.Height = 0 OrElse previewResolution.Width = 0 Then
                Return result
            End If

            Dim streamWidth = previewResolution.Width
            Dim streamHeight = previewResolution.Height
            If _displayOrientation = DisplayOrientations.Portrait OrElse _displayOrientation = DisplayOrientations.PortraitFlipped Then
                streamWidth = previewResolution.Height
                streamHeight = previewResolution.Width
            End If

            result.Width = previewControl.ActualWidth
            result.Height = previewControl.ActualHeight
            If (previewControl.ActualWidth / previewControl.ActualHeight > streamWidth / CType(streamHeight, Double)) Then
                Dim scale = previewControl.ActualHeight / streamHeight
                Dim scaledWidth = streamWidth * scale
                result.X = (previewControl.ActualWidth - scaledWidth) / 2.0
                result.Width = scaledWidth
            Else
                Dim scale = previewControl.ActualWidth / streamWidth
                Dim scaledHeight = streamHeight * scale
                result.Y = (previewControl.ActualHeight - scaledHeight) / 2.0
                result.Height = scaledHeight
            End If

            Return result
        End Function
#End Region
    End Class
End Namespace
