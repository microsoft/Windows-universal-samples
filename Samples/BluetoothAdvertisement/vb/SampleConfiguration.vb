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
Imports BluetoothAdvertisement

Namespace Global.SDKTemplate

    Public Partial Class MainPage
        Inherits Page

        Public Const FEATURE_NAME As String = "Bluetooth Low Energy Advertisement"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "Foreground watcher", .ClassType = GetType(Scenario1_Watcher)}, New Scenario() With {.Title = "Foreground publisher", .ClassType = GetType(Scenario2_Publisher)}, New Scenario() With {.Title = "Background watcher", .ClassType = GetType(Scenario3_BackgroundWatcher)}, New Scenario() With {.Title = "Background publisher", .ClassType = GetType(Scenario4_BackgroundPublisher)}}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class
End Namespace
