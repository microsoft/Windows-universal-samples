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
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports SDKTemplate
Imports System
Imports Windows.Media.Streaming.Adaptive
Imports Windows.UI.Core

Namespace Global.AdaptiveStreaming

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario3
        Inherits Page

        Private rootPage As MainPage

        Dim ams As AdaptiveMediaSource = Nothing

        Dim httpClient As Windows.Web.Http.HttpClient = Nothing

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
        End Sub

        Async Private Sub log(s As String)
            Await Me.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, New DispatchedHandler(Sub()
                                                                                                  Dim text As TextBlock = New TextBlock()
                                                                                                  text.Text = s
                                                                                                  text.TextWrapping = TextWrapping.WrapWholeWords
                                                                                                  stkOutput.Children.Add(text)
                                                                                              End Sub))
        End Sub

        Private Sub btnPlay_Click(sender As Object, e As Windows.UI.Xaml.RoutedEventArgs)
            If String.IsNullOrEmpty(txtInputURL.Text) Then
                rootPage.NotifyUser("Specify a URI to play", NotifyType.ErrorMessage)
                Return
            End If

            InitializeAdaptiveMediaSource(New System.Uri(txtInputURL.Text), mePlayer)
        End Sub

        Async Private Sub InitializeAdaptiveMediaSource(uri As System.Uri, m As MediaElement)
            httpClient = New Windows.Web.Http.HttpClient()
            httpClient.DefaultRequestHeaders.TryAppendWithoutValidation("X-CustomHeader", "This is a custom header")
            Dim result As AdaptiveMediaSourceCreationResult = Await AdaptiveMediaSource.CreateFromUriAsync(uri, httpClient)
            If result.Status = AdaptiveMediaSourceCreationStatus.Success Then
                ams = result.MediaSource
                m.SetMediaStreamSource(ams)
                AddHandler ams.DownloadRequested, AddressOf DownloadRequested
                AddHandler ams.DownloadCompleted, AddressOf DownloadCompleted
                AddHandler ams.DownloadFailed, AddressOf DownloadFailed
            Else
                rootPage.NotifyUser("Error creating the AdaptiveMediaSource" & vbLf & vbTab & result.Status.ToString(), NotifyType.ErrorMessage)
            End If
        End Sub

        Private Sub DownloadRequested(sender As AdaptiveMediaSource, args As AdaptiveMediaSourceDownloadRequestedEventArgs)
            If args.ResourceType = AdaptiveMediaSourceResourceType.Key Then
                Dim originalUri As String = args.ResourceUri.ToString()
                Dim secureUri As String = originalUri.Replace("http:", "https:")
                args.Result.ResourceUri = New Uri(secureUri)
            End If
        End Sub

        Private Sub DownloadCompleted(sender As AdaptiveMediaSource, args As AdaptiveMediaSourceDownloadCompletedEventArgs)
            log("Download Completed. Resource Type: " & args.ResourceType.ToString() & " Resource URI: " & args.ResourceUri.ToString())
        End Sub

        Private Sub DownloadFailed(sender As AdaptiveMediaSource, args As AdaptiveMediaSourceDownloadFailedEventArgs)
            log("Download Failured. Resource Type: " & args.ResourceType.ToString() & " Resource URI: " & args.ResourceUri.ToString() & vbLf & "HTTP Response:" & vbLf & args.HttpResponseMessage.ToString())
        End Sub
    End Class
End Namespace
