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
Imports System.Diagnostics
Imports System.Linq
Imports System.Text
Imports Windows.Storage

Namespace Global.BackgroundAudioShared

    ''' <summary>
    ''' Collection of string constants used in the entire solution. This file is shared for all projects
    ''' </summary>
    Public Module ApplicationSettingsHelper

        ''' <summary>
        ''' Function to read a setting value and clear it after reading it
        ''' </summary>
        Public Function ReadResetSettingsValue(key As String) As Object
            Debug.WriteLine(key)
            If Not ApplicationData.Current.LocalSettings.Values.ContainsKey(key) Then
                Debug.WriteLine("null returned")
                Return Nothing
            Else
                Dim value = ApplicationData.Current.LocalSettings.Values(key)
                ApplicationData.Current.LocalSettings.Values.Remove(key)
                Debug.WriteLine("value found " & value.ToString())
                Return value
            End If
        End Function

        ''' <summary>
        ''' Save a key value pair in settings. Create if it doesn't exist
        ''' </summary>
        Public Sub SaveSettingsValue(key As String, value As Object)
            Debug.WriteLine(key & ":" & (If(value Is Nothing, "null", value.ToString())))
            If Not ApplicationData.Current.LocalSettings.Values.ContainsKey(key) Then
                ApplicationData.Current.LocalSettings.Values.Add(key, value)
            Else
                ApplicationData.Current.LocalSettings.Values(key) = value
            End If
        End Sub
    End Module
End Namespace
