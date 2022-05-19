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
Imports Windows.Storage

Namespace Global.ApplicationDataSample

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Files
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim localFolder As StorageFolder = Nothing

        Dim localCounter As Integer = 0

        Dim localCacheFolder As StorageFolder = Nothing

        Dim localCacheCounter As Integer = 0

        Dim roamingFolder As StorageFolder = Nothing

        Dim roamingCounter As Integer = 0

        Dim temporaryFolder As StorageFolder = Nothing

        Dim temporaryCounter As Integer = 0

        Const filename As String = "sampleFile.txt"

        Public Sub New()
            Me.InitializeComponent()
            localFolder = ApplicationData.Current.LocalFolder
            localCacheFolder = ApplicationData.Current.LocalCacheFolder
            roamingFolder = ApplicationData.Current.RoamingFolder
            temporaryFolder = ApplicationData.Current.TemporaryFolder
            DisplayOutput()
        End Sub

        ' Guidance for Local, LocalCache, Roaming, and Temporary files.
        '
        ' Files are ideal for storing large data-sets, databases, or data that is
        ' in a common file-format.
        '
        ' Files can exist in either the Local, LocalCache, Roaming, or Temporary folders.
        '
        ' Roaming files will be synchronized across machines on which the user has
        ' singed in with a connected account.  Roaming of files is not instant; the
        ' system weighs several factors when determining when to send the data.  Usage
        ' of roaming data should be kept below the quota (available via the 
        ' RoamingStorageQuota property), or else roaming of data will be suspended.
        ' Files cannot be roamed while an application is writing to them, so be sure
        ' to close your application's file objects when they are no longer needed.
        '
        ' Local files are not synchronized, but are backed up, and can then be restored to a 
        ' machine different than where they were originally written. These should be for 
        ' important files that allow the feel that the user did not loose anything
        ' when they restored to a new device.
        '
        ' Temporary files are subject to deletion when not in use.  The system 
        ' considers factors such as available disk capacity and the age of a file when
        ' determining when or whether to delete a temporary file.
        '
        ' LocalCache files are for larger files that can be recreated by the app, and for
        ' machine specific or private files that should not be restored to a new device.
        Async Sub Increment_Local_Click(sender As [Object], e As RoutedEventArgs)
            localCounter = localCounter + 1
            Dim file As StorageFile = Await localFolder.CreateFileAsync(filename, CreationCollisionOption.ReplaceExisting)
            Await FileIO.WriteTextAsync(file, localCounter.ToString())
            Read_Local_Counter()
        End Sub

        Async Sub Read_Local_Counter()
            Try
                Dim file As StorageFile = Await localFolder.GetFileAsync(filename)
                Dim text As String = Await FileIO.ReadTextAsync(file)
                LocalOutputTextBlock.Text = "Local Counter: " & text
                localCounter = Integer.Parse(text)
            Catch
                LocalOutputTextBlock.Text = "Local Counter: <not found>"
            End Try
        End Sub

        Async Sub Increment_LocalCache_Click(sender As [Object], e As RoutedEventArgs)
            localCacheCounter = localCacheCounter + 1
            Dim file As StorageFile = Await localCacheFolder.CreateFileAsync(filename, CreationCollisionOption.ReplaceExisting)
            Await FileIO.WriteTextAsync(file, localCacheCounter.ToString())
            Read_LocalCache_Counter()
        End Sub

        Async Sub Read_LocalCache_Counter()
            Try
                Dim file As StorageFile = Await localCacheFolder.GetFileAsync(filename)
                Dim text As String = Await FileIO.ReadTextAsync(file)
                LocalCacheOutputTextBlock.Text = "LocalCache Counter: " & text
                localCacheCounter = Integer.Parse(text)
            Catch
                LocalCacheOutputTextBlock.Text = "LocalCache Counter: <not found>"
            End Try
        End Sub

        Async Sub Increment_Roaming_Click(sender As [Object], e As RoutedEventArgs)
            roamingCounter = roamingCounter + 1
            Dim file As StorageFile = Await roamingFolder.CreateFileAsync(filename, CreationCollisionOption.ReplaceExisting)
            Await FileIO.WriteTextAsync(file, roamingCounter.ToString())
            Read_Roaming_Counter()
        End Sub

        Async Sub Read_Roaming_Counter()
            Try
                Dim file As StorageFile = Await roamingFolder.GetFileAsync(filename)
                Dim text As String = Await FileIO.ReadTextAsync(file)
                RoamingOutputTextBlock.Text = "Roaming Counter: " & text
                roamingCounter = Integer.Parse(text)
            Catch
                RoamingOutputTextBlock.Text = "Roaming Counter: <not found>"
            End Try
        End Sub

        Async Sub Increment_Temporary_Click(sender As [Object], e As RoutedEventArgs)
            temporaryCounter = temporaryCounter + 1
            Dim file As StorageFile = Await temporaryFolder.CreateFileAsync(filename, CreationCollisionOption.ReplaceExisting)
            Await FileIO.WriteTextAsync(file, temporaryCounter.ToString())
            Read_Temporary_Counter()
        End Sub

        Async Sub Read_Temporary_Counter()
            Try
                Dim file As StorageFile = Await temporaryFolder.GetFileAsync(filename)
                Dim text As String = Await FileIO.ReadTextAsync(file)
                TemporaryOutputTextBlock.Text = "Temporary Counter: " & text
                temporaryCounter = Integer.Parse(text)
            Catch
                TemporaryOutputTextBlock.Text = "Temporary Counter: <not found>"
            End Try
        End Sub

        Sub DisplayOutput()
            Read_Local_Counter()
            Read_LocalCache_Counter()
            Read_Roaming_Counter()
            Read_Temporary_Counter()
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
        End Sub
    End Class
End Namespace
