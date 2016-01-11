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
Imports Windows.Foundation
Imports Windows.System
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Partial Public NotInheritable Class Scenario2_LaunchUri
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
            ViewPreference.ItemsSource = MainPage.ViewSizePreferences
            ViewPreference.SelectedIndex = 0
        End Sub

        ' Launch a URI.
        ''' <summary>
        Private Async Sub LaunchUriDefault()
            ' Create the URI to launch from a string.
            Dim uri = New Uri(UriToLaunch.Text)
            ' Launch the URI.
            Dim success As Boolean = Await Launcher.LaunchUriAsync(uri)
            If success Then
                rootPage.NotifyUser("URI launched: " & uri.AbsoluteUri, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Launch a URI. Show a warning prompt.
        ''' <summary>
        Private Async Sub LaunchUriWithWarning()
            ' Create the URI to launch from a string.
            Dim uri = New Uri(UriToLaunch.Text)
            ' Configure the warning prompt.
            Dim options = New LauncherOptions() With {.TreatAsUntrusted = True}
            ' Launch the URI.
            Dim success As Boolean = Await Launcher.LaunchUriAsync(uri, options)
            If success Then
                rootPage.NotifyUser("URI launched: " & uri.AbsoluteUri, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Launch a URI. Show an Open With dialog that lets the user chose the handler to use.
        ''' <summary>
        Private Async Sub LaunchUriOpenWith(sender As Object, e As RoutedEventArgs)
            ' Create the URI to launch from a string.
            Dim uri = New Uri(UriToLaunch.Text)
            ' Calulcate the position for the Open With dialog.
            ' An alternative to using the point is to set the rect of the UI element that triggered the launch.
            Dim openWithPosition As Point = MainPage.GetElementLocation(sender)
            ' Next, configure the Open With dialog.
            Dim options = New LauncherOptions()
            options.DisplayApplicationPicker = True
            options.UI.InvocationPoint = openWithPosition
            options.UI.PreferredPlacement = Windows.UI.Popups.Placement.Below
            ' Launch the URI.
            Dim success As Boolean = Await Launcher.LaunchUriAsync(uri, options)
            If success Then
                rootPage.NotifyUser("URI launched: " & uri.AbsoluteUri, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Launch a URI. Request to share the screen with the launched app.
        ''' <summary>
        Private Async Sub LaunchUriSplitScreen()
            ' Create the URI to launch from a string.
            Dim uri = New Uri(UriToLaunch.Text)
            ' Configure the request for split screen launch.
            Dim options = New LauncherOptions() With {.DesiredRemainingView = ViewPreference.SelectedValue}

            ' Launch the URI.
            Dim success As Boolean = Await Launcher.LaunchUriAsync(uri, options)
            If success Then
                rootPage.NotifyUser("URI launched: " & uri.AbsoluteUri, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub
    End Class
End Namespace
