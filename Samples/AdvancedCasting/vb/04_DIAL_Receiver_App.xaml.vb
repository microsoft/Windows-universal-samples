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
Imports ScreenCasting.Data.Azure
Imports ScreenCasting.Data.Common
Imports ScreenCasting.Util
Imports System
Imports Windows.ApplicationModel.Activation
Imports Windows.Storage
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.ScreenCasting

    Public NotInheritable Partial Class Scenario04
        Inherits Page

        Private rootPage As MainPage

        Private video As VideoMetaData = Nothing

        Private originalPosition As TimeSpan = New TimeSpan(0)

        Public Sub New()
            Me.InitializeComponent()
            rootPage = MainPage.Current
            AddHandler rootPage.SizeChanged, AddressOf RootPage_SizeChanged
        End Sub

        Protected Async Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
            AddHandler player.MediaOpened, AddressOf Player_MediaOpened
            AddHandler player.MediaFailed, AddressOf Player_MediaFailed
            AddHandler player.CurrentStateChanged, AddressOf Player_CurrentStateChanged
            AddHandler player.MediaEnded, AddressOf Player_MediaEnded
            If TypeOf e.Parameter Is DialReceiverActivatedEventArgs Then
                Dim activationArgs As DialReceiverActivatedEventArgs = CType(e.Parameter, DialReceiverActivatedEventArgs)
                'Parse the DIAL arguments from the activation arguments
                Dim dialArgs As DialLaunchArguments = DialLaunchArguments.Parse(activationArgs.Arguments)
                ' Get the list of available Azure videos.
                Dim dataProvider As AzureDataProvider = New AzureDataProvider()
                video = dataProvider.GetFromID(dialArgs.VideoId)
                rootPage.NotifyUser(String.Format("Opening '{0}'", video.Title), NotifyType.StatusMessage)
                Me.player.Source = video.VideoLink
                Me.LicenseText.Text = "License: " & video.License
                player.Position = dialArgs.Position
            Else
                Dim localVideo As StorageFile = Await StorageFile.GetFileFromApplicationUriAsync(New Uri("ms-appx:///Assets/casting.mp4"))
                Dim stream = Await localVideo.OpenAsync(FileAccessMode.Read)
                Me.player.SetSource(stream, localVideo.FileType)
            End If
        End Sub

        Private Sub Player_MediaEnded(sender As Object, e As RoutedEventArgs)
            If video Is Nothing Then
                Me.player.Position = New TimeSpan(0)
                Me.player.Play()
            End If
        End Sub

        Private Sub RootPage_SizeChanged(sender As Object, e As SizeChangedEventArgs)
            Me.player.Height = Me.Height
            Me.player.Width = Me.Width
        End Sub

#Region " Media Element Methods "
        Private Sub Player_CurrentStateChanged(sender As Object, e As RoutedEventArgs)
            If video Is Nothing Then
                rootPage.NotifyUser(String.Format("{0}", Me.player.CurrentState), NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser(String.Format("{0} '{1}'", Me.player.CurrentState, video.Title), NotifyType.StatusMessage)
            End If
        End Sub

        Private Sub Player_MediaFailed(sender As Object, e As ExceptionRoutedEventArgs)
            If video Is Nothing Then
                rootPage.NotifyUser(String.Format("Failed to load"), NotifyType.ErrorMessage)
            Else
                rootPage.NotifyUser(String.Format("Failed to load '{0}'", video.Title), NotifyType.ErrorMessage)
            End If
        End Sub

        Private Sub Player_MediaOpened(sender As Object, e As RoutedEventArgs)
            If video Is Nothing Then
                rootPage.NotifyUser(String.Format("Openend"), NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser(String.Format("Openend '{0}'", "Title"), NotifyType.StatusMessage)
            End If

            player.Play()
        End Sub
#End Region
    End Class
End Namespace
