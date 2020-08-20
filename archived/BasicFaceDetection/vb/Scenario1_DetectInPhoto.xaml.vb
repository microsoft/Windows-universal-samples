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
Imports Windows.Graphics.Imaging
Imports Windows.Media.FaceAnalysis
Imports Windows.Storage
Imports Windows.Storage.Pickers
Imports Windows.Storage.Streams
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Media.Imaging
Imports Windows.UI.Xaml.Navigation
Imports Windows.UI.Xaml.Shapes

Namespace Global.SDKTemplate

    ''' <summary>
    ''' Page for demonstrating FaceDetection on an image file.
    ''' </summary>
    Public NotInheritable Partial Class DetectFacesInPhoto
        Inherits Page

        Private ReadOnly lineBrush As SolidColorBrush = New SolidColorBrush(Windows.UI.Colors.Yellow)

        Private ReadOnly lineThickness As Double = 2.0

        Private ReadOnly fillBrush As SolidColorBrush = New SolidColorBrush(Windows.UI.Colors.Transparent)

        Private ReadOnly sourceImageHeightLimit As UInteger = 1280

        Private rootPage As MainPage

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Responds when we navigate to this page.
        ''' </summary>
        ''' <param name="e">Event data</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            Me.rootPage = MainPage.Current
        End Sub

        ''' <summary>
        ''' Takes the photo image and FaceDetector results and assembles the visualization onto the Canvas.
        ''' </summary>
        ''' <param name="displaySource">Bitmap object holding the image we're going to display</param>
        ''' <param name="foundFaces">List of detected faces; output from FaceDetector</param>
        Private Sub SetupVisualization(displaySource As WriteableBitmap, foundFaces As IList(Of DetectedFace))
            Dim brush As ImageBrush = New ImageBrush()
            brush.ImageSource = displaySource
            brush.Stretch = Stretch.Fill
            Me.PhotoCanvas.Background = brush
            If foundFaces IsNot Nothing Then
                Dim widthScale As Double = displaySource.PixelWidth / Me.PhotoCanvas.ActualWidth
                Dim heightScale As Double = displaySource.PixelHeight / Me.PhotoCanvas.ActualHeight
                For Each face In foundFaces
                    ' Create a rectangle element for displaying the face box but since we're using a Canvas
                    ' we must scale the rectangles according to the image’s actual size.
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
                    Me.PhotoCanvas.Children.Add(box)
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
        ''' Clears the display of image and face boxes.
        ''' </summary>
        Private Sub ClearVisualization()
            Me.PhotoCanvas.Background = Nothing
            Me.PhotoCanvas.Children.Clear()
            Me.rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' Computes a BitmapTransform to downscale the source image if it's too large. 
        ''' </summary>
        ''' <remarks>
        ''' Performance of the FaceDetector degrades significantly with large images, and in most cases it's best to downscale
        ''' the source bitmaps if they're too large before passing them into FaceDetector. Remember through, your application's performance needs will vary.
        ''' </remarks>
        ''' <param name="sourceDecoder">Source image decoder</param>
        ''' <returns>A BitmapTransform object holding scaling values if source image is too large</returns>
        Private Function ComputeScalingTransformForSourceImage(sourceDecoder As BitmapDecoder) As BitmapTransform
            Dim transform As BitmapTransform = New BitmapTransform()
            If sourceDecoder.PixelHeight > Me.sourceImageHeightLimit Then
                Dim scalingFactor As Single = CType(Me.sourceImageHeightLimit, Single) / CType(sourceDecoder.PixelHeight, Single)
                transform.ScaledWidth = CType(Math.Floor(sourceDecoder.PixelWidth * scalingFactor), UInteger)
                transform.ScaledHeight = CType(Math.Floor(sourceDecoder.PixelHeight * scalingFactor), UInteger)
            End If

            Return transform
        End Function

        ''' <summary>
        ''' Loads an image file (selected by the user) and runs the FaceDetector on the loaded bitmap. If successful calls SetupVisualization to display the results.
        ''' </summary>
        ''' <param name="sender">Button user clicked</param>
        ''' <param name="e">Event data</param>
        Private Async Sub OpenFile_Click(sender As Object, e As RoutedEventArgs)
            Dim faces As IList(Of DetectedFace) = Nothing
            Dim displaySource As WriteableBitmap = Nothing
            Try
                Dim photoPicker As FileOpenPicker = New FileOpenPicker()
                photoPicker.ViewMode = PickerViewMode.Thumbnail
                photoPicker.SuggestedStartLocation = PickerLocationId.PicturesLibrary
                photoPicker.FileTypeFilter.Add(".jpg")
                photoPicker.FileTypeFilter.Add(".jpeg")
                photoPicker.FileTypeFilter.Add(".png")
                photoPicker.FileTypeFilter.Add(".bmp")
                Dim photoFile As StorageFile = Await photoPicker.PickSingleFileAsync()
                If photoFile Is Nothing Then
                    Return
                End If

                Me.ClearVisualization()
                Me.rootPage.NotifyUser("Opening...", NotifyType.StatusMessage)
                Using fileStream As IRandomAccessStream = Await photoFile.OpenAsync(Windows.Storage.FileAccessMode.Read)
                    Dim decoder As BitmapDecoder = Await BitmapDecoder.CreateAsync(fileStream)
                    Dim transform As BitmapTransform = Me.ComputeScalingTransformForSourceImage(decoder)
                    Using originalBitmap As SoftwareBitmap = Await decoder.GetSoftwareBitmapAsync(decoder.BitmapPixelFormat, BitmapAlphaMode.Ignore, transform, ExifOrientationMode.IgnoreExifOrientation, ColorManagementMode.DoNotColorManage)
                        ' We need to convert the image into a format that's compatible with FaceDetector.
                        ' Gray8 should be a good type but verify it against FaceDetector’s supported formats.
                        Const InputPixelFormat As BitmapPixelFormat = BitmapPixelFormat.Gray8
                        If FaceDetector.IsBitmapPixelFormatSupported(InputPixelFormat) Then
                            Using detectorInput = SoftwareBitmap.Convert(originalBitmap, InputPixelFormat)
                                displaySource = New WriteableBitmap(originalBitmap.PixelWidth, originalBitmap.PixelHeight)
                                originalBitmap.CopyToBuffer(displaySource.PixelBuffer)
                                Me.rootPage.NotifyUser("Detecting...", NotifyType.StatusMessage)
                                ' Initialize our FaceDetector and execute it against our input image.
                                ' NOTE: FaceDetector initialization can take a long time, and in most cases
                                ' you should create a member variable and reuse the object.
                                ' However, for simplicity in this scenario we instantiate a new instance each time.
                                Dim detector As FaceDetector = Await FaceDetector.CreateAsync()
                                faces = Await detector.DetectFacesAsync(detectorInput)
                                Me.SetupVisualization(displaySource, faces)
                            End Using
                        Else
                            Me.rootPage.NotifyUser("PixelFormat '" & InputPixelFormat.ToString() & "' is not supported by FaceDetector", NotifyType.ErrorMessage)
                        End If
                    End Using
                End Using
            Catch ex As Exception
                Me.ClearVisualization()
                Me.rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage)
            End Try
        End Sub

        ''' <summary>
        ''' Updates any existing face bounding boxes in response to changes in the size of the Canvas.
        ''' </summary>
        ''' <param name="sender">Canvas object whose size has changed</param>
        ''' <param name="e">Event data</param>
        Private Sub PhotoCanvas_SizeChanged(sender As Object, e As SizeChangedEventArgs)
            Try
                If Me.PhotoCanvas.Background IsNot Nothing Then
                    Dim displaySource As WriteableBitmap = TryCast((TryCast(Me.PhotoCanvas.Background, ImageBrush)).ImageSource, WriteableBitmap)
                    Dim widthScale As Double = displaySource.PixelWidth / Me.PhotoCanvas.ActualWidth
                    Dim heightScale As Double = displaySource.PixelHeight / Me.PhotoCanvas.ActualHeight
                    For Each item In PhotoCanvas.Children
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
