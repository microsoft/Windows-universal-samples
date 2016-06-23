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
Imports System
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.IO
Imports System.Linq
Imports System.Runtime.InteropServices.WindowsRuntime
Imports Windows.Foundation
Imports Windows.Foundation.Collections
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Controls.Primitives
Imports Windows.UI.Xaml.Data
Imports Windows.UI.Xaml.Input
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Navigation

Namespace Global.BackgroundAudio

    ''' <summary>
    ''' A custom control for playlists that wraps a ListView.
    ''' </summary>
    Public NotInheritable Partial Class PlaylistView
        Inherits UserControl

        Dim _songs As New ObservableCollection(Of SongModel)()

        Public Sub New()
            Me.InitializeComponent()
            listView.ItemsSource = Songs
        End Sub

        Public Property SelectedIndex As Integer
            Get
                Return listView.SelectedIndex
            End Get

            Set
                listView.SelectedIndex = value
            End Set
        End Property

        Public Custom Event ItemClick As ItemClickEventHandler
            AddHandler(value As ItemClickEventHandler)
                AddHandler listView.ItemClick, value
            End AddHandler

            RemoveHandler(value As ItemClickEventHandler)
                RemoveHandler listView.ItemClick, value
            End RemoveHandler

            RaiseEvent(sender As Object, e As ItemClickEventArgs)
                Throw New InvalidOperationException()
            End RaiseEvent
        End Event

        Public Custom Event SelectionChanged As SelectionChangedEventHandler
            AddHandler(value As SelectionChangedEventHandler)
                AddHandler listView.SelectionChanged, value
            End AddHandler

            RemoveHandler(value As SelectionChangedEventHandler)
                RemoveHandler listView.SelectionChanged, value
            End RemoveHandler

            RaiseEvent(sender As Object, e As SelectionChangedEventArgs)
                Throw New InvalidOperationException()
            End RaiseEvent
        End Event

        Public ReadOnly Property Songs As ObservableCollection(Of SongModel)
            Get
                Return _songs
            End Get
        End Property

        Public Function GetSongById(id As Uri) As SongModel
            Return songs.Single(Function(s) s.MediaUri = id)
        End Function

        Public Function GetSongIndexById(id As Uri) As Integer
            Return songs.ToList().FindIndex(Function(s) s.MediaUri = id)
        End Function
    End Class
End Namespace
