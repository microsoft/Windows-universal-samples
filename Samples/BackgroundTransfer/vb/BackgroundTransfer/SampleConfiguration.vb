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
Imports BackgroundTransfer

Namespace Global.SDKTemplate

    Public Partial Class MainPage
        Inherits Page

        Public Const FEATURE_NAME As String = "BackgroundTransfer"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "File Download", .ClassType = GetType(Scenario1_Download)}, New Scenario() With {.Title = "File Upload", .ClassType = GetType(Scenario2_Upload)}, New Scenario() With {.Title = "Completion Notifications", .ClassType = GetType(Scenario3_Notifications)}, New Scenario() With {.Title = "Completion Groups", .ClassType = GetType(Scenario4_CompletionGroups)}}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class
End Namespace
