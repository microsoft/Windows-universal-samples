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
Imports System.Threading
Imports System.Linq
Imports Windows.ApplicationModel.Background
Imports Windows.Media
Imports Windows.Media.Playback
Imports Windows.Foundation.Collections
Imports Windows.Storage
Imports Windows.Media.Core
Imports System.Collections.Generic
Imports BackgroundAudioShared
Imports Windows.Foundation
Imports BackgroundAudioShared.Messages
Imports Windows.Storage.Streams

Namespace Global.BackgroundAudioTask

    ''' <summary>
    ''' Impletements IBackgroundTask to provide an entry point for app code to be run in background. 
    ''' Also takes care of handling UVC and communication channel with foreground
    ''' </summary>
    Public NotInheritable Class MyBackgroundAudioTask
        Implements IBackgroundTask

#Region " Private fields, properties "
        Private Const TrackIdKey As String = "trackid"

        Private Const TitleKey As String = "title"

        Private Const AlbumArtKey As String = "albumart"

        Private smtc As SystemMediaTransportControls

        Private playbackList As MediaPlaybackList = New MediaPlaybackList()

        Private deferral As BackgroundTaskDeferral

        Private foregroundAppState As AppState = AppState.Unknown

        Private backgroundTaskStarted As ManualResetEvent = New ManualResetEvent(False)

        Private playbackStartedPreviously As Boolean = False
#End Region

#Region " Helper methods "
        Function GetCurrentTrackId() As Uri
            If playbackList Is Nothing Then
                Return Nothing
            End If

            Return GetTrackId(playbackList.CurrentItem)
        End Function

        Function GetTrackId(item As MediaPlaybackItem) As Uri
            If item Is Nothing Then
                Return Nothing
            End If

            Return TryCast(item.Source.CustomProperties(TrackIdKey), Uri)
        End Function

#End Region

