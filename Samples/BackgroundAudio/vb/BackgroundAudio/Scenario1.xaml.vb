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
Imports BackgroundAudioShared
Imports BackgroundAudioShared.Messages
Imports SDKTemplate
Imports System
Imports System.Collections.Generic
Imports System.Diagnostics
Imports System.Linq
Imports System.Threading
Imports Windows.Foundation
Imports Windows.Foundation.Collections
Imports Windows.Media.Playback
Imports Windows.UI.Core
Imports Windows.UI.Popups
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Media.Imaging
Imports Windows.UI.Xaml.Navigation

Namespace Global.BackgroundAudio

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario1
        Inherits Page

#Region " Private Fields and Properties "
        Private rootPage As MainPage

        Private backgroundAudioTaskStarted As AutoResetEvent

        Private _isMyBackgroundTaskRunning As Boolean = False

        Private albumArtCache As Dictionary(Of String, BitmapImage) = New Dictionary(Of String, BitmapImage)()

        Private Const RPC_S_SERVER_UNAVAILABLE As Integer = -2147023174 ' &H800706BA

        Private ReadOnly Property IsMyBackgroundTaskRunning As Boolean
            Get
                If _isMyBackgroundTaskRunning Then
                    Return True
                End If

                Dim value As String = TryCast(ApplicationSettingsHelper.ReadResetSettingsValue(ApplicationSettingsConstants.BackgroundTaskState), String)
                If value Is Nothing Then
                    Return False
                Else
                    Try
                        _isMyBackgroundTaskRunning = EnumHelper.Parse(Of BackgroundTaskState)(value) = BackgroundTaskState.Running
                    Catch ex As ArgumentException
                        _isMyBackgroundTaskRunning = False
                    End Try

                    Return _isMyBackgroundTaskRunning
                End If
            End Get
        End Property
#End Region

        ''' <summary>
        ''' You should never cache the MediaPlayer And always call Current. It Is possible
        ''' for the background task to go away for several different reasons. When it does
        ''' an RPC_S_SERVER_UNAVAILABLE error Is thrown. We need to reset the foreground state
        ''' And restart the background task.
        ''' </summary>
        Private ReadOnly Property CurrentPlayer As MediaPlayer
            Get
                Dim mp As MediaPlayer = Nothing

                Try
                    mp = BackgroundMediaPlayer.Current
                Catch ex As Exception
                    If ex.HResult = RPC_S_SERVER_UNAVAILABLE Then
                        ' The Foreground app uses RPC to communicate with the background process.
                        ' If the background process crashes Or Is killed for any reason RPC_S_SERVER_UNAVAILABLE
                        ' Is returned when calling Current.
                        ResetAfterLostBackground()
                        StartBackgroundAudioTask()
                    End If
                End Try

                If mp Is Nothing Then
                    Throw New Exception("Failed to get a MediaPlayer instance.")
                End If

                Return mp
            End Get
        End Property

        ''' <summary>
        ''' The background task did exist, but it has disappeared. Put the foreground back into an initial state.
        ''' </summary>
        Private Sub ResetAfterLostBackground()
            BackgroundMediaPlayer.Shutdown()
            _isMyBackgroundTaskRunning = False
            backgroundAudioTaskStarted.Reset()
            prevButton.IsEnabled = True
            nextButton.IsEnabled = True
            ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.BackgroundTaskState, BackgroundTaskState.Unknown.ToString())
            playButton.Content = "| |"

            Try
                AddHandler BackgroundMediaPlayer.MessageReceivedFromBackground, AddressOf BackgroundMediaPlayer_MessageReceivedFromBackground
            Catch ex As Exception
                If ex.HResult = RPC_S_SERVER_UNAVAILABLE Then
                    Throw New Exception("Failed to get a MediaPlayer instance.")
                End If
            End Try
        End Sub

        Public Sub New()
            Me.InitializeComponent()
            Me.NavigationCacheMode = NavigationCacheMode.Required
            InitializeSongs()
            backgroundAudioTaskStarted = New AutoResetEvent(False)
        End Sub

        Sub InitializeSongs()
            ' Album art attribution
            ' Ring01.jpg      | Autumn Yellow Leaves           | George Hodan
            ' Ring02.jpg      | Abstract Background            | Larisa Koshkina
            ' Ring03Part1.jpg | Snow Covered Mountains         | Petr Kratochvil
            ' Ring03Part2.jpg | Tropical Beach With Palm Trees | Petr Kratochvil
            ' Ring03Part3.jpg | Alyssum Background             | Anne Lowe
            ' Initialize the playlist data/view model.
            ' In a production app your data would be sourced from a data store or service.
            ' Add complete tracks
            Dim song1 = New SongModel()
            song1.Title = "Ring 1"
            song1.MediaUri = New Uri("ms-appx:///Assets/Media/Ring01.wma")
            song1.AlbumArtUri = New Uri("ms-appx:///Assets/Media/Ring01.jpg")
            playlistView.Songs.Add(song1)
            Dim song2 = New SongModel()
            song2.Title = "Ring 2"
            song2.MediaUri = New Uri("ms-appx:///Assets/Media/Ring02.wma")
            song2.AlbumArtUri = New Uri("ms-appx:///Assets/Media/Ring02.jpg")
            playlistView.Songs.Add(song2)
            For i = 1 To 3
                Dim segment = New SongModel()
                segment.Title = "Ring 3 Part " & i
                segment.MediaUri = New Uri("ms-appx:///Assets/Media/Ring03Part" & i & ".wma")
                segment.AlbumArtUri = New Uri("ms-appx:///Assets/Media/Ring03Part" & i & ".jpg")
                playlistView.Songs.Add(segment)
            Next

            For Each song In playlistView.Songs
                Dim bitmap = New BitmapImage()
                bitmap.UriSource = song.AlbumArtUri
                albumArtCache(song.AlbumArtUri.ToString()) = bitmap
            Next
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.
        ''' This parameter is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
            AddHandler playlistView.ItemClick, AddressOf PlaylistView_ItemClick
            AddHandler Application.Current.Suspending, AddressOf ForegroundApp_Suspending
            AddHandler Application.Current.Resuming, AddressOf ForegroundApp_Resuming
            ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.AppState, AppState.Active.ToString())
        End Sub

        Protected Overrides Sub OnNavigatedFrom(e As NavigationEventArgs)
            If isMyBackgroundTaskRunning Then
                RemoveMediaPlayerEventHandlers()
                ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.BackgroundTaskState, BackgroundTaskState.Running.ToString())
            End If

            MyBase.OnNavigatedFrom(e)
        End Sub

