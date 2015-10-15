'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
'
'*********************************************************
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports Windows.Foundation
Imports SDKTemplate
Imports System

Namespace Global.AssociationLaunching

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class LaunchUri
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
            SizePreference.ItemsSource = MainPage.ViewSizePreferences
            SizePreference.SelectedItem = 0
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached. The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            If Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons") Then
                LaunchUriOpenWithPanel.Visibility = Visibility.Collapsed
                LaunchUriWithWarningPanel.Visibility = Visibility.Collapsed
                LaunchUriSplitScreenPanel.Visibility = Visibility.Collapsed
            End If
        End Sub

        ' Launch a URI.
        ''' <summary>
        Private Async Sub LaunchUriButton_Click(sender As Object, e As RoutedEventArgs)
            ' Create the URI to launch from a string.
            Dim uri = New Uri(UriToLaunch.Text)
            ' Launch the URI.
            Dim success As Boolean = Await Windows.System.Launcher.LaunchUriAsync(uri)
            If success Then
                rootPage.NotifyUser("URI launched: " & uri.AbsoluteUri, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Launch a URI. Show a warning prompt.
        ''' <summary>
        Private Async Sub LaunchUriWithWarningButton_Click(sender As Object, e As RoutedEventArgs)
            ' Create the URI to launch from a string.
            Dim uri = New Uri(UriToLaunch.Text)
            ' Configure the warning prompt.
            Dim options = New Windows.System.LauncherOptions()
            options.TreatAsUntrusted = True
            ' Launch the URI.
            Dim success As Boolean = Await Windows.System.Launcher.LaunchUriAsync(uri, options)
            If success Then
                rootPage.NotifyUser("URI launched: " & uri.AbsoluteUri, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Launch a URI. Show an Open With dialog that lets the user chose the handler to use.
        ''' <summary>
        Private Async Sub LaunchUriOpenWithButton_Click(sender As Object, e As RoutedEventArgs)
            ' Create the URI to launch from a string.
            Dim uri = New Uri(UriToLaunch.Text)
            ' Calulcate the position for the Open With dialog.
            ' An alternative to using the point is to set the rect of the UI element that triggered the launch.
            Dim openWithPosition As Point = GetOpenWithPosition(LaunchUriOpenWithButton)
            ' Next, configure the Open With dialog.
            Dim options = New Windows.System.LauncherOptions()
            options.DisplayApplicationPicker = True
            options.UI.InvocationPoint = openWithPosition
            options.UI.PreferredPlacement = Windows.UI.Popups.Placement.Below
            ' Launch the URI.
            Dim success As Boolean = Await Windows.System.Launcher.LaunchUriAsync(uri, options)
            If success Then
                rootPage.NotifyUser("URI launched: " & uri.AbsoluteUri, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Launch a URI. Request to share the screen with the launched app.
        ''' <summary>
        Private Async Sub LaunchUriSplitScreenButton_Click(sender As Object, e As RoutedEventArgs)
            ' Create the URI to launch from a string.
            Dim uri = New Uri(UriToLaunch.Text)
            ' Configure the request for split screen launch.
            Dim options = New Windows.System.LauncherOptions()
            If Not(Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons")) Then
                options.DesiredRemainingView = SizePreference.SelectedValue
            End If

            ' Launch the URI.
            Dim success As Boolean = Await Windows.System.Launcher.LaunchUriAsync(uri, options)
            If success Then
                rootPage.NotifyUser("URI launched: " & uri.AbsoluteUri, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' The Open With dialog should be displayed just under the element that triggered it.
        ''' <summary>
        Private Function GetOpenWithPosition(element As FrameworkElement) As Windows.Foundation.Point
            Dim buttonTransform As Windows.UI.Xaml.Media.GeneralTransform = element.TransformToVisual(Nothing)
            Dim desiredLocation As Point = buttonTransform.TransformPoint(New Point())
            desiredLocation.Y = desiredLocation.Y + element.ActualHeight
            Return desiredLocation
        End Function

        Private Sub OnPage_SizeChanged(sender As Object, e As SizeChangedEventArgs)
            If Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons") Then
                LaunchUriPanel.Orientation = Orientation.Vertical
                LaunchUriDescription.Width = Me.ActualWidth
                LaunchUriDescription.TextWrapping = TextWrapping.Wrap
                LaunchUriDescription.HorizontalAlignment = HorizontalAlignment.Left
                UriToLaunchPanel.Orientation = Orientation.Vertical
                UriToLaunchDescription.Width = Me.ActualWidth
                UriToLaunchDescription.TextWrapping = TextWrapping.Wrap
                UriToLaunchDescription.HorizontalAlignment = HorizontalAlignment.Left
            End If
        End Sub
    End Class
End Namespace
