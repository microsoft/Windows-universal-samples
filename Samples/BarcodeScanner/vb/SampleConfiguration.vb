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

        Public Const FEATURE_NAME As String = "Barcode Scanner"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "DataReceived event", .ClassType = GetType(Scenario1_BasicFunctionality)}, New Scenario() With {.Title = "Release/Retain functionality", .ClassType = GetType(Scenario2_MultipleScanners)}}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class
End Namespace
