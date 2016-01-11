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

Namespace Global.BackgroundAudioShared

    ''' <summary>
    ''' Simple helper for converting a string value to
    ''' its corresponding Enum literal.
    ''' 
    ''' e.g. "Running" -> BackgroundTaskState.Running
    ''' </summary>
    Public Module EnumHelper

        Public Function Parse(Of T)(value As String) As T
            Return CType([Enum].Parse(GetType(T), value), T)
        End Function
    End Module
End Namespace