#Region " Foreground App Lifecycle Handlers "

        ''' <summary>
        ''' Read persisted current track information from application settings
        ''' </summary>
        Private Function GetCurrentTrackIdAfterAppResume() As Uri
            Dim value As Object = ApplicationSettingsHelper.ReadResetSettingsValue(ApplicationSettingsConstants.TrackId)
            If value IsNot Nothing Then
                Return New Uri(CType(value, String))
            Else
                Return Nothing
            End If
        End Function

        ''' <summary>
        ''' Sends message to background informing app has resumed
        ''' Subscribe to MediaPlayer events
        ''' </summary>
        Sub ForegroundApp_Resuming(sender As Object, e As Object)
            ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.AppState, AppState.Active.ToString())
            If IsMyBackgroundTaskRunning Then
                AddMediaPlayerEventHandlers()
                MessageService.SendMessageToBackground(New AppResumedMessage())
                UpdateTransportControls(CurrentPlayer.CurrentState)
                Dim trackId = GetCurrentTrackIdAfterAppResume()
                txtCurrentTrack.Text = If(trackId Is Nothing, String.Empty, playlistView.GetSongById(trackId).Title)
                txtCurrentState.Text = CurrentPlayer.CurrentState.ToString()
            Else
                playButton.Content = ">"
                txtCurrentTrack.Text = String.Empty
                txtCurrentState.Text = "Background Task Not Running"
            End If
        End Sub

        ''' <summary>
        ''' Send message to Background process that app is to be suspended
        ''' Stop clock and slider when suspending
        ''' Unsubscribe handlers for MediaPlayer events
        ''' </summary>
        Sub ForegroundApp_Suspending(sender As Object, e As Windows.ApplicationModel.SuspendingEventArgs)
            Dim deferral = e.SuspendingOperation.GetDeferral()
            If IsMyBackgroundTaskRunning Then
                RemoveMediaPlayerEventHandlers()
                MessageService.SendMessageToBackground(New AppSuspendedMessage())
            End If

            ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.AppState, AppState.Suspended.ToString())
            deferral.Complete()
        End Sub

