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

Namespace Global.BackgroundAudioShared

    ''' <summary>
    ''' Collection of string constants used in the entire solution. This file is shared for all projects
    ''' </summary>
    Public Module ApplicationSettingsConstants

        ' Data keys
        Public Const TrackId As String = "trackid"

        Public Const Position As String = "position"

        Public Const BackgroundTaskState As String = "backgroundtaskstate"

        Public Const AppState As String = "appstate"

        Public Const AppSuspendedTimestamp As String = "appsuspendedtimestamp"

        Public Const AppResumedTimestamp As String = "appresumedtimestamp"
    End Module
End Namespace
