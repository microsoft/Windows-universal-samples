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
Imports ScreenCasting
Imports System.Reflection
Imports ScreenCasting.Util

Namespace Global.ScreenCasting

    Partial Public Class MainPage
        Inherits Page

        Public Const FEATURE_NAME As String = "ScreenCasting"

        Public ReadOnly Property Scenarios As List(Of Scenario)

        Private Sub LoadScenarios()
            If ApiInformation.IsTypePresent("Windows.Devices.Enumeration.DevicePicker") Then
                _Scenarios = New List(Of Scenario) From {New Scenario() With {.Title = "1 - Media Element Casting 101", .ClassType = Type.GetType("ScreenCasting.Scenario01", False)}, New Scenario() With {.Title = "2 - Casting APIs and a Custom Cast Button", .ClassType = Type.GetType("ScreenCasting.Scenario02", False)}, New Scenario() With {.Title = "3 - DIAL Sender Universal Windows app", .ClassType = Type.GetType("ScreenCasting.Scenario03", False)}, New Scenario() With {.Title = "4 - DIAL Receiver Windows Universal app", .ClassType = Type.GetType("ScreenCasting.Scenario04")}, New Scenario() With {.Title = "5 - Multi-View Media Application", .ClassType = Type.GetType("ScreenCasting.Scenario05")}, New Scenario() With {.Title = "6 - Combine Casting Methods", .ClassType = Type.GetType("ScreenCasting.Scenario06")}}
            Else
                _Scenarios = New List(Of Scenario) From {New Scenario() With {.Title = "DIAL Receiver Windows Universal Application", .ClassType = GetType(Scenario04)}}
            End If
        End Sub
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class
End Namespace
