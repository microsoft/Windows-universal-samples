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
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.ScreenCasting

    Public NotInheritable Partial Class Scenario01
        Inherits Page

        Private rootPage As MainPage

        Private video As VideoMetaData

        Public Sub New()
            Me.InitializeComponent()
            rootPage = MainPage.Current
            AddHandler player.MediaOpened, AddressOf Player_MediaOpened
            AddHandler player.MediaFailed, AddressOf Player_MediaFailed
            AddHandler player.CurrentStateChanged, AddressOf Player_CurrentStateChanged
            ' Get a video
            Dim dataProvider As AzureDataProvider = New AzureDataProvider()
            video = dataProvider.GetRandomVideo()
            Me.player.Source = video.VideoLink
            Me.LicenseText.Text = "License: " & video.License
            rootPage.NotifyUser(String.Format("Opening '{0}'", video.Title), NotifyType.StatusMessage)
            Me.player.TransportControls.IsCompact = True
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
        End Sub

#Region " Media Element Methods "
        Private Sub Player_CurrentStateChanged(sender As Object, e As RoutedEventArgs)
            If Me.player.CurrentState <> Windows.UI.Xaml.Media.MediaElementState.Closed Then
                rootPage.NotifyUser(String.Format("{0} '{1}'", Me.player.CurrentState, video.Title), NotifyType.StatusMessage)
            End If
        End Sub

        Private Sub Player_MediaFailed(sender As Object, e As ExceptionRoutedEventArgs)
            rootPage.NotifyUser(String.Format("Failed to load '{0}', {1}", video.Title, e.ErrorMessage), NotifyType.ErrorMessage)
        End Sub

        Private Sub Player_MediaOpened(sender As Object, e As RoutedEventArgs)
            rootPage.NotifyUser(String.Format("Openend '{0}'", video.Title), NotifyType.StatusMessage)
        End Sub
#End Region
    End Class
End Namespace
