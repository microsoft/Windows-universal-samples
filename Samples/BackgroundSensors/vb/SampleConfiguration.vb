' Copyright (c) Microsoft. All rights reserved.
Imports System
Imports System.Collections.Generic
Imports Windows.UI.Xaml.Controls

Namespace Global.SDKTemplate

    Public Partial Class MainPage
        Inherits Page

        Public Const FEATURE_NAME As String = "Background sensors"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "Accelerometer DeviceUse", .ClassType = GetType(Scenario1_DeviceUse)}}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class

    Public Structure SampleConstants

        Public Const Scenario1_DeviceUse_TaskName As String = "Scenario1_DeviceUse BackgroundTask"

        Public Const Scenario1_DeviceUse_TaskEntryPoint As String = "BackgroundTask.Scenario1_BackgroundTask"
    End Structure
End Namespace
