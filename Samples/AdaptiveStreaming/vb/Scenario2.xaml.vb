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
    Public NotInheritable Partial Class Scenario2
        Inherits Page

        Private rootPage As MainPage

        Dim ams As AdaptiveMediaSource = Nothing

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
        End Sub

        Private Sub log(s As String)
            Dim text As TextBlock = New TextBlock()
            text.Text = s
            text.TextWrapping = TextWrapping.WrapWholeWords
            stkOutput.Children.Add(text)
        End Sub

        Private Sub btnPlay_Click(sender As Object, e As Windows.UI.Xaml.RoutedEventArgs)
            If String.IsNullOrEmpty(txtInputURL.Text) Then
                rootPage.NotifyUser("Specify a URI to play", NotifyType.ErrorMessage)
                Return
            End If

            InitializeAdaptiveMediaSource(New System.Uri(txtInputURL.Text), mePlayer)
        End Sub

        Async Private Sub InitializeAdaptiveMediaSource(uri As System.Uri, m As MediaElement)
            Dim result As AdaptiveMediaSourceCreationResult = Await AdaptiveMediaSource.CreateFromUriAsync(uri)
            If result.Status = AdaptiveMediaSourceCreationStatus.Success Then
                ams = result.MediaSource
                m.SetMediaStreamSource(ams)
                outputBitrates()
                txtDownloadBitrate.Text = ams.InitialBitrate.ToString()
                txtPlaybackBitrate.Text = ams.InitialBitrate.ToString()
                AddHandler ams.DownloadRequested, AddressOf DownloadRequested
                AddHandler ams.DownloadBitrateChanged, AddressOf DownloadBitrateChanged
                AddHandler ams.PlaybackBitrateChanged, AddressOf PlaybackBitrateChanged
            Else
                rootPage.NotifyUser("Error creating the AdaptiveMediaSource" & vbLf & vbTab & result.Status.ToString(), NotifyType.ErrorMessage)
            End If
        End Sub

        Private Sub DownloadRequested(sender As AdaptiveMediaSource, args As AdaptiveMediaSourceDownloadRequestedEventArgs)
            UpdateBitrateUI(txtMeasuredBandwidth, CType(ams.InboundBitsPerSecond, UInteger))
        End Sub

        Private Sub DownloadBitrateChanged(sender As AdaptiveMediaSource, args As AdaptiveMediaSourceDownloadBitrateChangedEventArgs)
            UpdateBitrateUI(txtDownloadBitrate, args.NewValue)
        End Sub

        Private Sub PlaybackBitrateChanged(sender As AdaptiveMediaSource, args As AdaptiveMediaSourcePlaybackBitrateChangedEventArgs)
            UpdateBitrateUI(txtPlaybackBitrate, args.NewValue)
        End Sub

        Private Async Sub UpdateBitrateUI(t As TextBlock, newBitrate As UInteger)
            Await Me.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, New DispatchedHandler(Sub()
                                                                                                  t.Text = newBitrate.ToString()
                                                                                              End Sub))
        End Sub

        Private Sub outputBitrates()
            If ams IsNot Nothing Then
                Dim bitrates As String = "Available bitrates: "
                For Each b In ams.AvailableBitrates
                    bitrates &= b & " "
                Next

                log(bitrates)
            Else
                rootPage.NotifyUser("Error: Adaptive Media Source is not initialized.", NotifyType.ErrorMessage)
            End If
        End Sub

        Private Sub btnSetInitialBitrate_Click(sender As Object, e As RoutedEventArgs)
            If ams IsNot Nothing Then
                Dim bitrate As UInteger
                If UInteger.TryParse(txtInitialBitrate.Text, bitrate) Then
                    Try
                        ams.InitialBitrate = bitrate
                        log("Initial bitrate set to " & bitrate)
                    Catch
                        log("Initial bitrate must match a value from the manifest")
                    End Try
                Else
                    log("Initial bitrate must be set to a positive integer")
                End If
            Else
                rootPage.NotifyUser("Error: Adaptive Media Source is not initialized.", NotifyType.ErrorMessage)
            End If
        End Sub

        Private Sub btnSetMinBitrate_Click(sender As Object, e As RoutedEventArgs)
            If ams IsNot Nothing Then
                Dim bitrate As UInteger
                If UInteger.TryParse(txtMinBitrate.Text, bitrate) Then
                    Dim minbitrate As UInteger = ams.AvailableBitrates(0)
                    For Each b In ams.AvailableBitrates
                        If b >= bitrate Then
                            minbitrate = b
                            Exit For
                        End If
                    Next

                    ams.DesiredMinBitrate = minbitrate
                    log("Min bitrate set to " & minbitrate)
                Else
                    log("Min bitrate must be a positive integer")
                End If
            Else
                rootPage.NotifyUser("Error: Adaptive Media Source is not initialized.", NotifyType.ErrorMessage)
            End If
        End Sub

        Private Sub btnSetMaxBitrate_Click(sender As Object, e As RoutedEventArgs)
            If ams IsNot Nothing Then
                Dim bitrate As UInteger
                If UInteger.TryParse(txtMaxBitrate.Text, bitrate) Then
                    Dim maxbitrate As UInteger = ams.AvailableBitrates(ams.AvailableBitrates.Count - 1)
                    For Each b In ams.AvailableBitrates
                        If b <= bitrate Then
                            maxbitrate = b
                        Else
                            Exit For
                        End If
                    Next

                    ams.DesiredMaxBitrate = maxbitrate
                    log("Max bitrate set to " & maxbitrate)
                Else
                    log("Max bitrate must be a positive integer")
                End If
            Else
                rootPage.NotifyUser("Error: Adaptive Media Source is not initialized.", NotifyType.ErrorMessage)
            End If
        End Sub

        Private Sub btnSetBandwidthMeasurementWindow_Click(sender As Object, e As RoutedEventArgs)
            If ams IsNot Nothing Then
                Dim windowSize As Integer
                If Integer.TryParse(txtBandwidthMeasurementWindow.Text, windowSize) Then
                    ams.InboundBitsPerSecondWindow = New TimeSpan(0, 0, windowSize)
                    log("Bandwidth measurement window size set to " & windowSize)
                Else
                    log("Bandwidth measurement window size must be set to an int")
                End If
            Else
                rootPage.NotifyUser("Error: Adaptive Media Source is not initialized.", NotifyType.ErrorMessage)
            End If
        End Sub
    End Class
End Namespace
