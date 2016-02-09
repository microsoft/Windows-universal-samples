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

Namespace Global.SDKTemplate

    Public Partial Class MainPage
        Inherits Page

        Public Const FEATURE_NAME As String = "Accelerometer"

        Public ReadOnly Property scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "Data events", .ClassType = GetType(Scenario1_DataEvents)}, New Scenario() With {.Title = "Shake events", .ClassType = GetType(Scenario2_ShakeEvents)}, New Scenario() With {.Title = "Polling", .ClassType = GetType(Scenario3_Polling)}, New Scenario() With {.Title = "OrientationChange", .ClassType = GetType(Scenario4_OrientationChanged)}, New Scenario() With {.Title = "Data events batching", .ClassType = GetType(Scenario5_DataEventsBatching)}}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class
End Namespace
