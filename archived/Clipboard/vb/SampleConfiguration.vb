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
Imports System.Text
Imports Windows.ApplicationModel.DataTransfer
Imports Windows.UI.Core
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls

Namespace Global.SDKTemplate

    Public Partial Class MainPage
        Inherits Page

        Public Const FEATURE_NAME As String = "Clipboard VB sample"

        Private clipboardContentChanged As EventHandler(Of Object)

        Private needToPrintClipboardFormat As Boolean

        Private isApplicationWindowActive As Boolean = True

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "Copy and paste text", .ClassType = GetType(CopyText)}, New Scenario() With {.Title = "Copy and paste an image", .ClassType = GetType(CopyImage)}, New Scenario() With {.Title = "Copy and paste files", .ClassType = GetType(CopyFiles)}, New Scenario() With {.Title = "Other Clipboard operations", .ClassType = GetType(OtherScenarios)}}

        Public Sub EnableClipboardContentChangedNotifications(enable As Boolean)
            If enable Then
                clipboardContentChanged = New EventHandler(Of Object)(AddressOf OnClipboardChanged)
                AddHandler Windows.ApplicationModel.DataTransfer.Clipboard.ContentChanged, clipboardContentChanged
                AddHandler Window.Current.Activated, New WindowActivatedEventHandler(AddressOf OnWindowActivated)
            Else
                RemoveHandler Windows.ApplicationModel.DataTransfer.Clipboard.ContentChanged, clipboardContentChanged
                RemoveHandler Window.Current.Activated, New WindowActivatedEventHandler(AddressOf OnWindowActivated)
            End If
        End Sub

        Public Function BuildClipboardFormatsOutputString() As String
            Dim clipboardContent As DataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.GetContent()
            Dim output As StringBuilder = New StringBuilder()
            If clipboardContent IsNot Nothing AndAlso clipboardContent.AvailableFormats.Count > 0 Then
                output.Append("Available formats in the clipboard:")
                For Each format As String In clipboardContent.AvailableFormats
                    output.Append(Environment.NewLine & " * " & format)
                Next
            Else
                output.Append("The clipboard is empty")
            End If

            Return output.ToString()
        End Function

        Private Sub DisplayChangedFormats()
            Dim output As String = "Clipboard content has changed!" & Environment.NewLine
            output &= BuildClipboardFormatsOutputString()
            NotifyUser(output, NotifyType.StatusMessage)
        End Sub

        Private Sub HandleClipboardChanged()
            If Me.isApplicationWindowActive Then
                DisplayChangedFormats()
            Else
                Me.needToPrintClipboardFormat = True
            End If
        End Sub

        Private Sub OnWindowActivated(sender As Object, e As WindowActivatedEventArgs)
            Me.isApplicationWindowActive =(e.WindowActivationState <> CoreWindowActivationState.Deactivated)
            If Me.needToPrintClipboardFormat Then
                HandleClipboardChanged()
            End If
        End Sub

        Private Sub OnClipboardChanged(sender As [Object], e As [Object])
            HandleClipboardChanged()
        End Sub
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class
End Namespace
