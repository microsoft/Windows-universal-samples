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
Imports Windows.ApplicationModel.Activation
Imports Windows.Storage
Imports System.Threading.Tasks

Namespace Global.AssociationLaunching

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class LaunchFile
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim fileToLaunch As String = "Assets\microsoft-sdk.png"

        Public Sub New()
            Me.InitializeComponent()
            SizePreference.ItemsSource = MainPage.ViewSizePreferences
            SizePreference.SelectedIndex = 0
        End Sub

        Private Async Function GetFileToLaunch() As Task(Of StorageFile)
            ' First, get the image file from the package's image directory.
            Dim file = Await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFileAsync(fileToLaunch)
            file = Await file.CopyAsync(ApplicationData.Current.TemporaryFolder, "filetolaunch.png", NameCollisionOption.ReplaceExisting)
            Return file
        End Function

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached. The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            If Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons") Then
                LaunchFileOpenWithPanel.Visibility = Visibility.Collapsed
                LaunchFileWithWarningPanel.Visibility = Visibility.Collapsed
                LaunchFileSplitScreenPanel.Visibility = Visibility.Collapsed
            End If
        End Sub

        ' Launch a .png file that came with the package.
        ''' <summary>
        Private Async Sub LaunchFileButton_Click(sender As Object, e As RoutedEventArgs)
            ' First, get the image file from the package's image directory.
            Dim file = Await GetFileToLaunch()
            ' Next, launch the file.
            Dim success As Boolean = Await Windows.System.Launcher.LaunchFileAsync(file)
            If success Then
                rootPage.NotifyUser("File launched: " & file.Name, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Launch a .png file that came with the package. Show a warning prompt.
        ''' <summary>
        Private Async Sub LaunchFileWithWarningButton_Click(sender As Object, e As RoutedEventArgs)
            ' First, get the image file from the package's image directory.
            Dim file = Await GetFileToLaunch()
            ' Next, configure the warning prompt.
            Dim options = New Windows.System.LauncherOptions()
            options.TreatAsUntrusted = True
            ' Finally, launch the file.
            Dim success As Boolean = Await Windows.System.Launcher.LaunchFileAsync(file, options)
            If success Then
                rootPage.NotifyUser("File launched: " & file.Name, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Launch a .png file that came with the package. Show an Open With dialog that lets the user chose the handler to use.
        ''' <summary>
        Private Async Sub LaunchFileOpenWithButton_Click(sender As Object, e As RoutedEventArgs)
            ' First, get the image file from the package's image directory.
            Dim file = Await GetFileToLaunch()
            ' Calculate the position for the Open With dialog.
            ' An alternative to using the point is to set the rect of the UI element that triggered the launch.
            Dim openWithPosition As Point = GetOpenWithPosition(LaunchFileOpenWithButton)
            ' Next, configure the Open With dialog.
            Dim options = New Windows.System.LauncherOptions()
            options.DisplayApplicationPicker = True
            options.UI.InvocationPoint = openWithPosition
            options.UI.PreferredPlacement = Windows.UI.Popups.Placement.Below
            ' Finally, launch the file.
            Dim success As Boolean = Await Windows.System.Launcher.LaunchFileAsync(file, options)
            If success Then
                rootPage.NotifyUser("File launched: " & file.Name, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Launch a .png file that came with the package. Request to share the screen with the launched app.
        ''' <summary>
        Private Async Sub LaunchFileSplitScreenButton_Click(sender As Object, e As RoutedEventArgs)
            If Not(Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons")) Then
                ' First, get a file via the picker.
                Dim openPicker = New Windows.Storage.Pickers.FileOpenPicker()
                openPicker.FileTypeFilter.Add("*")
                Dim file As Windows.Storage.StorageFile = Await openPicker.PickSingleFileAsync()
                If file IsNot Nothing Then
                    ' Configure the request for split screen launch.
                    Dim options = New Windows.System.LauncherOptions()
                    options.DesiredRemainingView = SizePreference.SelectedValue

                    ' Next, launch the file.
                    Dim success As Boolean = Await Windows.System.Launcher.LaunchFileAsync(file, options)
                    If success Then
                        rootPage.NotifyUser("File launched: " & file.Name, NotifyType.StatusMessage)
                    Else
                        rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage)
                    End If
                Else
                    rootPage.NotifyUser("No file was picked.", NotifyType.ErrorMessage)
                End If
            End If
        End Sub

        ' Have the user pick a file, then launch it.
        ''' <summary>
        Private Async Sub PickAndLaunchFileButton_Click(sender As Object, e As RoutedEventArgs)
            ' First, get a file via the picker.
            Dim openPicker = New Windows.Storage.Pickers.FileOpenPicker()
            openPicker.FileTypeFilter.Add("*")
            Dim file As Windows.Storage.StorageFile = Await openPicker.PickSingleFileAsync()
            If file IsNot Nothing Then
                ' Next, launch the file.
                Dim success As Boolean = Await Windows.System.Launcher.LaunchFileAsync(file)
                If success Then
                    rootPage.NotifyUser("File launched: " & file.Name, NotifyType.StatusMessage)
                Else
                    rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage)
                End If
            Else
                rootPage.NotifyUser("No file was picked.", NotifyType.ErrorMessage)
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
    End Class
End Namespace
