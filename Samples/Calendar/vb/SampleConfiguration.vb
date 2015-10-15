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

        Public Const FEATURE_NAME As String = "Calendar VB sample"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "Display a calendar", .ClassType = GetType(SDKTemplate.Scenario1_Data)}, New Scenario() With {.Title = "Retrieve calendar statistics", .ClassType = GetType(SDKTemplate.Scenario2_Stats)}, New Scenario() With {.Title = "Calendar enumeration and math", .ClassType = GetType(SDKTemplate.Scenario3_Enum)}, New Scenario() With {.Title = "Calendar with Unicode extensions in languages", .ClassType = GetType(SDKTemplate.Scenario4_UnicodeExtensions)}, New Scenario() With {.Title = "Calendar time zone support", .ClassType = GetType(SDKTemplate.Scenario5_TimeZone)}}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class
End Namespace
