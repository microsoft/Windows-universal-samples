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
Imports BasicInput

Namespace Global.SDKTemplate

    Public Partial Class MainPage
        Inherits Page

        Public Const FEATURE_NAME As String = "Basic Input sample"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "Input Events", .ClassType = GetType(Scenario1)}, New Scenario() With {.Title = "PointerPoint Properties", .ClassType = GetType(Scenario2)}, New Scenario() With {.Title = "Device Capabilities", .ClassType = GetType(Scenario3)}, New Scenario() With {.Title = "XAML Manipulations", .ClassType = GetType(Scenario4)}, New Scenario() With {.Title = "Gesture Recognizer", .ClassType = GetType(Scenario5)}}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class
End Namespace
