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
Imports System.Linq
Imports System.Text
Imports System.Threading.Tasks
Imports Windows.Devices.Enumeration

Namespace Global.ScreenCasting.Util

    Public Class CustomDevicePickerFilter

        Private _supportedDeviceSelectors As IList(Of String) = New List(Of String)()

        Public ReadOnly Property SupportedDeviceSelectors As IList(Of String)
            Get
                Return _supportedDeviceSelectors
            End Get
        End Property

        Public Overrides Function ToString() As String
            Dim retval As String = String.Empty
            If supportedDeviceSelectors.Count > 0 Then
                retval = supportedDeviceSelectors(0)
            End If

            If supportedDeviceSelectors.Count > 1 Then
                retval = "(" & retval & ")"
            End If

            For idx = 1 To supportedDeviceSelectors.Count - 1
                retval = retval & " OR (" & supportedDeviceSelectors(idx) & ")"
            Next

            Return retval
        End Function
    End Class
End Namespace