#End Region

#Region " Background MediaPlayer Event handlers "

        ''' <summary>
        ''' MediaPlayer state changed event handlers. 
        ''' Note that we can subscribe to events even if Media Player is playing media in background
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="args"></param>
        Async Sub MediaPlayer_CurrentStateChanged(sender As MediaPlayer, args As Object)
            Dim currentState = sender.CurrentState
            Await Me.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                txtCurrentState.Text = currentState.ToString()
                UpdateTransportControls(currentState)
            End Sub)
        End Sub

        ''' <summary>
        ''' This event is raised when a message is recieved from BackgroundAudioTask
        ''' </summary>
        Async Sub BackgroundMediaPlayer_MessageReceivedFromBackground(sender As Object, e As MediaPlayerDataReceivedEventArgs)
            Dim trackChangedMessage As TrackChangedMessage = Nothing
            If MessageService.TryParseMessage(e.Data, trackChangedMessage) Then
                Await Me.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                    If trackChangedMessage.TrackId Is Nothing Then
                        playlistView.SelectedIndex = -1
                        albumArt.Source = Nothing
                        txtCurrentTrack.Text = String.Empty
                        prevButton.IsEnabled = False
                        nextButton.IsEnabled = False
                        Return
                    End If

                    Dim songIndex = playlistView.GetSongIndexById(trackChangedMessage.TrackId)
                    Dim song = playlistView.Songs(songIndex)
                    playlistView.SelectedIndex = songIndex
                    albumArt.Source = albumArtCache(song.AlbumArtUri.ToString())
                    txtCurrentTrack.Text = song.Title
                    prevButton.IsEnabled = True
                    nextButton.IsEnabled = True
                End Sub)
                Return
            End If

            Dim backgroundAudioTaskStartedMessage As BackgroundAudioTaskStartedMessage = Nothing
            If MessageService.TryParseMessage(e.Data, backgroundAudioTaskStartedMessage) Then
                Debug.WriteLine("BackgroundAudioTask started")
                backgroundAudioTaskStarted.Set()
                Return
            End If
        End Sub

#End Region

