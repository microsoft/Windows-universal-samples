'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
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
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Core
Imports Windows.Foundation
Imports Windows.Media

Namespace Global.PlaybackManager

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class PlaybackControl

        Dim rootPage As MainPage = MainPage.Current

        Dim cw As CoreWindow = Window.Current.CoreWindow

        Dim IsPlaying As Boolean = False

        Dim systemMediaControls As SystemMediaTransportControls = Nothing

        Public Sub New()
            Me.InitializeComponent()
            systemMediaControls = SystemMediaTransportControls.GetForCurrentView()
            AddHandler systemMediaControls.PropertyChanged, AddressOf SystemMediaControls_PropertyChanged
            AddHandler systemMediaControls.ButtonPressed, AddressOf SystemMediaControls_ButtonPressed
            systemMediaControls.IsPlayEnabled = True
            systemMediaControls.IsPauseEnabled = True
            systemMediaControls.IsStopEnabled = True
            systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Closed
        End Sub

        Protected Overrides Sub Finalize()
            If systemMediaControls IsNot Nothing Then
                RemoveHandler systemMediaControls.PropertyChanged, AddressOf SystemMediaControls_PropertyChanged
                RemoveHandler systemMediaControls.ButtonPressed, AddressOf SystemMediaControls_ButtonPressed
            End If
        End Sub

        Async Public Sub Play()
            Await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                OutputMedia.Play()
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Playing
            End Sub)
        End Sub

        Async Public Sub Pause()
            Await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                OutputMedia.Pause()
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Paused
            End Sub)
        End Sub

        Async Public Sub [Stop]()
            Await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                OutputMedia.Stop()
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Stopped
            End Sub)
        End Sub

        Public Sub SetAudioCategory(category As AudioCategory)
            OutputMedia.AudioCategory = category
        End Sub

        Public Sub SetAudioDeviceType(devicetype As AudioDeviceType)
            OutputMedia.AudioDeviceType = devicetype
        End Sub

        Public Async Sub SelectFile()
            Dim picker As Windows.Storage.Pickers.FileOpenPicker = New Windows.Storage.Pickers.FileOpenPicker()
            picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.MusicLibrary
            picker.FileTypeFilter.Add(".mp3")
            picker.FileTypeFilter.Add(".mp4")
            picker.FileTypeFilter.Add(".m4a")
            picker.FileTypeFilter.Add(".wma")
            picker.FileTypeFilter.Add(".wav")
            Dim file As Windows.Storage.StorageFile = Await picker.PickSingleFileAsync()
            If file IsNot Nothing Then
                Dim stream = Await file.OpenAsync(Windows.Storage.FileAccessMode.Read)
                OutputMedia.AutoPlay = False
                Await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                    OutputMedia.SetSource(stream, file.ContentType)
                End Sub)
            End If
        End Sub

        Async Sub DisplayStatus(text As String)
            Await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                Status.Text &= text & vbLf
            End Sub)
        End Sub

        Function GetTimeStampedMessage(eventText As String) As String
            Return eventText & "   " & DateTime.Now.Hour.ToString() & ":" & DateTime.Now.Minute.ToString() & ":" & DateTime.Now.Second.ToString()
        End Function

        Function SoundLevelToString(level As SoundLevel) As String
            Dim LevelString As String
            Select level
                Case SoundLevel.Muted
                    LevelString = "Muted"
                     Case SoundLevel.Low
                    LevelString = "Low"
                     Case SoundLevel.Full
                    LevelString = "Full"
                     Case Else
                    LevelString = "Unknown"
                     End Select

            Return LevelString
        End Function

        Async Sub AppMuted()
            Await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                If OutputMedia.CurrentState <> MediaElementState.Paused Then
                    IsPlaying = True
                    Pause()
                Else
                    IsPlaying = False
                End If
            End Sub)
            DisplayStatus(GetTimeStampedMessage("Muted"))
        End Sub

        Async Sub AppUnmuted()
            Await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                If IsPlaying Then
                    Play()
                End If
            End Sub)
            DisplayStatus(GetTimeStampedMessage("Unmuted"))
        End Sub

        Sub SystemMediaControls_PropertyChanged(sender As SystemMediaTransportControls, e As SystemMediaTransportControlsPropertyChangedEventArgs)
            If e.Property = SystemMediaTransportControlsProperty.SoundLevel Then
                Dim soundLevelState = sender.SoundLevel
                DisplayStatus(GetTimeStampedMessage("App sound level is [" & SoundLevelToString(soundLevelState) & "]"))
                If soundLevelState = SoundLevel.Muted Then
                    AppMuted()
                Else
                    AppUnmuted()
                End If
            End If
        End Sub

        Sub SystemMediaControls_ButtonPressed(sender As SystemMediaTransportControls, e As SystemMediaTransportControlsButtonPressedEventArgs)
            Select e.Button
                Case SystemMediaTransportControlsButton.Play
                    Play()
                    DisplayStatus(GetTimeStampedMessage("Play Pressed"))
                     Case SystemMediaTransportControlsButton.Pause
                    Pause()
                    DisplayStatus(GetTimeStampedMessage("Pause Pressed"))
                     Case SystemMediaTransportControlsButton.Stop
                    [Stop]()
                    DisplayStatus(GetTimeStampedMessage("Stop Pressed"))
                     Case Else
                     End Select
        End Sub

        Private Sub Button_Play_Click(sender As Object, e As RoutedEventArgs)
            Play()
        End Sub

        Private Sub Button_Pause_Click(sender As Object, e As RoutedEventArgs)
            Pause()
        End Sub

        Private Sub Button_Stop_Click(sender As Object, e As RoutedEventArgs)
            [Stop]()
        End Sub
    End Class
End Namespace
