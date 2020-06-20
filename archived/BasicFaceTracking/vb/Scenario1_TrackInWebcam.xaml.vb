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
Imports System.Collections.Generic
Imports System.Threading
Imports System.Threading.Tasks
Imports Windows.Graphics.Imaging
Imports Windows.Media
Imports Windows.Media.Capture
Imports Windows.Media.FaceAnalysis
Imports Windows.Media.MediaProperties
Imports Windows.System.Threading
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Media.Imaging
Imports Windows.UI.Xaml.Navigation
Imports Windows.UI.Xaml.Shapes

Namespace Global.SDKTemplate

    ''' <summary>
    ''' Page for demonstrating FaceTracking.
    ''' </summary>
    Public NotInheritable Partial Class TrackFacesInWebcam
        Inherits Page

        Private ReadOnly lineBrush As SolidColorBrush = New SolidColorBrush(Windows.UI.Colors.Yellow)

        Private ReadOnly lineThickness As Double = 2.0

        Private ReadOnly fillBrush As SolidColorBrush = New SolidColorBrush(Windows.UI.Colors.Transparent)

        Private rootPage As MainPage

        Private currentState As ScenarioState

        Private mediaCapture As MediaCapture

        Private videoProperties As VideoEncodingProperties

        Private faceTracker As FaceTracker

        Private frameProcessingTimer As ThreadPoolTimer

        Private frameProcessingSemaphore As SemaphoreSlim = New SemaphoreSlim(1)

        Public Sub New()
            Me.InitializeComponent()
            Me.currentState = ScenarioState.Idle
            AddHandler App.Current.Suspending, AddressOf Me.OnSuspending
        End Sub

        Private Enum ScenarioState
            Idle
            Streaming
        End Enum

        ''' <summary>
        ''' Responds when we navigate to this page.
        ''' </summary>
        ''' <param name="e">Event data</param>
        Protected Overrides Async Sub OnNavigatedTo(e As NavigationEventArgs)
            Me.rootPage = MainPage.Current
            If Me.faceTracker Is Nothing Then
                Me.faceTracker = Await FaceTracker.CreateAsync()
            End If
        End Sub

        ''' <summary>
        ''' Responds to App Suspend event to stop/release MediaCapture object if it's running and return to Idle state.
        ''' </summary>
        ''' <param name="sender">The source of the Suspending event</param>
        ''' <param name="e">Event data</param>
        Private Sub OnSuspending(sender As Object, e As Windows.ApplicationModel.SuspendingEventArgs)
            If Me.currentState = ScenarioState.Streaming Then
                Dim deferral = e.SuspendingOperation.GetDeferral()
                Try
                    Me.ChangeScenarioState(ScenarioState.Idle)
                Finally
                    deferral.Complete()
                End Try
            End If
        End Sub

        ''' <summary>
        ''' Initializes a new MediaCapture instance and starts the Preview streaming to the CamPreview UI element.
        ''' </summary>
        ''' <returns>Async Task object returning true if initialization and streaming were successful and false if an exception occurred.</returns>
        Private Async Function StartWebcamStreaming() As Task(Of Boolean)
            Dim successful As Boolean = True
            Try
                Me.mediaCapture = New MediaCapture()
                ' For this scenario, we only need Video (not microphone) so specify this in the initializer.
                ' NOTE: the appxmanifest only declares "webcam" under capabilities and if this is changed to include
                ' microphone (default constructor) you must add "microphone" to the manifest or initialization will fail.
                Dim settings As MediaCaptureInitializationSettings = New MediaCaptureInitializationSettings()
                settings.StreamingCaptureMode = StreamingCaptureMode.Video
                Await Me.mediaCapture.InitializeAsync(settings)
                AddHandler Me.mediaCapture.CameraStreamStateChanged, AddressOf Me.MediaCapture_CameraStreamStateChanged
                ' Cache the media properties as we'll need them later.
                Dim deviceController = Me.mediaCapture.VideoDeviceController
                Me.videoProperties = TryCast(deviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview), VideoEncodingProperties)
                Me.CamPreview.Source = Me.mediaCapture
                Await Me.mediaCapture.StartPreviewAsync()
                ' Use a 66 millisecond interval for our timer, i.e. 15 frames per second
                Dim timerInterval As TimeSpan = TimeSpan.FromMilliseconds(66)
                Me.frameProcessingTimer = Windows.System.Threading.ThreadPoolTimer.CreatePeriodicTimer(New Windows.System.Threading.TimerElapsedHandler(AddressOf ProcessCurrentVideoFrame), timerInterval)
            Catch ex As System.UnauthorizedAccessException
                Me.rootPage.NotifyUser("Webcam is disabled or access to the webcam is disabled for this app." & vbLf & "Ensure Privacy Settings allow webcam usage.", NotifyType.ErrorMessage)
                successful = False
            Catch ex As Exception
                Me.rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage)
                successful = False
            End Try

            Return successful
        End Function

        ''' <summary>
        ''' Safely stops webcam streaming (if running) and releases MediaCapture object.
        ''' </summary>
        Private Async Sub ShutdownWebCam()
            If Me.frameProcessingTimer IsNot Nothing Then
                Me.frameProcessingTimer.Cancel()
            End If

            If Me.mediaCapture IsNot Nothing Then
                If Me.mediaCapture.CameraStreamState = Windows.Media.Devices.CameraStreamState.Streaming Then
                    Await Me.mediaCapture.StopPreviewAsync()
                End If

                Me.mediaCapture.Dispose()
            End If

            Me.frameProcessingTimer = Nothing
            Me.CamPreview.Source = Nothing
            Me.mediaCapture = Nothing
        End Sub

        ''' <summary>
        ''' This method is invoked by a ThreadPoolTimer to execute the FaceTracker and Visualization logic at approximately 15 frames per second.
        ''' </summary>
        ''' <remarks>
        ''' Keep in mind this method is called from a Timer and not synchronized with the camera stream. Also, the processing time of FaceTracker
        ''' will vary depending on the size of each frame and the number of faces being tracked. That is, a large image with several tracked faces may
        ''' take longer to process.
        ''' </remarks>
        ''' <param name="timer">Timer object invoking this call</param>
        Private Async Sub ProcessCurrentVideoFrame(timer As ThreadPoolTimer)
            If Me.currentState <> ScenarioState.Streaming Then
                Return
            End If

            If Not frameProcessingSemaphore.Wait(0) Then
                Return
            End If

            Try
                Dim faces As IList(Of DetectedFace) = Nothing
                ' Create a VideoFrame object specifying the pixel format we want our capture image to be (NV12 bitmap in this case).
                ' GetPreviewFrame will convert the native webcam frame into this format.
                Const InputPixelFormat As BitmapPixelFormat = BitmapPixelFormat.Nv12
                Using previewFrame As VideoFrame = New VideoFrame(InputPixelFormat, CType(Me.videoProperties.Width, Integer), CType(Me.videoProperties.Height, Integer))
                    Await Me.mediaCapture.GetPreviewFrameAsync(previewFrame)
                    If FaceDetector.IsBitmapPixelFormatSupported(previewFrame.SoftwareBitmap.BitmapPixelFormat) Then
                        faces = Await Me.faceTracker.ProcessNextFrameAsync(previewFrame)
                    Else
                        Throw New System.NotSupportedException("PixelFormat '" & InputPixelFormat.ToString() & "' is not supported by FaceDetector")
                    End If

                    ' Create our visualization using the frame dimensions and face results but run it on the UI thread.
                    Dim previewFrameSize = New Windows.Foundation.Size(previewFrame.SoftwareBitmap.PixelWidth, previewFrame.SoftwareBitmap.PixelHeight)
                    Dim ignored = Me.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                        Me.SetupVisualization(previewFrameSize, faces)
                    End Sub)
                End Using
            Catch ex As Exception
                Dim ignored = Me.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                    Me.rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage)
                End Sub)
            Finally
                frameProcessingSemaphore.Release()
            End Try
        End Sub

        ''' <summary>
        ''' Takes the webcam image and FaceTracker results and assembles the visualization onto the Canvas.
        ''' </summary>
        ''' <param name="framePizelSize">Width and height (in pixels) of the video capture frame</param>
        ''' <param name="foundFaces">List of detected faces; output from FaceTracker</param>
        Private Sub SetupVisualization(framePizelSize As Windows.Foundation.Size, foundFaces As IList(Of DetectedFace))
            Me.VisualizationCanvas.Children.Clear()
            If foundFaces IsNot Nothing Then
                Dim widthScale As Double = framePizelSize.Width / Me.VisualizationCanvas.ActualWidth
                Dim heightScale As Double = framePizelSize.Height / Me.VisualizationCanvas.ActualHeight
                For Each face In foundFaces
                    ' Create a rectangle element for displaying the face box but since we're using a Canvas
                    ' we must scale the rectangles according to the frames's actual size.
                    Dim box As Rectangle = New Rectangle()
                    box.Width = CType((face.FaceBox.Width / widthScale), UInteger)
                    box.Height = CType((face.FaceBox.Height / heightScale), UInteger)
                    box.Fill = Me.fillBrush
                    box.Stroke = Me.lineBrush
                    box.StrokeThickness = Me.lineThickness
                    box.Margin = New Thickness(CType((face.FaceBox.X / widthScale), UInteger), CType((face.FaceBox.Y / heightScale), UInteger), 0, 0)
                    Me.VisualizationCanvas.Children.Add(box)
                Next
            End If
        End Sub

        ''' <summary>
        ''' Manages the scenario's internal state. Invokes the internal methods and updates the UI according to the
        ''' passed in state value. Handles failures and resets the state if necessary.
        ''' </summary>
        ''' <param name="newState">State to switch to</param>
        Private Async Sub ChangeScenarioState(newState As ScenarioState)
            Select newState
                Case ScenarioState.Idle
                    Me.ShutdownWebCam()
                    Me.VisualizationCanvas.Children.Clear()
                    Me.CameraStreamingButton.Content = "Start Streaming"
                    Me.currentState = newState
                     Case ScenarioState.Streaming
                    If Not Await Me.StartWebcamStreaming() Then
                        Me.ChangeScenarioState(ScenarioState.Idle)
                        Exit Select
                    End If

                    Me.VisualizationCanvas.Children.Clear()
                    Me.CameraStreamingButton.Content = "Stop Streaming"
                    Me.currentState = newState
                     End Select
        End Sub

        ''' <summary>
        ''' Handles MediaCapture changes by shutting down streaming and returning to Idle state.
        ''' </summary>
        ''' <param name="sender">The source of the event, i.e. our MediaCapture object</param>
        ''' <param name="args">Event data</param>
        Private Sub MediaCapture_CameraStreamStateChanged(sender As MediaCapture, args As Object)
            ' MediaCapture is not Agile and so we cannot invoke its methods on this caller's thread
            ' and instead need to schedule the state change on the UI thread.
            Dim ignored = Me.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                ChangeScenarioState(ScenarioState.Idle)
            End Sub)
        End Sub

        ''' <summary>
        ''' Handles "streaming" button clicks to start/stop webcam streaming.
        ''' </summary>
        ''' <param name="sender">Button user clicked</param>
        ''' <param name="e">Event data</param>
        Private Sub CameraStreamingButton_Click(sender As Object, e As RoutedEventArgs)
            If Me.currentState = ScenarioState.Streaming Then
                Me.rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage)
                Me.ChangeScenarioState(ScenarioState.Idle)
            Else
                Me.rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage)
                Me.ChangeScenarioState(ScenarioState.Streaming)
            End If
        End Sub
    End Class
End Namespace
