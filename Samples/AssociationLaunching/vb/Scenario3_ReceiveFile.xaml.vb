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
Imports System.Collections.Generic
Imports System.Threading.Tasks
Imports Windows.ApplicationModel.Activation
Imports Windows.Storage
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Partial Public NotInheritable Class Scenario3_ReceiveFile
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim Extension As String = "alsdkvb"

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Async Sub OnNavigatedTo(e As NavigationEventArgs)
            Dim args = TryCast(e.Parameter, FileActivatedEventArgs)
            If args IsNot Nothing Then
                Dim output As String = "Files received: " & args.Files.Count & vbLf
                For Each file In args.Files
                    output = output & file.Name & vbLf
                Next

                If args.NeighboringFilesQuery IsNot Nothing Then
                    ' If supported, we receive the pictures from the same folder as the file that was launched
                    ' because we declared the picturesLibrary capability. The order of the neighboring
                    ' files matches the sort order of the folder from which the file was launched.
                    Dim neighboringFiles As IReadOnlyList(Of StorageFile) = Await args.NeighboringFilesQuery.GetFilesAsync()
                    If neighboringFiles.Count > 0 Then
                        output = output & vbLf & "Neighboring files: " & neighboringFiles.Count & vbLf
                        Dim i As Integer
                        For i = 0 To Math.Min(neighboringFiles.Count, 2)
                            output = output & neighboringFiles(i).Name & vbLf
                        Next
                        Dim remaining As Integer = neighboringFiles.Count - i
                        If remaining > 0 Then
                            output = output & "and " & remaining & " more."
                        End If
                    End If
                End If
                rootPage.NotifyUser(output, NotifyType.StatusMessage)
            End If
        End Sub

        Private Async Sub CreateTestFile()
            Dim folder As StorageFolder = KnownFolders.PicturesLibrary
            Await folder.CreateFileAsync("Test " & Extension + " file." & Extension, CreationCollisionOption.ReplaceExisting)
            Await Windows.System.Launcher.LaunchFolderAsync(folder)
        End Sub

        Private Async Sub RemoveTestFile()
            Dim folder As StorageFolder = KnownFolders.PicturesLibrary
            Try
                Dim file As StorageFile = Await folder.GetFileAsync("Test " & Extension + " file." & Extension)
                Await file.DeleteAsync()
            Catch ex As Exception
                ' File I/O errors are reported as exceptions.
                ' Ignore errors here.
            End Try
        End Sub
    End Class
End Namespace
