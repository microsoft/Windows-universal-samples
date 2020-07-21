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
Imports System.Threading.Tasks
Imports Windows.ApplicationModel.DataTransfer
Imports Windows.Graphics.Imaging
Imports Windows.Storage
Imports Windows.Storage.Pickers
Imports Windows.Storage.Streams
Imports Windows.UI.Core
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Media.Imaging

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class CopyImage
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
            Me.Init()
        End Sub

        Sub Init()
            AddHandler CopyButton.Click, New RoutedEventHandler(AddressOf CopyButton_Click)
            AddHandler PasteButton.Click, New RoutedEventHandler(AddressOf PasteButton_Click)
            AddHandler CopyWithDelayedRenderingButton.Click, New RoutedEventHandler(AddressOf CopyWithDelayedRenderingButton_Click)
        End Sub

        Sub CopyButton_Click(sender As Object, e As RoutedEventArgs)
            Me.CopyBitmap(False)
        End Sub

        Sub CopyWithDelayedRenderingButton_Click(sender As Object, e As RoutedEventArgs)
            Me.CopyBitmap(True)
        End Sub

        Async Sub PasteButton_Click(sender As Object, e As RoutedEventArgs)
            rootPage.NotifyUser("", NotifyType.StatusMessage)
            ' Get the bitmap and display it.
            Dim dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.GetContent()
            If dataPackageView.Contains(StandardDataFormats.Bitmap) Then
                Dim imageReceived As IRandomAccessStreamReference = Nothing
                Try
                    imageReceived = Await dataPackageView.GetBitmapAsync()
                Catch ex As Exception
                    rootPage.NotifyUser("Error retrieving image from Clipboard: " & ex.Message, NotifyType.ErrorMessage)
                End Try

                If imageReceived IsNot Nothing Then
                    Using imageStream = Await imageReceived.OpenReadAsync()
                        Dim bitmapImage = New BitmapImage()
                        bitmapImage.SetSource(imageStream)
                        OutputImage.Source = bitmapImage
                        OutputImage.Visibility = Visibility.Visible
                        OutputText.Text = "Image is retrieved from the clipboard and pasted successfully."
                    End Using
                End If
            Else
                OutputText.Text = "Bitmap format is not available in clipboard"
                OutputImage.Visibility = Visibility.Collapsed
            End If
        End Sub

        Async Private Sub OnDeferredImageRequestedHandler(request As DataProviderRequest, imageFile As StorageFile)
            If imageFile IsNot Nothing Then
                ' Since this method is using "await" prior to setting the data in DataPackage,
                ' deferral object must be used
                Dim deferral = request.GetDeferral()
                Try
                    Using imageStream = Await imageFile.OpenAsync(FileAccessMode.Read)
                        ' Decode the image
                        Dim imageDecoder = Await BitmapDecoder.CreateAsync(imageStream)
                        ' Re-encode the image at 50% width and height
                        Dim inMemoryStream = New InMemoryRandomAccessStream()
                        Dim imageEncoder = Await BitmapEncoder.CreateForTranscodingAsync(inMemoryStream, imageDecoder)
                        imageEncoder.BitmapTransform.ScaledWidth = CType((imageDecoder.OrientedPixelWidth * 0.5), UInteger)
                        imageEncoder.BitmapTransform.ScaledHeight = CType((imageDecoder.OrientedPixelHeight * 0.5), UInteger)
                        Await imageEncoder.FlushAsync()
                        request.SetData(RandomAccessStreamReference.CreateFromStream(inMemoryStream))
                    End Using
                Finally
                    deferral.Complete()
                End Try

                Await log(OutputText, "Image has been set via deferral")
            Else
                Await log(OutputText, "Error: imageFile is null")
            End If
        End Sub

        Async Private Sub CopyBitmap(isDelayRendered As Boolean)
            Dim imagePicker = New FileOpenPicker With {.ViewMode = PickerViewMode.Thumbnail, .SuggestedStartLocation = PickerLocationId.PicturesLibrary}
            imagePicker.FileTypeFilter.Add(".jpg")
            imagePicker.FileTypeFilter.Add(".png")
            imagePicker.FileTypeFilter.Add(".bmp")
            imagePicker.FileTypeFilter.Add(".gif")
            imagePicker.FileTypeFilter.Add(".tif")
            Dim imageFile = Await imagePicker.PickSingleFileAsync()
            If imageFile IsNot Nothing Then
                Dim dataPackage = New DataPackage()
                If isDelayRendered Then
                    dataPackage.SetDataProvider(StandardDataFormats.Bitmap, Sub(request) OnDeferredImageRequestedHandler(request, imageFile))
                    OutputText.Text = "Image has been copied using delayed rendering"
                Else
                    dataPackage.SetBitmap(RandomAccessStreamReference.CreateFromFile(imageFile))
                    OutputText.Text = "Image has been copied"
                End If

                Try
                    Windows.ApplicationModel.DataTransfer.Clipboard.SetContent(dataPackage)
                Catch ex As Exception
                    rootPage.NotifyUser("Error copying content to Clipboard: " & ex.Message & ". Try again", NotifyType.ErrorMessage)
                End Try
            Else
                OutputText.Text = "No image was selected."
            End If
        End Sub

        Async Private Function log(textBlock As TextBlock, msg As String) As Task
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                textBlock.Text &= Environment.NewLine & msg
            End Sub)
        End Function
    End Class
End Namespace
