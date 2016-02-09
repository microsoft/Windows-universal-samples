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
Imports Windows.UI.Xaml.Controls
Imports AdaptiveStreaming

Namespace Global.SDKTemplate

    Public Partial Class MainPage
        Inherits Page

        Public Const FEATURE_NAME As String = "AdaptiveStreaming"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "Basic HLS/DASH Playback", .ClassType = GetType(Scenario1)}, New Scenario() With {.Title = "Configuring HLS/DASH Playback", .ClassType = GetType(Scenario2)}, New Scenario() With {.Title = "Customized Resource Acquisiton", .ClassType = GetType(Scenario3)}, New Scenario() With {.Title = "HLS/DASH Playback with PlayReady", .ClassType = GetType(Scenario4)}}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class
End Namespace