#Region " IBackgroundTask and IBackgroundTaskInstance Interface Members and handlers "

        ''' <summary>
        ''' The Run method is the entry point of a background task. 
        ''' </summary>
        ''' <param name="taskInstance"></param>
        Public Sub Run(taskInstance As IBackgroundTaskInstance) Implements IBackgroundTask.Run
            Debug.WriteLine("Background Audio Task " & taskInstance.Task.Name & " starting...")
            smtc = BackgroundMediaPlayer.Current.SystemMediaTransportControls
            AddHandler smtc.ButtonPressed, AddressOf smtc_ButtonPressed
            AddHandler smtc.PropertyChanged, AddressOf smtc_PropertyChanged
            smtc.IsEnabled = True
            smtc.IsPauseEnabled = True
            smtc.IsPlayEnabled = True
            smtc.IsNextEnabled = True
            smtc.IsPreviousEnabled = True
            ' Read persisted state of foreground app
            Dim value = ApplicationSettingsHelper.ReadResetSettingsValue(ApplicationSettingsConstants.AppState)
            If value Is Nothing Then
                foregroundAppState = AppState.Unknown
            Else
                foregroundAppState = EnumHelper.Parse(Of AppState)(value.ToString())
            End If

            AddHandler BackgroundMediaPlayer.Current.CurrentStateChanged, AddressOf Current_CurrentStateChanged
            AddHandler BackgroundMediaPlayer.MessageReceivedFromForeground, AddressOf BackgroundMediaPlayer_MessageReceivedFromForeground
            If foregroundAppState <> AppState.Suspended Then
                MessageService.SendMessageToForeground(New BackgroundAudioTaskStartedMessage())
            End If

            ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.BackgroundTaskState, BackgroundTaskState.Running.ToString())
            deferral = taskInstance.GetDeferral()
            backgroundTaskStarted.Set()
            AddHandler taskInstance.Task.Completed, AddressOf TaskCompleted
            AddHandler taskInstance.Canceled, New BackgroundTaskCanceledEventHandler(AddressOf OnCanceled)
        End Sub

        ''' <summary>
        ''' Indicate that the background task is completed.
        ''' </summary>       
        Sub TaskCompleted(sender As BackgroundTaskRegistration, args As BackgroundTaskCompletedEventArgs)
            Debug.WriteLine("MyBackgroundAudioTask " & sender.TaskId.ToString() & " Completed...")
            deferral.Complete()
        End Sub

        ''' <summary>
        ''' Handles background task cancellation. Task cancellation happens due to:
        ''' 1. Another Media app comes into foreground and starts playing music 
        ''' 2. Resource pressure. Your task is consuming more CPU and memory than allowed.
        ''' In either case, save state so that if foreground app resumes it can know where to start.
        ''' </summary>
        Private Sub OnCanceled(sender As IBackgroundTaskInstance, reason As BackgroundTaskCancellationReason)
            Debug.WriteLine("MyBackgroundAudioTask " & sender.Task.TaskId.ToString() & " Cancel Requested...")
            Try
                backgroundTaskStarted.Reset()
                ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.TrackId, If(GetCurrentTrackId() Is Nothing, Nothing, GetCurrentTrackId().ToString()))
                ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.Position, BackgroundMediaPlayer.Current.Position.ToString())
                ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.BackgroundTaskState, BackgroundTaskState.Canceled.ToString())
                ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.AppState, [Enum].GetName(GetType(AppState), foregroundAppState))
                If playbackList IsNot Nothing Then
                    RemoveHandler playbackList.CurrentItemChanged, AddressOf PlaybackList_CurrentItemChanged
                    playbackList = Nothing
                End If

                RemoveHandler BackgroundMediaPlayer.MessageReceivedFromForeground, AddressOf BackgroundMediaPlayer_MessageReceivedFromForeground
                RemoveHandler smtc.ButtonPressed, AddressOf smtc_ButtonPressed
                RemoveHandler smtc.PropertyChanged, AddressOf smtc_PropertyChanged
                BackgroundMediaPlayer.Shutdown()
            Catch ex As Exception
                Debug.WriteLine(ex.ToString())
            End Try

            deferral.Complete()
            Debug.WriteLine("MyBackgroundAudioTask Cancel complete...")
        End Sub

#End Region

#Region " SysteMediaTransportControls related functions and handlers "

        ''' <summary>
        ''' Update Universal Volume Control (UVC) using SystemMediaTransPortControl APIs
        ''' </summary>
        Private Sub UpdateUVCOnNewTrack(item As MediaPlaybackItem)
            If item Is Nothing Then
                smtc.PlaybackStatus = MediaPlaybackStatus.Stopped
                smtc.DisplayUpdater.MusicProperties.Title = String.Empty
                smtc.DisplayUpdater.Update()
                Return
            End If

            smtc.PlaybackStatus = MediaPlaybackStatus.Playing
            smtc.DisplayUpdater.Type = MediaPlaybackType.Music
            smtc.DisplayUpdater.MusicProperties.Title = TryCast(item.Source.CustomProperties(TitleKey), String)
            Dim albumArtUri = TryCast(item.Source.CustomProperties(AlbumArtKey), Uri)
            If albumArtUri IsNot Nothing Then
                smtc.DisplayUpdater.Thumbnail = RandomAccessStreamReference.CreateFromUri(albumArtUri)
            Else
                smtc.DisplayUpdater.Thumbnail = Nothing
            End If

            smtc.DisplayUpdater.Update()
        End Sub

        ''' <summary>
        ''' Fires when any SystemMediaTransportControl property is changed by system or user
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="args"></param>
        Sub smtc_PropertyChanged(sender As SystemMediaTransportControls, args As SystemMediaTransportControlsPropertyChangedEventArgs)
        End Sub

        ''' <summary>
        ''' This function controls the button events from UVC.
        ''' This code if not run in background process, will not be able to handle button pressed events when app is suspended.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="args"></param>
        Private Sub smtc_ButtonPressed(sender As SystemMediaTransportControls, args As SystemMediaTransportControlsButtonPressedEventArgs)
            Select args.Button
                Case SystemMediaTransportControlsButton.Play
                    Debug.WriteLine("UVC play button pressed")
                    ' When the background task has been suspended and the SMTC
                    ' starts it again asynchronously, some time is needed to let
                    ' the task startup process in Run() complete.
                    ' Wait for task to start. 
                    ' Once started, this stays signaled until shutdown so it won't wait
                    ' again unless it needs to.
                    Dim result As Boolean = backgroundTaskStarted.WaitOne(5000)
                    If Not result Then
                        Throw New Exception("Background Task didnt initialize in time")
                    End If

                    StartPlayback()
                     Case SystemMediaTransportControlsButton.Pause
                    Debug.WriteLine("UVC pause button pressed")
                    Try
                        BackgroundMediaPlayer.Current.Pause()
                    Catch ex As Exception
                        Debug.WriteLine(ex.ToString())
                    End Try

                     Case SystemMediaTransportControlsButton.Next
                    Debug.WriteLine("UVC next button pressed")
                    SkipToNext()
                     Case SystemMediaTransportControlsButton.Previous
                    Debug.WriteLine("UVC previous button pressed")
                    SkipToPrevious()
                     End Select
        End Sub

#End Region

#Region " Playlist management functions and handlers "

        ''' <summary>
        ''' Start playlist and change UVC state
        ''' </summary>
        Private Sub StartPlayback()
            Try
                If Not playbackStartedPreviously Then
                    playbackStartedPreviously = True
                    ' If the task was cancelled we would have saved the current track and its position. We will try playback from there.
                    Dim currentTrackId = ApplicationSettingsHelper.ReadResetSettingsValue(ApplicationSettingsConstants.TrackId)
                    Dim currentTrackPosition = ApplicationSettingsHelper.ReadResetSettingsValue(ApplicationSettingsConstants.Position)
                    If currentTrackId IsNot Nothing Then
                        ' Find the index of the item by name
                        Dim index = playbackList.Items.ToList().FindIndex(Function(item) GetTrackId(item).ToString() = CType(currentTrackId, String))
                        If currentTrackPosition Is Nothing Then
                            Debug.WriteLine("StartPlayback: Switching to track " & index)
                            playbackList.MoveTo(CType(index, UInteger))
                            BackgroundMediaPlayer.Current.Play()
                        Else
                            ' Play from exact position otherwise
                            Dim handler As TypedEventHandler(Of MediaPlaybackList, CurrentMediaPlaybackItemChangedEventArgs) = Nothing
                            handler = Sub(list As MediaPlaybackList, args As CurrentMediaPlaybackItemChangedEventArgs)
                                If args.NewItem Is playbackList.Items(index) Then
                                              RemoveHandler playbackList.CurrentItemChanged, handler
                                              ' Set position
                                              Dim position = TimeSpan.Parse(CType(currentTrackPosition, String))
                                              Debug.WriteLine("StartPlayback: Setting Position " & position.ToString())
                                              BackgroundMediaPlayer.Current.Position = position
                                    BackgroundMediaPlayer.Current.Play()
                                End If
                            End Sub
                            AddHandler playbackList.CurrentItemChanged, handler
                            Debug.WriteLine("StartPlayback: Switching to track " & index)
                            playbackList.MoveTo(CType(index, UInteger))
                        End If
                    Else
                        BackgroundMediaPlayer.Current.Play()
                    End If
                Else
                    BackgroundMediaPlayer.Current.Play()
                End If
            Catch ex As Exception
                Debug.WriteLine(ex.ToString())
            End Try
        End Sub

        ''' <summary>
        ''' Raised when playlist changes to a new track
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="args"></param>
        Sub PlaybackList_CurrentItemChanged(sender As MediaPlaybackList, args As CurrentMediaPlaybackItemChangedEventArgs)
            ' Get the new item
            Dim item = args.NewItem
            Debug.WriteLine("PlaybackList_CurrentItemChanged: " & (If(item Is Nothing, "null", GetTrackId(item).ToString())))
            UpdateUVCOnNewTrack(item)
            ' Get the current track
            Dim currentTrackId As Uri = Nothing
            If item IsNot Nothing Then
                currentTrackId = TryCast(item.Source.CustomProperties(TrackIdKey), Uri)
            End If

            If foregroundAppState = AppState.Active Then
                MessageService.SendMessageToForeground(New TrackChangedMessage(currentTrackId))
            Else
                ApplicationSettingsHelper.SaveSettingsValue(TrackIdKey, If(currentTrackId Is Nothing, Nothing, currentTrackId.ToString()))
            End If
        End Sub

        ''' <summary>
        ''' Skip track and update UVC via SMTC
        ''' </summary>
        Private Sub SkipToPrevious()
            smtc.PlaybackStatus = MediaPlaybackStatus.Changing
            playbackList.MovePrevious()
            BackgroundMediaPlayer.Current.Play()
        End Sub

        ''' <summary>
        ''' Skip track and update UVC via SMTC
        ''' </summary>
        Private Sub SkipToNext()
            smtc.PlaybackStatus = MediaPlaybackStatus.Changing
            playbackList.MoveNext()
            BackgroundMediaPlayer.Current.Play()
        End Sub
