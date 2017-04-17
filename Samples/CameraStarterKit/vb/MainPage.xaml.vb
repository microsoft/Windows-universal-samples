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
Imports CameraStarterKit.CameraStarterKit
Imports Windows.ApplicationModel
Imports Windows.Devices.Enumeration
Imports Windows.Devices.Sensors
Imports Windows.Foundation
Imports Windows.Foundation.Metadata
Imports Windows.Graphics.Display
Imports Windows.Graphics.Imaging
Imports Windows.Media
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

Namespace Global.CameraStarterKit

    Partial Public NotInheritable Class MainPage
        Inherits Page

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

        Private _isRecording As Boolean

        ' Information about the camera device
        Private _mirroringPreview As Boolean

        Private _externalCamera As Boolean

        ' Rotation Helper to simplify handling rotation compensation for the camera streams
        Private _rotationHelper As CameraRotationHelper

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
                        _mirroringPreview = (cameraDevice.EnclosureLocation.Panel = Windows.Devices.Enumeration.Panel.Front)
                    End If


                    ' Initialize rotationHelper
                    _rotationHelper = New CameraRotationHelper(cameraDevice.EnclosureLocation)
                    AddHandler _rotationHelper.OrientationChanged, AddressOf RotationHelper_OrientationChanged

                    Await StartPreviewAsync()
                    UpdateCaptureControls()
                End If
            End If
        End Function

        ''' <summary>
        ''' Handles an orientation changed event
        ''' </summary>
        Private Async Sub RotationHelper_OrientationChanged(updatePreview As Boolean)
            If updatePreview Then
                Await SetPreviewRotationAsync()
            End If
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Function() UpdateButtonOrientation())
        End Sub

        ''' <summary>
        ''' Uses the current device orientation in space and page orientation on the screen to calculate the rotation
        ''' transformation to apply to the controls
        ''' </summary>
        Private Function UpdateButtonOrientation()
            ' Rotate the buttons in the UI to match the rotation of the device
            Dim angle = CameraRotationHelper.ConvertSimpleOrientationToClockwiseDegrees(_rotationHelper.GetUIOrientation())
            Dim transform = New RotateTransform()
            transform.Angle = angle

            ' The RenderTransform is safe to use (i.e. it won't cause layout issues) in this case, because these buttons have a 1:1 aspect ratio
            PhotoButton.RenderTransform = transform
            VideoButton.RenderTransform = transform
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

            ' Add rotation metadata to the preview stream to make sure the aspect ratio / dimensions match when rendering and getting preview frames
            Dim rotation = _rotationHelper.GetCameraPreviewOrientation()
            Dim props = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview)
            props.Properties.Add(RotationKey, CameraRotationHelper.ConvertSimpleOrientationToClockwiseDegrees(rotation))
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
                Dim photoOrientation = CameraRotationHelper.ConvertSimpleOrientationToPhotoOrientation(_rotationHelper.GetCameraCaptureOrientation())
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
                Dim rotationAngle = CameraRotationHelper.ConvertSimpleOrientationToClockwiseDegrees(_rotationHelper.GetCameraCaptureOrientation())
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

                If _isPreviewing Then
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
            If _rotationHelper IsNot Nothing Then
                RemoveHandler _rotationHelper.OrientationChanged, AddressOf RotationHelper_OrientationChanged
                _rotationHelper = Nothing
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
            PhotoButton.IsEnabled = _isPreviewing
            VideoButton.IsEnabled = _isPreviewing
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

            AddHandler _systemMediaControls.PropertyChanged, AddressOf SystemMediaControls_PropertyChanged
        End Sub

        ''' <summary>
        ''' Unregisters event handlers for hardware buttons and orientation sensors
        ''' </summary>
        Private Sub UnregisterEventHandlers()
            If ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons") Then
                RemoveHandler HardwareButtons.CameraPressed, AddressOf HardwareButtons_CameraPressed
            End If

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
    End Class
End Namespace
