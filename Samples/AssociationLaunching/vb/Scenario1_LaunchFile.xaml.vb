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
Imports System.Threading.Tasks
Imports Windows.ApplicationModel.Activation
Imports Windows.Foundation
Imports Windows.Storage
Imports Windows.System
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Partial Public NotInheritable Class Scenario1_LaunchFile
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim fileToLaunch As String = "Assets\microsoft-sdk.png"

        Public Sub New()
            Me.InitializeComponent()
            ViewPreference.ItemsSource = MainPage.ViewSizePreferences
            ViewPreference.SelectedIndex = 0
        End Sub

        Private Async Function GetFileToLaunch() As Task(Of StorageFile)
            ' First, get the image file from the package's image directory.
            Dim file = Await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFileAsync(fileToLaunch)
            file = Await file.CopyAsync(ApplicationData.Current.TemporaryFolder, "filetolaunch.png", NameCollisionOption.ReplaceExisting)
            Return file
        End Function

        ' Launch a .png file that came with the package.
        ''' <summary>
        Private Async Sub LaunchFileDefault()
            ' First, get the image file from the package's image directory.
            Dim file = Await GetFileToLaunch()
            ' Next, launch the file.
            Dim success As Boolean = Await Launcher.LaunchFileAsync(file)
            If success Then
                rootPage.NotifyUser("File launched: " & file.Name, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Launch a .png file that came with the package. Show a warning prompt.
        ''' <summary>
        Private Async Sub LaunchFileWithWarning()
            ' First, get the image file from the package's image directory.
            Dim file = Await GetFileToLaunch()
            ' Next, configure the warning prompt.
            Dim options = New LauncherOptions() With {.TreatAsUntrusted = True}
            ' Finally, launch the file.
            Dim success As Boolean = Await Launcher.LaunchFileAsync(file, options)
            If success Then
                rootPage.NotifyUser("File launched: " & file.Name, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Launch a .png file that came with the package. Show an Open With dialog that lets the user chose the handler to use.
        ''' <summary>
        Private Async Sub LaunchFileOpenWith(sender As Object, e As RoutedEventArgs)
            ' First, get the image file from the package's image directory.
            Dim file = Await GetFileToLaunch()
            ' Calculate the position for the Open With dialog.
            ' An alternative to using the point is to set the rect of the UI element that triggered the launch.
            Dim openWithPosition As Point = MainPage.GetElementLocation(sender)
            ' Next, configure the Open With dialog.
            Dim options = New LauncherOptions()
            options.DisplayApplicationPicker = True
            options.UI.InvocationPoint = openWithPosition
            options.UI.PreferredPlacement = Windows.UI.Popups.Placement.Below
            ' Finally, launch the file.
            Dim success As Boolean = Await Launcher.LaunchFileAsync(file, options)
            If success Then
                rootPage.NotifyUser("File launched: " & file.Name, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Launch a .png file that came with the package. Request to share the screen with the launched app.
        ''' <summary>
        Private Async Sub LaunchFileSplitScreen()
            ' First, get a file via the picker.
            Dim openPicker = New Pickers.FileOpenPicker()
            openPicker.FileTypeFilter.Add("*")
            Dim file As StorageFile = Await openPicker.PickSingleFileAsync()
            If file IsNot Nothing Then
                ' Configure the request for split screen launch.
                Dim options = New LauncherOptions() With {.DesiredRemainingView = ViewPreference.SelectedValue}

                ' Next, launch the file.
                Dim success As Boolean = Await Launcher.LaunchFileAsync(file, options)
                If success Then
                    rootPage.NotifyUser("File launched: " & file.Name, NotifyType.StatusMessage)
                Else
                    rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage)
                End If
            Else
                rootPage.NotifyUser("No file was picked.", NotifyType.ErrorMessage)
            End If
        End Sub

        ' Have the user pick a file, then launch it.
        ''' <summary>
        Private Async Sub PickAndLaunchFile()
            ' First, get a file via the picker.
            Dim openPicker = New Pickers.FileOpenPicker()
            openPicker.FileTypeFilter.Add("*")
            Dim file As StorageFile = Await openPicker.PickSingleFileAsync()
            If file IsNot Nothing Then
                ' Next, launch the file.
                Dim success As Boolean = Await Launcher.LaunchFileAsync(file)
                If success Then
                    rootPage.NotifyUser("File launched: " & file.Name, NotifyType.StatusMessage)
                Else
                    rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage)
                End If
            Else
                rootPage.NotifyUser("No file was picked.", NotifyType.ErrorMessage)
            End If
        End Sub
    End Class
End Namespace