#End Region

#Region " Background Media Player Handlers "
        Sub Current_CurrentStateChanged(sender As MediaPlayer, args As Object)
            If sender.CurrentState = MediaPlayerState.Playing Then
                smtc.PlaybackStatus = MediaPlaybackStatus.Playing
            ElseIf sender.CurrentState = MediaPlayerState.Paused Then
                smtc.PlaybackStatus = MediaPlaybackStatus.Paused
            ElseIf sender.CurrentState = MediaPlayerState.Closed Then
                smtc.PlaybackStatus = MediaPlaybackStatus.Closed
            End If
        End Sub

        ''' <summary>
        ''' Raised when a message is recieved from the foreground app
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Sub BackgroundMediaPlayer_MessageReceivedFromForeground(sender As Object, e As MediaPlayerDataReceivedEventArgs)
            Dim appSuspendedMessage As AppSuspendedMessage = Nothing
            If MessageService.TryParseMessage(e.Data, appSuspendedMessage) Then
                Debug.WriteLine("App suspending")
                foregroundAppState = AppState.Suspended
                Dim currentTrackId = GetCurrentTrackId()
                ApplicationSettingsHelper.SaveSettingsValue(ApplicationSettingsConstants.TrackId, If(currentTrackId Is Nothing, Nothing, currentTrackId.ToString()))
                Return
            End If

            Dim appResumedMessage As AppResumedMessage = Nothing
            If MessageService.TryParseMessage(e.Data, appResumedMessage) Then
                Debug.WriteLine("App resuming")
                foregroundAppState = AppState.Active
                Return
            End If

            Dim startPlaybackMessage As StartPlaybackMessage = Nothing
            If MessageService.TryParseMessage(e.Data, startPlaybackMessage) Then
                Debug.WriteLine("Starting Playback")
                StartPlayback()
                Return
            End If

            Dim skipNextMessage As SkipNextMessage = Nothing
            If MessageService.TryParseMessage(e.Data, skipNextMessage) Then
                Debug.WriteLine("Skipping to next")
                SkipToNext()
                Return
            End If

            Dim skipPreviousMessage As SkipPreviousMessage = Nothing
            If MessageService.TryParseMessage(e.Data, skipPreviousMessage) Then
                Debug.WriteLine("Skipping to previous")
                SkipToPrevious()
                Return
            End If

            Dim trackChangedMessage As TrackChangedMessage = Nothing
            If MessageService.TryParseMessage(e.Data, trackChangedMessage) Then
                Dim index = playbackList.Items.ToList().FindIndex(Function(i) CType(i.Source.CustomProperties(TrackIdKey), Uri) = trackChangedMessage.TrackId)
                Debug.WriteLine("Skipping to track " & index)
                smtc.PlaybackStatus = MediaPlaybackStatus.Changing
                playbackList.MoveTo(CType(index, UInteger))
                BackgroundMediaPlayer.Current.Play()
                Return
            End If

            Dim updatePlaylistMessage As UpdatePlaylistMessage = Nothing
            If MessageService.TryParseMessage(e.Data, updatePlaylistMessage) Then
                CreatePlaybackList(updatePlaylistMessage.Songs)
                Return
            End If
        End Sub

        ''' <summary>
        ''' Create a playback list from the list of songs received from the foreground app.
        ''' </summary>
        ''' <param name="songs"></param>
        Private Sub CreatePlaybackList(songs As IEnumerable(Of SongModel))
            playbackList = New MediaPlaybackList()
            playbackList.AutoRepeatEnabled = True
            For Each song In songs
                Dim source = MediaSource.CreateFromUri(song.MediaUri)
                source.CustomProperties(TrackIdKey) = song.MediaUri
                source.CustomProperties(TitleKey) = song.Title
                source.CustomProperties(AlbumArtKey) = song.AlbumArtUri
                playbackList.Items.Add(New MediaPlaybackItem(source))
            Next

            BackgroundMediaPlayer.Current.AutoPlay = False
            BackgroundMediaPlayer.Current.Source = playbackList
            AddHandler playbackList.CurrentItemChanged, AddressOf PlaybackList_CurrentItemChanged
        End Sub
#End Region
    End Class
End Namespace
