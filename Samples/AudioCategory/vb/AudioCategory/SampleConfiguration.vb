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
Imports PlaybackManager

Namespace Global.SDKTemplate

    Public Partial Class MainPage
        Inherits Page

        ' Change the string below to reflect the name of your sample.
        ' This is used on the main page as the title of the sample.
        Public Const FEATURE_NAME As String = "Audio Category Sample"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "Movie", .ClassType = GetType(Scenario1)}, New Scenario() With {.Title = "Media", .ClassType = GetType(Scenario2)}, New Scenario() With {.Title = "Game Chat", .ClassType = GetType(Scenario3)}, New Scenario() With {.Title = "Speech", .ClassType = GetType(Scenario4)}, New Scenario() With {.Title = "Communications", .ClassType = GetType(Scenario5)}, New Scenario() With {.Title = "Alerts", .ClassType = GetType(Scenario6)}, New Scenario() With {.Title = "Sound Effects", .ClassType = GetType(Scenario7)}, New Scenario() With {.Title = "Game Effects", .ClassType = GetType(Scenario8)}, New Scenario() With {.Title = "Game Media", .ClassType = GetType(Scenario9)}, New Scenario() With {.Title = "Other", .ClassType = GetType(Scenario10)}}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class
End Namespace
