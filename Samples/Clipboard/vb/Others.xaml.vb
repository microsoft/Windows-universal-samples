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
Imports Windows.ApplicationModel.DataTransfer
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class OtherScenarios
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Private Shared registerContentChanged As Boolean

        Public Sub New()
            Me.InitializeComponent()
            Me.Init()
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            RegisterClipboardContentChanged.IsChecked = OtherScenarios.registerContentChanged
        End Sub

#Region " Scenario Specific Code "

        Sub Init()
            AddHandler ShowFormatButton.Click, New RoutedEventHandler(AddressOf ShowFormatButton_Click)
            AddHandler EmptyClipboardButton.Click, New RoutedEventHandler(AddressOf EmptyClipboardButton_Click)
            AddHandler RegisterClipboardContentChanged.Click, New RoutedEventHandler(AddressOf RegisterClipboardContentChanged_Click)
            AddHandler ClearOutputButton.Click, New RoutedEventHandler(AddressOf ClearOutputButton_Click)
        End Sub

#End Region

#Region " Button Click "

        Sub ShowFormatButton_Click(sender As Object, e As RoutedEventArgs)
            Me.DisplayFormats()
        End Sub

        Sub EmptyClipboardButton_Click(sender As Object, e As RoutedEventArgs)
            Try
                Windows.ApplicationModel.DataTransfer.Clipboard.Clear()
                OutputText.Text = "Clipboard has been emptied."
            Catch ex As Exception
                rootPage.NotifyUser("Error emptying Clipboard: " & ex.Message & ". Try again", NotifyType.ErrorMessage)
            End Try
        End Sub

        Sub RegisterClipboardContentChanged_Click(sender As Object, e As RoutedEventArgs)
            If OtherScenarios.registerContentChanged <> RegisterClipboardContentChanged.IsChecked.Value Then
                Me.ClearOutput()
                OtherScenarios.registerContentChanged = RegisterClipboardContentChanged.IsChecked.Value
                rootPage.EnableClipboardContentChangedNotifications(OtherScenarios.registerContentChanged)
                If OtherScenarios.registerContentChanged Then
                    OutputText.Text = "Successfully registered for clipboard update notification."
                Else
                    OutputText.Text = "Successfully un-registered for clipboard update notification."
                End If
            End If
        End Sub

        Sub ClearOutputButton_Click(sender As Object, e As RoutedEventArgs)
            Me.ClearOutput()
        End Sub

#End Region

#Region " Private helper methods "
        Private Sub ClearOutput()
            rootPage.NotifyUser("", NotifyType.StatusMessage)
            OutputText.Text = ""
        End Sub

        Private Sub DisplayFormats()
            rootPage.NotifyUser("", NotifyType.StatusMessage)
            OutputText.Text = rootPage.BuildClipboardFormatsOutputString()
        End Sub
#End Region
    End Class
End Namespace
