'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' This code is licensed under the Microsoft Public License.
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

        Public Const FEATURE_NAME As String = "ApplicationData"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "Files", .ClassType = GetType(ApplicationDataSample.Files)}, New Scenario() With {.Title = "Settings", .ClassType = GetType(ApplicationDataSample.Settings)}, New Scenario() With {.Title = "Setting Containers", .ClassType = GetType(ApplicationDataSample.SettingContainer)}, New Scenario() With {.Title = "Composite Settings", .ClassType = GetType(ApplicationDataSample.CompositeSettings)}, New Scenario() With {.Title = "DataChanged Event", .ClassType = GetType(ApplicationDataSample.DataChangedEvent)}, New Scenario() With {.Title = "Roaming: HighPriority", .ClassType = GetType(ApplicationDataSample.HighPriority)}, New Scenario() With {.Title = "ms-appdata:// Protocol", .ClassType = GetType(ApplicationDataSample.Msappdata)}, New Scenario() With {.Title = "Clear", .ClassType = GetType(ApplicationDataSample.ClearScenario)}, New Scenario() With {.Title = "SetVersion", .ClassType = GetType(ApplicationDataSample.SetVersion)}}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type

        Public Overrides Function ToString() As String
            Return Title
        End Function
    End Class
End Namespace