#Region " Button and Control Click Event Handlers "
        Private Sub PlaylistView_ItemClick(sender As Object, e As ItemClickEventArgs)
            Dim song = TryCast(e.ClickedItem, SongModel)
            Debug.WriteLine("Clicked item from App: " & song.MediaUri.ToString())
            If Not IsMyBackgroundTaskRunning OrElse MediaPlayerState.Closed = CurrentPlayer.CurrentState Then
                ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.TrackId, song.MediaUri.ToString())
                ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.Position, New TimeSpan().ToString())
                StartBackgroundAudioTask()
            Else
                MessageService.SendMessageToBackground(New TrackChangedMessage(song.MediaUri))
            End If

            If MediaPlayerState.Paused = CurrentPlayer.CurrentState Then
                CurrentPlayer.Play()
            End If
        End Sub

        ''' <summary>
        ''' Sends message to the background task to skip to the previous track.
        ''' </summary>
        Private Sub prevButton_Click(sender As Object, e As RoutedEventArgs)
            MessageService.SendMessageToBackground(New SkipPreviousMessage())
            prevButton.IsEnabled = False
        End Sub

        ''' <summary>
        ''' If the task is already running, it will just play/pause MediaPlayer Instance
        ''' Otherwise, initializes MediaPlayer Handlers and starts playback
        ''' track or to pause if we're already playing.
        ''' </summary>
        Private Sub playButton_Click(sender As Object, e As RoutedEventArgs)
            Debug.WriteLine("Play button pressed from App")
            If IsMyBackgroundTaskRunning Then
                If MediaPlayerState.Playing = CurrentPlayer.CurrentState Then
                    CurrentPlayer.Pause()
                ElseIf MediaPlayerState.Paused = CurrentPlayer.CurrentState Then
                    CurrentPlayer.Play()
                ElseIf MediaPlayerState.Closed = CurrentPlayer.CurrentState Then
                    StartBackgroundAudioTask()
                End If
            Else
                StartBackgroundAudioTask()
            End If
        End Sub

        ''' <summary>
        ''' Tells the background audio agent to skip to the next track.
        ''' </summary>
        ''' <param name="sender">The button</param>
        ''' <param name="e">Click event args</param>
        Private Sub nextButton_Click(sender As Object, e As RoutedEventArgs)
            MessageService.SendMessageToBackground(New SkipNextMessage())
            nextButton.IsEnabled = False
        End Sub

        Private Sub speedButton_Click(sender As Object, e As RoutedEventArgs)
            ' Create menu and add commands
            Dim popupMenu = New PopupMenu()
            popupMenu.Commands.Add(New UICommand("4.0x", Sub(command) CurrentPlayer.PlaybackRate = 4.0))
            popupMenu.Commands.Add(New UICommand("2.0x", Sub(command) CurrentPlayer.PlaybackRate = 2.0))
            popupMenu.Commands.Add(New UICommand("1.5x", Sub(command) CurrentPlayer.PlaybackRate = 1.5))
            popupMenu.Commands.Add(New UICommand("1.0x", Sub(command) CurrentPlayer.PlaybackRate = 1.0))
            popupMenu.Commands.Add(New UICommand("0.5x", Sub(command) CurrentPlayer.PlaybackRate = 0.5))
            ' Get button transform and then offset it by half the button
            ' width to center. This will show the popup just above the button.
            Dim button = CType(sender, Button)
            Dim transform = button.TransformToVisual(Nothing)
            Dim point = transform.TransformPoint(New Point(button.Width / 2, 0))
            ' Show popup
            Dim ignoreAsyncResult = popupMenu.ShowAsync(point)
        End Sub

#End Region

#Region " Media Playback Helper methods "
        Private Sub UpdateTransportControls(state As MediaPlayerState)
            If state = MediaPlayerState.Playing Then
                playButton.Content = "| |"
            Else
                playButton.Content = ">"
            End If
        End Sub

        ''' <summary>
        ''' Unsubscribes to MediaPlayer events. Should run only on suspend
        ''' </summary>
        Private Sub RemoveMediaPlayerEventHandlers()
            RemoveHandler CurrentPlayer.CurrentStateChanged, AddressOf Me.MediaPlayer_CurrentStateChanged
            Try
                RemoveHandler BackgroundMediaPlayer.MessageReceivedFromBackground, AddressOf Me.BackgroundMediaPlayer_MessageReceivedFromBackground
            Catch ex As Exception
                If ex.HResult = RPC_S_SERVER_UNAVAILABLE Then
                    ' Do nothing
                Else
                    Throw
                End If
            End Try
        End Sub

        ''' <summary>
        ''' Subscribes to MediaPlayer events
        ''' </summary>
        Private Sub AddMediaPlayerEventHandlers()
            AddHandler CurrentPlayer.CurrentStateChanged, AddressOf Me.MediaPlayer_CurrentStateChanged
            Try
                AddHandler BackgroundMediaPlayer.MessageReceivedFromBackground, AddressOf Me.BackgroundMediaPlayer_MessageReceivedFromBackground
            Catch ex As Exception
                If ex.HResult = RPC_S_SERVER_UNAVAILABLE Then
                    ' Internally MessageReceivedFromBackground calls Current which can throw RPC_S_SERVER_UNAVAILABLE
                    ResetAfterLostBackground()
                End If
            End Try
        End Sub

        ''' <summary>
        ''' Initialize Background Media Player Handlers and starts playback
        ''' </summary>
        Private Sub StartBackgroundAudioTask()
            AddMediaPlayerEventHandlers()
            Dim startResult = Me.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                Dim result As Boolean = backgroundAudioTaskStarted.WaitOne(10000)
                If result = True Then
                    MessageService.SendMessageToBackground(New UpdatePlaylistMessage(playlistView.Songs.ToList()))
                    MessageService.SendMessageToBackground(New StartPlaybackMessage())
                Else
                    Throw New Exception("Background Audio Task didn't start in expected time")
                End If
            End Sub)
            startResult.Completed = New AsyncActionCompletedHandler(AddressOf BackgroundTaskInitializationCompleted)
        End Sub

        Private Sub BackgroundTaskInitializationCompleted(action As IAsyncAction, status As AsyncStatus)
            If status = AsyncStatus.Completed Then
                Debug.WriteLine("Background Audio Task initialized")
            ElseIf status = AsyncStatus.Error Then
                Debug.WriteLine("Background Audio Task could not initialized due to an error ::" & action.ErrorCode.ToString())
            End If
        End Sub
#End Region
    End Class
End Namespace
