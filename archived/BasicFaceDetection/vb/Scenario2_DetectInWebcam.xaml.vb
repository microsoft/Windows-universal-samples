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
Imports System.Threading.Tasks
Imports Windows.Graphics.Imaging
Imports Windows.Media
Imports Windows.Media.Capture
Imports Windows.Media.FaceAnalysis
Imports Windows.Media.MediaProperties
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Media.Imaging
Imports Windows.UI.Xaml.Navigation
Imports Windows.UI.Xaml.Shapes

Namespace Global.SDKTemplate

    ''' <summary>
    ''' Page for demonstrating FaceDetection on a webcam snapshot.
    ''' </summary>
    Public NotInheritable Partial Class DetectFacesInWebcam
        Inherits Page

        Private ReadOnly lineBrush As SolidColorBrush = New SolidColorBrush(Windows.UI.Colors.Yellow)

        Private ReadOnly lineThickness As Double = 2.0

        Private ReadOnly fillBrush As SolidColorBrush = New SolidColorBrush(Windows.UI.Colors.Transparent)

        Private rootPage As MainPage

        Private currentState As ScenarioState

        Private mediaCapture As MediaCapture

        Private videoProperties As VideoEncodingProperties

        Private faceDetector As FaceDetector

        Public Sub New()
            Me.InitializeComponent()
            Me.currentState = ScenarioState.Idle
            AddHandler App.Current.Suspending, AddressOf Me.OnSuspending
        End Sub

        Private Enum ScenarioState
            Idle
            Streaming
            Snapshot
        End Enum

        ''' <summary>
        ''' Responds when we navigate to this page.
        ''' </summary>
        ''' <param name="e">Event data</param>
        Protected Overrides Async Sub OnNavigatedTo(e As NavigationEventArgs)
            Me.rootPage = MainPage.Current
            If Me.faceDetector Is Nothing Then
                Me.faceDetector = Await FaceDetector.CreateAsync()
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
            If Me.mediaCapture IsNot Nothing Then
                If Me.mediaCapture.CameraStreamState = Windows.Media.Devices.CameraStreamState.Streaming Then
                    Await Me.mediaCapture.StopPreviewAsync()
                End If

                Me.mediaCapture.Dispose()
            End If

            Me.CamPreview.Source = Nothing
            Me.mediaCapture = Nothing
        End Sub

        ''' <summary>
        ''' Captures a single frame from the running webcam stream and executes the FaceDetector on the image. If successful calls SetupVisualization to display the results.
        ''' </summary>
        ''' <returns>Async Task object returning true if the capture was successful and false if an exception occurred.</returns>
        Private Async Function TakeSnapshotAndFindFaces() As Task(Of Boolean)
            Dim successful As Boolean = True
            Try
                If Me.currentState <> ScenarioState.Streaming Then
                    Return False
                End If

                Dim displaySource As WriteableBitmap = Nothing
                Dim faces As IList(Of DetectedFace) = Nothing
                ' Create a VideoFrame object specifying the pixel format we want our capture image to be (NV12 bitmap in this case).
                ' GetPreviewFrame will convert the native webcam frame into this format.
                Const InputPixelFormat As BitmapPixelFormat = BitmapPixelFormat.Nv12
                Using previewFrame As VideoFrame = New VideoFrame(InputPixelFormat, CType(Me.videoProperties.Width, Integer), CType(Me.videoProperties.Height, Integer))
                    Await Me.mediaCapture.GetPreviewFrameAsync(previewFrame)
                    If FaceDetector.IsBitmapPixelFormatSupported(previewFrame.SoftwareBitmap.BitmapPixelFormat) Then
                        faces = Await Me.faceDetector.DetectFacesAsync(previewFrame.SoftwareBitmap)
                    Else
                        Me.rootPage.NotifyUser("PixelFormat '" & InputPixelFormat.ToString() & "' is not supported by FaceDetector", NotifyType.ErrorMessage)
                    End If

                    Using convertedSource As SoftwareBitmap = SoftwareBitmap.Convert(previewFrame.SoftwareBitmap, BitmapPixelFormat.Bgra8)
                        displaySource = New WriteableBitmap(convertedSource.PixelWidth, convertedSource.PixelHeight)
                        convertedSource.CopyToBuffer(displaySource.PixelBuffer)
                    End Using

                    Me.SetupVisualization(displaySource, faces)
                End Using
            Catch ex As Exception
                Me.rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage)
                successful = False
            End Try

            Return successful
        End Function

        ''' <summary>
        ''' Takes the webcam image and FaceDetector results and assembles the visualization onto the Canvas.
        ''' </summary>
        ''' <param name="displaySource">Bitmap object holding the image we're going to display</param>
        ''' <param name="foundFaces">List of detected faces; output from FaceDetector</param>
        Private Sub SetupVisualization(displaySource As WriteableBitmap, foundFaces As IList(Of DetectedFace))
            Dim brush As ImageBrush = New ImageBrush()
            brush.ImageSource = displaySource
            brush.Stretch = Stretch.Fill
            Me.SnapshotCanvas.Background = brush
            If foundFaces IsNot Nothing Then
                Dim widthScale As Double = displaySource.PixelWidth / Me.SnapshotCanvas.ActualWidth
                Dim heightScale As Double = displaySource.PixelHeight / Me.SnapshotCanvas.ActualHeight
                For Each face In foundFaces
                    ' Create a rectangle element for displaying the face box but since we're using a Canvas
                    ' we must scale the rectangles according to the image's actual size.
                    ' The original FaceBox values are saved in the Rectangle's Tag field so we can update the
                    ' boxes when the Canvas is resized.
                    Dim box As Rectangle = New Rectangle()
                    box.Tag = face.FaceBox
                    box.Width = CType((face.FaceBox.Width / widthScale), UInteger)
                    box.Height = CType((face.FaceBox.Height / heightScale), UInteger)
                    box.Fill = Me.fillBrush
                    box.Stroke = Me.lineBrush
                    box.StrokeThickness = Me.lineThickness
                    box.Margin = New Thickness(CType((face.FaceBox.X / widthScale), UInteger), CType((face.FaceBox.Y / heightScale), UInteger), 0, 0)
                    Me.SnapshotCanvas.Children.Add(box)
                Next
            End If

            Dim message As String
            If foundFaces Is Nothing OrElse foundFaces.Count = 0 Then
                message = "Didn't find any human faces in the image"
            ElseIf foundFaces.Count = 1 Then
                message = "Found a human face in the image"
            Else
                message = "Found " & foundFaces.Count & " human faces in the image"
            End If

            Me.rootPage.NotifyUser(message, NotifyType.StatusMessage)
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
                    Me.SnapshotCanvas.Background = Nothing
                    Me.SnapshotCanvas.Children.Clear()
                    Me.CameraSnapshotButton.IsEnabled = False
                    Me.CameraStreamingButton.Content = "Start Streaming"
                    Me.CameraSnapshotButton.Content = "Take Snapshot"
                    Me.currentState = newState
                     Case ScenarioState.Streaming
                    If Not Await Me.StartWebcamStreaming() Then
                        Me.ChangeScenarioState(ScenarioState.Idle)
                        Exit Select
                    End If

                    Me.SnapshotCanvas.Background = Nothing
                    Me.SnapshotCanvas.Children.Clear()
                    Me.CameraSnapshotButton.IsEnabled = True
                    Me.CameraStreamingButton.Content = "Stop Streaming"
                    Me.CameraSnapshotButton.Content = "Take Snapshot"
                    Me.currentState = newState
                     Case ScenarioState.Snapshot
                    If Not Await Me.TakeSnapshotAndFindFaces() Then
                        Me.ChangeScenarioState(ScenarioState.Idle)
                        Exit Select
                    End If

                    Me.ShutdownWebCam()
                    Me.CameraSnapshotButton.IsEnabled = True
                    Me.CameraStreamingButton.Content = "Start Streaming"
                    Me.CameraSnapshotButton.Content = "Clear Display"
                    Me.currentState = newState
                     End Select
        End Sub

        ''' <summary>
        ''' Handles MediaCapture changes by shutting down streaming and returning to Idle state.
        ''' </summary>
        ''' <param name="sender">The source of the event, i.e. our MediaCapture object</param>
        ''' <param name="args">Event data</param>
        Private Sub MediaCapture_CameraStreamStateChanged(sender As MediaCapture, args As Object)
            ' MediaCapture is not Agile and so we cannot invoke it's methods on this caller's thread
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

        ''' <summary>
        ''' Handles "snapshot" button clicks to take a snapshot or clear the current display.
        ''' </summary>
        ''' <param name="sender">Button user clicked</param>
        ''' <param name="e">Event data</param>
        Private Sub CameraSnapshotButton_Click(sender As Object, e As RoutedEventArgs)
            If Me.currentState = ScenarioState.Streaming Then
                Me.rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage)
                Me.ChangeScenarioState(ScenarioState.Snapshot)
            Else
                Me.rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage)
                Me.ChangeScenarioState(ScenarioState.Idle)
            End If
        End Sub

        ''' <summary>
        ''' Updates any existing face bounding boxes in response to changes in the size of the Canvas.
        ''' </summary>
        ''' <param name="sender">Canvas whose size has changed</param>
        ''' <param name="e">Event data</param>
        Private Sub SnapshotCanvas_SizeChanged(sender As Object, e As SizeChangedEventArgs)
            Try
                If Me.currentState = ScenarioState.Snapshot AndAlso Me.SnapshotCanvas.Background IsNot Nothing Then
                    Dim displaySource As WriteableBitmap = TryCast((TryCast(Me.SnapshotCanvas.Background, ImageBrush)).ImageSource, WriteableBitmap)
                    Dim widthScale As Double = displaySource.PixelWidth / Me.SnapshotCanvas.ActualWidth
                    Dim heightScale As Double = displaySource.PixelHeight / Me.SnapshotCanvas.ActualHeight
                    For Each item In Me.SnapshotCanvas.Children
                        Dim box As Rectangle = TryCast(item, Rectangle)
                        If box Is Nothing Then
                            Continue For
                        End If

                        ' We saved the original size of the face box in the rectangles Tag field.
                        Dim faceBounds As BitmapBounds = CType(box.Tag, BitmapBounds)
                        box.Width = CType((faceBounds.Width / widthScale), UInteger)
                        box.Height = CType((faceBounds.Height / heightScale), UInteger)
                        box.Margin = New Thickness(CType((faceBounds.X / widthScale), UInteger), CType((faceBounds.Y / heightScale), UInteger), 0, 0)
                    Next
                End If
            Catch ex As Exception
                Me.rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage)
            End Try
        End Sub
    End Class
End Namespace
